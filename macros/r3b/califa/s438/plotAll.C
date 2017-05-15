//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA S438 Data Display
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 30/09/14
//         Comments:
//         MAIN DISPLAY OF S438 CALIFA CRYSTALHIT
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      ROOT> .L plotAll.C
//      ROOT> plot("inputFile")
//     
//     where inputFile is the input file :) 
//  -------------------------------------------------------------------------

#include <iostream>
#include <fstream>
using namespace std;

void plot(TString inputFile="") {

  // CONFIGURATION
  ifstream input1;
  input1.open("./angles/petals_angels_angles.txt"); 
  Double_t polar[128]; Double_t azimuthal[128]; Int_t number=0;
  for(Int_t i=0;i<128;i++) input1 >> number >> polar[i] >> azimuthal[i]; 
  //

  gROOT->SetStyle("Default");
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TFile *file1 = TFile::Open(inputFile);

  //SETTINGS: reduce memory by disabling those canvas that you do not want!
  Bool_t plotMultiplicities=kTRUE; Bool_t plotEnergyvsAngle=kTRUE; Bool_t plotProtonCorr=kTRUE; Bool_t plotAddBack=kTRUE;

  //HISTOGRAMS DEFINITION FOR CRYSTALHITS
  TH1F* hMult=NULL; TH1F* hMult2=NULL; TH2F* hEnergyvsId=NULL; TH2F* hToT_EnergyvsId=NULL;
  if(plotMultiplicities) {
    hMult = new TH1F("hMult","Total CrystalHits vs crystalId",128,0,127);
    hMult2 = new TH1F("hMult2","Number of CrystalHit with max. E of the event vs crystalId",128,0,127);
    hEnergyvsId = new TH2F("hEnergyvsId","Energy vs crystalId",128,0,127,3000,-100,29900);
    hToT_EnergyvsId = new TH2F("hToT_EnergyvsId","ToT_Energy vs crystalId",128,0,127,3000,-1000,299000);
  }  
  TH2F* hP1EnergyvsPolar; TH2F* hP2EnergyvsPolar; TH2F* hP1ToT_EnergyvsPolar; TH2F* hP2ToT_EnergyvsPolar;
  if(plotEnergyvsAngle) {
    hP1EnergyvsPolar = new TH2F("hP1EnergyvsPolar","PETAL1: Energy vs polar",50,22,68,3000,-100,29900);
    hP2EnergyvsPolar = new TH2F("hP2EnergyvsPolar","PETAL2: Energy vs polar",50,22,68,3000,-100,29900);
    hP1ToT_EnergyvsPolar = new TH2F("hP1ToT_EnergyvsPolar","PETAL1: ToT_Energy vs polar",50,22,68,3000,-1000,299000);
    hP2ToT_EnergyvsPolar = new TH2F("hP2ToT_EnergyvsPolar","PETAL2: ToT_Energy vs polar",50,22,68,3000,-1000,299000);
  }
  TH2F* hToT_EnergyCorr=NULL; TH2F* hAngleCorr=NULL; TH2F* hToT_EnergyCorrLarge=NULL; TH2F* hAngleCorrLarge=NULL;
  if(plotProtonCorr) {
    hToT_EnergyCorr = new TH2F("hToT_EnergyCorr","ToT_Energy Petal1 vs ToT_Energy Petal2 (FULL COMBINATORIAL)",3000,-100,29900,3000,-100,29900);
    hEnergyCorr = new TH2F("hEnergyCorr","Energy Petal1 vs Energy Petal2 (FULL COMBINATORIAL)",3000,-100,29900,3000,-100,29900);
    hAngleCorr = new TH2F("hAngleCorr","Polar Petal1 vs polar Petal2 (FULL COMBINATORIAL)",50,22,68,50,22,68);
    hToT_EnergyCorrLarge = new TH2F("hToT_EnergyCorrLarge","ToT_Energy Petal1 vs ToT_Energy Petal2 (ONLY LARGEST ENERGY HIT)",3000,-100,29900,3000,-100,29900);
    hEnergyCorrLarge = new TH2F("hEnergyCorrLarge","Energy Petal1 vs Energy Petal2 (ONLY LARGEST ENERGY HIT)",3000,-100,29900,3000,-100,29900);
    hAngleCorrLarge = new TH2F("hAngleCorrLarge","Polar Petal1 vs polar Petal2 (ONLY LARGEST ENERGY HIT)",50,22,68,50,22,68);
  }  
  TH2F* hP1ABEnergy=NULL; TH2F* hP2ABEnergy=NULL; TH2F* hP1ABToT_Energy=NULL; TH2F* hP2ABToT_Energy=NULL;
  if(plotAddBack) {
    hP1ABEnergy= new TH2F("hP1ABEnergy","AddBack Energy in Petal1: second largest Energy vs largest Energy",3000,-100,29900,3000,-100,29900);
    hP2ABEnergy = new TH2F("hP2ABEnergy","AddBack Energy in Petal2: second largest Energy vs largest Energy",3000,-100,29900,3000,-100,29900);
    hP1ABToT_Energy = new TH2F("hP1ABToT_Energy","AddBack ToT_Energy in Petal1: second largest ToT_Energy vs largest ToT_Energy",3000,-1000,299000,3000,-1000,299000);
    hP2ABToT_Energy = new TH2F("hP2ABToT_Energy","AddBack ToT_Energy in Petal2: second largest ToT_Energy vs largest ToT_Energy",3000,-1000,299000,3000,-1000,299000);
  }

  TTree* caloTree = (TTree*)file1->Get("cbmsim");
 
  //Crystal Hits
  TClonesArray* crystalHitCA;  
  R3BCalifaCrystalCalData** crystalHit;
  crystalHitCA = new TClonesArray("R3BCalifaCrystalCalData",5);
  TBranch *branchCrystalHit = caloTree->GetBranch("CalifaCrystalCalData");
  if(branchCrystalHit) branchCrystalHit->SetAddress(&crystalHitCA);
  
  Int_t* crystalId=NULL; 
  Double_t* energies=NULL;
  Double_t* ToT_energies=NULL;
  Double_t maxEnergyP1=0; Double_t maxEnergyP2=0; Double_t maxToT_EnergyP1=0; Double_t maxToT_EnergyP2=0; 
  Int_t winnerEnergyP1=0; Int_t winnerEnergyP2=0; Int_t winnerToT_EnergyP1=0; Int_t winnerToT_EnergyP2=0;
  Int_t secondEnergyP1=0; Int_t secondEnergyP2=0; Int_t secondToT_EnergyP1=0; Int_t secondToT_EnergyP2=0;

  Long64_t nevents = caloTree->GetEntries();
  Int_t crystalHitsPerEvent =0;

  for(Int_t i=0;i<nevents;i++){
    if(i%100000 == 0) printf("Event:%i\n",i);
    crystalHitCA->Clear();
    caloTree->GetEvent(i);
    crystalHitsPerEvent = crystalHitCA->GetEntries(); 
    maxEnergyP1=-1; maxEnergyP2=-1; maxToT_EnergyP1=-1; maxToT_EnergyP2=-1; 
    winnerEnergyP1=-1; winnerEnergyP2=-1; winnerToT_EnergyP1=-1; winnerToT_EnergyP2=-1;
    secondEnergyP1=-1; secondEnergyP2=-1; secondToT_EnergyP1=-1; secondToT_EnergyP2=-1;

    if(crystalHitsPerEvent>0) {
      crystalHit = new R3BCalifaCrystalCalData*[crystalHitsPerEvent];
      crystalId = new Int_t[crystalHitsPerEvent]; 
      energies = new Double_t[crystalHitsPerEvent];
      ToT_energies = new Double_t[crystalHitsPerEvent];

      for(Int_t h=0;h<crystalHitsPerEvent;h++) {
	crystalHit[h] = (R3BCalifaCrystalCalData*) crystalHitCA->At(h);      
	//filling info
	if(plotMultiplicities) {
	  hMult->Fill(crystalHit[h]->GetCrystalId());
	  hMult2->Fill(crystalHit[h]->GetCrystalId()); //TO BE DONE BELOW! REMEMBER!!!
	  hEnergyvsId->Fill(crystalHit[h]->GetCrystalId(),crystalHit[h]->GetEnergy());
	  hToT_EnergyvsId->Fill(crystalHit[h]->GetCrystalId(),crystalHit[h]->GetToT_Energy());
	}
	if(plotEnergyvsAngle) {
	  if(crystalHit[h]->GetCrystalId()<64){//PETAL1
	    hP1EnergyvsPolar->Fill(polar[crystalHit[h]->GetCrystalId()],crystalHit[h]->GetEnergy());  //TO BE DONE BELOW! REMEMBER!!!
	    //hP1EnergyvsPolar->Fill(crystalHit[h]->GetCrystalId(),crystalHit[h]->GetEnergy());
	    hP1ToT_EnergyvsPolar->Fill(polar[crystalHit[h]->GetCrystalId()],crystalHit[h]->GetEnergy()); //TO BE DONE BELOW! REMEMBER!!!
	    //hP1ToT_EnergyvsPolar->Fill(crystalHit[h]->GetCrystalId(),crystalHit[h]->GetEnergy());
	  }
	  else{//PETAL2
	    hP2EnergyvsPolar->Fill(polar[crystalHit[h]->GetCrystalId()],crystalHit[h]->GetEnergy()); //TO BE DONE BELOW! REMEMBER!!!
	    //hP2EnergyvsPolar->Fill(crystalHit[h]->GetCrystalId(),crystalHit[h]->GetEnergy());
	    hP2ToT_EnergyvsPolar->Fill(polar[crystalHit[h]->GetCrystalId()],crystalHit[h]->GetEnergy()); //TO BE DONE BELOW! REMEMBER!!!
	    //hP2ToT_EnergyvsPolar->Fill(crystalHit[h]->GetCrystalId(),crystalHit[h]->GetEnergy());
	  }
	}
	
	//FILLING FOR ORDERING
	crystalId[h] = crystalHit[h]->GetCrystalId(); 
	energies[h] = crystalHit[h]->GetEnergy(); 
	ToT_energies[h] = crystalHit[h]->GetToT_Energy();
      }
      //TAKING THE LARGEST ENERGY IN EACH PETAL
      for(Int_t h=0;h<crystalHitsPerEvent;h++) {      
	if(crystalHit[h]->GetCrystalId()<64) {//PETAL1
	  if(maxEnergyP1<energies[h]){ secondEnergyP1=winnerEnergyP1; winnerEnergyP1=h; maxEnergyP1=energies[h];}  
	  if(maxToT_EnergyP1<ToT_energies[h]){ secondToT_EnergyP1=winnerToT_EnergyP1; winnerToT_EnergyP1=h; maxToT_EnergyP1=ToT_energies[h];}  
	}  
	else {//PETAL2
	  if(maxEnergyP2<energies[h]) { secondEnergyP2=winnerEnergyP2; winnerEnergyP2=h; maxEnergyP2=energies[h];} 
	  if(maxToT_EnergyP2<ToT_energies[h]){ secondToT_EnergyP2=winnerToT_EnergyP2; winnerToT_EnergyP2=h; maxToT_EnergyP2=ToT_energies[h];}   
	} 
      } 
      
      if(plotProtonCorr) {
	for(Int_t h=0;h<crystalHitsPerEvent;h++) {      
	  if(crystalHit[h]->GetCrystalId()<64) { //PETAL1
	    for(Int_t k=0;k<crystalHitsPerEvent;k++) {     
	      if(crystalHit[k]->GetCrystalId()>63) { //PETAL2
		hToT_EnergyCorr->Fill(ToT_energies[k],ToT_energies[h]); //FULL COMBINATORIAL!!
		hEnergyCorr->Fill(energies[k],energies[h]); //FULL COMBINATORIAL!!
		hAngleCorr->Fill(polar[crystalId[k]],polar[crystalId[h]]); 
	      }
	    }
	  }
	}
	if(winnerToT_EnergyP2>-1 && winnerToT_EnergyP1>-1) hToT_EnergyCorrLarge->Fill(ToT_energies[winnerToT_EnergyP2],ToT_energies[winnerToT_EnergyP1]);
	if(winnerEnergyP2>-1 && winnerEnergyP1>-1) hEnergyCorrLarge->Fill(energies[winnerEnergyP2],energies[winnerEnergyP1]);
	if(winnerToT_EnergyP2>-1 && winnerToT_EnergyP1>-1) hAngleCorrLarge->Fill(polar[crystalId[winnerToT_EnergyP2]],polar[crystalId[winnerToT_EnergyP1]]);
      }
      
      if(plotAddBack) {
	if(winnerEnergyP1>-1 && secondEnergyP1>-1) hP1ABEnergy->Fill(energies[winnerEnergyP1],energies[secondEnergyP1]);
	if(winnerToT_EnergyP1>-1 && secondToT_EnergyP1>-1) hP1ABToT_Energy->Fill(ToT_energies[winnerToT_EnergyP1],ToT_energies[secondToT_EnergyP1]);
	if(winnerEnergyP2>-1 && secondEnergyP2>-1) hP2ABEnergy->Fill(energies[winnerEnergyP2],energies[secondEnergyP2]);
	if(winnerToT_EnergyP2>-1 && secondToT_EnergyP2>-1) hP2ABToT_Energy->Fill(ToT_energies[winnerToT_EnergyP2],ToT_energies[secondToT_EnergyP2]);
      }
      
    }  
    if(crystalHitsPerEvent) delete [] crystalHit;
  }

  TCanvas* cMult=NULL;  TCanvas* cEnerg=NULL; TCanvas* cEneCorr=NULL; TCanvas* cAB=NULL;
  if(plotMultiplicities) {
    cMult= new TCanvas("cMult","Info on multiplicities and energies per crystalID",0,0,800,800);
    cMult->SetFillColor(0);    cMult->SetFrameFillColor(0);    cMult->Divide(2,2);
    cMult->cd(1);  hMult->Draw();
    cMult->cd(2);  hMult2->Draw();
    cMult->cd(3);  hEnergyvsId->Draw("COLZ");
    cMult->cd(4);  hToT_EnergyvsId->Draw("COLZ");
  }
  if(plotEnergyvsAngle) {
    cEnerg= new TCanvas("cEnerg","Info on energies per polar angle",0,0,800,800);
    cEnerg->SetFillColor(0);    cEnerg->SetFrameFillColor(0);    cEnerg->Divide(2,2);
    cEnerg->cd(1);  hP1EnergyvsPolar->Draw("COLZ");
    cEnerg->cd(2);  hP2EnergyvsPolar->Draw("COLZ");
    cEnerg->cd(3);  hP1ToT_EnergyvsPolar->Draw("COLZ");
    cEnerg->cd(4);  hP2ToT_EnergyvsPolar->Draw("COLZ");
  } 
  if(plotProtonCorr) {
    cEneCorr= new TCanvas("cEneCorr","Info on energy correlation between petals",0,0,1200,800);
    cEneCorr->SetFillColor(0);    cEneCorr->SetFrameFillColor(0);    cEneCorr->Divide(3,2);
    cEneCorr->cd(1);  hToT_EnergyCorr->Draw("COLZ");
    cEneCorr->cd(2);  hEnergyCorr->Draw("COLZ");
    cEneCorr->cd(3);  hAngleCorr->Draw("COLZ");
    cEneCorr->cd(4);  hToT_EnergyCorrLarge->Draw("COLZ");
    cEneCorr->cd(5);  hEnergyCorrLarge->Draw("COLZ");
    cEneCorr->cd(6);  hAngleCorrLarge->Draw("COLZ");
  }
  if(plotAddBack) {
    cAB= new TCanvas("cAB","Info on AddBack in each petal",0,0,800,800);
    cAB->SetFillColor(0);    cAB->SetFrameFillColor(0);    cAB->Divide(2,2);
    cAB->cd(1);  hP1ABEnergy->Draw("COLZ");
    cAB->cd(2);  hP2ABEnergy->Draw("COLZ");
    cAB->cd(3);  hP1ABToT_Energy->Draw("COLZ");
    cAB->cd(4);  hP2ABToT_Energy->Draw("COLZ");
  } 
}
	 
