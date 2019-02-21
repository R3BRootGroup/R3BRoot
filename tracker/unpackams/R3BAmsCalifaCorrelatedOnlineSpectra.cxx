// --------------------------------------------------------------
// -----       R3BAmsCalifaCorrelatedOnlineSpectra             --
// -----    Created 17/02/19  by J.L. Rodriguez-Sanchez        --
// ----- Fill AMS and CALIFA correlations in online histograms --
// --------------------------------------------------------------

/*
 *  This taks reads hit data from AMS and califa detectors and plots 
 *  online histograms   
 */

#include "R3BAmsCalifaCorrelatedOnlineSpectra.h"
#include "R3BAmsHitData.h"
#include "R3BCalifaHitData.h"
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

R3BAmsCalifaCorrelatedOnlineSpectra::R3BAmsCalifaCorrelatedOnlineSpectra()
  : FairTask("AmsCalifaOnlineSpectra", 1)
  , fHitItemsAms(NULL)
  , fHitItemsCalifa(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fNbDet(4) {
}

R3BAmsCalifaCorrelatedOnlineSpectra::R3BAmsCalifaCorrelatedOnlineSpectra(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fHitItemsAms(NULL)
  , fHitItemsCalifa(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fNbDet(4) {
}

R3BAmsCalifaCorrelatedOnlineSpectra::~R3BAmsCalifaCorrelatedOnlineSpectra() {
}

InitStatus R3BAmsCalifaCorrelatedOnlineSpectra::Init() {
  
  LOG(INFO) << "R3BAmsCalifaCorrelatedOnlineSpectra::Init " << FairLogger::endl;
  
  // try to get a handle on the EventHeader. EventHeader may not be 
  // present though and hence may be null. Take care when using.
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(fatal) << "R3BAmsCalifaCorrelatedOnlineSpectra::Init FairRootManager not found";
  header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

  FairRunOnline *run = FairRunOnline::Instance();
  run->GetHttpServer()->Register("",this);

  
  //create histograms of all detectors  
  
  //get access to Hit data
  fHitItemsAms = (TClonesArray*)mgr->GetObject("AmsHitData");
  if (!fHitItemsAms) {
   LOG(INFO)<<"R3BAmsCalifaCorrelatedOnlineSpectra::Init AmsHitData not found"<<FairLogger::endl;
  }

  //get access to Hit data
  fHitItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaHitData");
  if (!fHitItemsCalifa) {
   LOG(INFO)<<"R3BAmsCalifaCorrelatedOnlineSpectra::Init CalifaHitData not found"<<FairLogger::endl;
  }


    //  CANVAS 1  -------------------------------     
    cMap = new TCanvas("AMS_left_Petal_3_4", "Correlation AMS_left and Califa Petals 3 and 4", 10, 10, 500, 500);
    //cMap->Divide(2,2);
    char Name1[255]; 
    char Name2[255];


    fh_Califa_coinc_petal1 = new TH2F("Ams_left_vs_petal_3and4","Correlation AMS_left and Califa Petals 3 and 4",
                                 90, 0, 90, 90, 0, 90);
    fh_Califa_coinc_petal1->GetXaxis()->SetTitle("Theta AMS left (degrees)");
    fh_Califa_coinc_petal1->GetYaxis()->SetTitle("Theta petal 3 and 4 (degrees)");
    fh_Califa_coinc_petal1->GetYaxis()->SetTitleOffset(1.2);
    fh_Califa_coinc_petal1->GetXaxis()->CenterTitle(true);
    fh_Califa_coinc_petal1->GetYaxis()->CenterTitle(true);
    fh_Califa_coinc_petal1->Draw("COLZ");

  
  //MAIN FOLDER-AMS-CALIFA
  TFolder* mainfolAmsCal = new TFolder("AMS_CALIFA","AMS and CALIFA correlations");      
  mainfolAmsCal->Add(cMap);
  run->AddObject(mainfolAmsCal);

  //Register command to reset histograms
  run->GetHttpServer()->RegisterCommand("Reset_AMS_CALIFA", Form("/Objects/%s/->Reset_AMS_CALIFA_Histo()", GetName()));
    
  return kSUCCESS;
}

void R3BAmsCalifaCorrelatedOnlineSpectra::Reset_AMS_CALIFA_Histo()
{
    LOG(INFO) << "R3BAmsCalifaCorrelatedOnlineSpectra::Reset_AMS_CALIFA_Histo" << FairLogger::endl;


    fh_Califa_coinc_petal1->Reset();

    //Hit data
    for(Int_t i=0;i<fNbDet;i++){
     //fh_Ams_hit_Mul[i]->Reset();
     //fh_Ams_hit_Pos[i]->Reset();
     //fh_Ams_hit_E[i]->Reset();
    }
}


void R3BAmsCalifaCorrelatedOnlineSpectra::Exec(Option_t* option) {
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BAmsCalifaCorrelatedOnlineSpectra::Exec FairRootManager not found" << FairLogger::endl;


  double hitams[100];
  for(Int_t i = 0; i < 100; i++)hitams[i]=0.;
  int nbhitams=0;

  //Fill hit data
  if(fHitItemsAms && fHitItemsAms->GetEntriesFast()){
    Int_t nHits = fHitItemsAms->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BAmsHitData* hit = 
	(R3BAmsHitData*)fHitItemsAms->At(ihit);
      if (!hit) continue;
      if (hit->GetEnergyX()<100 || hit->GetEnergyY()<100 ||
          hit->GetEnergyX()>500 || hit->GetEnergyY()>500)
	      continue;

      if(hit->GetDetId()==3){
        hitams[nbhitams]=hit->GetTheta()/TMath::Pi()*180.;nbhitams++;
      }

    }
  }

  double hitcal[100];
  for(Int_t i = 0; i < 100; i++)hitcal[i]=0.;
  int nbhitcal=0;

 if(fHitItemsCalifa && fHitItemsCalifa->GetEntriesFast()){
    Int_t nHits = fHitItemsCalifa->GetEntriesFast();

    Double_t theta=0., phi=0.;
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BCalifaHitData* hit =
	(R3BCalifaHitData*)fHitItemsCalifa->At(ihit);
      if (!hit) continue;
      theta=hit->GetTheta()/TMath::Pi()*180.;
      phi=hit->GetPhi()/TMath::Pi()*180.;
      if(phi>-100&&phi<-10 && hit->GetEnergy()>3000.){// petal 3 and 4 and E>3000keV
        hitcal[nbhitcal]=hit->GetTheta()/TMath::Pi()*180.;nbhitcal++;
      }
    }
  }


    if (nbhitams>0 && nbhitcal>0)
    {
 
     for(Int_t i = 0; i < nbhitams; i++){
      for(Int_t j = 0; j < nbhitcal; j++)
       fh_Califa_coinc_petal1->Fill(hitams[i],hitcal[j]);
     }
    }

 
  fNEvents += 1;
}


void R3BAmsCalifaCorrelatedOnlineSpectra::FinishEvent() {

    if (fHitItemsAms)
    {
        fHitItemsAms->Clear();
    }
    if (fHitItemsCalifa)
    {
        fHitItemsCalifa->Clear();
    }
}


void R3BAmsCalifaCorrelatedOnlineSpectra::FinishTask() {

    if (fHitItemsAms && fHitItemsCalifa)
    {
       cMap->Write();
    }
}

ClassImp(R3BAmsCalifaCorrelatedOnlineSpectra)
