// ------------------------------------------------------------
// -----                  R3BGlobalAnalysis               -----
// -----          Created March 1st 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * In this task one can fill spectra of various detectors which are displayed  
 * online in a browser with the help of the http server.
 * One can also collect information from different data levels of various 
 * detectors and do a first analysis. 
 *
 * Task 1: This example file shows how to get access to LOS data (mapped and tcal), 
 * fill histograms. Include this file in r3broot, start the http server in you macro, 
 * call this task and start to analyse run 165.
 * Display the online histograms in a browser by connecting to the computer
 * which runs the task and open port 8080.
 * e.g.: http://lxg0897.gsi.de:8080
 * 
 * Task 2: Extend this file by showing similar histograms for the Ptof detector.
 * e.g. Create a new canvas for the Ptof detector and display the histogram of 
 * hit bars to see where the beam is hitting the detector with mapped data.
 * Make a histogram of the time-over-threshold of bar 2 (tot=tt-tl) with cal data.
 * 
 * Task 3: Combine the time information of the LOS detector and the average time
 * of bar 2 of the Ptof detector (tl1+tl2)/2 and create a histogram of the 
 * time-of-flight of the beam particles. Add a 2D histogram with ToT vs. ToF
 */


#include "R3BGlobalAnalysis.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BPaddleCalData.h"
#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

#include "TClonesArray.h"
#include <iostream>
using namespace std;


R3BGlobalAnalysis::R3BGlobalAnalysis()
    : FairTask("OnlineSpectra", 1)
    , fCalItemsLos(NULL)
    , fCalItemsPtof(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsPtof(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BGlobalAnalysis::R3BGlobalAnalysis(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fCalItemsPtof(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsPtof(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BGlobalAnalysis::~R3BGlobalAnalysis()
{
}

InitStatus R3BGlobalAnalysis::Init()
{

    LOG(INFO) << "R3BGlobalAnalysis::Init " << FairLogger::endl;
    
    FairRootManager* mgr = FairRootManager::Instance();    
    if (NULL == mgr) FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRunOnline *run = FairRunOnline::Instance();

    // LOS data
    // get access to Mapped data
    fMappedItemsLos = (TClonesArray*)mgr->GetObject("LosMapped");

    // get access to cal data
    fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");
 
    // define histograms 
    fh_los_channels = new TH1F("los_channels", "LOS channels", 6, 0., 6.); 
    fh_los_channels->GetXaxis()->SetTitle("PMT number");
    fh_los_channels->GetYaxis()->SetTitle("counts");  
    
    fh_los_tres = new TH1F("los_time_res", "LOS Time resolution", 4000, -4., 0.);
    fh_los_tres->GetXaxis()->SetTitle("delta t in ns");
    fh_los_tres->GetYaxis()->SetTitle("counts");  
        
    // define canvas in which all histograms are grouped
    TCanvas *cLos = new TCanvas("Los", "LOS", 10, 10, 500, 500);
    cLos->Divide(1, 2);
    cLos->cd(1);
    fh_los_channels->Draw();
    cLos->cd(2);
    fh_los_tres->Draw();
    cLos->cd(0);
    run->AddObject(cLos);

    // Ptof data
    // get access to Mapped data
    fMappedItemsPtof = (TClonesArray*)mgr->GetObject("PtofMapped");

    // get access to cal data
    fCalItemsPtof = (TClonesArray*)mgr->GetObject("PtofCal");

    // define histograms
    fh_ptof_channels = new TH1F("ptof_channels", "PToF channels", 8, 0, 8.);
    fh_ptof_channels->GetXaxis()->SetTitle("Bar number");
    fh_ptof_channels->GetYaxis()->SetTitle("counts");  

    fh_ptof_tot= new TH1F("ptof_tot", "PToF ToT", 500, 0, 50.);
    fh_ptof_tot->GetXaxis()->SetTitle("ToT in ns");
    fh_ptof_tot->GetYaxis()->SetTitle("counts");  

    fh_ptof_tof= new TH1F("ptof_tof", "PToF ToF", 1000, 0, 10.);
    fh_ptof_tof->GetXaxis()->SetTitle("ToF in ns");
    fh_ptof_tof->GetYaxis()->SetTitle("counts");  
    
    fh_ptof_tot_vs_tof= new TH2F("ptof_tot_vs_tof", "PToF ToT vs. ToF", 1000, 0, 10., 500, 0, 50.);
    fh_ptof_tot_vs_tof->GetXaxis()->SetTitle("ToF in ns");
    fh_ptof_tot_vs_tof->GetYaxis()->SetTitle("ToT in ns");  

    // define canvas in which all histograms are grouped
    TCanvas *cPtof = new TCanvas("Ptof", "PTof", 10, 10, 500, 500);
    cPtof->Divide(2, 2);
    cPtof->cd(1);
    fh_ptof_channels->Draw();    
    cPtof->cd(2);
    fh_ptof_tot->Draw();    
    cPtof->cd(3);
    fh_ptof_tof->Draw();    
    cPtof->cd(4);
    fh_ptof_tot_vs_tof->Draw("colz");        
    cPtof->cd(0);
    run->AddObject(cPtof);
 
       
    return kSUCCESS;
}



void R3BGlobalAnalysis::Exec(Option_t* option)
{

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr) FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    
    // check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
		return;


    // if mapped data of LOS are available, fill the histograms
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

    // if calibrated data of LOS are available, fill histograms
    Double_t timeLos=0.;  // This is the start time which can be used for ToF determination of all detectors.
    if(fCalItemsLos)
    {
      Int_t nHits = fCalItemsLos->GetEntriesFast();    
      // loop over hits
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	  R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(ihit);

          Int_t iDet=calData->GetDetector();
          fh_los_tres->Fill((calData->fTime_r_ns+calData->fTime_l_ns)/2. -     		
	                  (calData->fTime_t_ns+calData->fTime_b_ns)/2.);
          timeLos=(calData->fTime_r_ns+calData->fTime_l_ns+calData->fTime_t_ns+calData->fTime_b_ns)/4.;		 
      }            						 
    }

    // if mapped data of Ptof are available, fill the histograms
    if(fMappedItemsPtof)
    {
      Int_t nHits = fMappedItemsPtof->GetEntriesFast();    
      // loop over hits
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	R3BPaddleTamexMappedData *hit = (R3BPaddleTamexMappedData*)fMappedItemsPtof->At(ihit);
        if (!hit) continue; // should not happen

        Int_t iPlane = hit->GetPlaneId(); // 1..n
        Int_t iBar   = hit->GetBarId();   // 1..n
           
        fh_ptof_channels->Fill(iBar);       
      }
    }

    // if calibrated data of LOS are available, fill histograms
    if(fCalItemsPtof)
    {
      // define leading and trailing edge times of PMT1 and PMT2
      Double_t t1l=0.;
      Double_t t2l=0.;
      Double_t t1t=0.;
      Double_t t2t=0.;
      
      // define time-over-threshold of PMT1 and PMT2
      Double_t tot1=0.;
      Double_t tot2=0.;
     
      Int_t nHits = fCalItemsPtof->GetEntriesFast();    
      
      // loop over hits
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	  R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsPtof->At(ihit);
          if (!hit) continue; // should not happen

          Int_t iPlane  = hit->GetPlane();    // 1..n
          Int_t iBar  = hit->GetBar();    // 1..n

          // get all times of one bar
          t1l=hit->fTime1L_ns;
          t2l=hit->fTime2L_ns;
          t1t=hit->fTime1T_ns;
          t2t=hit->fTime2T_ns;

	  // calculate time over threshold and check if clock counter went out of range
          while(t1t - t1l < 0.) {
	    t1t=t1t+2048.*fClockFreq; 
	  }

          while(t2t-t2l < 0.) {
	    t2t=t2t+2048.*fClockFreq; 
          }
	 
	  // calculate time of flight relative to LOS and check if clock counter went out of range
	  while(t1l-timeLos<0.){
	    t1t=t1t+2048.*fClockFreq; 
	    t1l=t1l+2048.*fClockFreq; 
	    t2t=t2t+2048.*fClockFreq; 
	    t2l=t2l+2048.*fClockFreq; 			  
          }
       
          tot1=t1t - t1l;		      
          // negative time-over-thresholds should not happen
	  if(tot1<0) {
	          LOG(WARNING) << "Negative ToT "<< tot1<<FairLogger::endl;	
	          LOG(WARNING) << "times1: " << t1t << " " << t1l << FairLogger::endl;		  
	      }

          tot2=t2t - t2l;	
          // negative time-over-thresholds should not happen
          if(tot2<0) {
              LOG(WARNING) << "Negative ToT "<< tot2<<FairLogger::endl;              
              LOG(WARNING) << "times2: " << t2t << " " << t2l << FairLogger::endl;		 
          }
 
          // fill histograms for bar 2 as example
          if(iBar==2)
          {
            fh_ptof_tot->Fill(sqrt(tot1*tot2));
            fh_ptof_tof->Fill((t1l+t2l)/2.-timeLos-3580.);
            fh_ptof_tot_vs_tof->Fill((t1l+t2l)/2.-timeLos-3580.,sqrt(tot1*tot2));
         }        	
      }	
   }   
}

void R3BGlobalAnalysis::FinishEvent()
{
    if (fCalItemsLos)
    {
        fCalItemsLos->Clear();
    }
    if (fMappedItemsLos)
    {
        fMappedItemsLos->Clear();
    }
    if (fCalItemsPtof)
    {
        fCalItemsPtof->Clear();
    }
    if (fMappedItemsPtof)
    {
        fMappedItemsPtof->Clear();
    }

}

void R3BGlobalAnalysis::FinishTask()
{
}

ClassImp(R3BGlobalAnalysis)
