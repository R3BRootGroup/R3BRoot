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

#include "R3BRpcStripPreCalData.h"

R3BRpcStripPreCalData::R3BRpcStripPreCalData()
    : FairMultiLinkedData()
    , fChannelId(0)
    , fTime(0)
    , fTot(0)
    , fSide(0)
{
}

R3BRpcStripPreCalData::R3BRpcStripPreCalData(UShort_t channelId, double Time, double Tot, UShort_t Side)
    : FairMultiLinkedData()
    , fChannelId(channelId)
    , fTime(Time)
    , fTot(Tot)
    , fSide(Side)
{
}

ClassImp(R3BRpcStripPreCalData);
