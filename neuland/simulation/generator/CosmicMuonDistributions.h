#pragma once
#include <Math/GenVector/Cartesian3D.h>
#include <R3BNeulandCommon.h>
#include <TRandom.h>
#include <stdexcept>

namespace R3B::Neuland
{
    class EnergyDist
    {
      public:
        void set_mean_sigma(double mean, double sigma)
        {
            mean_ = mean;
            sigma_ = sigma;
        }

        auto operator()(TRandom* rd_engine_) const -> double
        {
            auto energy = rd_engine_->Gaus(mean_, sigma_);
            return energy;
        };

      private:
        double mean_{ 300. };
        double sigma_{ 2. };
    };

    class AngleDist
    {
      public:
        double operator()(TRandom* rd_engine_) const
        {
            const auto n_steps = int{ 10000 };
            const auto step_size = 0.1;

            auto target_distribution = [](double x) -> double { return std::pow(std::cos(x), 2.); };

            double current_angle = 0.0;
            double new_angle;

            for (int i = 0; i < n_steps; ++i)
            {
                new_angle = rd_engine_->Gaus(current_angle, step_size);

                if (new_angle < -M_PI_2 || new_angle > M_PI_2)
                {
                    continue;
                }

                double acceptance_ratio = target_distribution(new_angle) / target_distribution(current_angle);

                if (rd_engine_->Uniform(0.0, 1.0) < acceptance_ratio)
                {
                    current_angle = new_angle;
                }
            }

            return current_angle;
        }
    };

    struct DetectorBoxSize
    {
      public:
        double xmin{};
        double xmax{};
        double ymin{};
        double ymax{};
        double zmin{};
        double zmax{};
    };

    class PositionDist
    {
      public:
        void set_box_size(DetectorBoxSize detector_box_size)
        {
            detector_box_size_.xmin = detector_box_size.xmin;
            detector_box_size_.xmax = detector_box_size.xmax;
            detector_box_size_.ymin = detector_box_size.ymin;
            detector_box_size_.ymax = detector_box_size.ymax;
            detector_box_size_.zmin = detector_box_size.zmin;
            detector_box_size_.zmax = detector_box_size.zmax;
        }

        auto operator()(TRandom* rd_engine_) const -> ROOT::Math::Cartesian3D<double>
        {
            auto box_positions = ROOT::Math::Cartesian3D<double>{};
            if (rd_engine_ != nullptr)
            {
                box_positions.SetX(rd_engine_->Uniform(detector_box_size_.xmin, detector_box_size_.xmax));
                box_positions.SetY(rd_engine_->Uniform(detector_box_size_.ymin, detector_box_size_.ymax));
                box_positions.SetZ(rd_engine_->Uniform(detector_box_size_.zmin, detector_box_size_.zmax));
                return box_positions;
            }
            throw std::runtime_error("rnd engine is nullptr!");
        }

      private:
        DetectorBoxSize detector_box_size_{};
    };
} // namespace R3B::Neuland
