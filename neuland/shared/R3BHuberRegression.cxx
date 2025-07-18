#include "R3BHuberRegression.h"
#include <Math/Functor.h>
#include <Math/Minimizer.h>
#include <Math/ProbFuncMathCore.h>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <memory>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace R3B
{
    HuberRegressor::HuberRegressor(Config config, std::unique_ptr<ROOT::Math::Minimizer> minimizer)
        : config_{ config }
        , minimizer_{ std::move(minimizer) }
    {
        minimizer_->SetTolerance(config.tolerance);
        minimizer_->SetPrintLevel(-1);
        reset_parameters();
    }

    auto HuberRegressor::train_from_data(const std::vector<double>& x_vals, const std::vector<double>& y_vals) -> bool
    {
        // auto loss_functor =
        auto functor = ROOT::Math::Functor{ [&](const double* raw_pars) -> double { return calculate_loss(raw_pars); },
                                            HuberRegressor::n_pars };
        minimizer_->SetFunction(functor);
        x_vals_ = std::span{ x_vals };
        y_vals_ = std::span{ y_vals };
        auto is_ok = minimizer_->Minimize();

        const auto errors = std::span(minimizer_->Errors(), HuberRegressor::n_pars);
        set_par_values(minimizer_->X());
        set_par_errors(minimizer_->Errors());
        result_.iteration = static_cast<int>(minimizer_->NIterations());

        if (not is_ok)
        {
            result_.is_success = false;
            return false;
        }

        result_.is_success = true;
        return true;
    }

    void HuberRegressor::set_par_values(const double* raw_pars_ptr)
    {
        const auto vals = std::span(raw_pars_ptr, HuberRegressor::n_pars);
        result_.weight.value = vals[0];
        result_.bias.value = vals[1];
        result_.sigma.value = vals[2];
    }

    void HuberRegressor::set_par_errors(const double* raw_error_ptr)
    {
        const auto vals = std::span(raw_error_ptr, HuberRegressor::n_pars);
        result_.weight.error = vals[0];
        result_.bias.error = vals[1];
        result_.sigma.error = vals[2];
    }

    void HuberRegressor::reset_parameters()
    {
        minimizer_->SetVariable(0, "weight", config_.weight.init, config_.weight.learning_rate);
        minimizer_->SetVariable(1, "bias", config_.bias.init, config_.bias.learning_rate);
        minimizer_->SetVariable(2, "sigma", config_.sigma.init, config_.sigma.learning_rate);
    }

    auto HuberRegressor::calculate_p_value(const std::vector<double>& y_errs, double scale) -> double
    {
        auto chi_square_view = std::views::zip_transform(
            [&](auto x_val, auto y_val, auto y_err)
            {
                auto [is_outlier, residual] = check_outlier(x_val, y_val);

                if (is_outlier)
                {
                    return 0.;
                }
                return residual * residual / y_err / scale;
            },
            x_vals_,
            y_vals_,
            y_errs);
        const auto chi_square = std::ranges::fold_left(chi_square_view, 0., std::plus{});
        return ROOT::Math::chisquared_cdf_c(chi_square, static_cast<double>(y_errs.size() - 1));
    }

    auto HuberRegressor::check_outlier(double x_val,
                                       double y_val,
                                       const std::pair<double, double>& weight_bias) const -> std::pair<bool, double>
    {
        const auto residual = std::abs(y_val - (x_val * weight_bias.first) - weight_bias.second);
        const auto thresh = std::abs(result_.sigma.value * config_.epsilon);
        return std::make_pair(residual > thresh, residual);
    }

    auto HuberRegressor::check_outlier(double x_val, double y_val) const -> std::pair<bool, double>
    {
        return check_outlier(x_val, y_val, std::make_pair(result_.weight.value, result_.bias.value));
    }

    auto HuberRegressor::calculate_loss(const double* raw_pars) -> double
    {
        set_par_values(raw_pars);
        const auto weight = result_.weight.value;
        const auto bias = result_.bias.value;
        const auto sigma = result_.sigma.value;

        const auto n_data = static_cast<int>(x_vals_.size());
        auto n_outliers = 0;

        auto loss = std::ranges::fold_left(std::views::zip_transform(
                                               [&](auto x_val, auto y_val)
                                               {
                                                   const auto [is_outlier, residual] = check_outlier(x_val, y_val);
                                                   if (is_outlier)
                                                   {
                                                       ++n_outliers;
                                                       return 2. * config_.epsilon * residual;
                                                   }
                                                   return residual * residual / sigma;
                                               },
                                               x_vals_,
                                               y_vals_),
                                           (config_.regularization_rate * weight * weight),
                                           std::plus{});
        loss += n_data * sigma - sigma * n_outliers * config_.epsilon * config_.epsilon;
        return loss;
    }
} // namespace R3B
