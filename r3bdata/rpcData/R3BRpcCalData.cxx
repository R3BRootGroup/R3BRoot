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

#include "R3BRpcCalData.h"

R3BRpcCalData::R3BRpcCalData()
    : FairMultiLinkedData()
    , fChannelId(-1)
    // MODIFY ME!!!!!!!!!!!!!!!!!
    , fEnergy(-1.)
    , fToT_Energy(-1.)
    , fTime(0)
{
}

R3BRpcCalData::R3BRpcCalData(Int_t ident,
                             // MODIFY ME!!!!!!!!!!!!!!!!!

                             Double_t energy,
                             uint64_t time,
                             Double_t tot_energy)
    : FairMultiLinkedData()
    , fChannelId(ident)
    // MODIFY ME!!!!!!!!!!!!!!!!!
    , fEnergy(energy)
    , fToT_Energy(tot_energy)
    , fTime(time)
{
}

R3BRpcCalData::R3BRpcCalData(const R3BRpcCalData& right)
    : FairMultiLinkedData(right)
    , fChannelId(right.fChannelId)
    // MODIFY ME!!!!!!!!!!!!!!!!!
    , fEnergy(right.fEnergy)
    , fToT_Energy(right.fToT_Energy)
    , fTime(right.fTime)
{
}

ClassImp(R3BRpcCalData);
