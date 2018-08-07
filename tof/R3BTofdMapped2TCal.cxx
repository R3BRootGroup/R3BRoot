// ------------------------------------------------------------
// -----                  R3BTofdMapped2TCal                -----
// -----          Created Feb 4th 2016 by R.Plag          -----
// ------------------------------------------------------------

/* March 2016
 * Reconstructing data into PaddleItems. Each paddle holds two times,
 * (for PM1 and PM2). This allows to plot
 * the time differences e.g. via evt->Draw(...) interactively (aka without
 * looping over all channels) which is crucial for a quick check of the
 * detector status during the experiment.
 *  
 */


#include "R3BTofdMapped2TCal.h"

#include "R3BTCalEngine.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"

#define IS_NAN(x) TMath::IsNaN(x)

R3BTofdMapped2TCal::R3BTofdMapped2TCal()
    : FairTask("TofdTcal", 1)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPaddleCalData"))
    , fNofCalItems(0)
    , fNofTcalPars(0)
    , fNofEdges(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BTofdMapped2TCal::R3BTofdMapped2TCal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPaddleCalData"))
    , fNofCalItems(0)
    , fNofTcalPars(0)
    , fNofEdges(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BTofdMapped2TCal::~R3BTofdMapped2TCal()
{
    if (fCalItems)
    {
        delete fCalItems;
        fCalItems = NULL;
        fNofCalItems = 0;
    }
}

InitStatus R3BTofdMapped2TCal::Init()
{
	fNofTcalPars = fTcalPar->GetNumModulePar();
	if (fNofTcalPars==0)
	{
		LOG(ERROR) << "There are no TCal parameters in container TofdTCalPar" << FairLogger::endl;
		return kFATAL;
	}

    LOG(INFO) << "R3BTofdMapped2TCal::Init : read " << fNofTcalPars << " modules" << FairLogger::endl;
    

	// try to get a handle on the EventHeader. EventHeader may not be 
	// present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");


	// get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("TofdMapped");
    if (NULL == fMappedItems)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch TofdMapped not found");


	// request storage of Cal data in output tree
    mgr->Register("TofdCal", "Land", fCalItems, kTRUE);

    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BTofdMapped2TCal::SetParContainers()
{
    fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
    if (!fTcalPar)
    {
		LOG(ERROR) << "Could not get access to TofdTCalPar-Container." << FairLogger::endl;
		fNofTcalPars=0;
		return;
	}
}

InitStatus R3BTofdMapped2TCal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

 
void R3BTofdMapped2TCal::Exec(Option_t* option)
{
	// check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
		return;

    Int_t nHits = fMappedItems->GetEntriesFast();

   Int_t timefine = -10;
   Int_t timecoarse = -10;
   

   for (Int_t ihit = 0; ihit < nHits; ihit++)
   {
	   R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fMappedItems->At(ihit);
	   if (!hit) continue; // should be an assert()	

	   Int_t iPlane = hit->GetPlaneId(); // 1..n
	   Int_t iBar   = hit->GetBarId();   // 1..n
	   Int_t iSide  = hit->GetSideId();  // 1/2
	   Int_t iEdge  = hit->GetEdgeId();  // 1/2

	   if ((iPlane<1) || (iPlane>fNofPlanes))
	   {
		   LOG(DEBUG) << "R3BTofdMapped2TCal::Exec : Plane number out of range: " << 
			   iPlane << FairLogger::endl;
		   continue;
	   }       
	   if ((iBar<1) || (iBar>fPaddlesPerPlane))
	   {
		   LOG(DEBUG) << "R3BTofdMapped2TCal::Exec : Bar number out of range: " << 
			   iBar << ", "<<fPaddlesPerPlane<<FairLogger::endl;
		   continue;
	   }

	   R3BPaddleCalData* calData = new ((*fCalItems)[fNofCalItems++]) R3BPaddleCalData(iPlane,iBar);

	if (1 == iSide) {
		if (1 == iEdge) {
			timefine = hit->GetFineTime1LE();
			timecoarse = hit->GetCoarseTime1LE();
		} else {
			timefine = hit->GetFineTime1TE();
			timecoarse = hit->GetCoarseTime1TE();
		}
	} else {
		if (1 == iEdge) {
			timefine = hit->GetFineTime2LE();
			timecoarse = hit->GetCoarseTime2LE();
		} else {
			timefine = hit->GetFineTime2TE();
			timecoarse = hit->GetCoarseTime2TE();
		}
	}	
	 
	 R3BTCalModulePar *par = fTcalPar->GetModuleParAt(iPlane, iBar, 2 * iSide + iEdge - 2);
	 
	 if (!par)
	 {
		 LOG(INFO) << "R3BTofdMapped2TCal::Exec : Tcal par not found, Plane: " << 
			 iPlane << ", Bar: " << iBar << ", Edge: " << iEdge << FairLogger::endl;
		 continue;
	 }

	 // Convert TDC to [ns] ...
	 Double_t time_ns = par->GetTimeVFTX( timefine );
	 /*   
	      if (time_ns < 0. || time_ns > fClockFreq )
	      {
	      LOG(ERROR) << 
	      "R3BTofdMapped2TCal::Exec : bad time in ns: Plane= " << iPlane << 
	      ", bar= " << iBar << ",tube= " << (tube+1) <<", nHits= "<<nHits<<
	      ", time in channels = " << timefine <<
	      ", time in ns = " << time_ns  << FairLogger::endl;
	      continue;
	      }
	  */ 
	 // ... and add clock time
	 time_ns = fClockFreq - time_ns + timecoarse * fClockFreq;

	 calData->SetTime(hit->GetSideId() - 1, hit->GetEdgeId() - 1, time_ns);

	  /*       
	   // convert times to ns
	   for (int tube=0;tube<2;tube++){ // PM1 and PM2   
		   for (int edge=0;edge<2;edge++) // loop over leading and trailing edge
	       {
	
			   // PM1:
		       R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iPlane, iBar, tube*2 + edge + 1); // 1..4
		       
		       if (!par)
		       {
		           LOG(INFO) << "R3BTofdMapped2TCal::Exec : Tcal par not found, Plane: " << 
		           iPlane << ", Bar: " << iBar << ", Tube: " << (tube+1) << FairLogger::endl;
		           continue;
		       }
		       
		       // Convert TDC to [ns] ...
		       Double_t time_ns = par->GetTimeVFTX( hit->GetFineTime(tube , edge) );
		      

           
		       if (time_ns < 0. || time_ns > fClockFreq )
		       {
		           LOG(ERROR) << 
		           "R3BTofdMapped2TCal::Exec : bad time in ns: Plane= " << iPlane << 
		           ", bar= " << iBar << ",tube= " << (tube+1) <<
		           ", time in channels = " << hit->GetFineTime(tube,edge) <<
		           ", time in ns = " << time_ns  << FairLogger::endl;
		           continue;
		       }
		      
			   // ... and add clock time
		       time_ns = fClockFreq - time_ns + hit->GetCoarseTime(tube , edge) * fClockFreq;
		       
		       calData->SetTime(tube,edge,time_ns);
	
		   }
        }
        */
       
    }

}

void R3BTofdMapped2TCal::FinishEvent()
{
    if (fCalItems)
    {
        fCalItems->Clear();
        fNofCalItems = 0;
    }

}

void R3BTofdMapped2TCal::FinishTask()
{
}

ClassImp(R3BTofdMapped2TCal)
