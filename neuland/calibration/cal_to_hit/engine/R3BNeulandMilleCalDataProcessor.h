#pragma once

#include <Fit/Fitter.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCommon.h>
#include <TF1.h>
#include <fmt/core.h>
#include <unordered_map>
#include <vector>

namespace R3B::Neuland::Calibration
{
    struct MilleCalData
    {
      public:
        MilleCalData() = default;
        explicit MilleCalData(const BarCalData& bar_cal_data)
            : module_num{ bar_cal_data.module_num }
            , left{ bar_cal_data.left.front() }
            , right{ bar_cal_data.right.front() }
        {
        }
        unsigned int module_num = 0; // 1 based bar num
        CalDataSignal left;
        CalDataSignal right;
    };

    class MilleDataProcessor
    {
      public:
        struct FitPar
        {
            double offset = 0.;
            double slope = 0.;
        };
        struct FitResult
        {
            FitPar x_z;
            FitPar y_z;
        };
        struct FitData
        {
            std::vector<double> z_vals;
            std::vector<double> z_errs;
            std::vector<double> vals;
            std::vector<double> errs;

            void clear()
            {
                z_vals.clear();
                z_errs.clear();
                vals.clear();
                errs.clear();
            }
            [[nodiscard]] auto size() const { return z_vals.size(); }
        };

        explicit MilleDataProcessor(int num_of_modules);
        auto filter(const std::vector<BarCalData>& signals) -> bool;
        [[nodiscard]] auto get_data() const -> const auto& { return data_regsiters_; }
        [[nodiscard]] auto get_fit() const -> const auto& { return fit_result_; }
        [[nodiscard]] auto calculate_residual(double val, int module_num) const -> double;
        void reset();

        void set_p_value_cut(double val) { p_value_cut_ = val; }

      private:
        double p_value_cut_ = DEFAULT_CALIBRATION_P_VALUE_CUT;
        std::unordered_map<int, std::vector<MilleCalData>> data_regsiters_;
        FitResult fit_result_;
        FitData x_z_vals_;
        FitData y_z_vals_;

        ROOT::Fit::Fitter fitter_;
        TF1 fit_function_{ "mille_fitting", "[0] * x + [1]" };

        void init_data_registers(int num_of_modules);
        void remove_isolated_bar_signal();
        void fill_fit_data();
        void reset_fitpars();
        auto fit_planes() -> bool;
        auto fit_plane_data() -> bool;
        auto linear_fit(const FitData& data, FitPar& fit_par) -> bool;
    };
} // namespace R3B::Neuland::Calibration

template <>
class fmt::formatter<R3B::Neuland::Calibration::MilleCalData>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::Calibration::MilleCalData& signal, FmtContent& ctn) const
    {
        return format_to(
            ctn.out(), "ModuleNum: {}, left bar: {}, right bar: {}", signal.module_num, signal.left, signal.right);
    }
};
