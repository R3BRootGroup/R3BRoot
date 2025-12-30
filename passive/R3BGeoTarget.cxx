/******************************************************************************
 *   Copyright (C) 2009 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2009-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BGeoTarget.h"

ClassImp(R3BGeoTarget)

    R3BGeoTarget::R3BGeoTarget()
{
    // Constructor
    fName = "target";
    strcpy(modName, "t");
    strcpy(eleName, "t");
    maxSectors = 0;
    maxModules = 1;
}
