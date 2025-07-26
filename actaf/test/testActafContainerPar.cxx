
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

#include "R3BActafMappingPar.h"
#include "gtest/gtest.h"
#include <map>

namespace
{
    TEST(testActafMappingPar, GetNbPads)
    {
        R3BActafMappingPar par;
        par.SetNbPads(100);

        EXPECT_EQ(par.GetNbPads(), 100);
    }

    TEST(testActafMappingPar, GetGeoVersion)
    {
        R3BActafMappingPar par;
        par.SetGeoVersion(2024);

        EXPECT_EQ(par.GetGeoVersion(), 2024);
    }

    TEST(testActafMappingPar, GetNbFADCModules)
    {
        R3BActafMappingPar par;

        EXPECT_EQ(par.GetNbFADCModules(), 8);
    }
} // namespace
