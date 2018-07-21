// ------------------------------------------------------------
// -----                  R3BAmsOnlineSpectra             -----
// -----    Created 22/07/18  by J.L. Rodriguez-Sanchez   -----
// -----           Fill AMS online histograms             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with AMS online data  
 */

#include "R3BAmsOnlineSpectra.h"
#include "R3BAmsMappedData.h"
#include "R3BAmsStripCalData.h"
#include "R3BEventHeader.h"


#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFolder.h"

#include "TClonesArray.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <array>
#include "TMath.h"

using namespace std;

R3BAmsOnlineSpectra::R3BAmsOnlineSpectra()
  : FairTask("AmsOnlineSpectra", 1)
  , fMappedItemsAms(NULL)
  , fCalItemsAms(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fCalON(0) {
}

R3BAmsOnlineSpectra::R3BAmsOnlineSpectra(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItemsAms(NULL)
  , fCalItemsAms(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fCalON(0) {
}

R3BAmsOnlineSpectra::~R3BAmsOnlineSpectra() {
}

InitStatus R3BAmsOnlineSpectra::Init() {
  
  LOG(INFO) << "R3BAmsOnlineSpectra::Init " << FairLogger::endl;
  
  // try to get a handle on the EventHeader. EventHeader may not be 
  // present though and hence may be null. Take care when using.
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BAmsOnlineSpectra::Init FairRootManager not found" << FairLogger::endl;
  
  
  header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
  FairRunOnline *run = FairRunOnline::Instance();
  
  //get access to Mapped data
  fMappedItemsAms = (TClonesArray*)mgr->GetObject("amsMappedData");
  
  //get access to Cal data
  //fCalItemsAms = (TClonesArray*)mgr->GetObject("AmsStripCalData");
  
    Double_t bins=200;
    Double_t maxE=3500;
    Double_t minE=0;
    
    
    //  CANVAS 4  -------------------------------     
    TCanvas* cCal = new TCanvas("AMS_mapped", "mapped info", 10, 10, 500, 500);

       
    fh_Ams_energy_allStrips = new TH2F("fh_Ams_energy_allStrips", 
				     "Energy strip vs strip number", 
				     1024, 0, 1024, 200, 0, 3500);    
    fh_Ams_energy_allStrips->GetXaxis()->SetTitle("Strip number");
    fh_Ams_energy_allStrips->GetYaxis()->SetTitle("Energy [channels]");      
   // cCal->Add(fh_Ams_energy_allStrips);
    
    fh_Ams_energy_allStrips->Draw("col");
     // run->AddObject(cCalifa4[i]);  


    //MAIN FOLDER-AMS
    TFolder* mainfolAms = new TFolder("AMS", "AMS info");      
    //mainfolAms->Add(fh_Califa_cryId_petal);
    //mainfolAms->Add(fh_Califa_cryId_energy);
    //for (Int_t i=0;i<fCalifaNumPetals;i++){
      mainfolAms->Add(cCal);
    //}
    run->AddObject(mainfolAms);
    		
  
  return kSUCCESS;
}


void R3BAmsOnlineSpectra::Exec(Option_t* option) {
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BAmsOnlineSpectra::Exec FairRootManager not found" << FairLogger::endl;


  
  if(fMappedItemsAms && fMappedItemsAms->GetEntriesFast()){
    Int_t nHits = fMappedItemsAms->GetEntriesFast();
    
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BAmsMappedData* hit = 
	(R3BAmsMappedData*)fMappedItemsAms->At(ihit);
      if (!hit) continue;

      fh_Ams_energy_allStrips->Fill(500,hit->GetEnergy());

    }
  }
  

  fNEvents += 1;
}


void R3BAmsOnlineSpectra::FinishEvent() {

}


void R3BAmsOnlineSpectra::FinishTask() {

}

ClassImp(R3BAmsOnlineSpectra)
