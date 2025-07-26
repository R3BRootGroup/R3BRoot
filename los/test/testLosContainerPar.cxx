
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

#include "R3BLosHitPar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testLosHitPar, Getp0)
    {
        R3BLosHitPar par;
        par.Setp0(3);

        EXPECT_EQ(par.Getp0(), 3);
    }
    TEST(testLosHitPar, Getp1)
    {
        R3BLosHitPar par;
        par.Setp1(2);

        EXPECT_EQ(par.Getp1(), 2);
    }
    TEST(testLosHitPar, GetNumParamsTamexLE)
    {
        R3BLosHitPar par;

        EXPECT_EQ(par.GetNumParamsTamexLE(), 2);
    }
} // namespace
