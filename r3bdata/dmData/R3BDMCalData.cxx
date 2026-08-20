/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BDMCalData.h"
#include <Rtypes.h>
#include <RtypesCore.h>

// NOLINTNEXTLINE
R3BDMCalData::R3BDMCalData(UShort_t DetId, UShort_t TDCId, UShort_t channelId, double Time, double Tot)
    : fDetId(DetId)
    , fTDCId(TDCId)
    , fChannelId(channelId)
    , fTime(Time)
    , fTot(Tot)
{
}

ClassImp(R3BDMCalData);
