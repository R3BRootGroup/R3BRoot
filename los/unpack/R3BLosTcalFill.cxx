// ----------------------------------------------------------------
// -----                      R3BLosTcalFill                  -----
// -----             Created 18-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#include "R3BLosTcalFill.h"
#include "R3BLosRawHit.h"

#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunIdGenerator.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"

R3BLosTcalFill::R3BLosTcalFill()
    : fUpdateRate(1000000)
    , fMinStats(100000)
    , fNofCh(0)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fEngine(NULL)
{
}

R3BLosTcalFill::R3BLosTcalFill(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fNofCh(0)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fEngine(NULL)
{
}

R3BLosTcalFill::~R3BLosTcalFill()
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

InitStatus R3BLosTcalFill::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    fHits = (TClonesArray*)rm->GetObject("LosRawHit");
    if (!fHits)
    {
        return kFATAL;
    }
    
    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("LosTCalPar");
    fCal_Par->setChanged();

    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BLosTcalFill::Exec(Option_t* option)
{
    Int_t nHits = fHits->GetEntries();
    R3BLosRawHit* hit;
    Int_t iCh;
    Int_t channel;

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BLosRawHit*)fHits->At(i);
        if (!hit)
        {
            continue;
        }

        // Check bar ID
        iCh = hit->GetChannel();
        if (iCh >= fNofCh)
        {
            LOG(ERROR) << "R3BLosTcalFill::Exec() : wrong channel: " << iCh << FairLogger::endl;
            continue;
        }

        channel = iCh;

        // Check validity of module
        if (channel < 0 || channel >= fNofCh)
        {
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal detector ID...");
            exit(-1);
        }

        // Fill TAC histogram
        fEngine->Fill(1, channel + 1, 1, hit->GetTdc());
    }

    // Increment events
    fNEvents += 1;
}

void R3BLosTcalFill::FinishEvent()
{
}

void R3BLosTcalFill::FinishTask()
{
    fEngine->CalculateParamVFTX();
}

ClassImp(R3BLosTcalFill)
