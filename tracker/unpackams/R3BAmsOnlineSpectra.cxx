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
#include "TRandom.h"

#include "TClonesArray.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <array>
#include "TMath.h"
#include "TVector3.h"

using namespace std;

R3BAmsOnlineSpectra::R3BAmsOnlineSpectra()
  : FairTask("AmsOnlineSpectra", 1)
  , fMappedItemsAms(NULL)
  , fCalItemsAms(NULL)
  , fHitItemsAms(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fNbDet(4) {
}

R3BAmsOnlineSpectra::R3BAmsOnlineSpectra(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItemsAms(NULL)
  , fCalItemsAms(NULL)
  , fHitItemsAms(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fNbDet(4) {
}

R3BAmsOnlineSpectra::~R3BAmsOnlineSpectra() {
}

InitStatus R3BAmsOnlineSpectra::Init() {
  
  LOG(INFO) << "R3BAmsOnlineSpectra::Init " << FairLogger::endl;
  
  // try to get a handle on the EventHeader. EventHeader may not be 
  // present though and hence may be null. Take care when using.
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(fatal) << "R3BAmsOnlineSpectra::Init FairRootManager not found";
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
    cCal->Divide(2,fNbDet);

    //  CANVAS 3  -------------------------------    
    cHit[0] = new TCanvas("AMS_hit_top", "Hit info", 10, 10, 500, 500);
    cHit[0]->Divide(2,2);
    cHit[1] = new TCanvas("AMS_hit_right", "Hit info", 10, 10, 500, 500);
    cHit[1]->Divide(2,2);
    cHit[2] = new TCanvas("AMS_hit_bottom", "Hit info", 10, 10, 500, 500);
    cHit[2]->Divide(2,2);
    cHit[3] = new TCanvas("AMS_hit_left", "Hit info", 10, 10, 500, 500);
    cHit[3]->Divide(2,2);

    char Name1[255]; 
    char Name2[255];

    //Mapped data
    for(Int_t i=0;i<fNbDet;i++){//one histo per detector
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
    for(Int_t i=0;i<fNbDet;i++){//two histo per detector
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
    for(Int_t i=0;i<fNbDet;i++){//one histo per detector
     sprintf(Name1, "fh_Ams_hit_Mul_%d", i+1);	  
     sprintf(Name2, "Multiplicity Det: %d", i+1);
     fh_Ams_hit_Mul[i] = new TH1F(Name1, Name2, 10, -0.5, 9.5);    
     fh_Ams_hit_Mul[i]->GetXaxis()->SetTitle("Multiplicity");
     fh_Ams_hit_Mul[i]->GetYaxis()->SetTitle("Counts");
     fh_Ams_hit_Mul[i]->GetXaxis()->CenterTitle(true);
     fh_Ams_hit_Mul[i]->GetYaxis()->CenterTitle(true);
     fh_Ams_hit_Mul[i]->GetYaxis()->SetTitleOffset(1.1);
     fh_Ams_hit_Mul[i]->GetXaxis()->SetTitleOffset(1.1);
     cHit[i]->cd(1);
     fh_Ams_hit_Mul[i]->Draw("");

     sprintf(Name1, "fh_Ams_hit_Pos_%d", i+1);	  
     sprintf(Name2, "Y vs Z for AMS Det: %d", i+1);
     fh_Ams_hit_Pos[i] = new TH2F(Name1, Name2, 350, 0, 70., 200, 0, 40.);    
     fh_Ams_hit_Pos[i]->GetXaxis()->SetTitle("Z [mm]");
     fh_Ams_hit_Pos[i]->GetYaxis()->SetTitle("XorY [mm]");
     fh_Ams_hit_Pos[i]->GetXaxis()->CenterTitle(true);
     fh_Ams_hit_Pos[i]->GetYaxis()->CenterTitle(true);
     fh_Ams_hit_Pos[i]->GetYaxis()->SetTitleOffset(1.1);
     fh_Ams_hit_Pos[i]->GetXaxis()->SetTitleOffset(1.1);
     cHit[i]->cd(2);
     fh_Ams_hit_Pos[i]->Draw("col");

     sprintf(Name1, "fh_Ams_hit_E_%d", i+1);	  
     sprintf(Name2, "Energy_Y vs Energy_Z for AMS Det: %d", i+1);
     fh_Ams_hit_E[i] = new TH2F(Name1, Name2, 2000, 0, 6000., 2000, 0, 6000.);    
     fh_Ams_hit_E[i]->GetXaxis()->SetTitle("Energy_Z [ADC units]");
     fh_Ams_hit_E[i]->GetYaxis()->SetTitle("Energy_Y [ADC units]");
     fh_Ams_hit_E[i]->GetXaxis()->CenterTitle(true);
     fh_Ams_hit_E[i]->GetYaxis()->CenterTitle(true);
     fh_Ams_hit_E[i]->GetYaxis()->SetTitleOffset(1.1);
     fh_Ams_hit_E[i]->GetXaxis()->SetTitleOffset(1.1);
     cHit[i]->cd(3);
     fh_Ams_hit_E[i]->Draw("col");

     sprintf(Name1, "fh_Ams_hit_E_theta_%d", i+1);	  
     sprintf(Name2, "Energy_Z vs Theta for AMS Det: %d", i+1);
     fh_Ams_hit_E_theta[i] = new TH2F(Name1, Name2, 500, 0, 90., 500, 0., 6000.);    
     fh_Ams_hit_E_theta[i]->GetXaxis()->SetTitle("Theta [degrees]");
     fh_Ams_hit_E_theta[i]->GetYaxis()->SetTitle("Energy_Z [ADC units]");
     fh_Ams_hit_E_theta[i]->GetXaxis()->CenterTitle(true);
     fh_Ams_hit_E_theta[i]->GetYaxis()->CenterTitle(true);
     fh_Ams_hit_E_theta[i]->GetYaxis()->SetTitleOffset(1.1);
     fh_Ams_hit_E_theta[i]->GetXaxis()->SetTitleOffset(1.1);
     cHit[i]->cd(4);
     fh_Ams_hit_E_theta[i]->Draw("col");
    }


    //CANVAS 7
    cAMSangles = new TCanvas("AMS_Theta_vs_Phi",
				    "Theta vs Phi",
				    10, 10, 500, 500);
    fh_ams_theta_phi = new TH2F("Ams_theta_vs_phi","AMS theta vs phi",
                                 90, 0, 90,180, -180, 180);
    fh_ams_theta_phi->GetXaxis()->SetTitle("Theta (degrees)");
    fh_ams_theta_phi->GetYaxis()->SetTitle("Phi (degrees)");
    fh_ams_theta_phi->GetXaxis()->SetTitleOffset(1.2);
    fh_ams_theta_phi->GetYaxis()->SetTitleOffset(1.2);
    fh_ams_theta_phi->GetXaxis()->CenterTitle(true);
    fh_ams_theta_phi->GetYaxis()->CenterTitle(true);
    fh_ams_theta_phi->Draw("COLZ");


    //CANVAS 8
    cHitone = new TCanvas("AMS_theta_top_bottom", "theta between opposite detectors", 10, 10, 500, 500);
    cHitone->Divide(1,2);

    fh_ams_thetatop_thetabottom = new TH2F("Ams_theta_top_vs_theta_bottom","AMS theta top vs theta bottom",
                                 90, 0, 90,90, 0, 90);
    fh_ams_thetatop_thetabottom->GetXaxis()->SetTitle("Theta top (degrees)");
    fh_ams_thetatop_thetabottom->GetYaxis()->SetTitle("Theta bottom (degrees)");
    fh_ams_thetatop_thetabottom->GetXaxis()->SetTitleOffset(1.2);
    fh_ams_thetatop_thetabottom->GetYaxis()->SetTitleOffset(1.2);
    fh_ams_thetatop_thetabottom->GetXaxis()->CenterTitle(true);
    fh_ams_thetatop_thetabottom->GetYaxis()->CenterTitle(true);
    cHitone->cd(1);
    fh_ams_thetatop_thetabottom->Draw("COLZ");

    sprintf(Name1, "fh_Ams_opening_angle1");	  
    sprintf(Name2, "AMS Opening Angle (beam hits target at (0,0,0))");
    fh_Ams_openangle1 = new TH1F(Name1, Name2, 140, 0, 140);    
    fh_Ams_openangle1->GetXaxis()->SetTitle("Opening angle [degrees]");
    fh_Ams_openangle1->GetYaxis()->SetTitle("Counts");
    fh_Ams_openangle1->GetXaxis()->CenterTitle(true);
    fh_Ams_openangle1->GetYaxis()->CenterTitle(true);
    fh_Ams_openangle1->GetYaxis()->SetTitleOffset(1.2);
    fh_Ams_openangle1->GetXaxis()->SetTitleOffset(1.2);
    cHitone->cd(2);
    fh_Ams_openangle1->Draw();


    //  CANVAS 9  -------------------------------    
    cHittwo = new TCanvas("AMS_theta_left_right", "theta between opposite detectors", 10, 10, 500, 500);
    cHittwo->Divide(1,2);

    fh_ams_thetaright_thetaleft = new TH2F("Ams_theta_left_vs_theta_right","AMS theta left vs theta right",
                                 90, 0, 90,90, 0, 90);
    fh_ams_thetaright_thetaleft->GetXaxis()->SetTitle("Theta left (degrees)");
    fh_ams_thetaright_thetaleft->GetYaxis()->SetTitle("Theta right (degrees)");
    fh_ams_thetaright_thetaleft->GetXaxis()->SetTitleOffset(1.2);
    fh_ams_thetaright_thetaleft->GetYaxis()->SetTitleOffset(1.2);
    fh_ams_thetaright_thetaleft->GetXaxis()->CenterTitle(true);
    fh_ams_thetaright_thetaleft->GetYaxis()->CenterTitle(true);
    cHittwo->cd(1);
    fh_ams_thetaright_thetaleft->Draw("COLZ");


    sprintf(Name1, "fh_Ams_opening_angle2");	  
    sprintf(Name2, "AMS Opening Angle (beam hits target at (0,0,0))");
    fh_Ams_openangle2 = new TH1F(Name1, Name2, 140, 0, 140);    
    fh_Ams_openangle2->GetXaxis()->SetTitle("Opening angle [degrees]");
    fh_Ams_openangle2->GetYaxis()->SetTitle("Counts");
    fh_Ams_openangle2->GetXaxis()->CenterTitle(true);
    fh_Ams_openangle2->GetYaxis()->CenterTitle(true);
    fh_Ams_openangle2->GetYaxis()->SetTitleOffset(1.2);
    fh_Ams_openangle2->GetXaxis()->SetTitleOffset(1.2);
    cHittwo->cd(2);
    fh_Ams_openangle2->Draw();


    //  CANVAS 10  ------------------------------ 
    cHitPhis = new TCanvas("AMS_Phi_Angles", "Phi between opposite detectors", 10, 10, 500, 500);
    fh_ams_phis = new TH2F("Ams_phis","AMS Phi angles between opposite detectors",
                                 90, 0, 360,90, 0, 360);
    fh_ams_phis->GetXaxis()->SetTitle("Phi1 (degrees)");
    fh_ams_phis->GetYaxis()->SetTitle("Phi2 (degrees)");
    fh_ams_phis->GetXaxis()->SetTitleOffset(1.2);
    fh_ams_phis->GetYaxis()->SetTitleOffset(1.2);
    fh_ams_phis->GetXaxis()->CenterTitle(true);
    fh_ams_phis->GetYaxis()->CenterTitle(true);
    fh_ams_phis->Draw("COLZ");


    //MAIN FOLDER-AMS
    TFolder* mainfolAms = new TFolder("AMS","AMS info");      
    mainfolAms->Add(cMap);
    if(fCalItemsAms)mainfolAms->Add(cCal);
    if(fHitItemsAms)for(Int_t i=0;i<fNbDet;i++)mainfolAms->Add(cHit[i]);
    mainfolAms->Add(cAMSangles);
    mainfolAms->Add(cHitone);
    mainfolAms->Add(cHittwo);
    mainfolAms->Add(cHitPhis);
    run->AddObject(mainfolAms);

    //Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_AMS", Form("/Objects/%s/->Reset_AMS_Histo()", GetName()));
    
  return kSUCCESS;
}

void R3BAmsOnlineSpectra::Reset_AMS_Histo()
{
    LOG(INFO) << "R3BAmsOnlineSpectra::Reset_AMS_Histo" << FairLogger::endl;

    //Mapped data
    for(Int_t i=0;i<fNbDet;i++){
     fh_Ams_energy_allStrips[i]->Reset();
    }

    //Cal data
    for(Int_t i=0;i<fNbDet;i++){
     for(Int_t j=0;j<2;j++){
      fh_Ams_energy_allCalStrips[i*2+j]->Reset();
     }
    }
    
    //Hit data
    for(Int_t i=0;i<fNbDet;i++){
     fh_Ams_hit_Mul[i]->Reset();
     fh_Ams_hit_Pos[i]->Reset();
     fh_Ams_hit_E[i]->Reset();
     fh_Ams_hit_E_theta[i]->Reset();
    }
    fh_ams_theta_phi->Reset();
    fh_ams_thetatop_thetabottom->Reset();
    fh_ams_thetaright_thetaleft->Reset();
    fh_ams_phis->Reset();
    fh_Ams_openangle1->Reset();
    fh_Ams_openangle2->Reset();
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

  TVector3 master[1000][fNbDet];
  Int_t mulhit[fNbDet];
  for(Int_t i=0;i<fNbDet;i++)mulhit[i]=0;
  //Fill hit data
  if(fHitItemsAms && fHitItemsAms->GetEntriesFast()){
    Int_t nHits = fHitItemsAms->GetEntriesFast();
    //std::cout << nHits << std::endl;
       //std::cout << "Ev:" << std::endl;
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BAmsHitData* hit = 
	(R3BAmsHitData*)fHitItemsAms->At(ihit);
      if (!hit) continue;
      fh_Ams_hit_Pos[hit->GetDetId()]->Fill(hit->GetX(),hit->GetY());
      fh_Ams_hit_E[hit->GetDetId()]->Fill(hit->GetEnergyX(),hit->GetEnergyY());
      fh_Ams_hit_E_theta[hit->GetDetId()]->Fill(hit->GetTheta()/TMath::Pi()*180.,hit->GetEnergyX());
      fh_ams_theta_phi->Fill(hit->GetTheta()/TMath::Pi()*180.,hit->GetPhi()/TMath::Pi()*180.);
      if(mulhit[hit->GetDetId()]<1000){
       master[mulhit[hit->GetDetId()]][hit->GetDetId()].SetMagThetaPhi(1.,hit->GetTheta(),hit->GetPhi()); 
       //std::cout << hit->GetDetId() << std::endl;
      }
      mulhit[hit->GetDetId()]++;
    }
    for(Int_t i=0;i<fNbDet;i++)fh_Ams_hit_Mul[i]->Fill(mulhit[i]);
  }

  Double_t thetaopen=0., phi1=0., phi2=0.;

  for(Int_t i=0;i<mulhit[0];i++){
   for(Int_t j=i+1;j<mulhit[1];j++){
     phi1=master[i][0].Phi();
     phi2=master[j][1].Phi();
     if(phi1<0)phi1=phi1+2.*TMath::Pi();
     if(phi2<0)phi2=phi2+2.*TMath::Pi();
     if(0.5<gRandom->Uniform(0.,1.))
      fh_ams_phis->Fill(phi1/TMath::Pi()*180.,phi2/TMath::Pi()*180.);
     else
      fh_ams_phis->Fill(phi2/TMath::Pi()*180.,phi1/TMath::Pi()*180.);
   }
  }

  for(Int_t i=0;i<mulhit[0];i++){
   for(Int_t j=i+1;j<mulhit[2];j++){
    //std::cout<< master[i].Angle(master[j])/TMath::Pi()*180. <<std::endl;
    thetaopen=master[i][0].Angle(master[j][2])/TMath::Pi()*180.;
    if(thetaopen>0){
     fh_ams_thetatop_thetabottom->Fill(master[i][0].Theta()/TMath::Pi()*180.,master[j][2].Theta()/TMath::Pi()*180.);
     fh_Ams_openangle1->Fill(thetaopen);
     phi1=master[i][0].Phi();
     phi2=master[j][2].Phi();
     //std::cout<< phi1 << " " <<phi2 <<std::endl;
     if(phi1<0)phi1=phi1+2.*TMath::Pi();
     if(phi2<0)phi2=phi2+2.*TMath::Pi();
     if(0.5<gRandom->Uniform(0.,1.))
      fh_ams_phis->Fill(phi1/TMath::Pi()*180.,phi2/TMath::Pi()*180.);
     else
      fh_ams_phis->Fill(phi2/TMath::Pi()*180.,phi1/TMath::Pi()*180.);
    }else{std::cout << "Problem with opening angles" << std::endl;}
   }
  }

  for(Int_t i=0;i<mulhit[0];i++){
   for(Int_t j=i+1;j<mulhit[3];j++){
     phi1=master[i][0].Phi();
     phi2=master[j][3].Phi();
     if(phi1<0)phi1=phi1+2.*TMath::Pi();
     if(phi2<0)phi2=phi2+2.*TMath::Pi();
     if(0.5<gRandom->Uniform(0.,1.))
      fh_ams_phis->Fill(phi1/TMath::Pi()*180.,phi2/TMath::Pi()*180.);
     else
      fh_ams_phis->Fill(phi2/TMath::Pi()*180.,phi1/TMath::Pi()*180.);
   }
  }

  for(Int_t i=0;i<mulhit[1];i++){
   for(Int_t j=i+1;j<mulhit[2];j++){
     phi1=master[i][1].Phi();
     phi2=master[j][2].Phi();
     if(phi1<0)phi1=phi1+2.*TMath::Pi();
     if(phi2<0)phi2=phi2+2.*TMath::Pi();
     if(0.5<gRandom->Uniform(0.,1.))
      fh_ams_phis->Fill(phi1/TMath::Pi()*180.,phi2/TMath::Pi()*180.);
     else
      fh_ams_phis->Fill(phi2/TMath::Pi()*180.,phi1/TMath::Pi()*180.);
   }
  }



  for(Int_t i=0;i<mulhit[1];i++){
   for(Int_t j=i+1;j<mulhit[3];j++){
    
    //std::cout<< master[i].Angle(master[j])/TMath::Pi()*180. <<std::endl;
    thetaopen=master[i][1].Angle(master[j][3])/TMath::Pi()*180.;
    if(thetaopen>0){
     fh_ams_thetaright_thetaleft->Fill(master[i][1].Theta()/TMath::Pi()*180.,master[j][3].Theta()/TMath::Pi()*180.);
     fh_Ams_openangle2->Fill(thetaopen);
     phi1=master[i][1].Phi();
     phi2=master[j][3].Phi();
     if(phi1<0)phi1=phi1+2.*TMath::Pi();
     if(phi2<0)phi2=phi2+2.*TMath::Pi();
     if(0.5<gRandom->Uniform(0.,1.))
      fh_ams_phis->Fill(phi1/TMath::Pi()*180.,phi2/TMath::Pi()*180.);
     else
      fh_ams_phis->Fill(phi2/TMath::Pi()*180.,phi1/TMath::Pi()*180.);
    }else{std::cout << "Problem with opening angles" << std::endl;}
   }
  }

  for(Int_t i=0;i<mulhit[2];i++){
   for(Int_t j=i+1;j<mulhit[3];j++){
     phi1=master[i][2].Phi();
     phi2=master[j][3].Phi();
     if(phi1<0)phi1=phi1+2.*TMath::Pi();
     if(phi2<0)phi2=phi2+2.*TMath::Pi();
     if(0.5<gRandom->Uniform(0.,1.))
      fh_ams_phis->Fill(phi1/TMath::Pi()*180.,phi2/TMath::Pi()*180.);
     else
      fh_ams_phis->Fill(phi2/TMath::Pi()*180.,phi1/TMath::Pi()*180.);
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
	for(Int_t i=0;i<fNbDet;i++)cHit[i]->Write();
        cAMSangles->Write();
        cHitone->Write();
        cHittwo->Write();
        cHitPhis->Write();
    }
}

ClassImp(R3BAmsOnlineSpectra)
