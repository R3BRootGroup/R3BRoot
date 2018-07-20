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
  , fCalON(0) {
}

R3BCalifaOnlineSpectra::R3BCalifaOnlineSpectra(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItemsCalifa(NULL)
  , fCalItemsCalifa(NULL)
  , fTrigger(-1)
  , fNEvents(0)
  , fCalifaNumPetals(1)
  , fCalON(0) {
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
  
  
  header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
  FairRunOnline *run = FairRunOnline::Instance();
  
  //get access to Mapped data
  fMappedItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaMappedData");
  
  //get access to Cal data
  fCalItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaCrystalCalData");
  
  
  if(fMappedItemsCalifa || fCalItemsCalifa){     
    //reading the file
    Bool_t noFile = kFALSE;    
    ifstream *FileHistos = new ifstream(fCalifaFile);
    if(!FileHistos->is_open()){
      LOG(WARNING) << "R3BCalifaOnlineSpectra:  No Histogram definition file" 
		   << FairLogger::endl;
      noFile=kTRUE;
    }
    
    Double_t arry_bins[fCalifaNumPetals*64+1];
    Double_t arry_maxE[fCalifaNumPetals*64+1];
    Double_t arry_minE[fCalifaNumPetals*64+1];
    Double_t bins;
    Double_t maxE;
    Double_t minE;
    
    if(!noFile) {
      for (Int_t i=0; i<fCalifaNumPetals*64+1; i++){
	*FileHistos>>arry_bins[i]>>arry_minE[i]>>arry_maxE[i];	
      }
    }
    else{
      for (Int_t i=0; i<fCalifaNumPetals*64+1; i++){
	arry_bins[i] = 3000;
	arry_minE[i] = 0; 
	arry_maxE[i] = 3000;	
      }
    }
    
    bins= arry_bins[fCalifaNumPetals*64]; //the last line is a general setting
    maxE= arry_maxE[fCalifaNumPetals*64];
    minE= arry_minE[fCalifaNumPetals*64];
    
    fh_Califa_cryId_petal = new TH2F("fh_Califa_cryId_petal", 
				     "Califa petal number vs cryId", 
				     64, 0, 63, 8, 0, 7);    
    fh_Califa_cryId_petal->GetXaxis()->SetTitle("CrystalId");
    fh_Califa_cryId_petal->GetYaxis()->SetTitle("Petal number");      
    run->AddObject(fh_Califa_cryId_petal);
    
    TString Name1; TString Name2; TString Name3; TString Yaxis1; 
    if(fCalON==true){
      Name1="fh_Califa_Cal_cryId_energy";
      Name2="Califa_Cal energy vs cryId";
      Name3="Califa_Cal energy vs cryId";
      Yaxis1="Energy (keV)";
    }
    else{	
      Name1="fh_Califa_Map_cryId_energy";
      Name2="Califa_Map energy vs cryId";
      Name3="Califa_Map energy vs cryId";
      Yaxis1="Energy (channels)";
    }
    fh_Califa_cryId_energy = 
      new TH2F(Name1, Name2, 64*fCalifaNumPetals, 0., 64*fCalifaNumPetals-1, 
	       bins, minE, maxE);    
    fh_Califa_cryId_energy->GetXaxis()->SetTitle("CrystalId");
    fh_Califa_cryId_energy->GetYaxis()->SetTitle(Yaxis1);
    run->AddObject(fh_Califa_cryId_energy);
    
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for (Int_t k=0;k<4;k++){
	for(Int_t j=0; j<16;j++){  
	  char Name5[255]; char Name6[255]; TString Xaxis1;
	  if (fCalON==true){	    				
	    sprintf(Name5, "h_Califa_Cal_Petal_%d_%d_Crystal_%d_energy", 
		    i+1, k+1, j+1);		  
	    sprintf(Name6, "Califa_Cal Petal %d.%d Crystal %d energy", 
		    i+1,k+1, j+1); //cryId 1-16
	    Xaxis1="Energy (keV)";
	  }
	  else{
	    sprintf(Name5, "h_Califa_Map_Petal_%d_%d_Crystal_%d_energy", 
		    i+1, k+1, j+1);		  
	    sprintf(Name6, "Califa_Map Petal %d.%d Crystal %d energy", 
		    i+1,k+1, j+1); //cryId 1-16
	    Xaxis1="Energy (channels)";
	  }
	  fh_Califa_crystals[i][j+16*k] = 
	    new TH1F(Name5, Name6, 
		     arry_bins[j+16*k], arry_minE[j+16*k], arry_maxE[j+16*k]);
	  fh_Califa_crystals[i][j+16*k]->GetXaxis()->SetTitle(Xaxis1);
	  run->AddObject(fh_Califa_crystals[i][j+16*k]);	  
	}	
      }
    }
    
    for (Int_t i=0;i<fCalifaNumPetals;i++){
      char Name7[255]; char Name8[255]; TString Xaxis2;
      if (fCalON==true){	    				
	sprintf(Name7, "h_Califa_Cal_energy_per_petal_%d", i+1);	  
	sprintf(Name8, "Califa_Cal Energy per petal %d", i+1);
	Xaxis2="Energy (keV)";
      }
      else{
	sprintf(Name7, "h_Califa_Map_energy_per_petal_%d", i+1);	  
	sprintf(Name8, "Califa_Map Energy per petal %d", i+1);
	Xaxis2="Energy (channels)";
      }
      fh_Califa_energy_per_petal[i] = new TH1F(Name7, Name8, bins, minE, maxE);
      fh_Califa_energy_per_petal[i]->GetXaxis()->SetTitle(Xaxis2);
      run->AddObject(fh_Califa_energy_per_petal[i]);
    }
    /*
    if(fCalifaOneCrystal>0){
      TString Name10; TString Name11; TString Xaxis3;
      if (fCalON==true){	    				
	Name10="fh_Califa_Cal_energy_oneCry";		  
	Name11="Califa_Cal energy for OneCrystal";
	Xaxis3="Energy (keV)";
      }
      else{
	Name10="fh_Califa_Map_energy_oneCry";		  
	Name11="Califa_Map energy for OneCrystal";
	Xaxis3="Energy (channels)";
      }
      fh_Califa_energy_oneCry = 
	new TH1F(Name10, Name11, 
		 arry_bins[fCalifaOneCrystal-1], 
		 arry_minE[fCalifaOneCrystal-1], 
		 arry_maxE[fCalifaOneCrystal-1]);      
      fh_Califa_energy_oneCry->GetXaxis()->SetTitle(Xaxis3);
      run->AddObject(fh_Califa_energy_oneCry);      
    }
    */

    
    //CANVAS 1
    TCanvas *cCalifa1 = new TCanvas("Califa_petal_vs_cryId", 
				    "Califa_petal_vs_cryId", 
				    10, 10, 500, 500);
    //cCalifa1->Divide(2);
    //cCalifa1->cd(1);
    fh_Califa_cryId_petal->Draw("COLZ");
    //cCalifa1->cd(0);
    run->AddObject(cCalifa1);
    
    /*
    //CANVAS 2
    TCanvas *cCalifa2 = new TCanvas(Name3, Name3, 10, 10, 500, 500);
    fh_Califa_cryId_energy->Draw("COLZ");
    run->AddObject(cCalifa2);
    */
    
    //CANVAS 3 
    TCanvas* cCalifa3[fCalifaNumPetals][4];		
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for (Int_t k=0;k<4;k++){
	char Name4[255];
	sprintf(Name4, "Califa_Petal_%d_section_%d", i+1,k+1);
	cCalifa3[i][k] = new TCanvas(Name4, Name4, 10, 10, 500, 500);
	cCalifa3[i][k]->Divide(4,4);
	for(Int_t j=0; j<16;j++){  
	  cCalifa3[i][k]->cd(j+1);
	  gPad->SetLogy();
	  fh_Califa_crystals[i][j+16*k]->Draw();
	}
	cCalifa3[i][k]->cd(0);
	run->AddObject(cCalifa3[i][k]);
      }
    }
    /*
    TCanvas* cCalifa3;	
    char Name4[255];
    sprintf(Name4, "Califa_Petal_2_seccion_2");
    cCalifa3 = new TCanvas(Name4, Name4, 10, 10, 950, 650);
    cCalifa3->Divide(4,4);
    for(Int_t j=0; j<16;j++){  
      cCalifa3->cd(j+1);
      fh_Califa_crystals[1][j+16*1]->Draw();
    }
    //cCalifa3->cd(0);
    run->AddObject(cCalifa3);
*/
    //FOLDER 3 
    TFolder* folCalifa3[fCalifaNumPetals][4];		
    for(Int_t i=0; i<fCalifaNumPetals; i++){
      for (Int_t k=0;k<4;k++){
	char Name44[255];
	sprintf(Name44, "Califa Petal %d.%d", i+1,k+1);
	folCalifa3[i][k] = new TFolder(Name44, Name44);
	for(Int_t j=0; j<16;j++){  
	  folCalifa3[i][k]->Add(fh_Califa_crystals[i][j+16*k]);
	}
	run->AddObject(folCalifa3[i][k]);
      }
    }
    
    //  CANVAS 4  -------------------------------     
    TCanvas* cCalifa4[fCalifaNumPetals];
    for (Int_t i=0;i<fCalifaNumPetals;i++){
      char Name9[255];
      if (fCalON==true)	sprintf(Name9, "Califa_Cal_Energy_petal_%d", i+1);
      else sprintf(Name9, "Califa_Map_Energy_petal_%d", i+1);
      cCalifa4[i] = new TCanvas(Name9, Name9, 10, 10, 500, 500);
      fh_Califa_energy_per_petal[i]->Draw();
      run->AddObject(cCalifa4[i]);
    }
    
    //FOLDER4
    TFolder* folCalifa4;
    char Name99[255];
    if (fCalON==true)	sprintf(Name99, "Califa_Cal petals info");
    else sprintf(Name99, "Califa_Map Energy petals info");  
    folCalifa4 = new TFolder(Name99, Name99);      
    folCalifa4->Add(fh_Califa_cryId_petal);
    folCalifa4->Add(fh_Califa_cryId_energy);
    for (Int_t i=0;i<fCalifaNumPetals;i++){
      folCalifa4->Add(fh_Califa_energy_per_petal[i]);
    }
    run->AddObject(folCalifa4);

    /*
    // CANVAS 5
    if(fCalifaOneCrystal>0){
      TString Name12;
      if (fCalON==true) Name12="Califa_Cal OneCrystal Energy";
      else Name12="Califa_Map OneCrystal Energy";
      TCanvas *cCalifa5 = new TCanvas(Name12, Name12, 10, 10, 500, 500);
      fh_Califa_energy_oneCry->Draw();
      run->AddObject(cCalifa5);
    }
    */
  }//--------------------		
  
  return kSUCCESS;
}


void R3BCalifaOnlineSpectra::Exec(Option_t* option) {
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BCalifaOnlineSpectra::Exec FairRootManager not found" << FairLogger::endl;

  
  // check for requested trigger (is this right?)
  //if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
  //  return;
  
  if(fCalON==false && fMappedItemsCalifa && fMappedItemsCalifa->GetEntriesFast()){
    Int_t nHits = fMappedItemsCalifa->GetEntriesFast();
    
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BCalifaMappedData* hit = 
	(R3BCalifaMappedData*)fMappedItemsCalifa->At(ihit);
      if (!hit) continue;
      
      //  if (hit->GetCrystalId()==fCalifaOneCrystal){
      //	fh_Califa_energy_oneCry->Fill(hit->GetEnergy());
      //}
      
      Int_t cryId = hit->GetCrystalId()-1;				
      Int_t petal = (Int_t)(cryId)/64;//from 0 to 7
      Int_t cryId_petal = cryId-64*(petal);//from 0 to 63
      
      fh_Califa_cryId_energy->Fill(hit->GetCrystalId(),hit->GetEnergy());
      //individual energy histo for each crystalId
      fh_Califa_crystals [petal][cryId_petal]->Fill(hit->GetEnergy());
      //energy(channels) sum for each petal 
      fh_Califa_energy_per_petal[petal]->Fill(hit->GetEnergy());
      //crystalId vs petal number 
      fh_Califa_cryId_petal->Fill(cryId_petal, petal);
    }
  }
  
  if(fCalON==true && fCalItemsCalifa && fCalItemsCalifa->GetEntriesFast()){
    Int_t nHits = fCalItemsCalifa->GetEntriesFast();
    
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BCalifaCrystalCalData* hit = 
	(R3BCalifaCrystalCalData*)fCalItemsCalifa->At(ihit);
      if (!hit) continue;
      
      //if (hit->GetCrystalId()==fCalifaOneCrystal){
      //	fh_Califa_energy_oneCry->Fill(hit->GetEnergy());
      //}
      
      Int_t cryId = hit->GetCrystalId()-1;			
      Int_t petal = (Int_t)(cryId)/64;//from 0 to 7
      Int_t cryId_petal = cryId-64*(petal);//from 0 to 63
      
      fh_Califa_cryId_energy->Fill(hit->GetCrystalId(),hit->GetEnergy());
      //individual energy histo for each crystalId
      fh_Califa_crystals [petal][cryId_petal]->Fill(hit->GetEnergy());
      //energy(channels) sum for each petal 
      fh_Califa_energy_per_petal[petal]->Fill(hit->GetEnergy());
      //crystalId vs petal number 
      fh_Califa_cryId_petal->Fill(cryId_petal, petal);
    }
  }
  fNEvents += 1;
}


void R3BCalifaOnlineSpectra::FinishEvent() {
  // CALIFA
  // fh_Califa_cryId_petal->Draw("COLZ");
  //fh_Califa_cryId_energy->Draw("COLZ");
  //fh_Califa_energy_oneCry->Draw();  
  
  //if (fCalItemsCalifa) {
  //  fCalItemsCalifa->Clear();
  //}
  //if (fMappedItemsCalifa) {
  //  fMappedItemsCalifa->Clear();
  //}
}


void R3BCalifaOnlineSpectra::FinishTask() {
  
  // if (fMappedItemsCalifa || fCalItemsCalifa){
    // CALIFA
    //fh_Califa_cryId_petal->Draw("COLZ");
    //fh_Califa_cryId_energy->Draw("COLZ");
    //fh_Califa_energy_oneCry->Draw();
    
    //------------
    //TFile *MyFile = new TFile("hist.root","RECREATE"); 
    //if ( MyFile->IsOpen() ) cout << "File opened successfully"<<endl;
    //fh_Califa_cryId_petal->Write();
    //fh_Califa_cryId_energy->Write();
    
    //for(Int_t i=0; i<fCalifaNumPetals; i++){
    //  for (Int_t k=0;k<4;k++){
    //	for(Int_t j=0; j<16;j++){
    //	  fh_Califa_crystals[i][j+16*k]->Write();
    //	}
    //}
    //}
    
    //for (Int_t i=0;i<fCalifaNumPetals;i++){
    //  fh_Califa_energy_per_petal[i]->Write();
    //}
    //fh_Califa_energy_oneCry->Write();
    //MyFile->Print();
    //MyFile->Close();
  //} 
}

ClassImp(R3BCalifaOnlineSpectra)
