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

#include "R3BRpcPmtMappedData.h"

R3BRpcPmtMappedData::R3BRpcPmtMappedData()
    : fChannelId(0)
    , fFineTime(0)
    , fCoarseTime(0)
    , fEdge(0)
{
}

R3BRpcPmtMappedData::R3BRpcPmtMappedData(UShort_t channelId, uint64_t fineTime, uint64_t coarseTime, Int_t edge)
: fChannelId(channelId)
, fFineTime(fineTime)
, fCoarseTime(coarseTime)
, fEdge(edge)
{
}

ClassImp(R3BRpcPmtMappedData);
