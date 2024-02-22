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

#pragma once

#define EIGEN_HAS_STD_RESULT_OF 0

#include "MilleEntry.h"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iostream>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

namespace rng = ranges;

namespace R3B
{

    template <int NGlobal, int NLocal>
    class RankChecker
    {
      public:
        using GlobalMatrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
        using LocalMatrix = Eigen::Matrix<float, NLocal, NLocal>;
        using OffDiagonalMatrix = Eigen::Matrix<float, NLocal, Eigen::Dynamic>;
        RankChecker() = default;
        void add_entry(const MilleDataPoint& input_point)
        {
            if (not input_point.locals.empty())
            {
                local_matrix_ += calculate_local_matrix(input_point.locals);
                off_diagonal_matrix_ += calculate_off_diagonal(input_point.locals, input_point.globals);
            }
            update_global_matrix_first(input_point.globals);
        }

        void reset()
        {
            local_matrix_.setZero();
            off_diagonal_matrix_.setZero();
        }

        void calculate()
        {
            update_global_matrix_second();
            coefficients_ = coefficients_first_ - coefficients_second_;
        }

        auto get_decomp() { return Eigen::FullPivLU<decltype(coefficients_)>(coefficients_); }

      private:
        GlobalMatrix coefficients_ = GlobalMatrix::Zero(NGlobal, NGlobal);
        GlobalMatrix coefficients_first_ = GlobalMatrix::Zero(NGlobal, NGlobal);
        GlobalMatrix coefficients_second_ = GlobalMatrix::Zero(NGlobal, NGlobal);

        LocalMatrix local_matrix_;
        OffDiagonalMatrix off_diagonal_matrix_ = GlobalMatrix::Zero(NLocal, NGlobal);

        auto calculate_local_matrix(const std::vector<float>& local_derivs) -> Eigen::Matrix<float, NLocal, NLocal>
        {
            auto local_derivs_matrix = Eigen::Matrix<float, NLocal, NLocal>{};
            local_derivs_matrix.setZero();

            if (NLocal != local_derivs.size())
            {
                throw std::logic_error(
                    fmt::format("input local dim {} is out of bound (N = {})!", local_derivs.size(), NLocal));
            }

            for (int row_index{ 0 }; row_index < NLocal; ++row_index)
            {
                for (int column_index{ 0 }; column_index < NLocal; ++column_index)
                {
                    local_derivs_matrix(row_index, column_index) = local_derivs[row_index] * local_derivs[column_index];
                }
            }
            // std::cout << "local deriv matrix: \n" << local_derivs_matrix << "\n";

            return local_derivs_matrix;
        }

        auto calculate_off_diagonal(const std::vector<float>& local_derivs,
                                    const std::vector<std::pair<int, float>>& global_derivs)
            -> Eigen::Matrix<float, NLocal, Eigen::Dynamic>
        {
            auto off_diagonal_matrix = Eigen::Matrix<float, NLocal, NGlobal>{};
            off_diagonal_matrix.setZero();

            for (int row_index{ 0 }; row_index < NLocal; ++row_index)
            {
                for (const auto& [column_id, global_deriv_column] : global_derivs)
                {
                    off_diagonal_matrix(row_index, column_id - 1) = local_derivs[row_index] * global_deriv_column;
                }
            }

            return off_diagonal_matrix;
        }

        void update_global_matrix_second()
        {
            if (local_matrix_.isZero() or off_diagonal_matrix_.isZero())
            {
                return;
            }

            const auto new_global_matrix_second =
                off_diagonal_matrix_.transpose() * local_matrix_.inverse() * off_diagonal_matrix_;
            coefficients_second_ += new_global_matrix_second;

            // Eigen::FullPivLU<LocalMatrix> lu_decomp(local_matrix_);
            // lu_decomp.setThreshold(0.001);
            // std::cout << "local: " << local_matrix_ << "\n";
            // std::cout << "local rank: " << lu_decomp.rank() << "\n";
            // std::cout << "local image: " << lu_decomp.image(local_matrix_) << "\n";
            // std::cout << "inverse: " << local_matrix_.inverse() << "\n";
        }

        void update_global_matrix_first(const std::vector<std::pair<int, float>>& global_derivs)
        {
            if (global_derivs.empty())
            {
                return;
            }

            auto max_id = rng::max(global_derivs | rng::views::keys);

            if (max_id > NGlobal)
            {
                throw std::logic_error(fmt::format("Global index {} out of bound (N={})!", max_id, NGlobal));
            }

            for (const auto& [row_id, global_deriv_row] : global_derivs)
            {
                for (const auto& [column_id, global_deriv_column] : global_derivs)
                {
                    coefficients_first_(row_id - 1, column_id - 1) += global_deriv_row * global_deriv_column;
                }
            }
        }
    };
} // namespace R3B
