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

#include "TClonesArray.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BRpcCal2HitPar.h"
#include "R3BRpcCalData.h"
#include "R3BRpcHitPar.h"

#include <iostream>
#include <stdlib.h>

R3BRpcCal2HitPar::R3BRpcCal2HitPar()
    : R3BRpcCal2HitPar("R3B CALIFA Tot Calibration Parameters Finder ", 1)
{
}

R3BRpcCal2HitPar::R3BRpcCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fHitPar(NULL)
    , fCalDataCA(NULL)
    , fNumChannels(64)
    , fDebugMode(false)
{
}

R3BRpcCal2HitPar::~R3BRpcCal2HitPar()
{
    LOG(INFO) << "R3BRpcCal2HitPar: Delete instance";
    if (fCalDataCA)
        delete fCalDataCA;
}

void R3BRpcCal2HitPar::SetParContainers()
{
    // Parameter Container
    // Reading califaMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }
}

void R3BRpcCal2HitPar::SetParameter() {}

InitStatus R3BRpcCal2HitPar::Init()
{
    LOG(INFO) << "R3BRpcCal2HitPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fCalDataCA = (TClonesArray*)rootManager->GetObject("RPCCalData");
    if (!fCalDataCA)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() RPCCalData not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fHitPar = (R3BRpcHitPar*)rtdb->getContainer("RPCHitPar");
    if (!fHitPar)

    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() Couldn't get handle on RPCHitPar container";
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    return kSUCCESS;
}

InitStatus R3BRpcCal2HitPar::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BRpcCal2HitPar::Reset() {}

void R3BRpcCal2HitPar::FinishEvent() {}

void R3BRpcCal2HitPar::FinishTask() {}

void R3BRpcCal2HitPar::Exec(Option_t* opt)
{
    Int_t nHits = fCalDataCA->GetEntries();
    if (!nHits)
        return;

    R3BRpcCalData** CalHit = new R3BRpcCalData*[nHits];
    Int_t crystalId = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        CalHit[i] = (R3BRpcCalData*)(fCalDataCA->At(i));
        // DO THE WORK
    }

    if (CalHit)
        delete[] CalHit;
    return;
}

ClassImp(R3BRpcCal2HitPar)
