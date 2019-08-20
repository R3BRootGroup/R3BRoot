// ----------------------------------------------------------------
// -----            R3BSci8Mapped2CalPar (7ps VFTX)            -----
// -----           Created August 7th by A. Kelic-Heil        -----
// -----           Following R3BLosMapped2CalPar              -----
// ----------------------------------------------------------------

/* Some notes:
 * 
 * This file handles 7ps VFTX and TAMEX2, hence we have three times per channel.
 * 
 */

#include "R3BSci8Mapped2CalPar.h"
#include "R3BSci8MappedData.h"
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
#include "math.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BSci8Mapped2CalPar::R3BSci8Mapped2CalPar()
    : FairTask("R3BSci8Mapped2CalPar", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofDetectors(0)
    , fNofChannels(0)
    , fNofTypes(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BSci8Mapped2CalPar::R3BSci8Mapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofDetectors(0)
    , fNofChannels(0)
    , fNofTypes(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BSci8Mapped2CalPar::~R3BSci8Mapped2CalPar()
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

InitStatus R3BSci8Mapped2CalPar::Init()
{
	
    for(UInt_t i = 0; i < 2; i++)
    {
     for(UInt_t k = 0; k < 3; k++)
     {
		Icount[i][k] = 0; 
	 }
	} 	 	
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    
    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
	// may be = NULL!


    fMapped = (TClonesArray*)rm->GetObject("Sci8Mapped");    
    if (!fMapped)
    {
        return kFATAL;
    }

    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("Sci8TCalPar");
    fCal_Par->setChanged();

    if (!fNofModules)
    {
		LOG(ERROR) << "R3BSci8Mapped2CalPar::Init() Number of modules not set. ";
        return kFATAL;
    }
    
    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BSci8Mapped2CalPar::Exec(Option_t* option)
{
	// test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger)) 
		return;

    UInt_t nHits = fMapped->GetEntries();

    // Loop over mapped hits
    for (UInt_t i = 0; i < nHits; i++)
    {
		
        R3BSci8MappedData* hit = (R3BSci8MappedData*)fMapped->At(i);
        if (!hit) 
        {
			continue; // should not happen
	     }		

        
        // channel numbers are supposed to be 1-based (1..n)
        UInt_t iDetector = hit->GetDetector()-1; // now 0..n-1
        UInt_t iChannel  = hit->GetChannel()-1;  // now 0..n-1
        UInt_t iType     = hit->GetType();       // 0,1,2
                         
        if (iDetector > (fNofDetectors-1)) 
        {
            LOG(ERROR) << "R3BSci8Mapped2CalPar::Exec() : more detectors than expected! Det: " << (iDetector+1) << " allowed are 1.." << fNofDetectors;
            continue;
        }
        if (iChannel > (fNofChannels-1)) 
        {
            LOG(ERROR) << "R3BSci8Mapped2CalPar::Exec() : more channels than expected! Channel: " << (iChannel+1) << " allowed are 1.." << fNofChannels;
            continue;
        }

        if (iType > (fNofTypes-1)) 
        {
            LOG(ERROR) << "R3BSci8Mapped2CalPar::Exec() : more time-types than expected! Type: " << iType << " allowed are 0.." << (fNofTypes-1);
            continue;
        }
  

        // Fill TAC histogram for VFTX and TAMEX
         
         Icount[iChannel][iType]++;
        
        fEngine->Fill(iDetector+1, iChannel + 1, iType + 1, hit->GetTimeFine());
                      
              
    }

    // Increment events
    fNEvents += 1;
}

void R3BSci8Mapped2CalPar::FinishEvent()
{
	
	
}

void R3BSci8Mapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    
    
    fCal_Par->printParams();
    
    
    for(UInt_t i = 0; i < 2; i++)
    {
     for(UInt_t k = 0; k < 3; k++)
     {
		if(Icount[i][k] > 0) cout<<"R3BSci8Mapped2CalPar::FinishTask  Channel: "<<i<<", Type: "<<k<<", Count: "<<Icount[i][k]<<endl; 
	 }
	} 	 
}

ClassImp(R3BSci8Mapped2CalPar)
