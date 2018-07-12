// ----------------------------------------------------------------
// -----            R3BTofdMapped2TCalPar (7ps VFTX)            -----
// -----           Created Apr 2016 by R.Plag             -----
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

#include "R3BTofdMapped2TCalPar.h"
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

R3BTofdMapped2TCalPar::R3BTofdMapped2TCalPar()
    : FairTask("R3BTofdMapped2TCalPar", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPlanes(0)
    , fPaddlesPerPlane(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BTofdMapped2TCalPar::R3BTofdMapped2TCalPar(const char* name, Int_t iVerbose)
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

R3BTofdMapped2TCalPar::~R3BTofdMapped2TCalPar()
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

InitStatus R3BTofdMapped2TCalPar::Init()
{	
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    
    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
	// may be = NULL!
	
    fMapped = (TClonesArray*)rm->GetObject("TofdMapped");
    if (!fMapped)
    {
        return kFATAL;
    }

	// container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
	// to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
	if (!fCal_Par)
	{
		LOG(ERROR) << "R3BTofdMapped2TCalPar::Init() Couldn't get handle on TofdTCalPar. " << FairLogger::endl;
        return kFATAL;
		
	}
	
    fCal_Par->setChanged();

    if (!fNofModules)
    {
		LOG(ERROR) << "R3BTofdMapped2TCalPar::Init() Number of modules not set. " << FairLogger::endl;
        return kFATAL;
    }
    
    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BTofdMapped2TCalPar::Exec(Option_t* option)
{
	// test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger)) 
		return;

    Int_t nHits = fMapped->GetEntries();

    Int_t Nbar = nHits / 4; // 4 times (L/T PM1 & L/T PM2)
    Int_t timetemp;
    Int_t edge;
    
    if(nHits%4 != 0){
	//	cout<<"R3BTofdMapped2TCalPar:: Some channels are missing, event skipped!!! "<< nHits<<endl;
	//	return;
	}
	
     
    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
		
        R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fMapped->At(i);
        if (!hit) continue; // should not happen

        Int_t iPlane = hit->GetPlaneId(); // 1..n
        Int_t iBar   = hit->GetBarId();   // 1..n
        
        
        if (iPlane>fNofPlanes) // this also errors for iDetector==0
        {
            LOG(ERROR) << "R3BTofdMapped2TCalPar::Exec() : more planes than expected! Plane: " << iPlane << " allowed are 1.." << fNofPlanes << FairLogger::endl;
            continue;
        }
        if (iBar>fPaddlesPerPlane) // same here
        {
            LOG(ERROR) << "R3BTofdMapped2TCalPar::Exec() : more bars then expected! Det: " << iBar << " allowed are 1.." << fPaddlesPerPlane << FairLogger::endl;
            continue;
        }

         if(i < Nbar){ 
           timetemp = hit->GetFineTime1LE();
           edge = 1;
	     }
	     if(i < 2*Nbar && i >= Nbar){
		   timetemp = hit->GetFineTime1TE();
		   edge = 2;
		 }  
	     if(i < 3*Nbar && i >= 2*Nbar){ 
			timetemp = hit->GetFineTime2LE();
			edge = 3;
		 }	
	     if(i < 4*Nbar && i >= 3*Nbar){
			timetemp = hit->GetFineTime2TE();
			edge = 4;
		}	
	  
      //   cout<<"Mappet2CalPar: "<<i<<", "<<iBar<<", "<<edge<<", "<<timetemp<<endl;
         fEngine->Fill(iPlane, iBar, edge, timetemp);

/*	
		for (Int_t edge=0;edge<2;edge++)
		{	
	        // Fill TAC histogram
	        //fEngine->Fill(iModule, hit->GetFineTime(edge));
	        fEngine->Fill(iPlane, iBar, 0 + edge+1, hit->GetFineTime(0,edge));
	        fEngine->Fill(iPlane, iBar, 2 + edge+1, hit->GetFineTime(1,edge));
	    }
	  */
    }

    // Increment events
    fNEvents += 1;
}

void R3BTofdMapped2TCalPar::FinishEvent()
{
}

void R3BTofdMapped2TCalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fCal_Par->printParams();
}

ClassImp(R3BTofdMapped2TCalPar)
