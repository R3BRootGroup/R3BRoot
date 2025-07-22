
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

#include "R3BFrsSciCalPar.h"
#include "R3BFrsSciTcalPar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testFrsSciTcalPar, GetNumDets)
    {
        R3BFrsSciTcalPar par;
        par.SetNumDets(2);

        EXPECT_EQ(par.GetNumDets(), 2);
    }
    TEST(testFrsSciTcalPar, GetNumPmts)
    {
        R3BFrsSciTcalPar par;
        par.SetNumPmts(3);

        EXPECT_EQ(par.GetNumPmts(), 3);
    }
    TEST(testFrsSciCalPar, GetNumDets)
    {
        R3BFrsSciCalPar par;
        par.SetNumDets(2);

        EXPECT_EQ(par.GetNumDets(), 2);
    }
    TEST(testFrsSciCalPar, GetNumPmts)
    {
        R3BFrsSciCalPar par;
        par.SetNumPmts(3);

        EXPECT_EQ(par.GetNumPmts(), 3);
    }
} // namespace
