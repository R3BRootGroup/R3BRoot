#include "R3BFTCalEngine.h"
#include "R3BException.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandMapToCalPar.h"
#include "R3BShared.h"
#include "R3BValueError.h"
#include <TH1.h>
#include <TH2.h>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <range/v3/algorithm/for_each.hpp>
#include <string_view>
#include <utility>

namespace R3B::Neuland::calibration
{
    auto FTType2Str(FTType type) -> std::string_view
    {
        switch (type)
        {
            case FTType::leftleading:
                return "left_leading";
            case FTType::rightleading:
                return "right_leading";
            case FTType::lefttrailing:
                return "left_trailing";
            case FTType::righttrailing:
                return "right_trailing";
            case FTType::trigger:
                return "trigger";
            default:
                throw R3B::logic_error("unresolvable types");
        }
    }

    ModuleCal::ModuleCal(std::string_view hist_name, int mID)
        : FTBaseCal{ hist_name, mID, { FTType::trigger } }
    {
        auto initHist = R3B::make_hist<TH1I>(hist_name.data(), hist_name.data(), MaxFTValue, 0.5, MaxFTValue + 0.5);
        InitAllDistributions(initHist.get());
    }

    PlaneCal::PlaneCal(std::string_view hist_name, int mID)
        : FTBaseCal{ hist_name,
                     mID,
                     { FTType::leftleading, FTType::lefttrailing, FTType::rightleading, FTType::righttrailing } }
    {
        auto initHist = R3B::make_hist<TH2I>(hist_name.data(),
                                             hist_name.data(),
                                             BarsPerPlane,
                                             +0.5,
                                             BarsPerPlane + 0.5,
                                             MaxFTValue,
                                             +0.5,
                                             MaxFTValue + 0.5);
        InitAllDistributions(initHist.get());
    }

    namespace
    {
        using ValueErrors = FTCalStrategy::ValueErrors;
        constexpr int uniform_err_divider = 12;
        constexpr auto uniform_err_divider_sqrt = SQRT_12;
        const auto sqrt_3 = std::numbers::sqrt3;

        struct InputInfo
        {
            double previous_sum;
            double bin_entry;
            double total_entry;
        };

        auto calculate_meanerror_exact(const InputInfo& input) -> ValueError<double>
        {
            const auto base_variance = input.bin_entry * input.bin_entry / uniform_err_divider;
            const auto sum_term = input.previous_sum + (input.bin_entry / 3);
            const auto bin_prob = input.bin_entry / input.total_entry;
            const auto pre_prob = input.previous_sum / input.total_entry;
            const auto residual = ((1 - bin_prob) * sum_term) - (input.previous_sum * pre_prob);

            const auto mean = input.previous_sum + (input.bin_entry / 2);
            return ValueError<double>{ mean, std::sqrt(base_variance + residual) };
        }

        auto calculate_meanerror_approx(const InputInfo& input) -> ValueError<double>
        {
            const auto mean = input.previous_sum + (input.bin_entry / 2);

            const auto base_error = input.bin_entry / uniform_err_divider_sqrt;
            const auto bin_prob = input.bin_entry / input.total_entry;
            const auto pre_prob = input.previous_sum / input.total_entry;
            const auto residual_main = (-(pre_prob - 0.5) * (pre_prob - 0.5)) + 0.25;
            const auto residual_factor = (bin_prob == 0.) ? 0. : sqrt_3 / bin_prob;

            return ValueError<double>{ mean, base_error + (residual_main * residual_factor) };
        }

        auto calculate_meanerror_uniform_only(const InputInfo& input) -> ValueError<double>
        {
            const auto mean = input.previous_sum + (input.bin_entry / 2);
            const auto base_error = input.bin_entry / uniform_err_divider_sqrt;
            return ValueError<double>{ mean, base_error };
        }

        auto calculate_meanerror_none(const InputInfo& input) -> ValueError<double>
        {
            const auto mean = input.previous_sum + (input.bin_entry / 2);
            return ValueError<double>{ mean, 0. };
        }

        auto use_method(FTCalErrorMethod methodtype)
        {
            switch (methodtype)
            {
                case FTCalErrorMethod::exact:
                    return +[](const InputInfo& input) -> ValueError<double>
                    { return calculate_meanerror_exact(input); };
                case FTCalErrorMethod::approx:
                    return +[](const InputInfo& input) -> ValueError<double>
                    { return calculate_meanerror_approx(input); };
                case FTCalErrorMethod::uniform_only:
                    return +[](const InputInfo& input) -> ValueError<double>
                    { return calculate_meanerror_uniform_only(input); };
                case FTCalErrorMethod::none:
                    return +[](const InputInfo& input) -> ValueError<double>
                    { return calculate_meanerror_none(input); };
                default:
                    throw R3B::logic_error("undefined enumerator for method type!");
            }
        }

        auto extract_bin_data(TH1* hist, int max_bin) -> ValueErrors
        {
            auto output = ValueErrors{};
            output.reserve(max_bin);
            // root histogram starts from index 1
            for (size_t index{ 1 }; index < max_bin + 1; ++index)
            {
                output.emplace_back(hist->GetBinContent(static_cast<int>(index)), 0.);
            }
            return output;
        }

        void scale_to_real_ns(ValueErrors& value_errors, double period, double total_entry)
        {
            ranges::for_each(value_errors,
                             [&](ValueError<double>& value_error) -> void
                             {
                                 value_error.value = value_error.value / total_entry * period;
                                 value_error.error = value_error.error / total_entry * period;
                             });
        }

        auto calculate_value_errors(TH1* hist, int max_bin, double total_entry, FTCalErrorMethod methodtype)
            -> std::pair<ValueErrors, int>
        {
            auto output = extract_bin_data(hist, max_bin);

            auto method = use_method(methodtype);
            auto previous_sum = 0.;
            for (auto& value_error : output)
            {
                auto& value = value_error.value;
                auto& error = value_error.error;
                const auto inputInfo =
                    InputInfo{ .previous_sum = previous_sum, .bin_entry = value, .total_entry = total_entry };
                previous_sum += value;
                auto new_value_error = method(inputInfo);
                value = new_value_error.value;
                error = new_value_error.error;
            }
            const auto overflow = total_entry - previous_sum;
            return std::make_pair(output, overflow);
        }
    } // namespace

    auto FTCalStrategy::GetChannel2Time(TH1* hist) const -> FTChannel2TimeRelation
    {
        const auto total_entry = hist->GetEntries();

        auto bin_num = hist->GetNbinsX();
        auto max_bin = (max_bin_number_ == 0) ? bin_num : max_bin_number_;
        max_bin = (bin_num > max_bin) ? max_bin : bin_num;

        auto [value_errors, overflow] = calculate_value_errors(hist, max_bin, total_entry, error_method_);

        scale_to_real_ns(value_errors, cycle_period_, total_entry);

        auto calRelation = FTChannel2TimeRelation{};
        calRelation.value_error = std::move(value_errors);
        calRelation.hist_overflow = overflow;
        return calRelation;
    }
} // namespace R3B::Neuland::calibration
