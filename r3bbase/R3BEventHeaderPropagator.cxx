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

#include "FairLogger.h"
#include <FairRootManager.h>

#include "R3BEventHeaderPropagator.h"
#include "R3BFileSource.h"

R3BEventHeaderPropagator::R3BEventHeaderPropagator()
    : R3BEventHeaderPropagator("R3BEventHeaderPropagator", 1, "EventHeader.")
{
}

R3BEventHeaderPropagator::R3BEventHeaderPropagator(const TString& name, Int_t iVerbose, const TString& nameheader)
    : FairTask(name, iVerbose)
    , fNameHeader(nameheader)
    , fHeader(nullptr)
    , fSource(nullptr)
{
}

R3BEventHeaderPropagator::~R3BEventHeaderPropagator()
{
    if (fHeader)
        delete fHeader;
}

InitStatus R3BEventHeaderPropagator::Init()
{
    LOG(info) << "R3BEventHeaderPropagator::Init()";
    FairRootManager* frm = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)frm->GetObject("EventHeader.");
    if (!fHeader)
    {
        fHeader = (R3BEventHeader*)frm->GetObject("R3BEventHeader");
        LOG(warn) << "R3BEventHeaderPropagator::Init() FairEventHeader not found";
    }
    else
        LOG(info) << "R3BEventHeaderPropagator::Init() EventHeader found";

    frm->Register(fNameHeader, "EventHeader", fHeader, kTRUE);

    fSource = R3BFileSource::Instance();
    if (!fSource)
    {
        LOG(error) << "R3BEventHeaderPropagator::Init() R3BFileSource not found";
    }
    return kSUCCESS;
}

void R3BEventHeaderPropagator::Exec(Option_t* option)
{
    if (fSource)
        fHeader->SetRunId(fSource->GetRunId());
    return;
}

ClassImp(R3BEventHeaderPropagator);
