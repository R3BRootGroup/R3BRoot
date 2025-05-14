#include "NeulandParticleFilter.h"
#include "R3BException.h"
#include "R3BNeulandPoint.h"
#include <fmt/core.h>
#include <fmt/ranges.h> // NOLINT
#include <magic_enum/magic_enum.hpp>
#include <range/v3/view/zip.hpp>
#include <string>
#include <utility>

namespace R3B::Neuland
{
    namespace
    {
        inline auto check_if_within_range(double value, std::pair<double, double> min_max) -> bool
        {

            if (min_max.first == min_max.second)
            {
                return true;
            }
            else if (min_max.first < min_max.second)
            {
                return (value > min_max.first) and (value < min_max.second);
            }
            else
            {
                return (value > min_max.first) or (value < min_max.second);
            }
        }
    } // namespace

    auto ParticleFilter::Create(const Options& options) -> ParticleFilter
    {
        if (options.particle_names.size() != options.energy_range_maxs.size())
        {
            throw R3B::logic_error(fmt::format(
                "Number of particle types specified (={}) is not equal to the number of range maximum (={}).",
                options.particle_names.size(),
                options.energy_range_maxs.size()));
        }
        if (options.particle_names.size() != options.energy_range_mins.size())
        {
            throw R3B::logic_error(fmt::format(
                "Number of particle types specified (={}) is not equal to the number of range minimum (={}).",
                options.particle_names.size(),
                options.energy_range_mins.size()));
        }

        auto filter = ParticleFilter{ options.mode };
        filter.SetMaxEnergy(options.global_max_energy);
        filter.SetMinEnergy(options.global_min_energy);

        for (auto [type, range_min, range_max] :
             ranges::views::zip(options.particle_names, options.energy_range_mins, options.energy_range_maxs))
        {
            filter.AddParticle(type, range_min, range_max);
        }

        return filter;
    }

    auto ParticleFilter::IsPointAllowed(const R3BNeulandPoint& neuland_point) -> bool
    {
        if (not check_if_within_range(neuland_point.GetEnergyLoss(),
                                      std::pair{ minimum_allowed_energy_, maximum_allowed_energy_ }))
        {
            return false;
        }

        switch (mode_)
        {
            case Mode::whitelist:
                return is_allowed_with_whitelist(neuland_point);
            case Mode::blacklist:
                return is_allowed_with_blacklist(neuland_point);
            case Mode::pass:
                return true;
        }
        return true;
    }

    void ParticleFilter::AddParticle(const std::string& particle_name, double min_energy, double max_energy)
    {
        const auto pid = pdg_converter_.get_pid(particle_name);
        if (list_.find(pid) == list_.end())
        {
            list_.emplace(pid, std::pair{ min_energy, max_energy });
        }
        else
        {
            list_.at(pid) = std::pair{ min_energy, max_energy };
        }
    }

    auto ParticleFilter::is_allowed_with_whitelist(const R3BNeulandPoint& neuland_point) -> bool
    {
        const auto particle_pid = neuland_point.GetPID();
        if (list_.find(particle_pid) != list_.end())
        {
            return check_if_within_range(neuland_point.GetEnergyLoss(), list_.at(particle_pid));
        }
        return false;
    }

    auto ParticleFilter::is_allowed_with_blacklist(const R3BNeulandPoint& neuland_point) -> bool
    {
        const auto particle_pid = neuland_point.GetPID();
        if (list_.find(particle_pid) != list_.end())
        {
            return not check_if_within_range(neuland_point.GetEnergyLoss(), list_.at(particle_pid));
        }
        return true;
    }

    auto ParticleFilter::Print() -> std::string
    {
        return fmt::format("Filter Content: \n"
                           "=============================\n"
                           "-- Mode: {}\n"
                           "-- Global minimun energy:{}\n"
                           "-- Global maximum energy: {}\n"
                           "-- particles: {}\n"
                           "=============================",
                           magic_enum::enum_name(mode_),
                           minimum_allowed_energy_,
                           maximum_allowed_energy_,
                           list_);
    }

    void ParticleFilter::Reset()
    {
        mode_ = Mode::pass;
        minimum_allowed_energy_ = 0.;
        maximum_allowed_energy_ = 0.;
        list_.clear();
    }
} // namespace R3B::Neuland
