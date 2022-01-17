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

#include "R3BRpcStripCalData.h"

R3BRpcStripCalData::R3BRpcStripCalData()
    : FairMultiLinkedData()
    , fChannelId(0)
    , fTimeRight(0)
    , fTimeLeft(0)
    , fTotRight(0)
    , fTotLeft(0)
{
}

R3BRpcStripCalData::R3BRpcStripCalData(UShort_t channelId, double TimeRight, double TimeLeft, double TotRight, double TotLeft)
    : FairMultiLinkedData()
    , fChannelId(channelId)
    , fTimeRight(TimeRight)
    , fTimeLeft(TimeLeft)
    , fTotRight(TotRight)
    , fTotLeft(TotLeft)
{
}

ClassImp(R3BRpcStripCalData);
