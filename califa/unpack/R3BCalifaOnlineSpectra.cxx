// ------------------------------------------------------------
// -----                  R3BCalifaOnlineSpectra          -----
// -----          Created April 21th 2018 by E. Galiana   -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with CALIFA online data  
 */

#include "R3BCalifaOnlineSpectra.h"
#include "R3BCalifaMappedData.h"
#include "R3BCalifaCrystalCalData.h"
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
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <array>
#include "TMath.h"

using namespace std;

R3BCalifaOnlineSpectra::R3BCalifaOnlineSpectra()
  : FairTask("CALIFAOnlineSpectra", 1)
  , fMappedItemsCalifa(NULL)
  , fCalItemsCalifa(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fCalifaNumPetals(1)
  , fNumCrystalPetal(64)
  , fMapHistos_bins(3000)
  , fMapHistos_max(3000)
  , fRaw2Cal(kFALSE)
  , fLogScale(kTRUE)
  , fFebex2Preamp(kTRUE)
  , fCalON(kTRUE) {
}

R3BCalifaOnlineSpectra::R3BCalifaOnlineSpectra(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItemsCalifa(NULL)
  , fCalItemsCalifa(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fCalifaNumPetals(1)
  , fNumCrystalPetal(64)
  , fMapHistos_bins(3000)
  , fMapHistos_max(3000)
  , fRaw2Cal(kFALSE)
  , fLogScale(kTRUE)
  , fFebex2Preamp(kTRUE)
  , fCalON(kTRUE) {
}

R3BCalifaOnlineSpectra::~R3BCalifaOnlineSpectra() {
}

InitStatus R3BCalifaOnlineSpectra::Init() {
  
  LOG(INFO) << "R3BCalifaOnlineSpectra::Init " << FairLogger::endl;
  
  // try to get a handle on the EventHeader. EventHeader may not be 
  // present though and hence may be null. Take care when using.
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BCalifaOnlineSpectra::Init FairRootManager not found" << FairLogger::endl;
  
  //Define Preamp sequence for histograms 
  Int_t PreampOrder[16]={6,5,4,3,2,1,0,7,8,15,14,13,12,11,10,9};
   for(Int_t i=0;i<16;i++){
     fOrderFebexPreamp[i]=PreampOrder[i];
   }

  header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
  FairRunOnline *run = FairRunOnline::Instance();

  run->GetHttpServer()->Register("",this);
  
  //get access to Mapped data
  fMappedItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaMappedData");
  if (!fMappedItemsCalifa) { return kFATAL;}
  
  //get access to Cal data
  fCalItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaCrystalCalData");
  if (!fCalItemsCalifa) {
   LOG(INFO)<<"R3BCalifaOnlineSpectra::Init CalifaCrystalCalData not found"<<FairLogger::endl;
   fCalON=kFALSE;
  }


  //reading the file
    Bool_t noFile = kFALSE;    
    ifstream *FileHistos = new ifstream(fCalifaFile);
    if(!FileHistos->is_open()){
      LOG(WARNING)<<"R3BCalifaOnlineSpectra:  No Histogram definition file"<<FairLogger::endl;
      noFile=kTRUE;
    }

    Double_t arry_bins[fCalifaNumPetals*fNumCrystalPetal];
    Double_t arry_maxE[fCalifaNumPetals*fNumCrystalPetal];
    Double_t arry_minE[fCalifaNumPetals*fNumCrystalPetal];

    Double_t bins=fMapHistos_bins;
    Double_t maxE=fMapHistos_max;
    Double_t minE=0.;

    if(!noFile) {
      for (Int_t i=0; i<fCalifaNumPetals*fNumCrystalPetal; i++){
	*FileHistos>>arry_bins[i]>>arry_minE[i]>>arry_maxE[i];	
      }
    }
    else{
      for (Int_t i=0; i<fCalifaNumPetals*fNumCrystalPetal; i++){
	arry_bins[i] = fMapHistos_bins;
	arry_minE[i] = 0; 
	arry_maxE[i] = fMapHistos_max;	
      }
    }

    //CANVAS 1
    cCalifa1 = new TCanvas("Califa_petal_vs_cryNb", 
				    "Califa_petal_vs_cryNb", 
				    10, 10, 500, 500);

    fh_Califa_cryId_petal = new TH2F("fh_Califa_cryNb_petal","Califa petal number vs crystal number",
                    fNumCrystalPetal,1,fNumCrystalPetal+1,fCalifaNumPetals,1,fCalifaNumPetals+1);
    fh_Califa_cryId_petal->GetXaxis()->SetTitle("Crystal number [1-64]");
    fh_Califa_cryId_petal->GetYaxis()->SetTitle("Petal number [1-7]"); 
    fh_Califa_cryId_petal->GetYaxis()->SetTitleOffset(1.2);
    fh_Califa_cryId_petal->GetXaxis()->CenterTitle(true);
    fh_Califa_cryId_petal->GetYaxis()->CenterTitle(true);
    fh_Califa_cryId_petal->Draw("COLZ");

    TString Name1; 
    TString Name2;  
    TString Yaxis1; 

    TString Name3; 
    TString Name4; 
    TString Yaxis2; 

    //raw data
    Name1="fh_Califa_Map_cryNb_energy";
    Name2="Califa_Map energy vs cryNb";
    Yaxis1="Energy (channels)";

    //cal data
    Name3="fh_Califa_Cal_cryNb_energy";
    Name4="Califa_Cal energy vs cryNb";
    Yaxis2="Energy (keV)";

    //CANVAS 2
    cCalifa2 = new TCanvas(Name1, Name2, 10, 10, 500, 500);
    fh_Califa_cryId_energy = 
      new TH2F(Name1, Name2, fNumCrystalPetal*fCalifaNumPetals, 1., fNumCrystalPetal*fCalifaNumPetals+1., 
	       bins, minE, maxE);    
    fh_Califa_cryId_energy->GetXaxis()->SetTitle("Crystal number");
    fh_Califa_cryId_energy->GetYaxis()->SetTitle(Yaxis1);
    fh_Califa_cryId_energy->GetYaxis()->SetTitleOffset(1.4);
    fh_Califa_cryId_energy->GetXaxis()->CenterTitle(true);
    fh_Califa_cryId_energy->GetYaxis()->CenterTitle(true);
    fh_Califa_cryId_energy->Draw("COLZ");

    fh_Califa_cryId_energy_cal = 
      new TH2F(Name3, Name4, fNumCrystalPetal*fCalifaNumPetals, 1., fNumCrystalPetal*fCalifaNumPetals+1., 
	       bins, minE, maxE);    
    fh_Califa_cryId_energy_cal->GetXaxis()->SetTitle("Crystal number");
    fh_Califa_cryId_energy_cal->GetYaxis()->SetTitle(Yaxis2);
    fh_Califa_cryId_energy_cal->GetYaxis()->SetTitleOffset(1.4);
    fh_Califa_cryId_energy_cal->GetXaxis()->CenterTitle(true);
    fh_Califa_cryId_energy_cal->GetYaxis()->CenterTitle(true);


    //CANVAS 3
    cCalifa3 = new TCanvas("Califa Energy_per_petal", "Califa Energy per petal", 10, 10, 500, 500);
    cCalifa3->Divide(4,2);
    char Name5[255]; char Name6[255]; TString Xaxis1;
    char Name7[255]; char Name8[255]; TString Xaxis2; 
    for (Int_t i=0;i<fCalifaNumPetals;i++){

      sprintf(Name5, "h_Califa_Map_energy_per_petal_%d", i+1);	  
      sprintf(Name6, "Califa Map Energy per petal %d", i+1);
      Xaxis1="Energy (channels)";
      fh_Califa_energy_per_petal[i] = new TH1F(Name5, Name6, bins, minE, maxE);
      fh_Califa_energy_per_petal[i]->GetXaxis()->SetTitle(Xaxis1);
   				
      sprintf(Name7, "h_Califa_Cal_energy_per_petal_%d", i+1);	  
      sprintf(Name8, "Califa Cal Energy per petal %d", i+1);
      Xaxis2="Energy (keV)";
      fh_Califa_energy_per_petal_cal[i] = new TH1F(Name7, Name8, bins, minE, maxE);
      fh_Califa_energy_per_petal_cal[i]->GetXaxis()->SetTitle(Xaxis2);
    
      cCalifa3->cd(i+1);
      fh_Califa_energy_per_petal[i]->Draw();
    }

    //CANVAS 4 		
    char Name9[255]; 
    char Name10[255]; 	char Name11[255]; TString Xaxis3;
    char Name12[255]; 	char Name13[255]; TString Xaxis4;  
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for (Int_t k=0;k<4;k++){
	sprintf(Name9, "Califa_Petal_%d_section_%d", i+1,k+1);
	cCalifa4[i][k] = new TCanvas(Name9, Name9, 10, 10, 500, 500);
	cCalifa4[i][k]->Divide(4,4);
	for(Int_t j=0; j<16;j++){  

	  sprintf(Name10, "h_Califa_Map_Petal_%d_%d_Crystal_%d_energy (Febex)",i+1, k+1, j+1);		  
	  sprintf(Name11, "Califa_Map Petal %d.%d Crystal %d energy (Febex)", i+1,k+1, j+1); 
	  Xaxis3="Energy (channels)";

	  sprintf(Name12, "h_Califa_Cal_Petal_%d_%d_Crystal_%d_energy (Febex)", i+1, k+1, j+1);		  
	  sprintf(Name13, "Califa_Cal Petal %d.%d Crystal %d energy (Febex)", i+1,k+1, j+1);
	  Xaxis4="Energy (keV)";

	  fh_Califa_crystals[i][j+16*k] = new TH1F(Name10, Name11, 
		     arry_bins[j+16*k], arry_minE[j+16*k], arry_maxE[j+16*k]);
	  fh_Califa_crystals[i][j+16*k]->SetTitleSize(0.8,"t");
	  fh_Califa_crystals[i][j+16*k]->GetXaxis()->SetTitle(Xaxis3);
	  fh_Califa_crystals[i][j+16*k]->GetXaxis()->SetLabelSize(0.06);
	  fh_Califa_crystals[i][j+16*k]->GetYaxis()->SetLabelSize(0.07);
	  fh_Califa_crystals[i][j+16*k]->GetXaxis()->SetTitleSize(0.05);
	  fh_Califa_crystals[i][j+16*k]->GetXaxis()->CenterTitle(true);
	  fh_Califa_crystals[i][j+16*k]->GetYaxis()->CenterTitle(true);
	  fh_Califa_crystals[i][j+16*k]->GetXaxis()->SetTitleOffset(0.95);
	  fh_Califa_crystals[i][j+16*k]->SetFillColor(kGreen); 

	  fh_Califa_crystals_cal[i][j+16*k] = new TH1F(Name12, Name13, 
		     arry_bins[j+16*k], arry_minE[j+16*k], arry_maxE[j+16*k]);
	  fh_Califa_crystals_cal[i][j+16*k]->GetXaxis()->SetTitle(Xaxis4);
	  fh_Califa_crystals_cal[i][j+16*k]->GetXaxis()->SetLabelSize(0.06);
	  fh_Califa_crystals_cal[i][j+16*k]->GetYaxis()->SetLabelSize(0.07);
	  fh_Califa_crystals_cal[i][j+16*k]->GetXaxis()->SetTitleSize(0.05);
	  fh_Califa_crystals_cal[i][j+16*k]->GetXaxis()->CenterTitle(true);
	  fh_Califa_crystals_cal[i][j+16*k]->GetYaxis()->CenterTitle(true);
	  fh_Califa_crystals_cal[i][j+16*k]->GetXaxis()->SetTitleOffset(0.95);
	  fh_Califa_crystals_cal[i][j+16*k]->SetFillColor(kGreen); 

	  cCalifa4[i][k]->cd(j+1);
	  gPad->SetLogy();
	  fh_Califa_crystals[i][j+16*k]->Draw();
	}
      }
    }


    //CANVAS 5
    cCalifa5 = new TCanvas("Califa_petal_1and2_vs_petal_6", 
				    "Coinc. 1and2 vs 6", 
				    10, 10, 500, 500);

    fh_Califa_coinc_petal1 = new TH2F("fh_Califa_petal_1and2_vs_petal_6","Califa coinc. petals 1and2 vs 6",
                                 bins, minE, maxE,bins, minE, maxE);
    fh_Califa_coinc_petal1->GetXaxis()->SetTitle("Energy petal 1 and 2 (channels)");
    fh_Califa_coinc_petal1->GetYaxis()->SetTitle("Energy petal 6 (channels)"); 
    fh_Califa_coinc_petal1->GetYaxis()->SetTitleOffset(1.2);
    fh_Califa_coinc_petal1->GetXaxis()->CenterTitle(true);
    fh_Califa_coinc_petal1->GetYaxis()->CenterTitle(true);
    fh_Califa_coinc_petal1->Draw("COLZ");


    //CANVAS 6
    cCalifa6 = new TCanvas("Califa_petal_3_vs_petal_7", 
				    "Coinc. 3 vs 7", 
				    10, 10, 500, 500);

    fh_Califa_coinc_petal2 = new TH2F("fh_Califa_petal_3_vs_petal_7","Califa coinc. petals 3 vs 7",
                                 bins, minE, maxE,bins, minE, maxE);
    fh_Califa_coinc_petal2->GetXaxis()->SetTitle("Energy petal 3 (channels)");
    fh_Califa_coinc_petal2->GetYaxis()->SetTitle("Energy petal 7 (channels)"); 
    fh_Califa_coinc_petal2->GetYaxis()->SetTitleOffset(1.2);
    fh_Califa_coinc_petal2->GetXaxis()->CenterTitle(true);
    fh_Califa_coinc_petal2->GetYaxis()->CenterTitle(true);
    fh_Califa_coinc_petal2->Draw("COLZ");


    //MAIN FOLDER-Califa
    TFolder* mainfolCalifa = new TFolder("CALIFA","CALIFA info");      
    mainfolCalifa->Add(cCalifa1);
    mainfolCalifa->Add(cCalifa2);
    mainfolCalifa->Add(cCalifa3);
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for (Int_t k=0;k<4;k++){
       mainfolCalifa->Add(cCalifa4[i][k]);
      }
    }
    mainfolCalifa->Add(cCalifa5);
    mainfolCalifa->Add(cCalifa6);
    run->AddObject(mainfolCalifa);

    //Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Califa", Form("/Objects/%s/->Reset_CALIFA_Histo()", GetName()));
    //Register command to change the histogram scales (Log/Lineal)
    run->GetHttpServer()->RegisterCommand("Log_Califa", Form("/Objects/%s/->Log_CALIFA_Histo()", GetName()));
    //Register command for moving between Febex and Preamp channels
     run->GetHttpServer()->RegisterCommand("Febex2Preamp_Califa", Form("/Objects/%s/->Febex2Preamp_CALIFA_Histo()", GetName()));
    //Register command for moving between histograms of mapped and cal levels
    if(fCalON){
     run->GetHttpServer()->RegisterCommand("Map2Cal_Califa", Form("/Objects/%s/->Map2Cal_CALIFA_Histo()", GetName()));
    }


  return kSUCCESS;
}

void R3BCalifaOnlineSpectra::Reset_CALIFA_Histo()
{
    LOG(INFO) << "R3BCalifaOnlineSpectra::Reset_CALIFA_Histo" << FairLogger::endl;

   fh_Califa_cryId_petal->Reset();
   fh_Califa_cryId_energy->Reset();
   fh_Califa_coinc_petal1->Reset();
   fh_Califa_coinc_petal2->Reset();

    for(Int_t i=0; i<fCalifaNumPetals; i++){
      fh_Califa_energy_per_petal[i]->Reset();
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  fh_Califa_crystals[i][j+16*k]->Reset();
       }
      }
    }

   if(fCalON){
    fh_Califa_cryId_energy_cal->Reset();
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      fh_Califa_energy_per_petal_cal[i]->Reset();
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  fh_Califa_crystals_cal[i][j+16*k]->Reset();
       }
      }
    }
   }
}

void R3BCalifaOnlineSpectra::Log_CALIFA_Histo()
{
    LOG(INFO) << "R3BCalifaOnlineSpectra::Log_CALIFA_Histo" << FairLogger::endl;

    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
	for(Int_t j=0; j<16;j++){  
	  cCalifa4[i][k]->cd(j+1);
	  if(fLogScale){
            gPad->SetLogy(0);
          }else{
	    gPad->SetLogy(1);
          }
        }
      }
    }

    if(fLogScale) fLogScale=kFALSE; 
    else   fLogScale=kTRUE;
}

void R3BCalifaOnlineSpectra::Map2Cal_CALIFA_Histo()
{
    LOG(INFO) << "R3BCalifaOnlineSpectra::Map2Cal_CALIFA_Histo" << FairLogger::endl;

  if(fFebex2Preamp){//Febex sequence

    if(fRaw2Cal){
     cCalifa2->cd();
     fh_Califa_cryId_energy->Draw("COLZ");

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      cCalifa3->cd(i+1);
      fh_Califa_energy_per_petal[i]->Draw();
     }

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(j+1);
	  gPad->SetLogy();
	  fh_Califa_crystals[i][j+16*k]->SetFillColor(kGreen); 
	  fh_Califa_crystals[i][j+16*k]->Draw();
       }
      }
     }

     fRaw2Cal=kFALSE;
    }else{

     cCalifa2->cd();
     fh_Califa_cryId_energy_cal->Draw("COLZ");

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      cCalifa3->cd(i+1);
      fh_Califa_energy_per_petal_cal[i]->Draw();
     }

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(j+1);
	  gPad->SetLogy();
	  fh_Califa_crystals_cal[i][j+16*k]->SetFillColor(kGreen); 
	  fh_Califa_crystals_cal[i][j+16*k]->Draw();
       }
      }
     }

     fRaw2Cal=kTRUE;
    }

  }else{//Preamp sequence

    if(fRaw2Cal){
     cCalifa2->cd();
     fh_Califa_cryId_energy->Draw("COLZ");

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      cCalifa3->cd(i+1);
      fh_Califa_energy_per_petal[i]->Draw();
     }

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(fOrderFebexPreamp[j]+1);
	  gPad->SetLogy();
	  fh_Califa_crystals[i][j+16*k]->SetFillColor(45); 
	  fh_Califa_crystals[i][j+16*k]->Draw();
       }
      }
     }

     fRaw2Cal=kFALSE;
    }else{

     cCalifa2->cd();
     fh_Califa_cryId_energy_cal->Draw("COLZ");

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      cCalifa3->cd(i+1);
      fh_Califa_energy_per_petal_cal[i]->Draw();
     }

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(fOrderFebexPreamp[j]+1);
	  gPad->SetLogy();
	  fh_Califa_crystals_cal[i][j+16*k]->SetFillColor(45); 
	  fh_Califa_crystals_cal[i][j+16*k]->Draw();
       }
      }
     }

     fRaw2Cal=kTRUE;
    }

  }

}

void R3BCalifaOnlineSpectra::Febex2Preamp_CALIFA_Histo()
{
  LOG(INFO) << "R3BCalifaOnlineSpectra::Febex2Preamp_CALIFA_Histo" << FairLogger::endl;

  char Name[255];

  //Raw data
  if(!fRaw2Cal){
   if(fFebex2Preamp){//Febex to Preamp sequence

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(fOrderFebexPreamp[j]+1);
	  gPad->SetLogy();
	  fh_Califa_crystals[i][j+16*k]->SetFillColor(45); 
	  sprintf(Name, "Califa_Map Petal %d.%d Crystal %d energy (Preamp)", i+1,k+1, j+1);
          fh_Califa_crystals[i][j+16*k]->SetTitle(Name);
	  fh_Califa_crystals[i][j+16*k]->Draw();
       }
      }
     }
    fFebex2Preamp=kFALSE;
   }else{

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(j+1);
	  gPad->SetLogy();
	  fh_Califa_crystals[i][j+16*k]->SetFillColor(kGreen); 
	  sprintf(Name, "Califa_Map Petal %d.%d Crystal %d energy (Febex)", i+1,k+1, j+1); 
          fh_Califa_crystals[i][j+16*k]->SetTitle(Name);
	  fh_Califa_crystals[i][j+16*k]->Draw();
       }
      }
     }
    fFebex2Preamp=kTRUE;
   }

  }else{//Cal data

   if(fFebex2Preamp){//Febex to Preamp sequence

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(fOrderFebexPreamp[j]+1);
	  gPad->SetLogy();
	  fh_Califa_crystals_cal[i][j+16*k]->SetFillColor(45); 
	  fh_Califa_crystals_cal[i][j+16*k]->Draw();
       }
      }
     }
    fFebex2Preamp=kFALSE;
   }else{

     for(Int_t i=0; i<fCalifaNumPetals; i++){
      for(Int_t k=0;k<4;k++){
       for(Int_t j=0; j<16;j++){
	  cCalifa4[i][k]->cd(j+1);
	  gPad->SetLogy();
	  fh_Califa_crystals_cal[i][j+16*k]->SetFillColor(kGreen); 
	  fh_Califa_crystals_cal[i][j+16*k]->Draw();
       }
      }
     }
    fFebex2Preamp=kTRUE;
   }

  }

}

void R3BCalifaOnlineSpectra::Exec(Option_t* option) {
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BCalifaOnlineSpectra::Exec FairRootManager not found" << FairLogger::endl;

  
  if(fMappedItemsCalifa && fMappedItemsCalifa->GetEntriesFast()){
    Int_t nHits = fMappedItemsCalifa->GetEntriesFast();

    Double_t E0=0., E1=0., E2=0., E5=0., E6=0.;
    
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BCalifaMappedData* hit = 
	(R3BCalifaMappedData*)fMappedItemsCalifa->At(ihit);
      if (!hit) continue;
         
      Int_t cryId = hit->GetCrystalId()-1;				
      Int_t petal = (Int_t)(cryId)/fNumCrystalPetal;//from 0 to 7
      Int_t cryId_petal = cryId-fNumCrystalPetal*(petal);//from 0 to 63
      
      fh_Califa_cryId_energy->Fill(hit->GetCrystalId(),hit->GetEnergy());
      //individual energy histo for each crystalId
      fh_Califa_crystals [petal][cryId_petal]->Fill(hit->GetEnergy());
      //energy(channels) sum for each petal 
      fh_Califa_energy_per_petal[petal]->Fill(hit->GetEnergy());
      if(petal==0)E0=E0+hit->GetEnergy();
      if(petal==1)E1=E1+hit->GetEnergy();
      if(petal==2)E2=E2+hit->GetEnergy();
      if(petal==5)E5=E5+hit->GetEnergy();
      if(petal==6)E6=E6+hit->GetEnergy();
      //crystalId vs petal number 
      fh_Califa_cryId_petal->Fill(cryId_petal+1, petal+1);
    }

   if(E0+E1>0 && E5>0)fh_Califa_coinc_petal1->Fill(E0+E1, E5);
   if(E2>0 && E6>0)fh_Califa_coinc_petal2->Fill(E2, E6);

  }
  
  if(fCalON==kTRUE && fCalItemsCalifa && fCalItemsCalifa->GetEntriesFast()){
    Int_t nHits = fCalItemsCalifa->GetEntriesFast();
    
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BCalifaCrystalCalData* hit = 
	(R3BCalifaCrystalCalData*)fCalItemsCalifa->At(ihit);
      if (!hit) continue;
    
      Int_t cryId = hit->GetCrystalId()-1;			
      Int_t petal = (Int_t)(cryId)/fNumCrystalPetal;//from 0 to 7
      Int_t cryId_petal = cryId-fNumCrystalPetal*(petal);//from 0 to 63
      
      fh_Califa_cryId_energy_cal->Fill(hit->GetCrystalId(),hit->GetEnergy());
      //individual energy histo for each crystalId
      fh_Califa_crystals_cal[petal][cryId_petal]->Fill(hit->GetEnergy());
      //energy(channels) sum for each petal 
      fh_Califa_energy_per_petal_cal[petal]->Fill(hit->GetEnergy());
    }
  }
  fNEvents += 1;
}


void R3BCalifaOnlineSpectra::FinishEvent() {
    if (fMappedItemsCalifa)
    {
        fMappedItemsCalifa->Clear();
    }
    if (fCalItemsCalifa)
    {
        fCalItemsCalifa->Clear();
    }
}


void R3BCalifaOnlineSpectra::FinishTask() {
  
   if(fMappedItemsCalifa){
      cCalifa1->Write();
      cCalifa2->Write();
      cCalifa3->Write();
      for(Int_t i=0; i<fCalifaNumPetals; i++){
        for(Int_t k=0;k<4;k++){
          cCalifa4[i][k]->Write();
        }
      }
      cCalifa5->Write();
      cCalifa6->Write();
   }


  // if (fMappedItemsCalifa || fCalItemsCalifa){
    // CALIFA
    //fh_Califa_cryId_petal->Draw("COLZ");
    //fh_Califa_cryId_energy->Draw("COLZ");
    //fh_Califa_energy_oneCry->Draw();
    
    //------------
    //TFile *MyFile = new TFile("hist.root","RECREATE"); 
    //if ( MyFile->IsOpen() ) cout << "File opened successfully"<<endl;
    //fh_Califa_cryId_petal->Write();
   fh_Califa_cryId_energy->Write();

   //mapped data
   if (fMappedItemsCalifa){    
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for (Int_t k=0;k<4;k++){
    	for(Int_t j=0; j<16;j++){
    	  fh_Califa_crystals[i][j+16*k]->Write();
    	}
      }
    }
   }
   //cal data
   if (fCalItemsCalifa){
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for (Int_t k=0;k<4;k++){
    	for(Int_t j=0; j<16;j++){
    	  fh_Califa_crystals_cal[i][j+16*k]->Write();
    	}
      }
    }
   }

 
    //for (Int_t i=0;i<fCalifaNumPetals;i++){
    //  fh_Califa_energy_per_petal[i]->Write();
    //}
    //fh_Califa_energy_oneCry->Write();
    //MyFile->Print();
    //MyFile->Close();
  //} 
}

ClassImp(R3BCalifaOnlineSpectra)
