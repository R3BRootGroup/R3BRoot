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
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TClonesArray.h"
#include "TMath.h"



#define LOS_COINC_WINDOW_V_NS 1000 // VFTX 20
#define LOS_COINC_WINDOW_T_NS 1000 // TAMEX   1000   
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

//cout << "Trial" << endl;
   Int_t nHits = fMappedItems->GetEntriesFast();
   
//   if(nHits = 1) cout<<"nHits: "<<nHits<<endl;
   
   for (Int_t ihit = 0; ihit < nHits; ihit++)  // nHits = Nchannel_LOS * NTypes = 4 or 8 * 3
    {
       R3BLosMappedData* hit = (R3BLosMappedData*)fMappedItems->At(ihit);
       if (!hit) continue;
 
       // channel numbers are stored 1-based (1..n)
       UInt_t iDet  = hit->GetDetector(); // 1..
       UInt_t iCha  = hit->GetChannel();  // 1..
       UInt_t iType = hit->GetType();     // 0,1,2
       
 // cout<<"Det "<<iDet<<", Channel "<<iCha<<", type "<<iType<<", nHits "<<nHits<<endl;
 
  //     cout<<"R3BLosMapped2Cal: det. "<< iDet <<", chann. "<< iCha <<", type "<<iType<<", timeFine "<<hit->GetTimeFine()<<endl;
       
              
	   if ((iDet<1) || (iDet>fNofDetectors))
	   {
           LOG(INFO) << "R3BLosMapped2Cal::Exec : Detector number out of range: " << 
           iDet << FairLogger::endl;
           continue;
       }
       
	   // Fetch calib data for current channel
       // new: 

        R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iDet, iCha, iType+1);
        
       if (!par)
       {
           LOG(INFO) << "R3BLosMapped2Cal::Exec : Tcal par not found, Detector: " << 
           iDet << ", Channel: " << iCha << ", Type: "<<iType<< FairLogger::endl;
           continue;
       }
       
       // Convert TDC to [ns] ...

        Double_t times_raw_ns = par->GetTimeVFTX( hit->GetTimeFine() );
        
        
   //     cout<<"Mapped2Cal 1: "<<iCha<<", "<<iType<<", "<<hit->GetTimeFine() <<", "<<hit->GetTimeCoarse()<<", "<<times_raw_ns<<endl;
     
        if (times_raw_ns < 0. || times_raw_ns > fClockFreq || IS_NAN(times_raw_ns) )
        {
			
           LOG(INFO) << 
           "R3BLosMapped2Cal::Exec : Bad time in ns: det= " << iDet << 
           ", ch= " << iCha << 
           ", type= "<< iType <<
           ", time in channels = " << hit->GetTimeFine() <<
           ", time in ns = " << times_raw_ns  << FairLogger::endl;
           continue;
        }
   
        	   // ... and add clock time
        Double_t times_ns = fClockFreq-times_raw_ns + hit->GetTimeCoarse() * fClockFreq;
           
  //     cout<<"Mapped2Cal 2: "<<iCha<<", "<<iType<<", "<< hit->GetTimeFine()<<", "<<hit->GetTimeCoarse()<<", "<<times_raw_ns<<", "<<times_ns<<endl;           
       
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
//cout<<aCalItem<<" "<<iDet<<" "<<(int)aCalItem->GetDetector()<<endl;
		   if (aCalItem->GetDetector() != iDet) {
		   	// Do not consider an item for another detector.
		   	continue;
		   }
		   
		Double_t  LOS_COINC_WINDOW_NS;
		if(iType == 0) LOS_COINC_WINDOW_NS = LOS_COINC_WINDOW_V_NS;
		if(iType == 1 || iType == 2) LOS_COINC_WINDOW_NS = LOS_COINC_WINDOW_T_NS;   
		   
		   if(fabs(aCalItem->GetMeanTime()-times_ns) < LOS_COINC_WINDOW_NS)
		   {
			   // check if item is already set. If so, we need to skip this event!
			   
			   switch (iCha)
			   {
				   case 7 : {
				        	   if (iType == 0 && !IS_NAN(aCalItem->fTimeV_r_ns))  LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl; //break;
				        	   if (iType == 1 && !IS_NAN(aCalItem->fTimeL_r_ns))  LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
				        	   if (iType == 2 && !IS_NAN(aCalItem->fTimeT_r_ns))  LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
				            } break;
				   case 1 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_t_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_t_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_t_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;
				   case 3 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_l_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_l_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_l_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
				            } break;
				   case 5 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_b_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_b_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_b_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;
				   case 8 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_rt_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_rt_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_rt_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;
				   case 2 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_lt_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_lt_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_lt_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;
				   case 4 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_lb_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_lb_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_lb_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;
				   case 6 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_rb_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_rb_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_rb_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;
				   case 16 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_ref_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_ref_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_ref_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;        
					case 15 : {
					          if (iType == 0 && !IS_NAN(aCalItem->fTimeV_ref_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 1 && !IS_NAN(aCalItem->fTimeL_ref_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					          if (iType == 2 && !IS_NAN(aCalItem->fTimeT_ref_ns))   LOG(ERROR) << "R3BLosMapped2Cal::Exec : Skip event because of unhandle-able pileup." << FairLogger::endl;
					        } break;        

     
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
	   
	   Icounts_tot = Icounts_tot + 1;		   
	   if(iCha == 15)  // pixel time signal
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
    		if(iType == 0) {
			calItem->fTimeV_pix_ns   = times_ns;
			if ( calItem->fTimeV_pix_ns < 0. || IS_NAN(calItem->fTimeV_pix_ns) ) LOG(INFO)<<"Problem with  fTimeV_pix_ns: "<< calItem->fTimeV_pix_ns<< " "<<times_ns<<" "<<endl;
		    }		
			
			if(iType == 1) {
			calItem->fTimeL_pix_ns   = times_ns;
			if ( calItem->fTimeL_pix_ns < 0. || IS_NAN(calItem->fTimeL_pix_ns) ) LOG(INFO)<<"Problem with  fTimeL_pix_ns: "<< calItem->fTimeL_pix_ns<< " "<<times_ns<<" "<<endl;	
		    }

			if(iType == 2) {
			calItem->fTimeT_pix_ns   = times_ns;
			if ( calItem->fTimeT_pix_ns < 0. || IS_NAN(calItem->fTimeT_pix_ns) ) LOG(INFO)<<"Problem with  fTimeT_pix_ns: "<< calItem->fTimeT_pix_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }	  
	   if(iCha == 16)  // reference time signal
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
    		if(iType == 0) {
			calItem->fTimeV_ref_ns   = times_ns;
			if ( calItem->fTimeV_ref_ns < 0. || IS_NAN(calItem->fTimeV_ref_ns) ) LOG(INFO)<<"Problem with  fTimeV_ref_ns: "<< calItem->fTimeV_ref_ns<< " "<<times_ns<<" "<<endl;
		    }		
			
			if(iType == 1) {
			calItem->fTimeL_ref_ns   = times_ns;
			if ( calItem->fTimeL_ref_ns < 0. || IS_NAN(calItem->fTimeL_ref_ns) ) LOG(INFO)<<"Problem with  fTimeL_ref_ns: "<< calItem->fTimeL_ref_ns<< " "<<times_ns<<" "<<endl;	
		    }

			if(iType == 2) {
			calItem->fTimeT_ref_ns   = times_ns;
			if ( calItem->fTimeT_ref_ns < 0. || IS_NAN(calItem->fTimeT_ref_ns) ) LOG(INFO)<<"Problem with  fTimeT_ref_ns: "<< calItem->fTimeT_ref_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }	
		  if(iCha == 7)
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
    		if(iType == 0) {
			calItem->fTimeV_r_ns   = times_ns;
			if ( calItem->fTimeV_r_ns < 0. || IS_NAN(calItem->fTimeV_r_ns) ) LOG(INFO)<<"Problem with  fTimeV_r_ns: "<< calItem->fTimeV_r_ns<< " "<<times_ns<<" "<<endl;
		    }		
			
			if(iType == 1) {
			calItem->fTimeL_r_ns   = times_ns;
			if ( calItem->fTimeL_r_ns < 0. || IS_NAN(calItem->fTimeL_r_ns) ) LOG(INFO)<<"Problem with  fTimeL_r_ns: "<< calItem->fTimeL_r_ns<< " "<<times_ns<<" "<<endl;	
		    }

			if(iType == 2) {
			calItem->fTimeT_r_ns   = times_ns;
			if ( calItem->fTimeT_r_ns < 0. || IS_NAN(calItem->fTimeT_r_ns) ) LOG(INFO)<<"Problem with  fTimeT_r_ns: "<< calItem->fTimeT_r_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }	  
		  if(iCha == 1)
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
			if(iType == 0) {
			calItem->fTimeV_t_ns   = times_ns;
			if ( calItem->fTimeV_t_ns < 0. || IS_NAN(calItem->fTimeV_t_ns) ) LOG(INFO)<<"Problem with  fTimeV_t_ns: "<< calItem->fTimeV_t_ns<< " "<<times_ns<<" "<<endl;
		    }
			
			if(iType == 1) {
			calItem->fTimeL_t_ns   = times_ns;
			if ( calItem->fTimeL_t_ns < 0. || IS_NAN(calItem->fTimeL_t_ns) ) LOG(INFO)<<"Problem with  fTimeL_t_ns: "<< calItem->fTimeL_t_ns<< " "<<times_ns<<" "<<endl;	
		    }
			
			if(iType == 2) {
			calItem->fTimeT_t_ns   = times_ns;
			if ( calItem->fTimeT_t_ns < 0. || IS_NAN(calItem->fTimeT_t_ns) ) LOG(INFO)<<"Problem with  fTimeT_t_ns: "<< calItem->fTimeT_t_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }	
		  if(iCha == 3)
		  { 
			//  cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
			if(iType == 0) {
			calItem->fTimeV_l_ns   = times_ns;
			if ( calItem->fTimeV_l_ns < 0. || IS_NAN(calItem->fTimeV_l_ns) ) LOG(INFO)<<"Problem with  fTimeV_l_ns: "<< calItem->fTimeV_l_ns<< " "<<times_ns<<" "<<endl;
		    }
			
			if(iType == 1) {
			calItem->fTimeL_l_ns   = times_ns;
			if ( calItem->fTimeL_l_ns < 0. || IS_NAN(calItem->fTimeL_l_ns) ) LOG(INFO)<<"Problem with  fTimeL_l_ns: "<< calItem->fTimeL_l_ns<< " "<<times_ns<<" "<<endl;	
		    }
			
			if(iType == 2) {
			calItem->fTimeT_l_ns   = times_ns;
			if ( calItem->fTimeT_l_ns < 0. || IS_NAN(calItem->fTimeT_l_ns) ) LOG(INFO)<<"Problem with  fTimeT_l_ns: "<< calItem->fTimeT_l_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }		  
		  if(iCha == 5)
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
			if(iType == 0) {
			calItem->fTimeV_b_ns   = times_ns;
			if ( calItem->fTimeV_b_ns < 0. || IS_NAN(calItem->fTimeV_b_ns) ) LOG(INFO)<<"Problem with  fTimeV_b_ns: "<< calItem->fTimeV_b_ns<< " "<<times_ns<<" "<<endl;
		    }
			
			if(iType == 1) {
			calItem->fTimeL_b_ns   = times_ns;
			if ( calItem->fTimeL_b_ns < 0. || IS_NAN(calItem->fTimeL_b_ns) ) LOG(INFO)<<"Problem with  fTimeL_b_ns: "<< calItem->fTimeL_b_ns<< " "<<times_ns<<" "<<endl;	
		    }
			
			if(iType == 2) {
			calItem->fTimeT_b_ns   = times_ns;
			if ( calItem->fTimeT_b_ns < 0. || IS_NAN(calItem->fTimeT_b_ns) ) LOG(INFO)<<"Problem with  fTimeT_b_ns: "<< calItem->fTimeT_b_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }	
		  		  	  
		  if(iCha == 8)
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
			if(iType == 0) {
			calItem->fTimeV_rt_ns   = times_ns;
			if ( calItem->fTimeV_rt_ns < 0. || IS_NAN(calItem->fTimeV_rt_ns) ) LOG(INFO)<<"Problem with  fTimeV_rt_ns: "<< calItem->fTimeV_rt_ns<< " "<<times_ns<<" "<<endl;
		    }
			
			if(iType == 1) {
			calItem->fTimeL_rt_ns   = times_ns;
			if ( calItem->fTimeL_rt_ns < 0. || IS_NAN(calItem->fTimeL_rt_ns) ) LOG(INFO)<<"Problem with  fTimeL_rt_ns: "<< calItem->fTimeL_rt_ns<< " "<<times_ns<<" "<<endl;	
		    }
			
			if(iType == 2) {
			calItem->fTimeT_rt_ns   = times_ns;
			if ( calItem->fTimeT_rt_ns < 0. || IS_NAN(calItem->fTimeT_rt_ns) ) LOG(INFO)<<"Problem with  fTimeT_rt_ns: "<< calItem->fTimeT_rt_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }	  
		  if(iCha == 2)
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
			if(iType == 0) {
			calItem->fTimeV_lt_ns   = times_ns;
			if ( calItem->fTimeV_lt_ns < 0. || IS_NAN(calItem->fTimeV_lt_ns) ) LOG(INFO)<<"Problem with  fTimeV_lt_ns: "<< calItem->fTimeV_lt_ns<< " "<<times_ns<<" "<<endl;
		    }
			
			if(iType == 1) {
			calItem->fTimeL_lt_ns   = times_ns;
			if ( calItem->fTimeL_lt_ns < 0. || IS_NAN(calItem->fTimeL_lt_ns) ) LOG(INFO)<<"Problem with  fTimeL_lt_ns: "<< calItem->fTimeL_lt_ns<< " "<<times_ns<<" "<<endl;	
		    }
			
			if(iType == 2) {
			calItem->fTimeT_lt_ns   = times_ns;
			if ( calItem->fTimeT_lt_ns < 0. || IS_NAN(calItem->fTimeT_lt_ns) ) LOG(INFO)<<"Problem with  fTimeT_lt_ns: "<< calItem->fTimeT_lt_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }	
		  if(iCha == 4)
		  { 
			//cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
			if(iType == 0) {
			calItem->fTimeV_lb_ns   = times_ns;
			if ( calItem->fTimeV_lb_ns < 0. || IS_NAN(calItem->fTimeV_lb_ns) ) LOG(INFO)<<"Problem with  fTimeV_lb_ns: "<< calItem->fTimeV_lb_ns<< " "<<times_ns<<" "<<endl;
		    }
			
			if(iType == 1) {
			calItem->fTimeL_lb_ns   = times_ns;
			if ( calItem->fTimeL_lb_ns < 0. || IS_NAN(calItem->fTimeL_lb_ns) ) LOG(INFO)<<"Problem with  fTimeL_lb_ns: "<< calItem->fTimeL_lb_ns<< " "<<times_ns<<" "<<endl;	
		    }
			
			if(iType == 2) {
			calItem->fTimeT_lb_ns   = times_ns;
			if ( calItem->fTimeT_lb_ns < 0. || IS_NAN(calItem->fTimeT_lb_ns) ) LOG(INFO)<<"Problem with  fTimeT_lb_ns: "<< calItem->fTimeT_lb_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }		  
		  if(iCha == 6)
		  { 
			  //cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;	
			if(iType == 0) {
			calItem->fTimeV_rb_ns   = times_ns;
			if ( calItem->fTimeV_rb_ns < 0. || IS_NAN(calItem->fTimeV_rb_ns) ) LOG(INFO)<<"Problem with  fTimeV_rb_ns: "<< calItem->fTimeV_rb_ns<< " "<<times_ns<<" "<<endl;
		    }
			
			if(iType == 1) {
			calItem->fTimeL_rb_ns   = times_ns;
			if ( calItem->fTimeL_rb_ns < 0. || IS_NAN(calItem->fTimeL_rb_ns) ) LOG(INFO)<<"Problem with  fTimeL_rb_ns: "<< calItem->fTimeL_rb_ns<< " "<<times_ns<<" "<<endl;	
		    }
			
			if(iType == 2) {
			calItem->fTimeT_rb_ns   = times_ns;
			if ( calItem->fTimeT_rb_ns < 0. || IS_NAN(calItem->fTimeT_rb_ns) ) LOG(INFO)<<"Problem with  fTimeT_rb_ns: "<< calItem->fTimeT_rb_ns<< " "<<times_ns<<" "<<endl;					      
		    }
		  }
		  

//cout<<"Mapped2Cal "<<iCha<<", "<<iType<<", "<<times_ns<<endl;		  			
// cout<<"Icounts_tot "<<Icounts_tot<<endl;    
//cout<<"Done"<<endl;

//       }
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
