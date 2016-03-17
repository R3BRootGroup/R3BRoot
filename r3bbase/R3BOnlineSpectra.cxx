// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow  
 * to test the detectors online
 *  
 */


#include "R3BOnlineSpectra.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BPaddleCalData.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TClonesArray.h"

R3BOnlineSpectra::R3BOnlineSpectra()
    : FairTask("OnlineSpectra", 1)
    , fCalItemsLos(NULL)
    , fCalItemsTofd(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsTofd(NULL)
    , fTrigger(-1)
    , flosVeff(14.)
    , flosOffsetX(0.)
    , flosOffsetY(0.)
{
}

R3BOnlineSpectra::R3BOnlineSpectra(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fCalItemsTofd(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsTofd(NULL)
    , fTrigger(-1)
    , flosVeff(14.)
    , flosOffsetX(0.)
    , flosOffsetY(0.)
{
}

R3BOnlineSpectra::~R3BOnlineSpectra()
{
}

InitStatus R3BOnlineSpectra::Init()
{

    LOG(INFO) << "R3BOnlineSpectra::Init " << FairLogger::endl;
    


	// try to get a handle on the EventHeader. EventHeader may not be 
	// present though and hence may be null. Take care when using.
	
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

	// get access to Mapped data
    fMappedItemsLos = (TClonesArray*)mgr->GetObject("LosMapped");

	// get access to cal data
    fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");
 
    fh_los_channels = new TH1F("los_channels", "LOS channels", 6, 0., 6.); 
    fh_los_tres = new TH1F("los_time_res", "LOS Time resolution", 4000, -2., 2.);
    fh_los_pos = new TH2F("los_position", "LOS xy position", 1000, -10., 10.,1000, -10., 10.);
    
    FairRunOnline *run = FairRunOnline::Instance();
    run->AddObject(fh_los_channels);
    run->AddObject(fh_los_tres);
    run->AddObject(fh_los_pos);

    fMappedItemsTofd = (TClonesArray*)mgr->GetObject("TofdMapped");
	
    fh_tofd_channels_plane1 = new TH1F("tofd_channels_plane1", "Tofd channels plane 1", 15, -7., 8.); 
    fh_tofd_channels_plane2 = new TH1F("tofd_channels_plane2", "Tofd channels plane 2", 15, -7., 8.); 
    fh_tofd_channels_plane3 = new TH1F("tofd_channels_plane3", "Tofd channels plane 3", 15, -7., 8.); 
    fh_tofd_channels_plane4 = new TH1F("tofd_channels_plane4", "Tofd channels plane 4", 15, -7., 8.); 
    run->AddObject(fh_tofd_channels_plane1);
    run->AddObject(fh_tofd_channels_plane2);
    run->AddObject(fh_tofd_channels_plane3);
    run->AddObject(fh_tofd_channels_plane4);

    fCalItemsTofd = (TClonesArray*)mgr->GetObject("TofdCal");

    fh_tofd_tdiff_p1_b1=new TH1F("tofd_tdiff_p1_b1","Tofd time diff plane 1 bars 1",10000,10.,20.);
    fh_tofd_tdiff_p1_b1->GetXaxis()->SetTitle("Time / ns");
    fh_tofd_tdiff_p1_b1->GetYaxis()->SetTitle("Counts");


    run->AddObject(fh_tofd_tdiff_p1_b1);
    
   
    return kSUCCESS;
}



void R3BOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
	// check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
		return;


    if(fMappedItemsLos) 
    {
      Int_t nHits = fMappedItemsLos->GetEntriesFast();
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BLosMappedData* hit = (R3BLosMappedData*)fMappedItemsLos->At(ihit);
         if (!hit) continue;

         // channel numbers are stored 1-based (1..n)
         Int_t iDet = hit->GetDetector(); // 1..
         Int_t iCha = hit->GetChannel();  // 1..
         fh_los_channels->Fill(iCha);			         
      }
    }

    if(fCalItemsLos)
    {
      Int_t nHits = fCalItemsLos->GetEntriesFast();    
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(ihit);
        LOG(DEBUG) << "test1 : "<<flosOffsetX<<"  "<<flosOffsetY<<"  "<<flosVeff<<FairLogger::endl;
	LOG(DEBUG) <<"test2 : "<<calData->fTime_r_ns<<FairLogger::endl;

	fh_los_tres->Fill((calData->fTime_r_ns+calData->fTime_l_ns)/2. -     		
	                  (calData->fTime_t_ns+calData->fTime_b_ns)/2.);
	fh_los_pos->Fill((calData->fTime_r_ns-calData->fTime_l_ns)/2.*flosVeff-flosOffsetX,
	                 (calData->fTime_b_ns-calData->fTime_t_ns)/2.*flosVeff-flosOffsetY);
      }			
			  
    }

    if(fMappedItemsTofd)
    {
      Int_t nHits = fMappedItemsTofd->GetEntriesFast();    
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	R3BPaddleTamexMappedData *hit = (R3BPaddleTamexMappedData*)fMappedItemsTofd->At(ihit);
        if (!hit) continue; // should not happen

        Int_t iPlane = hit->GetPlaneId(); // 1..n
        Int_t iBar   = hit->GetBarId();   // 1..n
        Int_t iSide  = hit->GetSide();    // 1 or 2	                

            
        if(iPlane==1 && iSide==1)fh_tofd_channels_plane1->Fill(iBar);			         
        if(iPlane==1 && iSide==2)fh_tofd_channels_plane1->Fill(-iBar);			         

        if(iPlane==2 && iSide==1)fh_tofd_channels_plane2->Fill(iBar);			         
        if(iPlane==2 && iSide==2)fh_tofd_channels_plane2->Fill(-iBar);			         

        if(iPlane==3 && iSide==1)fh_tofd_channels_plane3->Fill(iBar);			         
        if(iPlane==3 && iSide==2)fh_tofd_channels_plane3->Fill(-iBar);			         
	
        if(iPlane==4 && iSide==1)fh_tofd_channels_plane4->Fill(iBar);			         
        if(iPlane==4 && iSide==2)fh_tofd_channels_plane4->Fill(-iBar);			         
	
      }		  
    }
    if(fCalItemsTofd)
    {
      Double_t t1=0.;
      Double_t t2=0.;
      Int_t nHits = fCalItemsTofd->GetEntriesFast();    
      
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsTofd->At(ihit);
        if (!hit) continue; // should not happen

        Double_t iPlane  = hit->GetPlane();    // 1..n
        Double_t iBar  = hit->GetBar();    // 1..n

        
        if(iPlane==1 && iBar==1) t1=(hit->fTime1L_ns+hit->fTime2L_ns)/2.;
        if(iPlane==2 && iBar==1) t2=(hit->fTime1L_ns+hit->fTime2L_ns)/2.;
	 
            
	
      }	

      if(t1>0. && t2>0.) fh_tofd_tdiff_p1_b1->Fill(t2-t1);
      	  
    }
}

void R3BOnlineSpectra::FinishEvent()
{
    if (fCalItemsLos)
    {
        fCalItemsLos->Clear();
    }
    if (fCalItemsTofd)
    {
        fCalItemsTofd->Clear();
    }
    if (fMappedItemsLos)
    {
        fMappedItemsLos->Clear();
    }
    if (fMappedItemsTofd)
    {
        fMappedItemsTofd->Clear();
    }

}

void R3BOnlineSpectra::FinishTask()
{
}

ClassImp(R3BOnlineSpectra)
