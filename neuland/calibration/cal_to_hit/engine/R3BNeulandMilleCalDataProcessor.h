#pragma once

#include "R3BHuberRegression.h"
#include "R3BValueError.h"
#include <Fit/Fitter.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCommon.h>
#include <Rtypes.h>
#include <TF1.h>
#include <cstdlib>
#include <fmt/base.h>
#include <fmt/core.h>
#include <unordered_map>
#include <vector>

namespace R3B::Neuland
{
    class Cal2HitPar;

}

namespace R3B::Neuland::Calibration
{

    /**
     * @class MilleCalData
     * @brief Data structure to store the cal level data for the data preprocessing.
     *
     * The major difference between the struct and the BarCalData is that MilleCalData only contains the bar data with
     * one hit whereas the BarCalData may contain multiple hits.
     */
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
        bool is_outlier = false;
        int module_num = 0;   //!< 1 based bar num
        float residual = 0.;  //!< residual value against the fitted line
        ValueErrorD position; //!< position of the hit along the bar direction
        CalDataSignal left;
        CalDataSignal right;

        ClassDefNV(MilleCalData, 1);
    };

    struct MilleFitPar
    {
        double offset = 0.;
        double slope = 0.;
        double p_value = 0.;

        static constexpr auto diff_threshold = 0.1; //!< 10% difference

        auto operator!=(const MilleFitPar& other) const
        {
            return (std::abs(other.offset - offset) / offset > diff_threshold) or
                   (std::abs(other.slope - slope) / slope > diff_threshold);
        }
        void clear()
        {
            offset = 0.;
            slope = 0.;
            p_value = 0.;
        }
        ClassDefNV(MilleFitPar, 1);
    };

    struct MilleFitResult
    {
        MilleFitPar x_z;
        MilleFitPar y_z;
        void clear()
        {
            x_z.clear();
            y_z.clear();
        }
        ClassDefNV(MilleFitResult, 1);
    };

    struct MilleTrackInfo
    {
        MilleFitResult time_data;
        MilleFitResult bar_disp_data;
        void clear()
        {
            time_data.clear();
            bar_disp_data.clear();
        }
        ClassDefNV(MilleTrackInfo, 1);
    };

    class MilleDataProcessor
    {
      public:
        using FitPar = MilleFitPar;
        using FitResult = MilleFitResult;
        using TrackInfo = MilleTrackInfo;
        struct FitDataSet
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

        struct TrackFitDataSet
        {
            FitDataSet bar_x_z;
            FitDataSet bar_y_z;
            FitDataSet time_data_x_z;
            FitDataSet time_data_y_z;

            void clear()
            {
                bar_x_z.clear();
                bar_y_z.clear();
                time_data_x_z.clear();
                time_data_y_z.clear();
            }
        };

        explicit MilleDataProcessor(int num_of_modules);

        /**
         * @brief Process the cal level data.
         *
         * This is the major public interface to process data input cal level data.
         *
         * It has following actions (in order):
         * - Emplace the cal level data (in type BarCalData) to the data buffer of MilleCalData if the BarCalData only
         * contains a single hit.
         * - Remove the isolated bar signals in each plane. (see @ref remove_isolated_bar_signal())
         * -
         *
         * @param name description
         * @return name description
         * @see ref
         */
        auto process(const std::vector<BarCalData>& signals, const Cal2HitPar& hit_par) -> bool;

        auto check_is_outlier(int module_num) const -> bool;

        void reset();

        void set_p_value_cut(double val) { p_value_cut_ = val; }

        [[nodiscard]] auto get_data() const -> const auto& { return data_buffers_; }
        [[nodiscard]] auto get_track_info() const -> const auto& { return track_info_; }
        [[nodiscard]] auto get_track_fit_data() const -> const auto& { return track_fit_data_; }
        // [[nodiscard]] auto calculate_residual(double val, int module_num) const -> float;

      private:
        double p_value_cut_ = DEFAULT_CALIBRATION_P_VALUE_CUT;

        /**
         * Data buffer for the event data. The buffer is reset in the end of the event.
         */
        std::unordered_map<int, std::vector<MilleCalData>> data_buffers_;
        TrackInfo track_info_;
        TrackFitDataSet track_fit_data_;
        HuberRegressor huber_regressor_;

        ROOT::Fit::Fitter fitter_;
        TF1 fit_function_{ "mille_fitting", "[0] * x + [1]" };

        void init_data_registers(int num_of_modules);

        /**
         * @brief Remove signals that are isolated in the plane.
         */
        void remove_isolated_bar_signal();
        void fill_bar_disp_dataset();
        void fill_time_dataset();
        void reset_fit_pars();
        auto fit_planes(const Cal2HitPar& hit_par) -> bool;
        auto linear_fit(const FitDataSet& data, FitPar& fit_par) -> bool;
        void check_fit_result();
    };
} // namespace R3B::Neuland::Calibration

#ifndef __CLING__
template <>
class fmt::formatter<R3B::Neuland::Calibration::MilleCalData>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::Calibration::MilleCalData& signal, FmtContent& ctn) const
    {
        return fmt::format_to(
            ctn.out(), "ModuleNum: {}, left bar: {}, right bar: {}", signal.module_num, signal.left, signal.right);
    }
};

template <>
class fmt::formatter<R3B::Neuland::Calibration::MilleDataProcessor::FitPar>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::Calibration::MilleDataProcessor::FitPar& fit_par, FmtContent& ctn) const
    {
        return fmt::format_to(
            ctn.out(), "slope: {}, offset: {}, p_value: {}", fit_par.slope, fit_par.offset, fit_par.p_value);
    }
};

template <>
class fmt::formatter<R3B::Neuland::Calibration::MilleDataProcessor::FitResult>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::Calibration::MilleDataProcessor::FitResult& fit_result,
                          FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "\n"
                              "\tx_z plane: {{{}}}\n"
                              "\ty_z plane: {{{}}}",
                              fit_result.x_z,
                              fit_result.y_z);
    }
};

template <>
class fmt::formatter<R3B::Neuland::Calibration::MilleDataProcessor::FitDataSet>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::Calibration::MilleDataProcessor::FitDataSet& fit_data,
                          FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "\tx = np.array({})\n"
                              "\tx_err = np.array({})\n"
                              "\ty = np.array({})\n"
                              "\ty_err = np.array({})",
                              fit_data.z_vals,
                              fit_data.z_errs,
                              fit_data.vals,
                              fit_data.errs);
    }
};
#endif
