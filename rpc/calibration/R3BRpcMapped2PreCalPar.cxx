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

#include "R3BRpcMapped2PreCalPar.h"

#include "R3BRpcStripMappedData.h"
#include "R3BRpcPmtMappedData.h"
#include "R3BRpcRefMappedData.h"

#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include <iostream>
#include <stdlib.h>

R3BRpcMapped2PreCalPar::R3BRpcMapped2PreCalPar()
    : R3BRpcMapped2PreCalPar("R3B RPC Calibration Parameters Finder ", 1)
{
}

R3BRpcMapped2PreCalPar::R3BRpcMapped2PreCalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTCalPar(NULL)
    , fEngine()
    , fMappedStripDataCA(NULL)
    , fMappedPmtDataCA(NULL)
    , fMappedRefDataCA(NULL)
    , fNumChannels(64)
    , fDebugMode(0)
{
}

R3BRpcMapped2PreCalPar::~R3BRpcMapped2PreCalPar()
{
    LOG(INFO) << "R3BRpcMapped2PreCalPar: Delete instance";
    
    delete fEngine;
}

InitStatus R3BRpcMapped2PreCalPar::Init()
{
    LOG(INFO) << "R3BRpcMapped2PreCalPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fMappedStripDataCA = (TClonesArray*)rootManager->GetObject("RpcStripMappedData");
    if (!fMappedStripDataCA)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() fMappedStripDataCA not found";
        return kFATAL;
    }

    fMappedPmtDataCA = (TClonesArray*)rootManager->GetObject("RpcPmtMappedData");
    if (!fMappedPmtDataCA)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() fMappedPmtDataCA not found";
        return kFATAL;
    }


    fMappedRefDataCA = (TClonesArray*)rootManager->GetObject("RpcRefMappedData");
    if (!fMappedRefDataCA)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() fMappedRefDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTCalPar = (R3BTCalPar*)rtdb->getContainer("RpcTCalPar");
    if (!fTCalPar)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() Couldn't get handle on RpcTCalPar container";
        return kFATAL;
    }

    int fMinStats = 10;

    fEngine = new R3BTCalEngine(fTCalPar, fMinStats);

    return kSUCCESS;
}

InitStatus R3BRpcMapped2PreCalPar::ReInit()
{
    return kSUCCESS;
}

void R3BRpcMapped2PreCalPar::Exec(Option_t* opt)
{

    //loop over the 3 mapped structures

    //loop over strip data
    Int_t nHits = fMappedStripDataCA->GetEntries();
    UInt_t iDetector = 0;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = (R3BRpcStripMappedData*)(fMappedStripDataCA->At(i));

        UInt_t iStrip = map1->GetStripId();   // now 1..41
        UInt_t iEdge_Side = map1->GetEdge()*2 + map1->GetSide();           // 0,3

        fEngine->Fill(iDetector+1, iStrip, iEdge_Side+1, map1->GetFineTime());
    }

    //loop over pmt data
    nHits = fMappedPmtDataCA->GetEntries();
    iDetector = 1;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map2 = (R3BRpcPmtMappedData*)(fMappedPmtDataCA->At(i));

        UInt_t iPmt = map2->GetChannelId();   // now 1..3
        UInt_t iEdge_Side = map2->GetEdge()*2 + map2->GetSide();           // 0,3

        fEngine->Fill(iDetector +1 ,iPmt, iEdge_Side+1, map2->GetFineTime());
    }

    //loop over ref data
    iDetector = 2;
    nHits = fMappedRefDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map3 = (R3BRpcRefMappedData*)(fMappedRefDataCA->At(i));

        UInt_t iFpga = map3->GetChannelId();   // now 1..41
        UInt_t iEdge_Side = 0;           // 0

        fEngine->Fill(iDetector+1,iFpga, iEdge_Side+1, map3->GetFineTime());
    }
}

void R3BRpcMapped2PreCalPar::Reset() {}

void R3BRpcMapped2PreCalPar::FinishEvent() {}

void R3BRpcMapped2PreCalPar::FinishTask() {
    fEngine->CalculateParamVFTX();
    fTCalPar->printParams();
}

ClassImp(R3BRpcMapped2PreCalPar)
