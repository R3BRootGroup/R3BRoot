//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Pulser Calibration - first part
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 30/09/14
//         Comments:
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      root> .L calPulser_part1.C
//      root> cal("inputFile_30","inputFile_300", peaks)
//              
//     where inputFile_30 and inputFile_300 are the TFile(s) with the Tree(s) 
//           with the pulser data to calibrate (30 and 300 MeV, respectively)
//           peaks are the number of peaks to look for
//
//  NOTE: each time you run this macro fitPars_pulser.txt will be overwritten. 
//  Store your results (for instance under fitPars_pulser_RUN.txt)
//  -------------------------------------------------------------------------
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"

#include <iostream>
#include <fstream>

using namespace std;

const Int_t npeaks = 10;

void cal(TString inputFile_30="",TString inputFile_300="", Int_t np=4) {
  
  gROOT->SetStyle("Default");
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  ofstream fitParFile;
  fitParFile.open("fitPars_pulser.txt");
  
  TFile *file1 = TFile::Open(inputFile_30);
  TFile *file2 = TFile::Open(inputFile_300);
  
  //HISTOGRAMS DEFINITION
  char histitle[100];
  TH1F** hEnergy_30 = new TH1F*[128];
  for(Int_t i=0;i<128;i++) {
    sprintf(histitle,"%s%i","hEnergy_30_",i);  	
    hEnergy_30[i] = new TH1F(histitle,histitle,20000,10000,30000);
  }  
  TH1F** hEnergy_300 = new TH1F*[128];
  for(Int_t i=0;i<128;i++) {
    sprintf(histitle,"%s%i","hEnergy_300_",i);  	
    hEnergy_300[i] = new TH1F(histitle,histitle,2000,1000,3000);
  }  

  TTree* caloTree = (TTree*)file1->Get("cbmsim");
  TTree* caloTree2 = (TTree*)file2->Get("cbmsim");
  
  //Raw Hits (input)
  TClonesArray* rawHitCA;  
  R3BCaloRawHit** rawHit;
  rawHitCA = new TClonesArray("R3BCaloRawHit",5);
  TBranch *branchRawHit = caloTree->GetBranch("CaloRawHit");
  if(branchRawHit) branchRawHit->SetAddress(&rawHitCA);
  
  Long64_t nevents = caloTree->GetEntries();
  Int_t rawHitsPerEvent =0;
  
  for(Int_t i=0;i<nevents;i++){
    if(i%100000 == 0) printf("Event:%i\n",i);
    
    rawHitCA->Clear();
    caloTree->GetEvent(i);
    rawHitsPerEvent = rawHitCA->GetEntries(); 
    
    if(rawHitsPerEvent>0) {
      rawHit = new R3BCaloRawHit*[rawHitsPerEvent];
      for(Int_t j=0;j<rawHitsPerEvent;j++){
	rawHit[j] = (R3BCaloRawHit*) rawHitCA->At(j);      
      }
    }
    //filling info
    if(rawHitsPerEvent>0) {
      for(Int_t h=0;h<rawHitsPerEvent;h++){
	hEnergy_30[rawHit[h]->GetCrystalId()]->Fill(rawHit[h]->GetEnergy());
      }
    }    
    if(rawHitsPerEvent) delete [] rawHit;    
  }
  //SECOND TREE
  nevents = caloTree2->GetEntries();
  rawHitsPerEvent =0;
  
  TBranch *branchRawHit2 = caloTree2->GetBranch("CaloRawHit");
  if(branchRawHit2) branchRawHit2->SetAddress(&rawHitCA);
  
  for(Int_t i=0;i<nevents;i++){
    if(i%100000 == 0) printf("Event:%i\n",i);
    
    rawHitCA->Clear();
    caloTree2->GetEvent(i);
    rawHitsPerEvent = rawHitCA->GetEntries(); 
    
    if(rawHitsPerEvent>0) {
      rawHit = new R3BCaloRawHit*[rawHitsPerEvent];
      for(Int_t j=0;j<rawHitsPerEvent;j++){
	rawHit[j] = (R3BCaloRawHit*) rawHitCA->At(j);      
      }
    }
    //filling info
    if(rawHitsPerEvent>0) {
      for(Int_t h=0;h<rawHitsPerEvent;h++){
	hEnergy_300[rawHit[h]->GetCrystalId()]->Fill(rawHit[h]->GetEnergy());
      }
    }    
    if(rawHitsPerEvent) delete [] rawHit;    
  }
  
  Double_t par[3000];
  par[0] = 0.8;
  par[1] = -0.6/1000;
  
  //CANVAS DEFINITION
  char canvastitle[100];
  TCanvas** canv_30 = new TCanvas*[8];
  for(Int_t i=0;i<8;i++) {
    sprintf(canvastitle,"%s%i","Raw_FEBEX_30_",i);  	
    canv_30[i] = new TCanvas(canvastitle,canvastitle,0,0,1220,900);
    canv_30[i]->SetFillColor(0);
    canv_30[i]->SetFrameFillColor(0);
    canv_30[i]->Divide(4,4);
  }
  TCanvas** canv_300 = new TCanvas*[8];
  for(Int_t i=0;i<8;i++) {
    sprintf(canvastitle,"%s%i","Raw_FEBEX_300_",i);  	
    canv_300[i] = new TCanvas(canvastitle,canvastitle,0,0,1220,900);
    canv_300[i]->SetFillColor(0);
    canv_300[i]->SetFrameFillColor(0);
    canv_300[i]->Divide(4,4);
  }
  
  TF1** myfit_30 = new TF1*[512];
  TF1** myfit_300 = new TF1*[512];
 
  Int_t nfound;
  TSpectrum *s = new TSpectrum(2*npeaks);
  
  //TH1F** hEnergy2_30 = new TH1F*[128];
  //TH1F** hEnergy2_300 = new TH1F*[128];
  TF1 *fline = new TF1("fline","pol1",0,1000);
  Float_t *xpeaks;
  Float_t xp=0;
  Int_t bin=0;
  Float_t yp=0;
  
  for(Int_t i=0;i<128;i++) { 
    //sprintf(canvastitle,"%s%i","hEnergy2_30_",i);  	   
    //TH1F *hEnergy2_30 = (TH1F*)hEnergy_30[i]->Clone(canvastitle);
    //sprintf(canvastitle,"%s%i","hEnergy2_300_",i);  	   
    //TH1F *hEnergy2_300 = (TH1F*)hEnergy_300[i]->Clone(canvastitle);

    if(i<16) canv_30[0]->cd(i+1);
    else if(i>15 &&i<32) canv_30[1]->cd(i-15);
    else if(i>31 &&i<48) canv_30[2]->cd(i-31);
    else if(i>47 &&i<64) canv_30[3]->cd(i-47);
    else if(i>63 &&i<80) canv_30[4]->cd(i-63);
    else if(i>79 &&i<96) canv_30[5]->cd(i-79);
    else if(i>95 &&i<112) canv_30[6]->cd(i-95);
    else if(i>111 &&i<128) canv_30[7]->cd(i-111);
    
    hEnergy_30[i]->Draw();
    //Use TSpectrum to find the peak candidates
    nfound = s->Search(hEnergy_30[i],20,"new");
    xpeaks = s->GetPositionX();
    for (Int_t poo=0;poo<nfound;poo++) {
      Float_t xp = xpeaks[poo];
      cout << " pos[" << poo << "]=" << xp; 
      sprintf(canvastitle,"%s%i%s%i","myfit_30_",i,"_",poo);  	
      myfit_30[i*poo] = new TF1(canvastitle,"[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2]) + [3]",xpeaks[poo]-25,xpeaks[poo]+25);
      myfit_30[i*poo]->SetParameter(0, 500);
      myfit_30[i*poo]->SetParameter(1, xpeaks[poo]);
      myfit_30[i*poo]->SetParameter(2, 20);
      myfit_30[i*poo]->SetParameter(3, 0);  
      myfit_30[i*poo]->SetLineColor(2);
      myfit_30[i*poo]->SetLineWidth(1);
      hEnergy_30[i]->Fit(canvastitle,"R");  
      fitParFile << myfit_30[i*poo]->GetParameter(0) << " " 
		 << myfit_30[i*poo]->GetParameter(1) << " "
		 << myfit_30[i*poo]->GetParameter(2) << " "
		 << myfit_30[i*poo]->GetParameter(3) << endl;
    }
    cout << endl;
    
    if(i<16) canv_300[0]->cd(i+1);
    else if(i>15 &&i<32) canv_300[1]->cd(i-15);
    else if(i>31 &&i<48) canv_300[2]->cd(i-31);
    else if(i>47 &&i<64) canv_300[3]->cd(i-47);
    else if(i>63 &&i<80) canv_300[4]->cd(i-63);
    else if(i>79 &&i<96) canv_300[5]->cd(i-79);
    else if(i>95 &&i<112) canv_300[6]->cd(i-95);
    else if(i>111 &&i<128) canv_300[7]->cd(i-111);
    
    hEnergy_300[i]->Draw();
    //Use TSpectrum to find the peak candidates
    nfound = s->Search(hEnergy_300[i],20,"new");
    xpeaks = s->GetPositionX();
    for (poo=0;poo<nfound;poo++) {
      Float_t xp = xpeaks[poo];
      cout << " pos[" << poo << "]=" << xp; 
      sprintf(canvastitle,"%s%i%s%i","myfit_300_",i,"_",poo);  	
      myfit_300[i*poo] = new TF1(canvastitle,"[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2]) + [3]",xpeaks[poo]-20,xpeaks[poo]+20);
      myfit_300[i*poo]->SetParameter(0, 500);
      myfit_300[i*poo]->SetParameter(1, xpeaks[poo]);
      myfit_300[i*poo]->SetParameter(2, 20);
      myfit_300[i*poo]->SetParameter(3, 0);  
      myfit_300[i*poo]->SetLineColor(2);
      myfit_300[i*poo]->SetLineWidth(1);
      hEnergy_300[i]->Fit(canvastitle,"R");  
      fitParFile << myfit_300[i*poo]->GetParameter(0) << " " 
		 << myfit_300[i*poo]->GetParameter(1) << " "
		 << myfit_300[i*poo]->GetParameter(2) << " "
		 << myfit_300[i*poo]->GetParameter(3) << endl;
    }
    cout << endl;
    
  }
  for(Int_t i=0;i<8;i++)   {
    canv_30[i]->Update();
    canv_300[i]->Update();
  }
  fitParFile.close();
  
}


