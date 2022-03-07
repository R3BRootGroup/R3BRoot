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

#include "R3BRpcPmtCalData.h"

R3BRpcPmtCalData::R3BRpcPmtCalData()
    : FairMultiLinkedData()
    , fChannelId(0)
    , fTimeTop(0)
    , fTimeBottom(0)
    , fTotTop(0)
    , fTotBottom(0)
{
}

R3BRpcPmtCalData::R3BRpcPmtCalData(UShort_t channelId, double TimeTop, double TimeBottom, double TotTop, double TotBottom)
    : FairMultiLinkedData()
    , fChannelId(channelId)
    , fTimeTop(TimeTop)
    , fTimeBottom(TimeBottom)
    , fTotTop(TotTop)
    , fTotBottom(TotBottom)
{
}

ClassImp(R3BRpcPmtCalData);
