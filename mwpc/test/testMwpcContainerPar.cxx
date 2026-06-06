/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BMwpc0CalPar.h"
#include "R3BMwpc1CalPar.h"
#include "R3BMwpc2CalPar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testMwpc0CalPar, GetNumPadsX)
    {
        R3BMwpc0CalPar par;
        par.SetNumPadsX(64);

        EXPECT_EQ(par.GetNumPadsX(), 64);
    }
    TEST(testMwpc1CalPar, GetNumPadsX)
    {
        R3BMwpc1CalPar par;
        par.SetNumPadsX(128);

        EXPECT_EQ(par.GetNumPadsX(), 128);
    }
    TEST(testMwpc2CalPar, GetNumPadsX)
    {
        R3BMwpc2CalPar par;
        par.SetNumPadsX(128);

        EXPECT_EQ(par.GetNumPadsX(), 128);
    }
} // namespace
