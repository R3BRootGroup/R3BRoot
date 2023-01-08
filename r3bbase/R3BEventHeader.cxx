/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BEventHeader.h"

R3BEventHeader::R3BEventHeader()
    : FairEventHeader()
    , fExpId(0)
    , fEventno(0)
    , fTrigger(0)
    , fTimeStamp(0)
    , fTpat(0)
    , fTStart(0)
{
}

R3BEventHeader::~R3BEventHeader() {}

void R3BEventHeader::Register(Bool_t Persistence) {}

ClassImp(R3BEventHeader);
