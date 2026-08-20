/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
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

#include "R3BDMMapped2CalPar.h"

#include "R3BDMMappedData.h"

#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include <iostream>
#include <stdlib.h>

R3BDMMapped2CalPar::R3BDMMapped2CalPar()
    : R3BDMMapped2CalPar("R3B DM Calibration Parameters Finder ", 1)
{
}

R3BDMMapped2CalPar::R3BDMMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

R3BDMMapped2CalPar::~R3BDMMapped2CalPar()
{
    LOG(info) << "R3BDMMapped2CalPar: Delete instance";

    delete fEngine;
}

InitStatus R3BDMMapped2CalPar::Init()
{
    LOG(info) << "R3BDMMapped2CalPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(error) << "R3BDMMapped2CalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BDMMappedData"));
    if (!fMappedDataCA)
    {
        LOG(error) << "R3BDMMapped2CalPar::Init() fMappedDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BDMMapped2CalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTCalPar = dynamic_cast<R3BTCalPar*>(rtdb->getContainer("DMTCalPar"));
    if (!fTCalPar)
    {
        LOG(error) << "R3BDMMapped2CalPar::Init() Couldn't get handle on DMTCalPar container";
        return kFATAL;
    }

    int fMinStats = 100;

    fEngine = new R3BTCalEngine(fTCalPar, fMinStats);

    return kSUCCESS;
}

InitStatus R3BDMMapped2CalPar::ReInit() { return kSUCCESS; }

void R3BDMMapped2CalPar::Exec(Option_t* opt)
{

    Int_t nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BDMMappedData*>(fMappedDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        UInt_t iCh = map1->GetChannelId();
        UInt_t iEdge_Side = map1->GetEdge();
        fEngine->Fill(iDetector + 1, iCh, iEdge_Side + 1, map1->GetFineTime());
    }
}

void R3BDMMapped2CalPar::FinishEvent() {}

void R3BDMMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fTCalPar->printParams();
}

ClassImp(R3BDMMapped2CalPar)
