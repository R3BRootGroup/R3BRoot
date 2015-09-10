//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Fast Calibration for two energy peaks
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 25/09/14
//         Comments:
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      root> .L calibrate.C
//      root> calibrate("inputFile",mode)
//
//      or
//      root> calibrate("inputFile",mode,peak1Left,peak1Right,peak2Left,peak2Right)
//              
//     where inputFile is the TFile with the Tree to calibrate
//           mode: 0 no initial calibration file
//                 1 improving  previous calibration file 
//           peak1Left... are the limits of the range for looking for the peaks
//
//  To work in mode 1 (improvement) call your candidate parameters 
//  file fitPars_init.txt and expect the improved version in fitPars_final.txt
//
//  NOTE: each time you run this macro fitPars_init.txt and fitPars_final.txt 
//  will be overwritten. Store your results (for instance under fitPars_RUN.txt)
//  -------------------------------------------------------------------------

#include <iostream>
#include <fstream>
using namespace std;

void calibrate(TString inputFile="", Int_t mode=0, 
	       Double_t peak1Left=900, Double_t peak1Right=1300,
	       Double_t peak2Left=2500, Double_t peak2Right=3000) {
  
  gROOT->SetStyle("Default");
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  ofstream fitParFile;
  if(mode==0) fitParFile.open("fitPars_init.txt");
  ofstream fitParFile2;
  fitParFile2.open("fitPars_final.txt");
  
  TFile *file1 = TFile::Open(inputFile);
  
  //HISTOGRAMS DEFINITION
  char histitle[100];
  TH1F** hEnergy = new TH1F*[128];
  for(Int_t i=0;i<128;i++) {
    sprintf(histitle,"%s%i","hEnergy",i);  	
    hEnergy[i] = new TH1F(histitle,histitle,peak2Right-peak1Left+200,peak1Left-100,peak2Right+100);
  }
  TH1F** hImprovedEnergy = new TH1F*[128];
  for(Int_t i=0;i<128;i++) {
    sprintf(histitle,"%s%i","hImprovedEnergy",i);  	
    hImprovedEnergy[i] = new TH1F(histitle,histitle,peak2Right-peak1Left+200,peak1Left-100,peak2Right+100);
  }
  
  TTree* caloTree = (TTree*)file1->Get("cbmsim");
  
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
	//	rawHit[j] = new R3BCaloRawHit;
	rawHit[j] = (R3BCaloRawHit*) rawHitCA->At(j);      
      }
    }
    //filling info
    if(rawHitsPerEvent>0) {
      for(Int_t h=0;h<rawHitsPerEvent;h++){
	hEnergy[rawHit[h]->GetCrystalId()]->Fill(rawHit[h]->GetEnergy());
	hImprovedEnergy[rawHit[h]->GetCrystalId()]->Fill(rawHit[h]->GetEnergy());
      }
    }    
    if(rawHitsPerEvent) delete [] rawHit;    
  }
  
  //HISTOGRAMS DEFINITION
  char f1title[100];
  TF1** myfit_1 = new TF1*[128];
  for(Int_t i=0;i<128;i++) {
    sprintf(f1title,"%s%i","myfit_1_",i);  	
    myfit_1[i] = new TF1(f1title,"[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2]) + [3]",peak1Left,peak1Right);
    myfit_1[i]->SetLineColor(2);
    myfit_1[i]->SetLineWidth(1);
  }
  
  TF1** myfit_2 = new TF1*[128];
  for(Int_t i=0;i<128;i++) {
    sprintf(f1title,"%s%i","myfit_2_",i);  	
    myfit_2[i] = new TF1(f1title,"[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2]) + [3]", peak2Left, peak2Right);
    myfit_2[i]->SetLineColor(3);
    myfit_2[i]->SetLineWidth(1);
  }
  
  //TCanvas* c1 = new TCanvas("c1","c1",0,0,1220,900);
  //c1->SetFillColor(0);
  //c1->SetFrameFillColor(0);
  //c1->Divide(4,4);
  
  if(mode==0){    
    for(Int_t i=0;i<128;i++) {
	myfit_1[i]->SetParameter(0, 1.0e+03);
	myfit_1[i]->SetParameter(1, (peak1Left+peak1Right)/2);
	myfit_1[i]->SetParameter(2, 50);
	myfit_1[i]->SetParameter(3, 0);
	
	myfit_2[i]->SetParameter(0, 1.0e+02);
	myfit_2[i]->SetParameter(1, (peak2Left+peak2Right)/2);
	myfit_2[i]->SetParameter(2, 50);
	myfit_2[i]->SetParameter(3, 0);
	sprintf(f1title,"%s%i","myfit_1_",i);  
	hEnergy[i]->Fit(f1title,"R");  
	fitParFile << myfit_1[i]->GetParameter(0) << " " 
		   << myfit_1[i]->GetParameter(1) << " "
		   << myfit_1[i]->GetParameter(2) << " "
		   << myfit_1[i]->GetParameter(3) << endl;
	//myfit_1->Draw("SAME");
	sprintf(f1title,"%s%i","myfit_2_",i);  
	hEnergy[i]->Fit(f1title,"R");
	fitParFile << myfit_2[i]->GetParameter(0) << " " 
		   << myfit_2[i]->GetParameter(1) << " "
		   << myfit_2[i]->GetParameter(2) << " "
		   << myfit_2[i]->GetParameter(3) << endl;
      }      
    }
    fitParFile.close();
  }
  
  Double_t par1=-999., par2=-999., par3=-999., par4=-999.;
  //CANVAS DEFINITION
  char canvastitle[100];
  TCanvas** canv = new TCanvas*[8];
  for(Int_t i=0;i<8;i++) {
    sprintf(canvastitle,"%s%i","Raw_FEBEX",i);  	
    canv[i] = new TCanvas(canvastitle,canvastitle,0,0,1220,900);
    canv[i]->SetFillColor(0);
    canv[i]->SetFrameFillColor(0);
    canv[i]->Divide(4,4);
  }
  
  ifstream fitParFile_a;
  fitParFile_a.open("fitPars_init.txt");
  Int_t canvas_i=0;
  TCanvas* dummy;
  
  TF1** myfit_3 = new TF1*[128];
  TF1** myfit_4 = new TF1*[128];
  
  for(Int_t i=0;i<128;i++) { 
      sprintf(f1title,"%s%i","myfit_3_",i); 
      fitParFile_a >> par1; cout << par1<< endl; 
      fitParFile_a >> par2; cout << par2<< endl; 
      fitParFile_a >> par3; cout << par3<< endl; 
      fitParFile_a >> par4; cout << par4<< endl; 
      myfit_3[i] = new TF1(f1title,"[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2]) + [3]", par2-2*par3, par2+2*par3); 
      myfit_3[i]->SetParameter(0, par1);
      myfit_3[i]->SetParameter(1, par2);
      myfit_3[i]->SetParameter(2, par3);
      myfit_3[i]->SetParameter(3, par4);
      myfit_3[i]->SetLineColor(2);
      myfit_3[i]->SetLineWidth(1);
      
      fitParFile_a >> par1; cout << par1<< endl; 
      fitParFile_a >> par2; cout << par2<< endl; 
      fitParFile_a >> par3; cout << par3<< endl; 
      fitParFile_a >> par4; cout << par4<< endl; 
      sprintf(f1title,"%s%i","myfit_4_",i); 
      myfit_4[i] = new TF1(f1title,"[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2]) + [3]", par2-2*par3, par2+2*par3); 
      myfit_4[i]->SetParameter(0, par1);
      myfit_4[i]->SetParameter(1, par2);
      myfit_4[i]->SetParameter(2, par3);
      myfit_4[i]->SetParameter(3, par4);    
      myfit_4[i]->SetLineColor(3);
      myfit_4[i]->SetLineWidth(1);
      
      if(i<16) canv[0]->cd(i+1);
      else if(i>15 &&i<32) canv[1]->cd(i-15);
      else if(i>31 &&i<48) canv[2]->cd(i-31);
      else if(i>47 &&i<64) canv[3]->cd(i-47);
      else if(i>63 &&i<80) canv[4]->cd(i-63);
      else if(i>79 &&i<96) canv[5]->cd(i-79);
      else if(i>95 &&i<112)  canv[6]->cd(i-95);
      else if(i>111 &&i<128)  canv[7]->cd(i-111);
      
      sprintf(f1title,"%s%i","myfit_3_",i);  
      hImprovedEnergy[i]->Fit(f1title,"R");  
      fitParFile2 << myfit_3[i]->GetParameter(0) << " " 
		  << myfit_3[i]->GetParameter(1) << " "
		  << myfit_3[i]->GetParameter(2) << " "
		  << myfit_3[i]->GetParameter(3) << endl;
      myfit_3[i]->Draw("SAME");
      sprintf(f1title,"%s%i","myfit_4_",i);  
      hImprovedEnergy[i]->Fit(f1title,"R");
      fitParFile2 << myfit_4[i]->GetParameter(0) << " " 
		  << myfit_4[i]->GetParameter(1) << " "
		  << myfit_4[i]->GetParameter(2) << " "
		  << myfit_4[i]->GetParameter(3) << endl;
    }    
  }
  fitParFile_a.close();
  fitParFile2.close();
}

