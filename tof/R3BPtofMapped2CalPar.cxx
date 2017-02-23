// ----------------------------------------------------------------
// -----            R3BPtofMapped2TCalPar (TAMEX)             -----
// ----------------------------------------------------------------


#include "R3BPtofMapped2CalPar.h"
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

R3BPtofMapped2CalPar::R3BPtofMapped2CalPar()
    : FairTask("R3BPtofMapped2CalPar", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPlanes(0)
    , fPaddlesPerPlane(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BPtofMapped2CalPar::R3BPtofMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPlanes(0)
    , fPaddlesPerPlane(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BPtofMapped2CalPar::~R3BPtofMapped2CalPar()
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


/*  !!!!!!!!!!!!!
 * Define new container in: 
 * /R3BRoot/tcal/R3BTCalContFact.cxx
 */
InitStatus R3BPtofMapped2CalPar::Init()
{	
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    
	
    fMapped = (TClonesArray*)rm->GetObject("PtofMapped");
    if (!fMapped)
    {
        return kFATAL;
    }

	// container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
	// to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("PtofTCalPar");
	if (!fCal_Par)
	{
		LOG(ERROR) << "R3BPtofMapped2CalPar::Init() Couldn't get handle on PtofTCalPar. " << FairLogger::endl;
        return kFATAL;
		
	}
	
    fCal_Par->setChanged();

    if (!fNofModules)
    {
		LOG(ERROR) << "R3BPtofMapped2CalPar::Init() Number of modules not set. " << FairLogger::endl;
        return kFATAL;
    }
    
    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BPtofMapped2CalPar::Exec(Option_t* option)
{

    Int_t nHits = fMapped->GetEntries();

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
		
        R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fMapped->At(i);
        if (!hit) continue; // should not happen

        Int_t iPlane = hit->GetPlaneId(); // 1..n
        Int_t iBar   = hit->GetBarId();   // 1..n
        
        if (iPlane>=fNofPlanes) // this also errors for iDetector==0
        {
            LOG(ERROR) << "R3BPtofMapped2CalPar::Exec() : more detectors than expected! Det: " << iPlane << " allowed are 1.." << fNofPlanes << FairLogger::endl;
            continue;
        }
        if (iBar>=fPaddlesPerPlane) // same here
        {
            LOG(ERROR) << "R3BPtofMapped2CalPar::Exec() : more bars then expected! Det: " << iBar << " allowed are 1.." << fPaddlesPerPlane << FairLogger::endl;
            continue;
        }


		for (Int_t edge=0;edge<2;edge++)
		{
	
	        // Fill TAC histogram
	        //fEngine->Fill(iModule, hit->GetFineTime(edge));
	        fEngine->Fill(iPlane, iBar, 0 + edge+1, hit->GetFineTime(0,edge));
	        fEngine->Fill(iPlane, iBar, 2 + edge+1, hit->GetFineTime(1,edge));

	    }
    }

    // Increment events
    fNEvents += 1;
}

void R3BPtofMapped2CalPar::FinishEvent()
{
}

void R3BPtofMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fCal_Par->printParams();
}

ClassImp(R3BPtofMapped2CalPar)
