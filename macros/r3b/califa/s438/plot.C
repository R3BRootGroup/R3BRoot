//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Data Display
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 26/09/14
//         Comments:
//         THAT IS A TEMPLATE FOR OTHER DISPLAY/ANALYSIS MACRO. COPY AND USE.
//         CAN DISPLAY RAWHITs OR CRYSTALHITs 
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      ROOT> .L plot.C
//      ROOT> plot("inputFile")
//     
//     where inputFile is the input file :) 
//     Define histograms and fill them in the loop!
//  -------------------------------------------------------------------------
void plot(TString inputFile="") {

  gROOT->SetStyle("Default");
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  TFile *file1 = TFile::Open(inputFile);
  
  //HISTOGRAMS DEFINITION
  TH1F* hEnergy = new TH1F("hEnergy","Energy",1000,0,2000);
  TH2F* hEnergyvsID = new TH2F("hEnergyvsID","EnergyvsID",500,0,2500,128,0,127);
  TH2F* hTimevsID = new TH2F("hTimevsID","TimevsID",500,1450000000000,1460000000000,128,0,127);
  TH2F* hEnergyvsTime = new TH2F("hEnergyvsTime","EnergyvsTime",500,0,2500,500,1450000000000,1650000000000);



  TTree* caloTree = (TTree*)file1->Get("cbmsim");
 
  //Raw Hits (input)
  TClonesArray* rawHitCA;  
  R3BCalifaMappedData** rawHit;
  rawHitCA = new TClonesArray("R3BCalifaMappedData",5);
  TBranch *branchRawHit = caloTree->GetBranch("CalifaMappedData");
  if(branchRawHit) branchRawHit->SetAddress(&rawHitCA);
  
  //Crystal Hits
  TClonesArray* crystalHitCA;  
  R3BCalifaCrystalCalData** crystalHit;
  crystalHitCA = new TClonesArray("R3BCalifaCrystalCalData",5);
  TBranch *branchCrystalHit = caloTree->GetBranch("CalifaCrystalCalData");
  if(branchCrystalHit) branchCrystalHit->SetAddress(&crystalHitCA);
  
  Long64_t nevents = caloTree->GetEntries();
  Int_t rawHitsPerEvent =0;
  Int_t crystalHitsPerEvent =0;
  
  for(Int_t i=0;i<nevents;i++){
    if(i%100000 == 0) printf("Event:%i\n",i);
    
    rawHitCA->Clear();
    crystalHitCA->Clear();

    caloTree->GetEvent(i);
    rawHitsPerEvent = rawHitCA->GetEntries(); 
    crystalHitsPerEvent = crystalHitCA->GetEntries(); 
    
    if(rawHitsPerEvent>0) {
      rawHit = new R3BCalifaMappedData*[rawHitsPerEvent];
      for(Int_t j=0;j<rawHitsPerEvent;j++){
	//rawHit[j] = new R3BCalifaMappedData;
	rawHit[j] = (R3BCalifaMappedData*) rawHitCA->At(j);      
      }
    }
    if(crystalHitsPerEvent>0) {
      crystalHit = new R3BCalifaCrystalCalData*[crystalHitsPerEvent];
      for(Int_t j=0;j<crystalHitsPerEvent;j++){
	//crystalHit[j] = new R3BCalifaCrystalCalData;
	crystalHit[j] = (R3BCalifaCrystalCalData*) crystalHitCA->At(j);      
      }
    }
    
    //filling info
    if(rawHitsPerEvent>0) {
      for(Int_t h=0;h<rawHitsPerEvent;h++){
	hEnergy->Fill(rawHit[h]->GetEnergy());
	hEnergyvsID->Fill(rawHit[h]->GetEnergy(),rawHit[h]->GetCrystalId());
	hTimevsID->Fill(rawHit[h]->GetTime(),rawHit[h]->GetCrystalId());
	hEnergyvsTime->Fill(rawHit[h]->GetEnergy(),rawHit[h]->GetTime());
      }
    }    
    if(crystalHitsPerEvent>0) {
      for(Int_t h=0;h<crystalHitsPerEvent;h++){
	hEnergy->Fill(crystalHit[h]->GetEnergy());
      }
    }  

    
    if(rawHitsPerEvent) delete [] rawHit;
    if(crystalHitsPerEvent) delete [] crystalHit;
    
  }
  
  TCanvas* c1 = new TCanvas("Info","Info",0,0,780,1200);
	c1->SetFillColor(0);
	c1->SetFrameFillColor(0);
	c1->Divide(2,2);
	//MC TRACK CANVAS
	c1->cd(1);
	hEnergy->Draw();
	c1->cd(2);
	hEnergyvsID->Draw("COLZ");
	c1->cd(3);
	hEnergyvsTime->Draw("COLZ");
	c1->cd(4);
	hTimevsID->Draw("COLZ");
}
	   
