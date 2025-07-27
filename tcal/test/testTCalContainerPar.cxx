
/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
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

#include "R3BTCalModulePar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testTCalModulePar, GetPlane)
    {
        R3BTCalModulePar par;
        par.SetPlane(2);

        EXPECT_EQ(par.GetPlane(), 2);
    }

    TEST(testTCalModulePar, GetPaddle)
    {
        R3BTCalModulePar par;
        par.SetPaddle(20);

        EXPECT_EQ(par.GetPaddle(), 20);
    }

    TEST(testTCalModulePar, GetSide)
    {
        R3BTCalModulePar par;
        par.SetSide(1);

        EXPECT_EQ(par.GetSide(), 1);
    }

    TEST(testTCalModulePar, GetNofChannels)
    {
        R3BTCalModulePar par;
        par.IncrementNofChannels();
        par.IncrementNofChannels();
        par.IncrementNofChannels();

        EXPECT_EQ(par.GetNofChannels(), 3);
    }
} // namespace
