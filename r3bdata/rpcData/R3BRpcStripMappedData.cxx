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

#include "R3BRpcStripMappedData.h"

R3BRpcStripMappedData::R3BRpcStripMappedData()
    : fStripId(0)
    , fFineTime(0)
    , fCoarseTime(0)
    , fEdge(0)
    , fSide(0)
{
}

R3BRpcStripMappedData::R3BRpcStripMappedData(UShort_t StripId, uint64_t FineTime, uint64_t CoarseTime, UShort_t Edge, UShort_t Side)
   : fStripId(StripId)
   , fFineTime(FineTime)
   , fCoarseTime(CoarseTime)
   , fEdge(Edge)
   , fSide(Side)
{
}

ClassImp(R3BRpcStripMappedData);
