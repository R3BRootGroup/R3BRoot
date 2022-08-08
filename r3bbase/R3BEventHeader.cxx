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

#include "R3BEventHeader.h"
//#include "FairLogger.h"
//#include "FairRootManager.h"

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

void R3BEventHeader::Register(Bool_t Persistence)
{
    /*   FairRootManager* frm = FairRootManager::Instance();
       LOG(INFO) << "R3BEventHeader::Register of R3BEventHeader";
       if (!frm)
           LOG(FATAL) << "R3BEventHeader::FairRootManager no found";
       frm->Register("EventHeader.", "R3BEvtHeader", this, Persistence);
       */
}

ClassImp(R3BEventHeader)
