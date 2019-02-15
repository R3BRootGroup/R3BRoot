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



  
  //MAIN FOLDER-AMS-CALIFA
  TFolder* mainfolAmsCal = new TFolder("AMS_CALIFA","AMS and CALIFA correlations");      
  //mainfolAmsCal->Add(cMap);
  run->AddObject(mainfolAmsCal);

  //Register command to reset histograms
  run->GetHttpServer()->RegisterCommand("Reset_AMS_CALIFA", Form("/Objects/%s/->Reset_AMS_CALIFA_Histo()", GetName()));
    
  return kSUCCESS;
}

void R3BAmsCalifaCorrelatedOnlineSpectra::Reset_AMS_CALIFA_Histo()
{
    LOG(INFO) << "R3BAmsCalifaCorrelatedOnlineSpectra::Reset_AMS_CALIFA_Histo" << FairLogger::endl;


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

/*

  //Fill hit data
  if(fHitItemsAms && fHitItemsAms->GetEntriesFast()){
    Int_t nHits = fHitItemsAms->GetEntriesFast();
    //std::cout << nHits << std::endl;
    Int_t mulhit[fNbDet];
    for(Int_t i=0;i<fNbDet;i++)mulhit[i]=0;
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BAmsHitData* hit = 
	(R3BAmsHitData*)fHitItemsAms->At(ihit);
      if (!hit) continue;
      fh_Ams_hit_Pos[hit->GetDetId()]->Fill(hit->GetX(),hit->GetY());
      fh_Ams_hit_E[hit->GetDetId()]->Fill(hit->GetEnergyX(),hit->GetEnergyY());
      mulhit[hit->GetDetId()]++;
    }
    for(Int_t i=0;i<fNbDet;i++)fh_Ams_hit_Mul[i]->Fill(mulhit[i]);
  }
 */
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
	//for(Int_t i=0;i<fNbDet;i++)cHit[i]->Write();
    }
}

ClassImp(R3BAmsCalifaCorrelatedOnlineSpectra)
