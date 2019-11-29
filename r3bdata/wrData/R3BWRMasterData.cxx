/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRMasterData                            -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#include "R3BWRMasterData.h"

R3BWRMasterData::R3BWRMasterData()
    : fTimeStamp(0)
{
}

//------------------------------

R3BWRMasterData::R3BWRMasterData(uint64_t timestamp)
    : fTimeStamp(timestamp)
{
}

ClassImp(R3BWRMasterData)
