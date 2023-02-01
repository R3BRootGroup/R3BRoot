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

// ------------------------------------------------------------
// -----                  R3BEventHeaderMapped2TCal                -----
// -----          Created Feb 13th 2018 by M.Heil          -----
// ------------------------------------------------------------

#include "R3BEventHeaderCal2Hit.h"
#include "FairRootManager.h"

R3BEventHeaderCal2Hit::R3BEventHeaderCal2Hit(int a_verbose)
    : FairTask("R3BEventHeaderCal2Hit", a_verbose)
    , fName("R3BEventHeaderCal2Hit")
    , fHeader(new R3BEventHeader)
{
}

R3BEventHeaderCal2Hit::~R3BEventHeaderCal2Hit() { delete fHeader; }

InitStatus R3BEventHeaderCal2Hit::Init()
{
    FairRootManager* mgr = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    mgr->Register("R3BEventHeaderCal2Hit", "EventHeader", fHeader, kTRUE);
    return kSUCCESS;
}

void R3BEventHeaderCal2Hit::SetParContainers() {}

InitStatus R3BEventHeaderCal2Hit::ReInit() { return kSUCCESS; }

void R3BEventHeaderCal2Hit::Exec(Option_t* option) {}

void R3BEventHeaderCal2Hit::FinishEvent() {}

void R3BEventHeaderCal2Hit::FinishTask() {}

ClassImp(R3BEventHeaderCal2Hit)
