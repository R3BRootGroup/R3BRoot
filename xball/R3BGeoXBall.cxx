/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

/////////////////////////////////////////////////////////////
// R3BGeoXBall
//
// Class for geometry of R3BCAL
//
/////////////////////////////////////////////////////////////

#include "R3BGeoXBall.h"
#include "FairGeoNode.h"
#include "FairLogger.h"

R3BGeoXBall::R3BGeoXBall()
{
    // Constructor
    fName = "xball";
    maxSectors = 0;
    maxModules = 99;
}

const char* R3BGeoXBall::getModuleName(Int_t m)
{
    // Returns the module name of sts number m
    if (m < 0)
    {
        LOG(error) << "R3BGeoXBall::getModuleName:: Module number " << m << " not known!";
        return "";
    }
    if (m < 9)
    {
        sprintf(modName, "calstation0%i", m + 1);
    }
    else
    {
        sprintf(modName, "calstation%i", m + 1);
    }
    return modName;
}

const char* R3BGeoXBall::getEleName(Int_t m)
{
    // Returns the element name of sts number m
    sprintf(eleName, "cal%i", m + 1);
    return eleName;
}

ClassImp(R3BGeoXBall)
