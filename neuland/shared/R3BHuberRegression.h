#pragma once

#include "R3BValueError.h"
#include <Math/Factory.h>
#include <Math/Functor.h>
#include <Math/GSLMinimizer.h>
#include <Math/Minimizer.h>
#include <Minuit2/Minuit2Minimizer.h>
#include <cstddef>
#include <fmt/base.h>
#include <memory>
#include <span>
#include <utility>
#include <vector>

namespace R3B
{
    struct HuberRegressorConfig
    {
        static constexpr auto DEFAULT_TOLERANCE = 0.01;
        static constexpr auto DEFAULT_EPSILON = 1.35;
        static constexpr auto DEFAULT_REG_RATE = 0.0001;

        struct ParConfig
        {
            ParConfig() = default;
            double init = 0.;
            double learning_rate = 1.;
        };
        HuberRegressorConfig() = default;

        double tolerance = DEFAULT_TOLERANCE;
        double epsilon = DEFAULT_EPSILON;
        double regularization_rate = DEFAULT_REG_RATE;
        ParConfig weight;
        ParConfig bias;
    };

    class HuberRegressor
    {
      public:
        struct Result
        {
            bool is_success = false;
            int iteration = 0;
            std::size_t n_data = 0;
            R3B::ValueErrorD weight;
            R3B::ValueErrorD bias;
        };
        using Config = HuberRegressorConfig;

        explicit HuberRegressor(
            Config config = Config{},
            std::unique_ptr<ROOT::Math::Minimizer> minimizer = std::unique_ptr<ROOT::Math::Minimizer>{
                ROOT::Math::Factory::CreateMinimizer("Minuit2") });

        auto train_from_data(const std::vector<double>& x_vals, const std::vector<double>& y_vals) -> bool;
        auto calculate_p_value(const std::vector<double>& y_errs, double scale = 1.) -> double;
        void reset_parameters();

        auto get_config_ref() -> Config& { return config_; }
        [[nodiscard]] auto get_result() const -> const Result& { return result_; }
        [[nodiscard]] auto check_outlier(double x_val, double y_val) const -> std::pair<bool, double>;
        [[nodiscard]] auto check_outlier(double x_val, double y_val, const std::pair<double, double>& weight_bias) const
            -> std::pair<bool, double>;

      private:
        static constexpr auto n_pars = 2;
        double sigma_ = 20.;
        Config config_;
        Result result_;
        std::span<const double> x_vals_;
        std::span<const double> y_vals_;
        std::unique_ptr<ROOT::Math::Minimizer> minimizer_;

        auto calculate_loss(const double* raw_pars) -> double;
        void set_par_values(const double* raw_pars_ptr);
        void set_par_errors(const double* raw_error_ptr);
    };

} // namespace R3B

#ifndef __CLING__

template <>
class fmt::formatter<R3B::HuberRegressor::Result>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::HuberRegressor::Result& result, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "{{iteration: {}, weight: {}, bias: {}, is_success: {}}}",
                              result.iteration,
                              result.weight,
                              result.bias,
                              result.is_success);
    }
};
#endif
