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
//#include "R3BPaddleTamexMappedData.h"
//#include "R3BPaddleCalData.h"
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
    , fMappedItemsLos(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BGlobalAnalysis::R3BGlobalAnalysis(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fMappedItemsLos(NULL)
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
}

void R3BGlobalAnalysis::FinishTask()
{
}

ClassImp(R3BGlobalAnalysis)
