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

#include "R3BRpcHitData.h"

R3BRpcHitData::R3BRpcHitData()
    : FairMultiLinkedData()
    , fChannelId(0)
    // MODIFY ME!!!!!!!!!!!!!!!!!
    , fEnergy(NAN)
    , fTime(NAN)
{
}

R3BRpcHitData::R3BRpcHitData(UInt_t ch, Double_t ene, Double_t tim)
    : FairMultiLinkedData()
    , fChannelId(ch)
    // MODIFY ME!!!!!!!!!!!!!!!!!
    , fEnergy(ene)
    , fTime(tim)
{
}

R3BRpcHitData::R3BRpcHitData(const R3BRpcHitData& right)
    : FairMultiLinkedData(right)
    , fChannelId(right.fChannelId)
    // MODIFY ME!!!!!!!!!!!!!!!!!
    , fEnergy(right.fEnergy)
    , fTime(right.fTime)
{
}

ClassImp(R3BRpcHitData);
