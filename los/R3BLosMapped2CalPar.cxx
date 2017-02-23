// ----------------------------------------------------------------
// -----            R3BLosMapped2CalPar (7ps VFTX)            -----
// -----           Created Feb 4th 2016 by R.Plag             -----
// ----------------------------------------------------------------

/* Some notes:
 * 
 * There are different versions of VFTX:
 * 10px delivering 8 leading edges in Ch 1-8 and 8 trailing edges in
 *      Ch 9-16. This one was used for LOS1 but is not used in the analysis
 *  7px delivering 8 leading edges only. Used for LOS2 and this is the
 *      LOS we use for analysis.
 * 
 * For s438b we had no synchronisation between 50 MHz tacquila clock 
 * and the 200 MHz VFTX clock so we need to always subtract the time
 * of the master trigger from the LOS time.
 * The master trigger is on the last channel of the VFTX and handled
 * as 5th los channel.
 * 
 * This file handles 7ps VFTX only, hence we have one edge per channel.
 * 
 */

#include "R3BLosMapped2CalPar.h"
#include "R3BLosMappedData.h"
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

R3BLosMapped2CalPar::R3BLosMapped2CalPar()
    : FairTask("R3BLosMapped2CalPar", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofDetectors(0)
    , fNofChannels(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BLosMapped2CalPar::R3BLosMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofDetectors(0)
    , fNofChannels(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BLosMapped2CalPar::~R3BLosMapped2CalPar()
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

InitStatus R3BLosMapped2CalPar::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    
    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
	// may be = NULL!
	
    fMapped = (TClonesArray*)rm->GetObject("LosMapped");
    if (!fMapped)
    {
        return kFATAL;
    }

    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("LosTCalPar");
    fCal_Par->setChanged();

    if (!fNofModules)
    {
		LOG(ERROR) << "R3BLosMapped2CalPar::Init() Number of modules not set. " << FairLogger::endl;
        return kFATAL;
    }
    
    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);
//    fEngine = new R3BTCalEngine(fCal_Par, fNofModules, fMinStats);

    return kSUCCESS;
}

void R3BLosMapped2CalPar::Exec(Option_t* option)
{
	// test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger)) 
		return;

    Int_t nHits = fMapped->GetEntries();

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
		
        R3BLosMappedData* hit = (R3BLosMappedData*)fMapped->At(i);
        if (!hit) continue; // should not happen

        
        // channel numbers are supposed to be 1-based (1..n)
        UChar_t iDetector = hit->GetDetector()-1; // now 0..n-1
        UChar_t iChannel  = hit->GetChannel()-1;  // now 0..n-1
                
        
        if (iDetector>=fNofDetectors) // this also errors for iDetector==0
        {
            LOG(ERROR) << "R3BLosMapped2CalPar::Exec() : more detectors than expected! Det: " << (iDetector+1) << " allowed are 1.." << fNofDetectors << FairLogger::endl;
            continue;
        }
        if (iChannel>=fNofChannels) // same here
        {
            LOG(ERROR) << "R3BLosMapped2CalPar::Exec() : more bars than expected! Det: " << (iDetector+1) << " allowed are 1.." << fNofChannels << FairLogger::endl;
            continue;
        }


		// remember: only one edge per channel
		UInt_t iModule = iDetector * fNofChannels + iChannel; // 0..n-1
		
        // Check validity of module
        if (iModule >= fNofModules)
        {
            LOG(ERROR) << "Detector: " << iDetector << "  Ch: " << iChannel << " => Module: " << iModule << " out of range."  << FairLogger::endl;
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal detector ID...");
        }

        // Fill TAC histogram
        //fEngine->Fill(iModule, hit->GetTimeFine());
        // *** new ***
        fEngine->Fill(iDetector+1, iChannel + 1, 1, hit->GetTimeFine());
    }

    // Increment events
    fNEvents += 1;
}

void R3BLosMapped2CalPar::FinishEvent()
{
}

void R3BLosMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fCal_Par->printParams();
}

ClassImp(R3BLosMapped2CalPar)
