#include "R3BNeulandPredecessor.h"
#include <R3BNeulandCalToHitParTask.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

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
            histogram->Scale(1 / histogram->GetEntries());
            auto* cumulative = histogram->GetCumulative();
            constexpr auto QUANTILES_NUM = 2;
            const auto quantiles =
                std::array<double, QUANTILES_NUM>{ 0.5 - (FITTING_RANGE_RATIO / 2.), 0.5 + (FITTING_RANGE_RATIO / 2.) };
            auto x_pos = std::array<double, QUANTILES_NUM>{};
            histogram->GetQuantiles(QUANTILES_NUM, x_pos.data(), quantiles.data());

            // First fit to determine the values of the slope and offset
            auto first_fit_res = cumulative->Fit("pol1", "SQ", "", x_pos[0], x_pos[1]);
            if (first_fit_res.Get() == nullptr)
            {
                R3BLOG(warn,
                       fmt::format("First linear fitting on time_diff CFD failed for the bar with id {}", bar_id));
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
            auto second_fit_res = cumulative->Fit(&fit_fun, "SQ", "", x_pos[0], x_pos[1]);
            if (second_fit_res.Get() == nullptr)
            {
                R3BLOG(warn, fmt::format("Second fitting on time_diff CFD failed for the bar with id {}", bar_id));
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

    void Predecessor::HistInit(DataMonitor& histograms)
    {
        const auto module_size = GetModuleSize();

        static constexpr auto TIME_DIFF_MAX = 100.; // ns
        static constexpr auto TIME_DIFF_BIN_NUM = 500;
        // static constexpr auto TIME_SUM_MAX = 800; // ns
        // static constexpr auto TIME_SUM_BIN_NUM = 400;

        hist_time_diff_ = histograms.add_hist<TH2D>("hist_time_diff",
                                                    "Time differences between two adjacent PMTs",
                                                    module_size,
                                                    0.5,
                                                    0.5 + module_size,
                                                    TIME_DIFF_BIN_NUM,
                                                    -TIME_DIFF_MAX,
                                                    TIME_DIFF_MAX);

        // hist_time_sum_ = histograms.add_hist<TH2D>("hist_time_sum",
        //                                            "Time summation between two adjacent PMTs",
        //                                            module_size,
        //                                            0.5,
        //                                            0.5 + module_size,
        //                                            TIME_SUM_BIN_NUM,
        //                                            0.,
        //                                            TIME_SUM_MAX + 0.);
    }

    void Predecessor::Init() { cal_to_hit_par_ = GetTask()->GetCal2HitPar(); }

    void Predecessor::AddSignal(const BarCalData& signal)
    {
        // all bar signal must have one signal on both sides
        if (signal.left.size() != 1 or signal.right.size() != 1)
        {
            return;
        }
        fill_hist(signal);
    }

    void Predecessor::fill_hist(const BarCalData& signal)
    {
        const auto& left_signal = signal.left.front();
        const auto& right_signal = signal.right.front();

        const auto module_num = signal.module_num;
        const auto t_diff = right_signal.leading_time - left_signal.leading_time;
        // const auto t_sum = right_signal.leading_time + left_signal.leading_time;

        hist_time_diff_->Fill(static_cast<int>(module_num), t_diff.value);
        // hist_time_sum_->Fill(static_cast<int>(module_num), t_sum.value);
    }

    auto Predecessor::SignalFilter(const std::vector<BarCalData>& signals) -> bool
    {
        // select out rays with few hits
        return signals.size() >= minimum_hit_;
    }

    void Predecessor::Calibrate(Cal2HitPar& hit_par)
    {
        const auto module_size = GetModuleSize();

        for (int bar_id{}; bar_id < module_size; ++bar_id)
        {
            auto* hist_t_diff_py = hist_time_diff_->ProjectionY("_py", bar_id + 1, bar_id + 1);
            const auto par_res = calculate_toffset_speed(hist_t_diff_py, bar_id);

            auto module_par = HitModulePar{};
            module_par.module_num = bar_id + 1;
            module_par.t_diff = par_res.t_offset;
            module_par.effective_speed = par_res.effective_speed;

            hit_par.AddModulePar(module_par);
        }
    }
} // namespace R3B::Neuland::Calibration
