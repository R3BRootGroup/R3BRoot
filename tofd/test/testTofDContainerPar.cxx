
/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BTofDHitModulePar.h"
#include "R3BTofDMappingPar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testTofDMappingPar, GetNbPlanes)
    {
        R3BTofDMappingPar par;
        par.SetNbPlanes(2);

        EXPECT_EQ(par.GetNbPlanes(), 2);
    }

    TEST(testTofDMappingPar, GetNbPaddles)
    {
        R3BTofDMappingPar par;
        par.SetNbPaddles(22);

        EXPECT_EQ(par.GetNbPaddles(), 22);
    }

    TEST(testTofDHitModulePar, GetPlane)
    {
        R3BTofDHitModulePar par;
        par.SetPlane(3);

        EXPECT_EQ(par.GetPlane(), 3);
    }

    TEST(testTofDHitModulePar, GetPaddle)
    {
        R3BTofDHitModulePar par;
        par.SetPaddle(40);

        EXPECT_EQ(par.GetPaddle(), 40);
    }

    TEST(testTofDHitModulePar, GetVeff)
    {
        R3BTofDHitModulePar par;
        par.SetVeff(1.5);

        EXPECT_EQ(par.GetVeff(), 1.5);
    }
} // namespace
