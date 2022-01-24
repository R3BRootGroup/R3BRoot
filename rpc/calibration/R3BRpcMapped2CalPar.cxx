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
#include "TObjArray.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BRpcCalPar.h"
#include "R3BRpcMapped2CalPar.h"
#include "R3BRpcMappedData.h"

#include <iostream>
#include <stdlib.h>

R3BRpcMapped2CalPar::R3BRpcMapped2CalPar()
    : R3BRpcMapped2CalPar("R3B RPC Calibration Parameters Finder ", 1)
{
}

R3BRpcMapped2CalPar::R3BRpcMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalPar(NULL)
    , fMappedDataCA(NULL)
    , fNumChannels(64)
    , fDebugMode(0)
{
}

R3BRpcMapped2CalPar::~R3BRpcMapped2CalPar()
{
    LOG(INFO) << "R3BRpcMapped2CalPar: Delete instance";
    if (fMappedDataCA)
        delete fMappedDataCA;
}

void R3BRpcMapped2CalPar::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }
}

void R3BRpcMapped2CalPar::SetParameter() {}

InitStatus R3BRpcMapped2CalPar::Init()
{
    LOG(INFO) << "R3BRpcMapped2CalPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(ERROR) << "R3BRpcMapped2CalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fMappedDataCA = (TClonesArray*)rootManager->GetObject("RPCMappedData");
    if (!fMappedDataCA)
    {
        LOG(ERROR) << "R3BRpcMapped2CalPar::Init() RpcMappedDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcMapped2CalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fCalPar = (R3BRpcCalPar*)rtdb->getContainer("RPCCalPar");
    if (!fCalPar)
    {
        LOG(ERROR) << "R3BRpcMapped2CalPar::Init() Couldn't get handle on RPCCalPar container";
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    return kSUCCESS;
}

InitStatus R3BRpcMapped2CalPar::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BRpcMapped2CalPar::Exec(Option_t* opt)
{
    Int_t nHits = fMappedDataCA->GetEntries();
    if (!nHits)
        return;

    R3BRpcMappedData** map = new R3BRpcMappedData*[nHits];
    Int_t channelId = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        map[i] = (R3BRpcMappedData*)(fMappedDataCA->At(i));
        // DO THE WORK HERE!!!
    }
    if (map)
        delete[] map;
    return;
}

void R3BRpcMapped2CalPar::Reset() {}

void R3BRpcMapped2CalPar::FinishEvent() {}

void R3BRpcMapped2CalPar::FinishTask() {}

ClassImp(R3BRpcMapped2CalPar)
