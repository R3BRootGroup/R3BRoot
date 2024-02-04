/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BLogger.h"
#include "R3BNeulandTamexReader2.h"
#include "TClonesArray.h"
#include "ext_data_client.h"
#include "ext_data_struct_info.hh"
#include "ext_h101_raw_nnp_tamex.h"
#include <R3BShared.h>
#include <boost/regex.hpp>

// TODO: C++20 std::span
#include <gsl/span>
// TODO: C++20 std::range
#include <range/v3/view.hpp>

/**
 ** ext_h101_raw_nnp_tamex.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:NN_P,id=h101_raw_nnp_tamex,NOTRIGEVENTNO,ext_h101_raw_nnp_tamex.h
 **/

using gsl::span;
namespace views = ranges::views;
using R3B::Side;
using Errors = R3BNeulandTamexReader2::Errors;

namespace
{
    constexpr size_t INITIAL_SIGNAL_SIZE = 20;
    constexpr size_t PRINT_ERROR_MAX = 100000;
    constexpr auto BarsPerPlane = 50;
    const auto errorStrings = std::map<Errors, std::string>{
        { Errors::module_size, "Incompatible module size" },
        { Errors::data_size, "Incompatible signal size" },
        { Errors::divider, "Incompatible signal dividers" },
        { Errors::indices, "Incompatible module IDs" },
    };

    const auto SIDES = std::array<Side, 2>{ Side::left, Side::right };
    template <typename BinaryOperation, typename Item0, typename Item1>
    inline constexpr auto CheckAmong(BinaryOperation optn, const Item0& item0, const Item1& item1) -> bool
    {
        return optn(item0, item1);
    }

    template <typename BinaryOperation, typename Item0, typename Item1, typename... Items>
    inline constexpr auto CheckAmong(BinaryOperation optn,
                                     const Item0& item0,
                                     const Item1& item1,
                                     const Items&... items) -> bool
    {
        return CheckAmong(optn, item0, item1) && CheckAmong(optn, item1, items...);
    }

    template <typename UnaryOperation, typename... Items>
    inline auto CheckAny(UnaryOperation optn, const Items&... items)
    {
        return (optn(items) || ...);
    }

    template <typename UnaryFunction, typename... Items>
    inline auto Zip_from(UnaryFunction optn, const Items&... items)
    {
        return ranges::zip_view(optn(items)...);
    }

    template <typename Signals, typename ModuleNum_divider, typename UnaryFunction>
    inline void FillData(const Signals& signals_view,
                         const ModuleNum_divider& moduleNum_divider_view,
                         UnaryFunction optn)
    {
        auto range_begin = signals_view.begin();
        auto range_iter = signals_view.begin();
        for (const auto& [module_num, divider] : moduleNum_divider_view)
        {
            for (; !(range_iter == signals_view.end() || range_iter == (range_begin + divider)); ++range_iter)
            {
                optn(*range_iter, module_num);
            }
        }
    }
} // namespace

R3BNeulandTamexReader2::R3BNeulandTamexReader2(EXT_STR_h101_raw_nnp_tamex_onion* data, size_t offset)
    : R3BReader("R3BNeulandTamexReader2")
    , max_limit_(PRINT_ERROR_MAX)
    , fOffset(offset)
    , numPlanes_(R3B::GetSize(inputData_->NN_P))
    , inputData_{ data }
    , mappedDataPtr_{ &mappedData_ }
    , mappedTrigDataPtr_{ &mappedTrigData_ }
{
    mappedData_.reserve(INITIAL_SIGNAL_SIZE);
    error_log_.insert_or_assign(Errors::module_size, 0);
    error_log_.insert_or_assign(Errors::data_size, 0);
    error_log_.insert_or_assign(Errors::indices, 0);
    error_log_.insert_or_assign(Errors::divider, 0);
}

Bool_t R3BNeulandTamexReader2::Init(ext_data_struct_info* a_struct_info) // NOLINT
{
    auto status_ok = 1;
    R3BLOG(info, "");
    EXT_STR_h101_raw_nnp_tamex_ITEMS_INFO(status_ok, *a_struct_info, fOffset, EXT_STR_h101_raw_nnp_tamex, 0); // NOLINT

    if (status_ok == 0)
    {
        R3BLOG(error, "Failed to setup structure information.");
        return false;
    }

    R3BLOG(info, "Number of planes " << numPlanes_);

    // Register output arrays in tree
    auto* rootMan = FairRootManager::Instance();
    rootMan->RegisterAny("NeulandMappedData", mappedDataPtr_, !is_online_);
    if (is_triggered_)
    {
        rootMan->RegisterAny("NeulandTrigMappedData", mappedTrigDataPtr_, !is_online_);
    }
    Reset();
    if (not is_online_)
    {
        histogram_init();
    }
    return kTRUE;
}

void R3BNeulandTamexReader2::histogram_init()
{
    const auto BarBinSize = numPlanes_ * 50;
    histograms_.add_hist<TH1I>(
        "bar_FT_l", "number of hits on left PMTS for each bar", BarBinSize, 0.5, 0.5 + BarBinSize);
    histograms_.add_hist<TH1I>(
        "bar_FT_r", "number of hits on right PMTS for each bar", BarBinSize, 0.5, 0.5 + BarBinSize);
}

template <typename ErrorLog, typename... Items>
auto CheckCondition(ErrorLog& log, const Items&... items) -> bool
{
    auto ApplyCriterium = [&log, &items...](Errors error, auto criterium) -> bool
    {
        if (!CheckAmong(criterium, items...))
        {
            ++log.at(error);
            return false;
        }
        return true;
    };
    return ApplyCriterium(Errors::module_size,
                          [](const auto& left, const auto& right) { return left.BM == right.BM; }) &&
           ApplyCriterium(Errors::data_size, [](const auto& left, const auto& right) { return left.B == right.B; }) &&
           ApplyCriterium(Errors::indices,
                          [](const auto& left, const auto& right) {
                              return ranges::equal(views::take(left.BMI, left.BM), views::take(right.BMI, right.BM));
                          }) &&
           ApplyCriterium(Errors::divider,
                          [](const auto& left, const auto& right)
                          { return ranges::equal(views::take(left.BME, left.BM), views::take(right.BME, right.BM)); });
}

template <typename ViewType>
auto R3BNeulandTamexReader2::extract_plane_signals(const ViewType& signalsPlane, int planeNum)
{
    auto planeSignals = R3BPaddleTamexMappedData2{ static_cast<unsigned int>(planeNum) };
    const auto signals_sides_view =
        ranges::zip_view(signalsPlane.tcl_T, signalsPlane.tfl_T, signalsPlane.tct_T, signalsPlane.tft_T, SIDES);
    for (const auto& [coarse_leading, fine_leading, coarse_trailing, fine_trailing, side] : signals_sides_view)
    {
        // pass if no data
        if (CheckAny([](const auto& item) { return item.BM == 0; },
                     coarse_leading,
                     fine_leading,
                     coarse_trailing,
                     fine_trailing))
        {
            R3BLOG(debug3, fmt::format("No signals at the current event from plane {}.", planeNum));
            continue;
        }

        if (!CheckCondition(error_log_, coarse_leading, fine_leading, coarse_trailing, fine_trailing))
        {
            return R3BPaddleTamexMappedData2{};
        }

        const auto module_size = coarse_leading.BM;
        const auto signal_size = coarse_leading.B;
        R3BLOG(debug, fmt::format("Signals at the current event from plane {}: {}", planeNum, signal_size));
        mappedData_.reserve(signal_size);
        const auto barNum_divider_view = ranges::zip_view(span(coarse_leading.BMI), span(coarse_leading.BME));

        const auto signals_view = Zip_from([](const auto& item) { return span(item.Bv); },
                                           coarse_leading,
                                           fine_leading,
                                           coarse_trailing,
                                           fine_trailing);
        // TODO C++20: planeID and side can be directly captured by the lambda. Before C++20, capturing struture
        // bindings is undefined behaviour
        auto side_temp = side;
        FillData(
            signals_view | ranges::views::take(signal_size),
            barNum_divider_view | ranges::views::take(module_size),
            [&](const auto& signals, const auto& barNum)
            {
                const auto& [cl_time, fl_time, ct_time, ft_time] = signals;
                auto doubleEdgeSignal = R3B::DoubleEdgeSignal{};
                doubleEdgeSignal.leading.fine = fl_time;
                doubleEdgeSignal.leading.coarse = cl_time;
                doubleEdgeSignal.trailing.fine = ft_time;
                doubleEdgeSignal.trailing.coarse = ct_time;

                planeSignals.push_back(side_temp, barNum, doubleEdgeSignal);
                R3BLOG(
                    debug3,
                    fmt::format(
                        "Writing a doube edge signal: barNum: {}, fl_time: {}, cl_time: {}, ft_time: {}, ct_time: {}",
                        barNum,
                        fl_time,
                        cl_time,
                        ft_time,
                        ct_time));

                if (is_online_)
                {
                    return;
                }

                if (side_temp == R3B::Side::left)
                {
                    histograms_.get("bar_FT_l")->Fill(barNum + (planeNum - 1) * BarsPerPlane);
                }
                else
                {
                    histograms_.get("bar_FT_r")->Fill(barNum + (planeNum - 1) * BarsPerPlane);
                }
            });
    }
    return planeSignals;
}

auto R3BNeulandTamexReader2::ReadSignals(EXT_STR_h101_raw_nnp_tamex_onion* inputData) -> bool
{
    const auto signalsAllPlanes = span(inputData->NN_P);

    mappedData_.clear();
    for (const auto& [signalsPlane, planeID] : ranges::zip_view(signalsAllPlanes, ranges::iota_view(1)))
    {
        auto planeSignals = extract_plane_signals(signalsPlane, planeID);
        if (!planeSignals.empty())
        {
            if (not is_online_)
            {
                histogram_action(planeSignals);
            }
            mappedData_.emplace_back(std::move(planeSignals));
        }
    }

    return true;
}

void R3BNeulandTamexReader2::histogram_action(const R3B::PaddleTamexMappedData& mappedData)
{
    for (const auto& [hist_name, action] : hist_actions_)
    {
        action(mappedData, histograms_.get(hist_name));
    }
}

template <typename ErrorLog, typename... Items>
auto CheckTrigCondition(ErrorLog& log, EXT_STR_h101_raw_nnp_tamex_onion* inputData)
{
    auto ApplyCriterium = [&log](Errors error, bool criterium) -> bool
    {
        if (!criterium)
        {
            ++log.at(error);
            return false;
        }
        return true;
    };
    return ApplyCriterium(Errors::module_size, inputData->NN_TRIGCM == inputData->NN_TRIGFM) &&
           ApplyCriterium(Errors::data_size, inputData->NN_TRIGC == inputData->NN_TRIGF) &&
           ApplyCriterium(Errors::indices,
                          ranges::equal(views::take(inputData->NN_TRIGCMI, inputData->NN_TRIGCM),
                                        views::take(inputData->NN_TRIGFMI, inputData->NN_TRIGFM))) &&
           ApplyCriterium(Errors::divider,
                          ranges::equal(views::take(inputData->NN_TRIGCME, inputData->NN_TRIGCM),
                                        views::take(inputData->NN_TRIGFME, inputData->NN_TRIGFM)));
}

auto R3BNeulandTamexReader2::ReadTriggerSignals(EXT_STR_h101_raw_nnp_tamex_onion* inputData) -> bool
{
    if (!is_triggered_ || inputData->NN_TRIGCM == 0 || inputData->NN_TRIGFM == 0)
    {
        return true;
    }

    if (!CheckTrigCondition(error_log_, inputData))
    {
        return false;
    }

    const auto module_size = inputData->NN_TRIGCM;
    const auto signal_size = inputData->NN_TRIGC;
    auto const trigger_signals_view =
        Zip_from([](const auto& item) { return span(item); }, inputData->NN_TRIGCv, inputData->NN_TRIGFv);
    auto const moduleNum_divider_view = ranges::zip_view(span(inputData->NN_TRIGCMI), span(inputData->NN_TRIGCME));
    FillData(ranges::views::take(trigger_signals_view, signal_size),
             ranges::views::take(moduleNum_divider_view, module_size),
             [&](const auto& signal_pack, const auto& module_num)
             {
                 const auto& [coarse_time, fine_time] = signal_pack;
                 auto trigDatum = mappedTrigData_.emplace(module_num, R3BPaddleTamexTrigMappedData{}).first;
                 trigDatum->second.module_num = module_num;
                 trigDatum->second.signal.fine = fine_time;
                 trigDatum->second.signal.coarse = coarse_time;
                 R3BLOG(debug3,
                        fmt::format("Writing a trig signal: module ID: {}, fine time: {}, coarse_time: {}",
                                    module_num,
                                    fine_time,
                                    coarse_time));
                 // std::cout << "trig signal: " << module_num << "\t" << fine_time << "\t" << coarse_time << "\n";
             });
    return true;
}

auto R3BNeulandTamexReader2::R3BRead() -> bool
{
    if (!(ReadSignals(inputData_) && ReadTriggerSignals(inputData_)))
    {
        return false;
    }
    if (fair::Logger::GetConsoleSeverity() <= fair::Severity::info && ++counter_ == max_limit_)
    {
        counter_ = 0;
        print_error();
    }
    return true;
}

auto R3BNeulandTamexReader2::check_trigger_needed(std::string_view item_name) const -> bool
{
    // TODO: not the exact regex, but it suffices
    const auto neuland_trigger_regex = boost::regex{ "^NN_TRIG(C|F)(v|M)?(I|E)?$" };
    auto res = is_triggered_ && boost::regex_match(item_name.data(), neuland_trigger_regex);
    return res;
}

auto R3BNeulandTamexReader2::check_bar_needed(std::string_view item_name) const -> bool
{
    const auto neuland_bar_regex = boost::regex{ R"(^NN_P(\d*)t.*$)" };
    auto result = boost::cmatch{};
    if (boost::regex_search(item_name.data(), result, neuland_bar_regex))
    {
        const auto plane_num = std::stoi(result.str(1));
        if (plane_num != 0 and plane_num <= numPlanes_)
        {
            return true;
        }
    }
    return false;
}

auto R3BNeulandTamexReader2::MismappedItemRequired(std::string_view item_name) const -> bool
{
    if (numPlanes_ == 0)
    {
        throw R3B::logic_error(
            "The plane number of NeuLAND is not set. Please use R3BNeulandTamexReader2::SetMaxNbPalnes to set it.");
    }

    return check_trigger_needed(item_name) || check_bar_needed(item_name);
}

void R3BNeulandTamexReader2::Reset()
{
    mappedData_.clear();
    mappedTrigData_.clear();
}

void R3BNeulandTamexReader2::print_error()
{
    fmt::print("----------Tamex Reader Error Summary:--------------\n");
    fmt::print("{0:^30}|{1:^10}\n", "Causes", "Counts");
    for (const auto& [key, counts] : error_log_)
    {
        fmt::print("{0:<30}|{1:^10}\n", errorStrings.at(key), counts);
    }
    fmt::print("---------------------------------------------------\n");
}

ClassImp(R3BNeulandTamexReader2);
