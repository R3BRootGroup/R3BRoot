// ------------------------------------------------------------
// -----                  R3BLosMapped2Cal                -----
// -----          Created Feb 4th 2016 by R.Plag          -----
// ------------------------------------------------------------

/* March 2016
 * Rewrote the Cal structure to provide individual leafs for the
 * left, top, right and bottom signals. This allows to plot
 * the time differences via cbmsim->Draw(...) interactively (aka without
 * looping over all channels) which is crucial for a quick check of the
 * detector status during the experiment.
 *  
 * There is a fifth channel "ref" available which holds the master 
 * trigger, as time reference.
 *  
 */


#include "R3BLosMapped2Cal.h"

#include "R3BTCalEngine.h"
#include "R3BLosMappedData.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"



#define LOS_COINC_WINDOW_NS 20       
#define IS_NAN(x) TMath::IsNaN(x)


R3BLosMapped2Cal::R3BLosMapped2Cal()
    : FairTask("LosTcal", 1)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BLosCalData"))
    , fNofCalItems(0)
    , fNofTcalPars(0)
    , fNofModules(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BLosMapped2Cal::R3BLosMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BLosCalData"))
    , fNofCalItems(0)
    , fNofTcalPars(0)
    , fNofModules(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BLosMapped2Cal::~R3BLosMapped2Cal()
{
    if (fCalItems)
    {
        delete fCalItems;
        fCalItems = NULL;
        fNofCalItems = 0;
    }
}

InitStatus R3BLosMapped2Cal::Init()
{
	fNofTcalPars = fTcalPar->GetNumModulePar();
	if (fNofTcalPars==0)
	{
		LOG(ERROR) << "There are no TCal parameters in container LosTCalPar" << FairLogger::endl;
		return kFATAL;
	}

    LOG(INFO) << "R3BLosMapped2Cal::Init : read " << fNofModules << " modules" << FairLogger::endl;
    
	// create the map for fast access in exec()
	/*
    for (Int_t i = 0; i < fNofTcalPars; i++)
    {
        R3BTCalModulePar* par = fTcalPar->GetModuleParAt(i);
        if (!par) 
        {
			LOG(INFO) << "No LOS Tcal pars for module " << i << FairLogger::endl;
			continue;
		}
        fMapPar[par->GetModuleId()] = par;
    }
*/

	// try to get a handle on the EventHeader. EventHeader may not be 
	// present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");


	// get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("LosMapped");
    if (NULL == fMappedItems)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LosMapped not found");


	// request storage of Cal data in output tree
    mgr->Register("LosCal", "Land", fCalItems, kTRUE);

    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BLosMapped2Cal::SetParContainers()
{
    fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("LosTCalPar");
    if (!fTcalPar)
    {
		LOG(ERROR) << "Could not get access to LosTCalPar-Container." << FairLogger::endl;
		fNofTcalPars=0;
		return;
	}
}

InitStatus R3BLosMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BLosMapped2Cal::Exec(Option_t* option)
{
	// check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
		return;

    Int_t nHits = fMappedItems->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
       R3BLosMappedData* hit = (R3BLosMappedData*)fMappedItems->At(ihit);
       if (!hit) continue;

       // channel numbers are stored 1-based (1..n)
       Int_t iDet = hit->GetDetector(); // 1..
       Int_t iCha = hit->GetChannel();  // 1..
       UInt_t module = (iDet-1) * fNofChannels + (iCha-1); // channel index in TCalPar 0..
       
	   if ((iDet<1) || (iDet>fNofDetectors))
	   {
           LOG(INFO) << "R3BLosMapped2Cal::Exec : Detector number out of range: " << 
           iDet << FairLogger::endl;
           continue;
       }
       
	   // Fetch calib data for current channel
       //R3BTCalModulePar* par = fMapPar[module]; // calibration data for cur ch
       // new: 
       R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iDet, iCha, 1);
        
       if (!par)
       {
           LOG(INFO) << "R3BLosMapped2Cal::Exec : Tcal par not found, Detector: " << 
           iDet << ", Channel: " << iCha << FairLogger::endl;
           continue;
       }
       
       // Convert TDC to [ns] ...
       Double_t time_ns = par->GetTimeVFTX( hit->GetTimeFine() );

       if (time_ns < 0. || time_ns > fClockFreq )
       {
           LOG(ERROR) << 
           "R3BLosMapped2Cal::Exec : bad time in ns: det= " << iDet << 
           ", ch= " << iCha << 
           ", time in channels = " << hit->GetTimeFine() <<
           ", time in ns = " << time_ns  << FairLogger::endl;
           continue;
       }

	   // ... and add clock time
       time_ns = fClockFreq-time_ns + hit->GetTimeCoarse() * fClockFreq;
       
       /* Note: we have multi-hit data...
        *  
        * So the map needs to have one item per detector and (multi-)hit
        * Then we need to establish a time window
        * Here, we have the hits unsorted in time and channel. If we 
        * reconstruct a detector hit using a time window, So:
        * 
        * For each single hit, search the list of detector hits. If a
        * matching hit is found (dt < window and item not yet set), add
        * item. Else create new detector hit.
        * 
        * This way, we theoretically *might* end up with two calItems 
        * which are actually just one. Hm... this should be very rare. 
        * Care about that later if it becomes necessary. 
        * 
        * Even though we have technically two LOS detectors in s438b,
        * only one really produces data so it's ok to throw all
        * detector hits into the same list (and hence traversing a longer
        * list than strictly necessary for the reconstruction)
        */
               
       // see if there is already a detector hit around that time 
       R3BLosCalData* calItem=NULL;
       for (int iCal=0;iCal<fNofCalItems;iCal++)
       {
		   R3BLosCalData* aCalItem=(R3BLosCalData*)fCalItems->At(iCal);
		   if (fabs(aCalItem->GetMeanTime()-time_ns) < LOS_COINC_WINDOW_NS)
		   {
			   // check if item is already set. If so, we need to skip this event!
			   switch (iCha)
			   {
				   case 1 : if (!IS_NAN(aCalItem->fTime_r_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;break;
				   case 2 : if (!IS_NAN(aCalItem->fTime_t_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;break;
				   case 3 : if (!IS_NAN(aCalItem->fTime_l_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;break;
				   case 4 : if (!IS_NAN(aCalItem->fTime_b_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;break;
				   case 5 : if (!IS_NAN(aCalItem->fTime_ref_ns)) LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;break;
			   }       			   
			   calItem=aCalItem;
			   break;
		   }
	   } 

       if (!calItem) 
       {
		    // there is no detector hit with matching time. Hence, create a new one.
			calItem = new ((*fCalItems)[fNofCalItems]) R3BLosCalData(iDet);
			fNofCalItems += 1;
	   }

	   // set the time to the correct cal item
	   switch (iCha)
	   {
		   case 1 : calItem->fTime_r_ns   = time_ns;break;
		   case 2 : calItem->fTime_t_ns   = time_ns;break;
		   case 3 : calItem->fTime_l_ns   = time_ns;break;
		   case 4 : calItem->fTime_b_ns   = time_ns;break;
		   case 5 : calItem->fTime_ref_ns = time_ns;break;
		   default: LOG(INFO) << "R3BLosMapped2Cal::Exec : Channel number out of range: " << 
           iCha << FairLogger::endl;
	   }       
    }

}

void R3BLosMapped2Cal::FinishEvent()
{
    if (fCalItems)
    {
        fCalItems->Clear();
        fNofCalItems = 0;
    }

}

void R3BLosMapped2Cal::FinishTask()
{
}

ClassImp(R3BLosMapped2Cal)
