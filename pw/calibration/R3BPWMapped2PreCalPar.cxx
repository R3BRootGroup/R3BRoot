/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
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

#include "R3BPWMapped2PreCalPar.h"

#include "R3BPWMappedData.h"

#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include <iostream>
#include <stdlib.h>

R3BPWMapped2PreCalPar::R3BPWMapped2PreCalPar()
    : R3BPWMapped2PreCalPar("R3B PW Calibration Parameters Finder ", 1)
{
}

R3BPWMapped2PreCalPar::R3BPWMapped2PreCalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

R3BPWMapped2PreCalPar::~R3BPWMapped2PreCalPar()
{
    LOG(info) << "R3BPWMapped2PreCalPar: Delete instance";

    delete fEngine;
}

InitStatus R3BPWMapped2PreCalPar::Init()
{
    LOG(info) << "R3BPWMapped2PreCalPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(error) << "R3BPWMapped2PreCalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BPWMappedData"));
    if (!fMappedDataCA)
    {
        LOG(error) << "R3BPWMapped2PreCalPar::Init() fMappedDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BPWMapped2PreCalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTCalPar = dynamic_cast<R3BTCalPar*>(rtdb->getContainer("PWTCalPar"));
    if (!fTCalPar)
    {
        LOG(error) << "R3BPWMapped2PreCalPar::Init() Couldn't get handle on PWTCalPar container";
        return kFATAL;
    }

    int fMinStats = 10000;

    fEngine = new R3BTCalEngine(fTCalPar, fMinStats);

    return kSUCCESS;
}

InitStatus R3BPWMapped2PreCalPar::ReInit() { return kSUCCESS; }

void R3BPWMapped2PreCalPar::Exec(Option_t* opt)
{

    Int_t nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BPWMappedData*>(fMappedDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        UInt_t iCh = map1->GetChannelId();
        UInt_t iEdge_Side = map1->GetEdge() * 2;
        fEngine->Fill(iDetector + 1, iCh, iEdge_Side + 1, map1->GetFineTime());
    }
}

void R3BPWMapped2PreCalPar::Reset() {}

void R3BPWMapped2PreCalPar::FinishEvent() {}

void R3BPWMapped2PreCalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fTCalPar->printParams();
}

ClassImp(R3BPWMapped2PreCalPar)
