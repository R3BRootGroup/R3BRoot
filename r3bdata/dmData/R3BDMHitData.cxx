/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BDMHitData.h"
#include <Rtypes.h>
#include <RtypesCore.h>

// NOLINTNEXTLINE
R3BDMHitData::R3BDMHitData(UInt_t detId, UInt_t pad, double pos_x, double pos_y, double time, double charge)
    : fDetId(detId)
    , fPad(pad)
    , fPosX(pos_x)
    , fPosY(pos_y)
    , fTime(time)
    , fCharge(charge)
{
}

ClassImp(R3BDMHitData);
