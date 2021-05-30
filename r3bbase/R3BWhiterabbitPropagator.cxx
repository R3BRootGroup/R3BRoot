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

// --------------------------------------------------------------
// -----             R3BWhiterabbitPropagator               -----
// -----   Created May 28th 2021 by J.L. Rodriguez-Sanchez  -----
// --------------------------------------------------------------

#include "R3BWhiterabbitPropagator.h"
#include "R3BWRMasterData.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

R3BWhiterabbitPropagator::R3BWhiterabbitPropagator()
    : FairTask("R3BWhiterabbitPropagator", 1)
    , fInputItem(NULL)
    , fOutputItem(NULL)
    , fNameWR("WRMaster")
{
}

R3BWhiterabbitPropagator::R3BWhiterabbitPropagator(const TString& name, Int_t iVerbose, const TString& namewr)
    : FairTask(name, iVerbose)
    , fInputItem(NULL)
    , fOutputItem(NULL)
    , fNameWR(namewr)
{
}

R3BWhiterabbitPropagator::~R3BWhiterabbitPropagator()
{
    LOG(INFO) << "R3BWhiterabbitPropagator::Delete instance";
    if (fInputItem)
        delete fInputItem;
    if (fOutputItem)
        delete fOutputItem;
}

InitStatus R3BWhiterabbitPropagator::Init()
{
    LOG(INFO) << "R3BWhiterabbitPropagator::Init()";
    FairRootManager* frm = FairRootManager::Instance();
    if (NULL == frm)
    {
        LOG(FATAL) << "R3BWhiterabbitPropagator::Init() FairRootManager not found";
        return kFATAL;
    }

    // get access to input data
    fInputItem = (TClonesArray*)frm->GetObject(fNameWR + "Data");
    if (!fInputItem)
    {
        LOG(FATAL) << "R3BWhiterabbitPropagator::Init() " + fNameWR + "Data not found";
        return kFATAL;
    }

    frm->Register(fNameWR + "Data", fNameWR, fInputItem, kTRUE);

    return kSUCCESS;
}

void R3BWhiterabbitPropagator::SetParContainers() {}

InitStatus R3BWhiterabbitPropagator::ReInit() { return kSUCCESS; }

void R3BWhiterabbitPropagator::Exec(Option_t* option) { return; }

void R3BWhiterabbitPropagator::FinishEvent() {}

void R3BWhiterabbitPropagator::FinishTask() {}

ClassImp(R3BWhiterabbitPropagator)
