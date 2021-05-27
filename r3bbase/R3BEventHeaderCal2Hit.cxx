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
// -----             R3BEventHeaderMapped2TCal            -----
// -----          Created Feb 13th 2021 by M.Heil         -----
// ------------------------------------------------------------

#include "R3BEventHeaderCal2Hit.h"
#include "FairLogger.h"
#include "R3BFileSource.h"

R3BEventHeaderCal2Hit::R3BEventHeaderCal2Hit()
    : FairTask("R3BEventHeaderCal2Hit", 1)
    , fHeader(nullptr)
    , fSource(nullptr)
{
}

R3BEventHeaderCal2Hit::R3BEventHeaderCal2Hit(int a_verbose)
    : FairTask("R3BEventHeaderCal2Hit", a_verbose)
    , fHeader(nullptr)
    , fSource(nullptr)
{
}

R3BEventHeaderCal2Hit::~R3BEventHeaderCal2Hit()
{
    if (fHeader)
        delete fHeader;
    if (fSource)
        delete fSource;
}

InitStatus R3BEventHeaderCal2Hit::Init()
{
    LOG(INFO) << "R3BEventHeaderCal2Hit::Init()";
    FairRootManager* frm = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)frm->GetObject("EventHeader.");
    if (!fHeader)
    {
        fHeader = (R3BEventHeader*)frm->GetObject("R3BEventHeader");
        LOG(WARNING) << "R3BEventHeaderCal2Hit::Init() FairEventHeader not found";
    }
    else
        LOG(INFO) << "R3BEventHeaderCal2Hit::Init() EventHeader found";

    frm->Register("EventHeader.", "EventHeader", fHeader, kTRUE);

    fSource = R3BFileSource::Instance();
    if (!fSource)
    {
        LOG(WARNING) << "R3BEventHeaderCal2Hit::Init() R3BFileSource not found";
    }

    return kSUCCESS;
}

void R3BEventHeaderCal2Hit::SetParContainers() {}

InitStatus R3BEventHeaderCal2Hit::ReInit() { return kSUCCESS; }

void R3BEventHeaderCal2Hit::Exec(Option_t* option)
{
    if (fSource)
        fHeader->SetRunId(fSource->GetRunId());

    return;
}

void R3BEventHeaderCal2Hit::FinishEvent() {}

void R3BEventHeaderCal2Hit::FinishTask() {}

ClassImp(R3BEventHeaderCal2Hit)
