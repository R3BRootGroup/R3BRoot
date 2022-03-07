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

#include "R3BRpcStripHitData.h"

R3BRpcStripHitData::R3BRpcStripHitData()
    : FairMultiLinkedData()
    , fChannelId(0)
    , fTime(0)
    , fPos(0)
    , fCharge(0)
{
}

R3BRpcStripHitData::R3BRpcStripHitData(UInt_t channel, double time, double pos, double charge)
    : FairMultiLinkedData()
    , fChannelId(channel)
    , fTime(time)
    , fPos(pos)
    , fCharge(charge)
{
}


ClassImp(R3BRpcStripHitData);
