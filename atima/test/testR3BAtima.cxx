/******************************************************************************
 *   Copyright (C) 2020 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2020-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BAtima.h"

#include <cmath>
#include <gtest/gtest.h>
#include <map>
#include <tuple>
#include <utility>
#include <vector>

namespace
{
    TEST(testR3BAtima, basicCalculation)
    {
        // Create the vector with the gas components for H2.
        std::vector<std::tuple<int, int, int>> components;
        components.push_back({ 1, 1, 2 }); // (A, Z, stoichiometry)

        // Calculate the gas density for 600Torr H2 from LISE.
        double density = 6.5643e-5; // g/cm3

        // Create the CATIMA ELoss model.
        R3B::Atima catimaModel(density, components);

        // Set the projectile to proton.
        catimaModel.SetProjectile(1, 1, 1.007825031898); // (A, Z, massUMA)

        // Calculate ranges for different energies with LISE.
        std::vector<std::pair<double, double>> kinEnergyRange; // (MeV, mm)
        kinEnergyRange.push_back(std::make_pair(1, 1.33e2));
        kinEnergyRange.push_back(std::make_pair(5, 2.2381e3));
        kinEnergyRange.push_back(std::make_pair(10, 7.8883e3));

        // Compare with the results from this class.
        for (auto pair : kinEnergyRange)
        {
            double energy = pair.first;
            double rangeLISE = pair.second;

            double range = catimaModel.GetRange(energy);
            EXPECT_TRUE(std::abs(range - rangeLISE) / rangeLISE < 0.05);
        }

        // Calculate ELosses for different distances using LISE at 10MeV initial energy.
        std::vector<std::pair<double, double>> distanceELoss; // (mm, MeV)
        distanceELoss.push_back(std::make_pair(100, 0.0694));
        distanceELoss.push_back(std::make_pair(1000, 0.7138));
        distanceELoss.push_back(std::make_pair(5000, 4.2409));

        // Compare with the results from this class.
        for (auto pair : distanceELoss)
        {
            double distance = pair.first;
            double ELossLISE = pair.second;

            double ELoss = catimaModel.GetEnergyLoss(10, distance);
            EXPECT_TRUE(std::abs(ELoss - ELossLISE) / ELossLISE < 0.05);
        }

        // Do the same for 4He projectile.
        catimaModel.SetProjectile(4, 2, 4.00260325413); // (A, Z, massUMA)

        std::vector<std::pair<double, double>> kinEnergyRange4He; // (MeV, mm)
        kinEnergyRange4He.push_back(std::make_pair(1, 2.5742e1));
        kinEnergyRange4He.push_back(std::make_pair(5, 2.0547e2));
        kinEnergyRange4He.push_back(std::make_pair(10, 6.5724e2));

        // Compare with the results from this class.
        for (auto pair : kinEnergyRange4He)
        {
            double energy = pair.first;
            double rangeLISE = pair.second;

            double range = catimaModel.GetRange(energy);
            EXPECT_TRUE(std::abs(range - rangeLISE) / rangeLISE < 0.05);
        }

        // Calculate ELosses for different distances using LISE at 10MeV initial energy.
        std::vector<std::pair<double, double>> distanceELoss4He; // (mm, MeV)
        distanceELoss4He.push_back(std::make_pair(100, 0.9121));
        distanceELoss4He.push_back(std::make_pair(500, 5.7726));
        distanceELoss4He.push_back(std::make_pair(1000, 10));

        // Compare with the results from this class.
        for (auto pair : distanceELoss4He)
        {
            double distance = pair.first;
            double ELossLISE = pair.second;

            double ELoss = catimaModel.GetEnergyLoss(10, distance);
            EXPECT_TRUE(std::abs(ELoss - ELossLISE) / ELossLISE < 0.05);
        }

        // Finally, for 4He, we test if GetRange works when setting a non-zero final energy.
        double energyIni{ 10 };
        double energyFin{ 4.227 };
        double rangeLISE{ 500 };
        double range = catimaModel.GetRange(energyIni, energyFin);
        EXPECT_TRUE(std::abs(range - rangeLISE) / rangeLISE < 0.05);
    }
} // namespace
