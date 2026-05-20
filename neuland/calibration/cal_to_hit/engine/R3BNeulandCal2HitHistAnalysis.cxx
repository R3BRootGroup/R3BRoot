#include "R3BNeulandCal2HitHistAnalysis.h"
#include "R3BDataMonitor.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandCommonFunc.h"
#include "R3BValueError.h"
#include <R3BNeulandCalToHitParTask.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1.h>
#include <TH2.h>
#include <array>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <unordered_map>
#include <utility>
#include <vector>

namespace R3B::Neuland::Calibration
{

    namespace
    {
        constexpr auto FITTING_RANGE_RATIO = 0.9;

        struct ParResult
        {
            ValueErrorD t_offset;
            ValueErrorD effective_speed;
        };

        auto calculate_toffset_speed(TH1D* histogram, int bar_id) -> ParResult
        {

            auto [cumulative, x_pos] = calculate_CDF_with_quantiles(histogram, FITTING_RANGE_RATIO);

            // First fit to determine the values of the slope and offset
            auto first_fit_res = cumulative->Fit("pol1", "SQC", "", x_pos[0], x_pos[1]);
            if (first_fit_res.Get() == nullptr)
            {
                LOGP(warn, "First linear fitting on time_diff CFD failed for the bar with id {}", bar_id);
                return {};
            }
            const auto slope = first_fit_res->Parameter(1);
            const auto offset = first_fit_res->Parameter(0);

            // Second fit to determine parameters and their errors. The slope and offset values are used for their
            // initial values. The reason why the second fit is needed is because the error of the t_offset value is
            // dependent on both offset and slope, which are not independent. By redefining the fitting parameter, the
            // error of t_offset can be directly given by the fitting algorithm.
            auto fit_fun = TF1{ "fit_fun", "[1]*x + 0.5 - [0] * [1]", x_pos[0], x_pos[1] };
            fit_fun.SetParameter(0, (0.5 - offset) / slope);
            fit_fun.SetParameter(1, slope);
            auto second_fit_res = cumulative->Fit(&fit_fun, "SQC", "", x_pos[0], x_pos[1]);
            if (second_fit_res.Get() == nullptr)
            {
                LOGP(warn, "Second fitting on time_diff CFD failed for the bar with id {}", bar_id);
                return {};
            }

            auto par_result = ParResult();
            par_result.t_offset.value = second_fit_res->Parameter(0);
            par_result.t_offset.error = second_fit_res->Error(0);
            par_result.effective_speed.value = second_fit_res->Parameter(1) * 2 * BarLength;
            par_result.effective_speed.error = second_fit_res->Error(1) * 2 * BarLength;
            return par_result;
        }
    } // namespace

    void HistAnalysis::HistInit(DataMonitor& histograms)
    {
        const auto module_size = GetModuleSize();

        static constexpr auto TIME_DIFF_MAX = 300.; // ns
        static constexpr auto TIME_DIFF_BIN_NUM = 600;

        hist_time_diff_ = histograms.add_hist<TH2D>("hist_time_diff",
                                                    "Time differences between two adjacent PMTs",
                                                    module_size,
                                                    0.5,
                                                    0.5 + module_size,
                                                    TIME_DIFF_BIN_NUM,
                                                    -TIME_DIFF_MAX,
                                                    TIME_DIFF_MAX);

        tsync_engine_.init_hist(histograms);
    }

    void HistAnalysis::Init()
    {
        cal_to_hit_par_ = GetTask()->GetCal2HitPar();
        tsync_engine_.set_number_of_modules(GetModuleSize());
        tsync_engine_.set_max_time_difference(DEFAULT_TSYNC_MAX_TIME_DIFF);
        tsync_engine_.init();
    }

    void HistAnalysis::AddSignals(const std::vector<BarCalData>& signals)
    {
        // all bar signal must have one signal on both sides
        for (const auto& signal : signals)
        {
            if (signal.left.size() != 1 or signal.right.size() != 1)
            {
                continue;
            }
            fill_hist(signal);
        }
    }

    void HistAnalysis::fill_hist(const BarCalData& signal)
    {
        const auto& left_signal = signal.left.front();
        const auto& right_signal = signal.right.front();

        const auto module_num = signal.module_num;
        const auto t_diff = right_signal.leading_time - left_signal.leading_time;
        const auto t_sum = right_signal.leading_time + left_signal.leading_time;

        hist_time_diff_->Fill(static_cast<int>(module_num), t_diff.value);
        tsync_engine_.add_point(t_sum.value, module_num);
        // hist_time_sum_->Fill(static_cast<int>(module_num), t_sum.value);
    }

    auto HistAnalysis::SignalFilter(const std::vector<BarCalData>& signals) -> bool
    {
        // select out rays with few hits
        return signals.size() >= minimum_hit_;
    }

    namespace
    {
        void calibrate_t_diff(TH2D* hist_time_diff, HitModulePar& module_par)
        {
            const auto module_num = module_par.module_num;

            auto* hist_t_diff_py = hist_time_diff->ProjectionY("_py", module_num, module_num);
            const auto par_res = calculate_toffset_speed(hist_t_diff_py, module_num);
            module_par.t_diff = par_res.t_offset;
            module_par.effective_speed = par_res.effective_speed;
        }

        void add_default_module_pars(Cal2HitPar& hit_par, int module_size)
        {
            auto module_pars =
                ranges::views::iota(1, module_size + 1) |
                ranges::views::transform([](int module_num)
                                         { return std::make_pair(module_num, HitModulePar{ module_num }); }) |
                ranges::to<std::unordered_map<int, HitModulePar>>();
            hit_par.SetModulePars(std::move(module_pars));
        }
    } // namespace

    void HistAnalysis::Calibrate(Cal2HitPar& hit_par)
    {
        add_default_module_pars(hit_par, GetModuleSize());
        for (auto& [module_num, module_par] : hit_par.GetListOfModuleParRef())
        {
            calibrate_t_diff(hist_time_diff_, module_par);
        }
        tsync_engine_.calibrate(hit_par);
    }

} // namespace R3B::Neuland::Calibration
