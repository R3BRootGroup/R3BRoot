
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

#include "R3BIncomingIDPar.h"
#include "R3BMSOffsetPar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testAnaIncomingPar, GetBeta_min)
    {
        R3BIncomingIDPar par;
        par.SetBeta_min(0.5);

        EXPECT_EQ(par.GetBeta_min(), 0.5);
    }
    TEST(testAnaIncomingPar, GetBeta_max)
    {
        R3BIncomingIDPar par;
        par.SetBeta_max(1.0);

        EXPECT_EQ(par.GetBeta_max(), 1.0);
    }
    TEST(testAnaMSOffsetPar, GetMSOffset)
    {
        R3BMSOffsetPar par;
        par.SetMSOffset(3.5);

        EXPECT_EQ(par.GetMSOffset(), 3.5);
    }
} // namespace
