// ----------------------------------------------------------------
// -----                 R3BNeulandMapped2CalPar              -----
// -----             Created 04-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#include "R3BNeulandMapped2CalPar.h"
#include "R3BNeulandMappedData.h"
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

R3BNeulandMapped2CalPar::R3BNeulandMapped2CalPar()
    : fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPMTs(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BNeulandMapped2CalPar::R3BNeulandMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPMTs(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BNeulandMapped2CalPar::~R3BNeulandMapped2CalPar()
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

InitStatus R3BNeulandMapped2CalPar::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
    if (!header)
    {
        return kFATAL;
    }
    fHits = (TClonesArray*)rm->GetObject("LandRawHitMapped");
    if (!fHits)
    {
        return kFATAL;
    }

    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("LandTCalPar");
    fCal_Par->setChanged();

    fEngine = new R3BTCalEngine(fCal_Par, 2*fNofPMTs, fMinStats);

    return kSUCCESS;
}

void R3BNeulandMapped2CalPar::Exec(Option_t* option)
{
    if (fTrigger >= 0)
    {
        if (header->GetTrigger() != fTrigger)
        {
            return;
        }
    }

    Int_t nHits = fHits->GetEntries();
    if (nHits > (fNofPMTs / 2))
    {
        return;
    }

    R3BNeulandMappedData* hit;
    Int_t iBar;
    Int_t iSide;
    Int_t channel;

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BNeulandMappedData*)fHits->At(i);
        if (!hit)
        {
            continue;
        }

        // Check bar ID
        iBar = hit->GetBarId();
        iSide = hit->GetSide();
        if ((iBar - 1) >= (fNofPMTs / 2))
        {
            LOG(ERROR) << "R3BNeulandMapped2CalPar::Exec() : wrong bar ID: " << iBar << FairLogger::endl;
            continue;
        }

        if (hit->Is17())
        {
            // 17-th channel
            channel = fNofPMTs + hit->GetSam() * (MAX_TACQUILA_MODULE + 1) * (MAX_TACQUILA_GTB + 1) + hit->GetGtb() * (MAX_TACQUILA_MODULE + 1) + hit->GetTacAddr();
        }
        else
        {
            // PMT signal
            channel = (Double_t)fNofPMTs / 2 * (iSide - 1) + iBar - 1;
        }

        // Check validity of module
        if (channel < 0 || channel >= (2 * fNofPMTs))
        {
            LOG(INFO) << "Bar:" << iBar << "  Side:" << iSide << "  " << channel << FairLogger::endl;
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal detector ID...");
        }

        // Fill TAC histogram
        fEngine->Fill(channel, hit->GetTacData());
    }

    // Increment events
    fNEvents += 1;
}

void R3BNeulandMapped2CalPar::FinishEvent()
{
}

void R3BNeulandMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamTacquila();
}

ClassImp(R3BNeulandMapped2CalPar)
