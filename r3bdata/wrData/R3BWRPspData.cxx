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
// -----                      R3BWRPspData                               -----
// -----                  Created 28/06/2021 by V. Panin                 -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#include "R3BWRPspData.h"

R3BWRPspData::R3BWRPspData()
    : fTimeStamp(0)
{
}

//------------------------------

R3BWRPspData::R3BWRPspData(uint64_t timestamp)
    : fTimeStamp(timestamp)
{
}

ClassImp(R3BWRPspData)
