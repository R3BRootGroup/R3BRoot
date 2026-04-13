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

#include "R3BPWCalData.h"

R3BPWCalData::R3BPWCalData()
    : fDetId(0)
    , fBar(0)
    , fTime0(0)
    , fTime1(0)
    , fTot0(0)
    , fTot1(0)
{
}

R3BPWCalData::R3BPWCalData(UShort_t detId, UShort_t Bar, double Time0, double Time1, double Tot0, double Tot1)
    : fDetId(detId)
    , fBar(Bar)
    , fTime0(Time0)
    , fTime1(Time1)
    , fTot0(Tot0)
    , fTot1(Tot1)
{
}

ClassImp(R3BPWCalData);
