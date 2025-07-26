
/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BBunchedFiberHitModulePar.h"
#include "R3BFiberMAPMTHitModulePar.h"
#include "R3BFiberMappingPar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testFiberMappingPar, GetNbChannels)
    {
        R3BFiberMappingPar par;
        par.SetNbChannels(20);

        EXPECT_EQ(par.GetNbChannels(), 20);
    }

    TEST(testFiberMappingPar, GetNbSides)
    {
        R3BFiberMappingPar par;
        par.SetNbSides(10);

        EXPECT_EQ(par.GetNbSides(), 10);
    }

    TEST(testFiberMAPMTHitModulePar, GetFiber)
    {
        R3BFiberMAPMTHitModulePar par;
        par.SetFiber(100);

        EXPECT_EQ(par.GetFiber(), 100);
    }

    TEST(testFiberMAPMTHitModulePar, GetGainUp)
    {
        R3BFiberMAPMTHitModulePar par;
        par.SetGainUp(12.24);

        EXPECT_EQ(par.GetGainUp(), 12.24);
    }

    TEST(testBunchedFiberHitModulePar, GetFiber)
    {
        R3BBunchedFiberHitModulePar par;
        par.SetFiber(100);

        EXPECT_EQ(par.GetFiber(), 100);
    }

    TEST(testBunchedFiberHitModulePar, GetVeff)
    {
        R3BBunchedFiberHitModulePar par;
        par.SetVeff(12.2);

        EXPECT_EQ(par.GetVeff(), 12.2);
    }
} // namespace
