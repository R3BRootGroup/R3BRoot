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

namespace
{
    TEST(testR3BAtima, basicCalculation)
    {
        // Create the CATIMA model
        R3BAtima catimaModel;

        // Set the projectile to 12C
        catimaModel.SetProjectile(12, 6); // (A, Z)
        double kinE = 1000.;              // MeV/A

        // Set material H2O with a thickness of 2 g/cm2
        double thickness = 2.;
        catimaModel.SetMaterial({ { 1, 1, 2 }, { 16, 8, 1 } }, 1., thickness);

        auto range = catimaModel.GetRange(kinE);
        EXPECT_TRUE(std::abs(range - 107.103) / 107.103 < 0.02);

        auto dedx = catimaModel.GetdEdx(kinE);
        EXPECT_TRUE(std::abs(dedx - 80.7664) / 80.7664 < 0.02);

        auto energy = catimaModel.GetEnergy(kinE, thickness);
        EXPECT_TRUE(std::abs(energy - 986.539) / 986.539 < 0.02);

        auto eloss = catimaModel.GetEnergyLoss(kinE, thickness);
        EXPECT_TRUE(std::abs(eloss - 161.533) / 161.533 < 0.02);
    }
} // namespace
