// ----------------------------------------------------------------
// -----                  R3BNeulandTcalFill                  -----
// -----             Created 27-01-2015 by M.Heil             -----
// ----------------------------------------------------------------

#include "R3BNeulandTcalFill.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BEventHeader.h"
#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunIdGenerator.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TF1.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BNeulandTcalFill::R3BNeulandTcalFill()
    : FairTask("R3BNeulandTCalFill", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPlanes(0)
    , fNofBars(0)
    , fNof17(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BNeulandTcalFill::R3BNeulandTcalFill(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPlanes(0)
    , fNofBars(0)
    , fNof17(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BNeulandTcalFill::~R3BNeulandTcalFill()
{
    if (fCal_Par)
    {
        delete fCal_Par;
    }
    if (fEngine)
    {
        delete fEngine;
    }
}

InitStatus R3BNeulandTcalFill::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
 /*   if (!header)
    {
        return kFATAL;
    }*/
    fHits = (TClonesArray*)rm->GetObject("NeulandTamexMappedData");
    if (!fHits)
    {
        return kFATAL;
    }

    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("LandTCalPar");
    fCal_Par->setChanged();

    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BNeulandTcalFill::Exec(Option_t*)
{
 /*   if (fTrigger >= 0)
    {
        if (header->GetTrigger() != fTrigger)
        {
            return;
        }
    }*/

    Int_t nHits = fHits->GetEntries();
//    LOG(INFO) << "number of hits:" << nHits << "   "  << FairLogger::endl;

/*
    if (nHits > (fNofPMTs / 2))
    {
        return;
    }
*/

    R3BPaddleTamexMappedData* hit;
    Int_t iPlane;
    Int_t iBar;
    Int_t iSide;
    

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BPaddleTamexMappedData*)fHits->At(i);
        if (!hit)
        {
            continue;
        }

        iPlane = hit->GetPlaneId();
        iBar = hit->GetBarId();
        
        if (iPlane>fNofPlanes)
        {
            LOG(ERROR) << "R3BNeulandTcalFill::Exec() : more planes then expected! Plane: " << iPlane << FairLogger::endl;
            continue;
        }
        if (iBar>fNofBars)
        {
            LOG(ERROR) << "R3BNeulandTcalFill::Exec() : more bars then expected! Plane: " << iBar << FairLogger::endl;
            continue;
        }

//        if (hit->Is17())
//        {
            // 17-th channel
//MH            channel = fNofPMTs + hit->GetGtb() * 20 + hit->GetTacAddr();
//        }
//        else
//        {
            // PMT signal
            iSide = hit->GetSide();
        
//            LOG(INFO) << "Plane: " << iPlane << " Bar: " << iBar << " Side: " << iSide << " Cal channel: " << channel << "   "  << FairLogger::endl;
             
//        }

        // Fill TAC histogram
        fEngine->Fill(iPlane, iBar, iSide, hit->GetFineTimeLE());
        fEngine->Fill(iPlane, iBar, iSide + 2, hit->GetFineTimeTE());
    }

    // Increment events
    fNEvents += 1;
}

void R3BNeulandTcalFill::FinishEvent()
{
}

void R3BNeulandTcalFill::FinishTask()
{
    fEngine->CalculateParamVFTX();
}

ClassImp(R3BNeulandTcalFill)
