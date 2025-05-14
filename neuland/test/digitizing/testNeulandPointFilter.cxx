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

#include "NeulandParticleFilter.h"
#include "R3BNeulandPoint.h"
#include <gtest/gtest.h>

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
namespace
{
    constexpr auto proton_pid = 2212;
    constexpr auto neutron_pid = 2112;
    constexpr auto electron_pid = 11;
    using R3B::Neuland::ParticleFilter;

    TEST(NeulandPointFilterTest, test_setters_getters)
    {
        auto filter = ParticleFilter{};
        EXPECT_EQ(filter.GetMode(), ParticleFilter::Mode::pass);
        EXPECT_EQ(filter.GetMinEnergy(), 0.);
        EXPECT_EQ(filter.GetMaxEnergy(), 0.);

        filter.SetMode(ParticleFilter::Mode::whitelist);
        EXPECT_EQ(filter.GetMode(), ParticleFilter::Mode::whitelist);

        filter.SetMaxEnergy(1.);
        EXPECT_EQ(filter.GetMaxEnergy(), 1.);

        filter.SetMinEnergy(1.);
        EXPECT_EQ(filter.GetMinEnergy(), 1.);

        filter.Reset();
        EXPECT_EQ(filter.GetMode(), ParticleFilter::Mode::pass);
        EXPECT_EQ(filter.GetMinEnergy(), 0.);
        EXPECT_EQ(filter.GetMaxEnergy(), 0.);
    }

    TEST(NeulandPointFilterTest, test_global_closed_range)
    {
        auto point = R3BNeulandPoint{};
        point.SetParticleId(proton_pid);
        constexpr auto test_energy_gev = 0.7;
        point.SetEnergyLoss(test_energy_gev);

        auto filter = R3B::Neuland::ParticleFilter{};
        auto test_min_energy = 0.6;
        auto test_max_energy = 0.9;
        filter.SetMinEnergy(test_min_energy);
        filter.SetMaxEnergy(test_max_energy);

        EXPECT_TRUE(filter.IsPointAllowed(point));

        test_min_energy = 0.8;
        test_max_energy = 0.9;
        filter.SetMinEnergy(test_min_energy);
        filter.SetMaxEnergy(test_max_energy);

        EXPECT_FALSE(filter.IsPointAllowed(point));
    }

    TEST(NeulandPointFilterTest, test_global_open_range)
    {
        auto point = R3BNeulandPoint{};
        point.SetParticleId(proton_pid);
        auto test_energy_gev = 0.7;
        point.SetEnergyLoss(test_energy_gev);

        auto filter = R3B::Neuland::ParticleFilter{};
        auto test_min_energy = 0.6;
        auto test_max_energy = 0.2;
        filter.SetMinEnergy(test_min_energy);
        filter.SetMaxEnergy(test_max_energy);

        EXPECT_TRUE(filter.IsPointAllowed(point));

        test_energy_gev = 0.1;
        point.SetEnergyLoss(test_energy_gev);

        filter.SetMinEnergy(test_min_energy);
        filter.SetMaxEnergy(test_max_energy);

        EXPECT_TRUE(filter.IsPointAllowed(point));

        test_energy_gev = 0.5;
        point.SetEnergyLoss(test_energy_gev);

        filter.SetMinEnergy(test_min_energy);
        filter.SetMaxEnergy(test_max_energy);

        EXPECT_FALSE(filter.IsPointAllowed(point));

        test_energy_gev = 1;
        test_min_energy = 0.2;
        point.SetEnergyLoss(test_energy_gev);

        filter.SetMinEnergy(test_min_energy);
        filter.SetMaxEnergy(test_min_energy);

        EXPECT_TRUE(filter.IsPointAllowed(point));
    }

    TEST(NeulandPointFilterTest, test_blacklist_mode)
    {
        auto filter = ParticleFilter{ ParticleFilter::Mode::blacklist };

        auto proton_point = R3BNeulandPoint{};
        proton_point.SetParticleId(proton_pid);
        proton_point.SetEnergyLoss(0.7);

        auto neutron_point = R3BNeulandPoint{};
        neutron_point.SetParticleId(neutron_pid);
        neutron_point.SetEnergyLoss(0.7);

        filter.AddParticle("proton", 0.6, 0.8);
        EXPECT_FALSE(filter.IsPointAllowed(proton_point));
        EXPECT_TRUE(filter.IsPointAllowed(neutron_point));

        proton_point.SetEnergyLoss(0.5);
        EXPECT_TRUE(filter.IsPointAllowed(proton_point));
    }

    TEST(NeulandPointFilterTest, test_whitelist_mode)
    {
        auto filter = ParticleFilter{ ParticleFilter::Mode::whitelist };

        auto proton_point = R3BNeulandPoint{};
        proton_point.SetParticleId(proton_pid);
        proton_point.SetEnergyLoss(0.7);

        auto neutron_point = R3BNeulandPoint{};
        neutron_point.SetParticleId(neutron_pid);
        neutron_point.SetEnergyLoss(0.7);

        filter.AddParticle("proton", 0.6, 0.8);
        EXPECT_TRUE(filter.IsPointAllowed(proton_point));
        EXPECT_FALSE(filter.IsPointAllowed(neutron_point));

        proton_point.SetEnergyLoss(0.5);
        EXPECT_FALSE(filter.IsPointAllowed(proton_point));

        filter.AddParticle("proton");
        EXPECT_TRUE(filter.IsPointAllowed(proton_point));

        filter.Reset();
        filter.AddParticle("proton", 0.6, 0.8);
    }

    TEST(NeulandPointFilterTest, test_multi_particles)
    {
        auto proton_point = R3BNeulandPoint{};
        proton_point.SetParticleId(proton_pid);
        proton_point.SetEnergyLoss(0.7);

        auto neutron_point = R3BNeulandPoint{};
        neutron_point.SetParticleId(neutron_pid);
        neutron_point.SetEnergyLoss(0.4);

        auto electron_point = R3BNeulandPoint{};
        electron_point.SetParticleId(electron_pid);
        electron_point.SetEnergyLoss(0.1);

        auto filter = ParticleFilter{ ParticleFilter::Mode::blacklist };
        filter.AddParticle("e-", 0., 1.);

        EXPECT_FALSE(filter.IsPointAllowed(electron_point));
        EXPECT_TRUE(filter.IsPointAllowed(proton_point));
        EXPECT_TRUE(filter.IsPointAllowed(neutron_point));

        filter.SetMode(ParticleFilter::Mode::whitelist);

        EXPECT_TRUE(filter.IsPointAllowed(electron_point));
        EXPECT_FALSE(filter.IsPointAllowed(proton_point));
        EXPECT_FALSE(filter.IsPointAllowed(neutron_point));
    }
} // namespace
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
