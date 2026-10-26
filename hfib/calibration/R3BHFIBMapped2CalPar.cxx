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

#include "R3BHFIBMapped2CalPar.h"

#include "R3BHFIBMappedData.h"

#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include <iostream>
#include <stdlib.h>

R3BHFIBMapped2CalPar::R3BHFIBMapped2CalPar()
    : R3BHFIBMapped2CalPar("R3B HFIB Calibration Parameters Finder ", 1)
{
}

R3BHFIBMapped2CalPar::R3BHFIBMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

R3BHFIBMapped2CalPar::~R3BHFIBMapped2CalPar()
{
    LOG(info) << "R3BHFIBMapped2CalPar: Delete instance";

    delete fEngine;
}

InitStatus R3BHFIBMapped2CalPar::Init()
{
    LOG(info) << "R3BHFIBMapped2CalPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(error) << "R3BHFIBMapped2CalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BHFIBMappedData"));
    if (!fMappedDataCA)
    {
        LOG(error) << "R3BHFIBMapped2CalPar::Init() fMappedDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BHFIBMapped2CalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTCalPar = dynamic_cast<R3BTCalPar*>(rtdb->getContainer("HFIBTCalPar"));
    if (!fTCalPar)
    {
        LOG(error) << "R3BHFIBMapped2CalPar::Init() Couldn't get handle on HFIBTCalPar container";
        return kFATAL;
    }

    int fMinStats = 100;

    fEngine = new R3BTCalEngine(fTCalPar, fMinStats);

    return kSUCCESS;
}

InitStatus R3BHFIBMapped2CalPar::ReInit() { return kSUCCESS; }

void R3BHFIBMapped2CalPar::Exec(Option_t* opt)
{

    Int_t nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BHFIBMappedData*>(fMappedDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        UInt_t iFib;
        UInt_t tdc;

        iFib = map1->GetChannelId();
        tdc = map1->GetTDC();
        UInt_t iEdge_Side = map1->GetEdge();
        UInt_t id = iDetector * 10 + tdc;
        fEngine->Fill(id, iFib, iEdge_Side + 1, map1->GetFineTime());
    }
}

void R3BHFIBMapped2CalPar::FinishEvent() {}

void R3BHFIBMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamTDCV4();
    fTCalPar->printParams();
}

ClassImp(R3BHFIBMapped2CalPar)
