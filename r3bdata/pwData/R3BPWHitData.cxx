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

#include "R3BPWHitData.h"

R3BPWHitData::R3BPWHitData()
    : fDetId(0)
    , fBar(0)
    , fTime(0)
    , fPos(0)
    , fCharge(0)
    , fTof(0)
{
}

R3BPWHitData::R3BPWHitData(UInt_t detId, UInt_t bar, double time, double pos, double charge, double tof)
    : fDetId(detId)
    , fBar(bar)
    , fTime(time)
    , fPos(pos)
    , fCharge(charge)
    , fTof(tof)
{
}

ClassImp(R3BPWHitData);
