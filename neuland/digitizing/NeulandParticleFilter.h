#pragma once
#include "R3BNeulandPoint.h"
#include <R3BPDGConverter.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace R3B::Neuland
{
    class ParticleFilter
    {
      public:
        /** @enum Mode
         *
         * @brief A enum class for the point filtering mode
         *
         * There are two modes:
         * - whitelist: all particles are in the blacklist and filtered out by default. AddParticle() function will add
         * the particle to the whitelist with a specified energy range.
         * - blacklist: all particles are in the whitelist and not filtered out by default. AddParticle() function will
         * - pass: No action. Everything is not filtered.
         * add the particle to the blacklist with a specified energy range.
         */
        enum class Mode : uint8_t
        {
            whitelist,
            blacklist,
            pass,
        };

        /** @class Options
         * @brief Option structure to create a filter
         *
         * Input configuration structure which can be used to create a filter
         *
         * The three vectors, #particle_types, #energy_range_mins and #energy_range_maxs have have the same size. The
         * min and max value of the energy range are corresponding to the particle type of the smame index number.
         *
         * @see Create()
         */
        struct Options
        {
            Mode mode = Mode::pass;                  //!< @see SetMode()
            double global_min_energy = 0.;           //!< @see SetMinEnergy()
            double global_max_energy = 0.;           //!< @see SetMaxEnergy()
            std::vector<std::string> particle_names; //!< @see AddParticle()
            std::vector<double> energy_range_mins;   //!< @see AddParticle()
            std::vector<double> energy_range_maxs;   //!< @see AddParticle()
        };

        /**
         * @brief Default constructor
         */
        ParticleFilter() = default;

        /**
         * @brief constructor with mode
         *
         * @param mode Filter mode
         */
        explicit ParticleFilter(Mode mode)
            : mode_{ mode }
        {
        }

        /**
         * @brief Generator for the filter object from the configuration
         *
         * Generating a filter using configurations specifed in the data structure.
         * @param options Configurations
         * @return A filter object
         */
        static auto Create(const Options& options) -> ParticleFilter;

        /**
         * @brief Add particle to the list
         *
         * Add the particle with the specified energy range. If the mode of the filter is Mode::blacklist, points whose
         * energies are within the range are filtered out. If the mode of the filter is Mode::whitelist, points whose
         * energyies are within the range are allowed.
         *
         * If the min_energy is smaller than max_energy, the energy range will be [min_energy, max_energy]. If the
         * min_energy is larger than max_energy, the range will be [0., max_energy] and [min_energy, inf).
         * @param particle_name Particle name
         * @param min_energy Energy half open range of [min_energy, inf) in the unit of [GeV]
         * @param max_energy Energy closed range of [0., max_energy] in the unit of [GeV]
         */
        void AddParticle(const std::string& particle_name, double min_energy = 0., double max_energy = 0.);

        /**
         * @brief Set the global energy range with a minimum value
         *
         * Set the global minimum energy range with [energy, inf).
         * @param energy Energy value to be set
         */
        void SetMinEnergy(double energy) { minimum_allowed_energy_ = energy; }

        /**
         * @brief Set the global energy range with a maximum value
         *
         * Set the global minimum energy range with [0., energy].
         * @param energy Energy value to be set
         */
        void SetMaxEnergy(double energy) { maximum_allowed_energy_ = energy; }

        /**
         * @brief Set the mode of the fileter
         */
        void SetMode(Mode mode) { mode_ = mode; }

        /**
         * @brief Get the global minimum energy threshold
         *
         * @return Global minimum energy threshold.
         */
        [[nodiscard]] auto GetMinEnergy() const -> double { return minimum_allowed_energy_; }

        /**
         * @brief Get the global max energy threshold
         *
         * @return Global minimum energy threshold.
         */
        [[nodiscard]] auto GetMaxEnergy() const -> double { return maximum_allowed_energy_; }

        /**
         * @brief Get the mode of the filter
         *
         * @return Mode of the filter.
         */
        [[nodiscard]] auto GetMode() const -> Mode { return mode_; }

        /**
         * @brief Check if the neuland point is allowed.
         *
         * @param neuland_point Neuland point data
         * @return false if the particle should be filtered and true if the particle is allowed.
         */
        auto IsPointAllowed(const R3BNeulandPoint& neuland_point) -> bool;

        /**
         * @brief Reset
         *
         * Reset the mode to the Mode::pass, global minimum energy to 0. GeV and clear the list_.
         */
        void Reset();

        /**
         * @brief Print the configuration of the current filter
         *
         * Print out the configurations specified by the Options data structure.
         * @return Multiline strings
         */
        auto Print() -> std::string;

        // void SetFilter(R3B::Neuland::BitSetParticle filtered_particles);
        // void SetFilter(R3B::Neuland::BitSetParticle filtered_particles, double minimum_allowed_energy);
        // [[nodiscard]] auto GetFilter() const -> R3B::Neuland::BitSetParticle { return filtered_particles_; }

      private:
        Mode mode_ = Mode::pass;                                      //!< Filtering mode
        double minimum_allowed_energy_ = 0.;                          //!< global minimum engergy in GeV
        double maximum_allowed_energy_ = 0.;                          //!< global maximum engergy in GeV
        std::unordered_map<int32_t, std::pair<double, double>> list_; //!< Whitelist with min energy values [GeV]
        PDGConverter pdg_converter_;
        // R3B::Neuland::BitSetParticle filtered_particles_ = R3B::Neuland::BitSetParticle::none;
        auto is_allowed_with_whitelist(const R3BNeulandPoint& neuland_point) -> bool;
        auto is_allowed_with_blacklist(const R3BNeulandPoint& neuland_point) -> bool;
    };

} // namespace R3B::Neuland
