// ----------------------------------------------------------------
// -----                     R3BLandTcalFill                  -----
// -----             Created 04-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#include "R3BLandTcalFill.h"
#include "R3BNeulandMappedItem.h"
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

R3BLandTcalFill::R3BLandTcalFill()
    : fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPMTs(0)
    , fNof17(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BLandTcalFill::R3BLandTcalFill(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPMTs(0)
    , fNof17(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BLandTcalFill::~R3BLandTcalFill()
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

InitStatus R3BLandTcalFill::Init()
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

    fEngine = new R3BTCalEngine(fCal_Par, fNofPMTs + fNof17, fMinStats);

    return kSUCCESS;
}

void R3BLandTcalFill::Exec(Option_t* option)
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

    R3BNeulandMappedItem* hit;
    Int_t iBar;
    Int_t iSide;
    Int_t channel;

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BNeulandMappedItem*)fHits->At(i);
        if (!hit)
        {
            continue;
        }

        // Check bar ID
        iBar = hit->GetBarId();
        if ((iBar - 1) >= (fNofPMTs / 2))
        {
            LOG(ERROR) << "R3BLandTcalFill::Exec() : wrong bar ID: " << iBar << FairLogger::endl;
            continue;
        }

        if (hit->Is17())
        {
            // 17-th channel
            channel = fNofPMTs + hit->GetGtb() * 20 + hit->GetTacAddr();
        }
        else
        {
            // PMT signal
            iSide = hit->GetSide();
            channel = (Double_t)fNofPMTs / 2 * (iSide - 1) + iBar - 1;
        }

        // Check validity of module
        if (channel < 0 || channel >= (2 * fNofPMTs))
        {
            LOG(INFO) << "Bar:" << iBar << "  Side:" << iSide << "  " << channel << "   " << (2 * fNofPMTs + fNof17)
                      << "   " << hit->GetGtb() << FairLogger::endl;
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal detector ID...");
        }

        // Fill TAC histogram
        fEngine->Fill(channel, hit->GetTacData());
    }

    // Increment events
    fNEvents += 1;
}

void R3BLandTcalFill::FinishEvent()
{
}

void R3BLandTcalFill::FinishTask()
{
    fEngine->CalculateParamTacquila();
}

ClassImp(R3BLandTcalFill)
