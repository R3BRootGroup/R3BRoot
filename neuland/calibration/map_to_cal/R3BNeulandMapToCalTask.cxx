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

#include "R3BNeulandMapToCalTask.h"
#include "R3BDataMonitor.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandCommonFunc.h"
#include "R3BNeulandMapToCalPar.h"
#include "R3BPaddleTamexMappedData2.h"
#include "R3BShared.h"
#include "R3BValueError.h"
#include <FairRootManager.h>
#include <R3BEventHeader.h>
#include <R3BException.h>
#include <TH1.h>
#include <TH2.h>
#include <algorithm>
#include <cmath>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iterator>
#include <range/v3/view/map.hpp>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
    // NOLINTBEGIN
    auto GetWalkCorrection(double value) -> double
    {
        value = value * 17.2278 / 162.464;
        const auto veronika3 = -4.29359 + 17.3841 / sqrt(value) + 0.073181; // veronika 3
        const auto veronika2 = 1.59667 + 78.5274 * pow(value, -1.97051) - 4.00192 / value - 0.125473 * value +
                               0.00130958 * value * value; // veronika 2
        const auto result = (value > 25.0000) ? veronika3 : (veronika3 + veronika2) / 2.;
        return -1. * result;
    }
    // NOLINTEND

} // namespace

namespace R3B::Neuland
{
    Map2CalTask::Map2CalTask(const Config& config)
        : CalibrationTask(config.name, 1)
        , config_{ config }
        , map_data_{ get_from_sep_string(0, config.read) }
        , trig_map_data_{ get_from_sep_string(1, config.read) }
        , calibration_par_{ get_from_sep_string(2, config.read) }
        , calibration_trig_par_{ get_from_sep_string(3, config.read) }
        , cal_data_{ get_from_sep_string(0, config.write) }
    {
        SetTrigger(config.mode);
    }

    void Map2CalTask::SetExtraPar(FairRuntimeDb* /*rtdb*/)
    {
        calibration_par_.init(this);
        calibration_trig_par_.init(this);
    }

    void Map2CalTask::HistogramInit(DataMonitor& histograms)
    {
        const auto total_bar_num = plane_num_ * BarsPerPlane;
        constexpr auto BINSIZE_TOT = 1000;
        constexpr auto BINSIZE_LT = 1000;
        constexpr auto BINSIZE_TIME = 8000;
        constexpr auto HIST_MAX_TIME = 6000.;
        constexpr auto HIST_MIN_TIME = -5000.;
        constexpr auto HIST_MAX_TOT = 600.;
        constexpr auto HIST_MAX_LT = 2000.;

        histograms.add_hist<TH1D>("module_num", "Counts with module ids", total_bar_num, -0.5, total_bar_num + 0.5);
        histograms.add_hist<TH1D>("ToT", "Time over threshold", BINSIZE_TOT * 2, 0., BINSIZE_TOT + 0.);
        histograms.add_hist<TH1D>("LeadingTime", "Leading time", BINSIZE_LT, 0., HIST_MAX_LT);
        histograms.add_hist<TH1D>("TriggerTime", "Trigger time", BINSIZE_LT, 0., HIST_MAX_LT);
        histograms.add_hist<TH1D>("Leading_minus_trigger", "Time", BINSIZE_TIME, HIST_MIN_TIME, HIST_MAX_TIME);
        histograms.add_hist<TH1I>(
            "Bar_hitNum_l", "Number of hits on the left PMT per bar", total_bar_num, 0.5, 0.5 + total_bar_num);
        histograms.add_hist<TH1I>(
            "Bar_hitNum_r", "Number of hits on the right PMT per bar", total_bar_num, 0.5, 0.5 + total_bar_num);

        histograms.add_hist<TH2D>("TimeLVsBar",
                                  "Time_vs_bar_left",
                                  total_bar_num,
                                  0.5,
                                  0.5 + total_bar_num,
                                  BINSIZE_TIME,
                                  HIST_MIN_TIME,
                                  HIST_MAX_TIME);
        histograms.add_hist<TH2D>("TimeRVsBar",
                                  "Time_vs_bar_right",
                                  total_bar_num,
                                  0.5,
                                  0.5 + total_bar_num,
                                  BINSIZE_TIME,
                                  HIST_MIN_TIME,
                                  HIST_MAX_TIME);

        histograms.add_hist<TH2D>(
            "ToTLVsBar", "ToT_vs_bar_left", total_bar_num, 0.5, 0.5 + total_bar_num, BINSIZE_TOT, 0., HIST_MAX_TOT);
        histograms.add_hist<TH2D>(
            "ToTRVsBar", "ToT_vs_bar_right", total_bar_num, 0.5, 0.5 + total_bar_num, BINSIZE_TOT, 0., HIST_MAX_TOT);
    }

    void Map2CalTask::ExtraInit(FairRootManager* /*rootMan*/)
    {
        map_data_.init();
        trig_map_data_.init();
        cal_data_.init(not IsHistDisabled());
        set_pmt_num();
        set_ct_freq();
    }

    void Map2CalTask::set_pmt_num()
    {
        if (plane_num_ = GetBasePar()->get_num_of_planes(); plane_num_ == 0)
        {
            LOGP(warn, "plane number obtained from the calibration parameters is 0!");
        }
        else
        {
            total_pmt_nums_ = 2 * plane_num_ * BarsPerPlane;
            LOGP(info, "total number of PMTs set to be {}", total_pmt_nums_);
        }

        if (total_pmt_nums_ == 0)
        {
            throw R3B::runtime_error("The number of PMTs cannot be zero!");
        }
    }

    void Map2CalTask::set_ct_freq()
    {
        if (auto ct_freq = calibration_par_->GetSlowClockFrequency(); ct_freq == 0)
        {
            LOGP(warn, "Coarse time frequency obtained from parameters is 0! Use default value.");
            coarse_time_frequency_ = COARSE_TIME_CLOCK_FREQUENCY_MHZ;
        }
        else
        {
            coarse_time_frequency_ = ct_freq;
            max_coarse_time_ = 1000. * coarse_time_max_num_ / coarse_time_frequency_;
            LOGP(info, "Coarse time frequency set to be {} MHz", ct_freq);
        }
    }

    auto Map2CalTask::CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool
    {

        auto signal_size = map_data_.size();
        LOGP(debug2,
             "Minimal signal size: {}. Current signal size: {}. Number of PMTs: {}",
             config_.min_stat,
             signal_size,
             total_pmt_nums_);
        if (std::cmp_less(signal_size, config_.min_stat))
        {
            LOGP(debug2,
                 "condition of the minimal size is not met with current paddle signal size. Skip the "
                 "current event.");
            ConditionFillToHist(hist_condition, "undersized");
            return false;
        }
        return config_.enable_pulse_mode ? signal_size > total_pmt_nums_ : signal_size < total_pmt_nums_ / 2;
    }

    void Map2CalTask::TriggeredExec() { calibrate(); }

    auto Map2CalTask::convert_to_real_time(R3BTCalPar2* calPar,
                                           SingleEdgeSignal signal,
                                           FTType ftType,
                                           int module_num) const -> ValueError<double>
    {
        const auto& modulePar = calPar->GetParamAt(module_num);
        const auto fineTime = modulePar.GetFineTime(ftType, signal.fine);

        // signal time in ns
        const auto coarseTime = 1000. / coarse_time_frequency_ * static_cast<float>(signal.coarse + 1);
        return { coarseTime - fineTime.value, fineTime.error };
    }

    auto Map2CalTask::get_trigger_time(int module_num, Side side) const -> ValueError<double>
    {

        const auto& triggerMap = GetBasePar()->get_trig_id_map();
        auto triggerIDPair = triggerMap.find(module_num);
        if (triggerIDPair == triggerMap.end())
        {
            const auto eventNum = GetEventHeader()->GetEventno();
            LOGP(error, "Can't find the trigger ID for the module ID {} in the event {}", module_num, eventNum);
            return { -1., 0 };
        }
        const auto triggerID = (side == Side::left) ? triggerIDPair->second.first : triggerIDPair->second.second;
        const auto trigData = std::ranges::find_if(
            trig_map_data_, [&triggerID](const auto& ele) -> bool { return ele.first == triggerID; });
        if (trigData == trig_map_data_.end())
        {
            const auto eventNum = GetEventHeader()->GetEventno();
            LOGP(error,
                 "No such trigger ID {} in mappedTrigData for moduleNum {} in the event {}!",
                 triggerID,
                 module_num,
                 eventNum);
            LOGP(error, "Available trigIDs: {}", fmt::join(trig_map_data_ | ranges::views::keys, ", "));
            return { -1., 0 };
        }
        return convert_to_real_time(
            calibration_trig_par_.get(), trigData->second.signal, FTType::trigger, trigData->first);
    }

    auto Map2CalTask::get_tot(const DoubleEdgeSignal& pmtSignal, int module_num, R3B::Side module_side) const
        -> ValueError<double>
    {
        const auto leadFType = (module_side == Side::left) ? FTType::leftleading : FTType::rightleading;
        const auto trailFType = (module_side == Side::left) ? FTType::lefttrailing : FTType::righttrailing;
        const auto leadingT = convert_to_real_time(calibration_par_.get(), pmtSignal.leading, leadFType, module_num);
        const auto trailingT = convert_to_real_time(calibration_par_.get(), pmtSignal.trailing, trailFType, module_num);
        const auto time_over_thresh = trailingT - leadingT;
        LOGP(debug3, "leading :{} trailing: {}", leadingT.value, trailingT.value);
        return (time_over_thresh.value > 0) ? time_over_thresh : time_over_thresh + max_coarse_time_;
    }

    void Map2CalTask::overflow_correct(R3B::Neuland::CalDataSignal& calSignal) const
    {
        // trigger time should come after signal time
        if (calSignal.leading_time.value > calSignal.trigger_time.value)
        {
            calSignal.leading_time -= max_coarse_time_;
        }
    }

    auto Map2CalTask::doubleEdgeSignal_to_calSignal(const DoubleEdgeSignal& double_edge_signal,
                                                    R3B::Side side,
                                                    int module_num) const -> CalDataSignal
    {
        auto calDataSignal = CalDataSignal{};
        const auto ftType = (side == R3B::Side::left) ? FTType::leftleading : FTType::rightleading;
        calDataSignal.time_over_threshold = get_tot(double_edge_signal, module_num, side);
        const auto walk_correction =
            (config_.enable_walk_effect) ? GetWalkCorrection(calDataSignal.time_over_threshold.value) : 0.;
        calDataSignal.leading_time =
            convert_to_real_time(calibration_par_.get(), double_edge_signal.leading, ftType, module_num) +
            walk_correction;
        calDataSignal.trigger_time = get_trigger_time(module_num, side);
        overflow_correct(calDataSignal);
        LOGP(debug, "Adding a new cal signal: {}", calDataSignal);
        return calDataSignal;
    }

    auto Map2CalTask::mapBarSignal_to_calSignals(const MapBarSignal& map_bar_signals,
                                                 int module_num,
                                                 R3B::Side side) const -> std::vector<CalDataSignal>
    {
        const auto& signals = (side == Side::left) ? map_bar_signals.left : map_bar_signals.right;
        auto calSignals = std::vector<CalDataSignal>{};
        calSignals.reserve(signals.size());
        std::ranges::transform(signals,
                               std::back_inserter(calSignals),
                               [module_num, side, this](const auto& dESignal) -> CalDataSignal
                               { return doubleEdgeSignal_to_calSignal(dESignal, side, module_num); });
        return calSignals;
    }

    void Map2CalTask::fill_cal_data(BarCalData& cal, const MapBarSignal& signals)
    {
        cal.left = mapBarSignal_to_calSignals(signals, cal.module_num, Side::left);
        cal.right = mapBarSignal_to_calSignals(signals, cal.module_num, Side::right);

        if (not IsHistDisabled())
        {
            histogram_monitor(cal, Side::left);
            histogram_monitor(cal, Side::right);
        }
    }

    void Map2CalTask::calibrate()
    {
        LOGP(debug2, "mapped Data size: {}", map_data_.size());

        if (trig_map_data_.size() == 0)
        {
            return;
        }
        for (const auto& [plane_num, plane_signals] : map_data_)
        {
            for (const auto& [bar_num, bar_signals] : plane_signals.bars)
            {
                if (not IsHistDisabled())
                {
                    GetHistMonitor().get("module_num")->Fill(bar_num);
                }
                const auto module_num = Neuland_PlaneBar2ModuleNum(plane_num, bar_num);
                auto& cal = cal_data_.get().emplace_back(module_num);
                fill_cal_data(cal, bar_signals);
            }
        }
    }

    void Map2CalTask::histogram_monitor(const BarCalData& cal, Side side)
    {
        auto& histograms = GetHistMonitor();
        const auto module_num = cal.module_num;
        const auto& calSignals = (side == Side::left) ? cal.left : cal.right;

        for (const auto& signal : calSignals)
        {
            const auto lTime = signal.leading_time.value;
            const auto time_over_thresh = signal.time_over_threshold.value;
            const auto triggerTime = signal.trigger_time.value;
            histograms.get("ToT")->Fill(time_over_thresh);
            histograms.get("LeadingTime")->Fill(lTime);
            histograms.get("TriggerTime")->Fill(triggerTime);
            histograms.get("Leading_minus_trigger")->Fill(lTime - triggerTime);
            if (side == Side::left)
            {
                histograms.get("TimeLVsBar")->Fill(module_num, lTime - triggerTime);
                histograms.get("ToTLVsBar")->Fill(module_num, time_over_thresh);
                histograms.get("Bar_hitNum_l")->Fill(module_num);
            }
            else
            {
                histograms.get("TimeRVsBar")->Fill(module_num, lTime - triggerTime);
                histograms.get("ToTRVsBar")->Fill(module_num, time_over_thresh);
                histograms.get("Bar_hitNum_r")->Fill(module_num);
            }
        }
    }

    void Map2CalTask::FinishEvent() {}
} // namespace R3B::Neuland
