// ------------------------------------------------------------
// -----                  R3BTofdMapped2TCal                -----
// -----          Created Feb 4th 2016 by R.Plag          -----
// ------------------------------------------------------------

/* March 2016
 * Reconstructing data into PaddleItems. Each paddle holds two times,
 * (for PM1 and PM2). This allows to plot
 * the time differences e.g. via cbmsim->Draw(...) interactively (aka without
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

/* In this loop we combine left and right PMs into one struct "R3BPaddleCalData".
 * And again: multi hit data. How to do the reconstruction?
 * 
 * This detector shouldnt have that much data.
 * So in 0th-order, we can simply loop over all the data we have already
 * converted and see if something fits together.
 */
 
#define COINC_TIME_NS 1000 
#define PM1 1
#define PM2 2
#define LEADING_EDGE  0
#define TRAILING_EDGE 1
 
void R3BTofdMapped2TCal::Exec(Option_t* option)
{
	// check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
		return;

    Int_t nHits = fMappedItems->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
       R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fMappedItems->At(ihit);
       if (!hit) continue;

       Int_t iPlane = hit->GetPlaneId(); // 1..n
       Int_t iBar   = hit->GetBarId();   // 1..n
       Int_t iSide  = hit->GetSide();    // 1 or 2	
       
	   if ((iPlane<1) || (iPlane>fNofPlanes))
	   {
           LOG(INFO) << "R3BTofdMapped2TCal::Exec : Plane number out of range: " << 
           iPlane << FairLogger::endl;
           continue;
       }       
	   if ((iBar<1) || (iBar>fPaddlesPerPlane))
	   {
           LOG(INFO) << "R3BTofdMapped2TCal::Exec : Bar number out of range: " << 
           iBar << FairLogger::endl;
           continue;
       }

	   // convert times to ns
	   double timeLT_ns[2]={0.0,0.0}; // leading and trailing edge
	   for (int edge=0;edge<2;edge++) // loop over leading and trailing edge
       {
	       R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iPlane, iBar, (iSide-1)*2 + edge+1);
	       if (!par)
	       {
	           LOG(INFO) << "R3BTofdMapped2TCal::Exec : Tcal par not found, Plane: " << 
	           iPlane << ", Bar: " << iBar << FairLogger::endl;
	           continue;
	       }

	       
	       // Convert TDC to [ns] ...
	       Double_t time_ns = par->GetTimeVFTX( hit->GetFineTime(edge) );
	
	       if (time_ns < 0. || time_ns > fClockFreq )
	       {
	           LOG(ERROR) << 
	           "R3BTofdMapped2TCal::Exec : bad time in ns: Plane= " << iPlane << 
	           ", bar= " << iBar << 
	           ", time in channels = " << hit->GetFineTime(edge) <<
	           ", time in ns = " << time_ns  << FairLogger::endl;
	           continue;
	       }
	
		   // ... and add clock time
	       timeLT_ns[edge] = fClockFreq-time_ns + hit->GetCoarseTime(edge) * fClockFreq;
	   }

	if (IS_NAN(timeLT_ns[0]) && IS_NAN(timeLT_ns[1])) continue;	



		// see if we have already an incomplete CalItem that belongs to the 
		// current hit (=> existingPart)
		R3BPaddleCalData *existingPart=NULL;
		
		for (Int_t ie=0;ie<fNofCalItems;ie++)
		{
			R3BPaddleCalData* aCalData=(R3BPaddleCalData*)fCalItems->At(ie);
			
			if ((aCalData->GetPlane()!=iPlane) ||
			    (aCalData->GetBar()  !=iBar)) continue; // not the droid we're looking for
			    
			if (iSide==PM1) // this hit has data for PM1
			{
				// make sure PM1 data in existing aCalData is still empty and that
				// times of existing PM2 and new PM1 are within coinc window:
				if ((IS_NAN(aCalData->fTime1L_ns) && IS_NAN(aCalData->fTime1T_ns)) && 
					((fabs(aCalData->fTime2L_ns-timeLT_ns[LEADING_EDGE])  < COINC_TIME_NS) || 
					 (fabs(aCalData->fTime2T_ns-timeLT_ns[TRAILING_EDGE]) < COINC_TIME_NS)))
				{
					existingPart=aCalData;
					break;
				}
			}
			else // new hit is for side=2
			{
				if ((IS_NAN(aCalData->fTime2L_ns) && IS_NAN(aCalData->fTime2T_ns)) &&
					((fabs(aCalData->fTime1L_ns-timeLT_ns[LEADING_EDGE])  < COINC_TIME_NS) || 
					 (fabs(aCalData->fTime1T_ns-timeLT_ns[TRAILING_EDGE]) < COINC_TIME_NS)))
				{
					existingPart=aCalData;
					break;
				}
			}
		}



       if (!existingPart) 
       {
			existingPart = new ((*fCalItems)[fNofCalItems]) R3BPaddleCalData(iPlane,iBar);
			fNofCalItems += 1;
	   }
	   
				
	   // finally set the time to the correct cal item
	   if (iSide==PM1)
	   {
	     existingPart->fTime1L_ns   = timeLT_ns[0];
	     existingPart->fTime1T_ns   = timeLT_ns[1];
	   }
	   else // PM2
	   {
	     existingPart->fTime2L_ns   = timeLT_ns[0];
	     existingPart->fTime2T_ns   = timeLT_ns[1];
	   }
	   
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
