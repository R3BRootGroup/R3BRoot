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
#include "R3BAmsHitData.h"
#include "R3BEventHeader.h"
#include "THttpServer.h"

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
#include <fstream>
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
  , fHitItemsAms(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fCalON(0) {
}

R3BAmsOnlineSpectra::R3BAmsOnlineSpectra(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItemsAms(NULL)
  , fCalItemsAms(NULL)
  , fHitItemsAms(NULL)
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
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "R3BAmsOnlineSpectra::Init FairRootManager not found");
  header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

  FairRunOnline *run = FairRunOnline::Instance();
  run->GetHttpServer()->Register("",this);

  
  //create histograms of all detectors  
  
  //get access to Mapped data
  fMappedItemsAms = (TClonesArray*)mgr->GetObject("AmsMappedData");
  if (!fMappedItemsAms) { return kFATAL;}
  
  //get access to Cal data
  fCalItemsAms = (TClonesArray*)mgr->GetObject("AmsStripCalData");

  //get access to Hit data
  fHitItemsAms = (TClonesArray*)mgr->GetObject("AmsHitData");

  
    //Energy range for strips
    Double_t bins=200;
    Double_t maxE=3500;
    Double_t minE=0;
    
    //  CANVAS 1  -------------------------------     
    cMap = new TCanvas("AMS_mapped", "mapped info", 10, 10, 500, 500);
    cMap->Divide(2,2);

    //  CANVAS 2  -------------------------------     
    cCal = new TCanvas("AMS_cal", "Cal info", 10, 10, 500, 500);
    cCal->Divide(2,4);

    //  CANVAS 3  -------------------------------     
    cHit = new TCanvas("AMS_hit", "Hit info", 10, 10, 500, 500);
    cHit->Divide(2,2);

    char Name1[255]; 
    char Name2[255];

    //Mapped data
    for(Int_t i=0;i<4;i++){//one histo per detector
     sprintf(Name1, "fh_Ams_energy_allStrips_%d", i+1);	  
     sprintf(Name2, "Energy vs strip number for AMS Det: %d", i+1);
     fh_Ams_energy_allStrips[i] = new TH2F(Name1, Name2, 1024, 0, 1024, bins, minE, maxE);    
     fh_Ams_energy_allStrips[i]->GetXaxis()->SetTitle("Strip number");
     fh_Ams_energy_allStrips[i]->GetYaxis()->SetTitle("Energy [channels]");
     fh_Ams_energy_allStrips[i]->GetYaxis()->SetTitleOffset(1.4);
     fh_Ams_energy_allStrips[i]->GetXaxis()->CenterTitle(true);
     fh_Ams_energy_allStrips[i]->GetYaxis()->CenterTitle(true);
     cMap->cd(i+1);
     fh_Ams_energy_allStrips[i]->Draw("col"); 
    }

    //Cal data
    for(Int_t i=0;i<4;i++){//two histo per detector
     for(Int_t j=0;j<2;j++){
      sprintf(Name1, "fh_Ams_energy_allCalStrips_%d", i*2+j);	  
      sprintf(Name2, "Cal_Energy vs strip number for AMS Det: %d and Side: %d", i+1, j);
      if(j==0)
      fh_Ams_energy_allCalStrips[i*2+j] = new TH2F(Name1, Name2, 640, 0, 640, bins, minE, maxE);
      else
      fh_Ams_energy_allCalStrips[i*2+j] = new TH2F(Name1, Name2, 384, 0, 384, bins, minE, maxE);
      fh_Ams_energy_allCalStrips[i*2+j]->GetXaxis()->SetTitle("Strip number");
      fh_Ams_energy_allCalStrips[i*2+j]->GetYaxis()->SetTitle("Energy [channels]");
      fh_Ams_energy_allCalStrips[i*2+j]->GetYaxis()->SetTitleOffset(1.4);
      fh_Ams_energy_allCalStrips[i*2+j]->GetXaxis()->CenterTitle(true);
      fh_Ams_energy_allCalStrips[i*2+j]->GetYaxis()->CenterTitle(true);
      cCal->cd(i*2+1+j);
      fh_Ams_energy_allCalStrips[i*2+j]->Draw("col");
     }
    }

    //Hit data
    for(Int_t i=0;i<4;i++){//one histo per detector
     sprintf(Name1, "fh_Ams_hit_%d", i+1);	  
     sprintf(Name2, "Y vs X for AMS Det: %d", i+1);
     fh_Ams_hit[i] = new TH2F(Name1, Name2, 350, 0, 70., 200, 0, 40.);    
     fh_Ams_hit[i]->GetXaxis()->SetTitle("X [mm]");
     fh_Ams_hit[i]->GetYaxis()->SetTitle("Y [mm]");
     fh_Ams_hit[i]->GetXaxis()->CenterTitle(true);
     fh_Ams_hit[i]->GetYaxis()->CenterTitle(true);
     fh_Ams_hit[i]->GetYaxis()->SetTitleOffset(1.1);
     fh_Ams_hit[i]->GetXaxis()->SetTitleOffset(1.1);
     cHit->cd(i+1);
     fh_Ams_hit[i]->Draw("col"); 
    }

    //MAIN FOLDER-AMS
    TFolder* mainfolAms = new TFolder("AMS","AMS info");      
    mainfolAms->Add(cMap);
    mainfolAms->Add(cCal);
    mainfolAms->Add(cHit);
    run->AddObject(mainfolAms);

    //Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_AMS", Form("/Objects/%s/->Reset_AMS_Histo()", GetName()));
    
  return kSUCCESS;
}

void R3BAmsOnlineSpectra::Reset_AMS_Histo()
{
    LOG(INFO) << "R3BAmsOnlineSpectra::Reset_AMS_Histo" << FairLogger::endl;

    //Mapped data
    for(Int_t i=0;i<4;i++){
     fh_Ams_energy_allStrips[i]->Reset();
    }

    //Cal data
    for(Int_t i=0;i<4;i++){
     for(Int_t j=0;j<2;j++){
      fh_Ams_energy_allCalStrips[i*2+j]->Reset();
     }
    }
    
    //Hit data
    for(Int_t i=0;i<4;i++){
     fh_Ams_hit[i]->Reset();
    }
}


void R3BAmsOnlineSpectra::Exec(Option_t* option) {
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BAmsOnlineSpectra::Exec FairRootManager not found" << FairLogger::endl;

  //Fill mapped data
  if(fMappedItemsAms && fMappedItemsAms->GetEntriesFast()){
    Int_t nHits = fMappedItemsAms->GetEntriesFast();
    
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BAmsMappedData* hit = 
	(R3BAmsMappedData*)fMappedItemsAms->At(ihit);
      if (!hit) continue;
      fh_Ams_energy_allStrips[hit->GetDetectorId()]->Fill(hit->GetStripId(),hit->GetEnergy());
    }
  }

  //Fill cal data
  if(fCalItemsAms && fCalItemsAms->GetEntriesFast()){
    Int_t nHits = fCalItemsAms->GetEntriesFast();
       // std::cout << "hit:"<<nHits << std::endl;
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BAmsStripCalData* hit = 
	(R3BAmsStripCalData*)fCalItemsAms->At(ihit);
      if (!hit) continue;
        //std::cout << "hit:"<<hit->GetDetId() << " " << hit->GetSideId() << std::endl;
       fh_Ams_energy_allCalStrips[hit->GetDetId()*2+hit->GetSideId()]->Fill(hit->GetStripId(),hit->GetEnergy());
    }
  }

  //Fill hit data
  if(fHitItemsAms && fHitItemsAms->GetEntriesFast()){
    Int_t nHits = fHitItemsAms->GetEntriesFast();
    //std::cout << nHits << std::endl;
    //FIXME: check plots for multiplicity>1
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BAmsHitData* hit = 
	(R3BAmsHitData*)fHitItemsAms->At(ihit);
      if (!hit) continue;
      fh_Ams_hit[hit->GetDetId()]->Fill(hit->GetX(),hit->GetY());
    }
  }
 
  fNEvents += 1;
}


void R3BAmsOnlineSpectra::FinishEvent() {

    if (fMappedItemsAms)
    {
        fMappedItemsAms->Clear();
    }
    if (fCalItemsAms)
    {
        fCalItemsAms->Clear();
    }
    if (fHitItemsAms)
    {
        fHitItemsAms->Clear();
    }
}


void R3BAmsOnlineSpectra::FinishTask() {

    if(fMappedItemsAms){
	cMap->Write();
    }
    if (fCalItemsAms)
    {
	cCal->Write();
    }
    if (fHitItemsAms)
    {
	cHit->Write();
    }
}

ClassImp(R3BAmsOnlineSpectra)
