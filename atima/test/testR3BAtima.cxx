/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
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
#include "R3BAtimaCache.h"
#include "gtest/gtest.h"

namespace
{
    constexpr auto ELOSS_Prot100 = 1.085;

    TEST(testR3BAtima, basicCalculation)
    {
        // Eloss of 100 MeV Protons in 1cm LH2
        const auto eloss = R3BAtima::Calculate_mm(1., 1., 100., R3BAtimaTargetMaterial::LH2, 10).ELoss_AMeV;

        EXPECT_NEAR(eloss, ELOSS_Prot100, 0.01);
    }

    TEST(testR3BAtima, cacheCalculation)
    {
        const auto cache = R3BAtimaCache(1., 1., { 100., 200., 10 }, R3BAtimaTargetMaterial::LH2, { 10., 50., 4 });
        EXPECT_GT(cache(100., 18.).ELoss_AMeV, ELOSS_Prot100);
        EXPECT_LT(cache(107., 10.).ELoss_AMeV, ELOSS_Prot100);
    }

    TEST(testR3BAtima, writeReadCache)
    {
        {
            // create cache file
            const auto cache1 =
                R3BAtimaCache(1., 1., { 100., 200., 10 }, R3BAtimaTargetMaterial::LH2, { 10., 50., 4 }, "test.atima");
        }
        // read cache file
        const auto cache1 =
            R3BAtimaCache(1., 1., { 100., 200., 10 }, R3BAtimaTargetMaterial::LH2, { 10., 50., 4 }, "test.atima");
        // calculate without cache file
        const auto cache2 = R3BAtimaCache(1., 1., { 100., 200., 10 }, R3BAtimaTargetMaterial::LH2, { 10., 50., 4 });
        EXPECT_EQ(cache1(100., 20.).ELoss_AMeV, cache2(100., 20.).ELoss_AMeV);
    }

} // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
