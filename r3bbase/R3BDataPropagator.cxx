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

#include "FairLogger.h"

#include "R3BDataPropagator.h"
#include "R3BLogger.h"

#include "TClonesArray.h"

R3BDataPropagator::R3BDataPropagator()
    : R3BDataPropagator("R3BDataPropagator", 1)
{
}

R3BDataPropagator::R3BDataPropagator(const TString& name, Int_t iVerbose, const TString& namebranch)
    : FairTask(name, iVerbose)
    , fNameBranch(namebranch)
    , fInputData(NULL)
{
}

R3BDataPropagator::~R3BDataPropagator()
{
    if (fInputData)
    {
        delete fInputData;
        fInputData = NULL;
    }
}

InitStatus R3BDataPropagator::Init()
{
    LOG(INFO) << "R3BDataPropagator::Init()";
    FairRootManager* frm = FairRootManager::Instance();

    fInputData = (TClonesArray*)frm->GetObject(fNameBranch);
    if (!fInputData)
    {
        R3BLOG(FATAL, "branch " << fNameBranch << "not found");
        return kFATAL;
    }
    else
        R3BLOG(INFO, "branch " << fNameBranch << " found");

    frm->Register(fNameBranch, "Data", fInputData, kTRUE);

    return kSUCCESS;
}

void R3BDataPropagator::Exec(Option_t* option) { return; }

ClassImp(R3BDataPropagator);
