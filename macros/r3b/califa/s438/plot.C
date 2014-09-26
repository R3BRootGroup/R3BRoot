//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Raw Data Display
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 25/09/14
//         Comments:
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l plotRaw.C
//                         
//     BUT FIRST, select in the //SETTINGS section the simulation features 
//	(the macro will plot and text information as a function of these settings)
//  -------------------------------------------------------------------------

void plot(TString inputFile) {

  gROOT->SetStyle("Default");
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  TFile *file1 = TFile::Open(inputFile);
  
  //HISTOGRAMS DEFINITION
  TH1F* hEnergy = new TH1F("hEnergy","Energy",1000,0,10000);
 
  TTree* caloTree = (TTree*)file1->Get("cbmsim");
 
  //Raw Hits (input)
  TClonesArray* rawHitCA;  
  R3BCaloRawHit** rawHit;
  rawHitCA = new TClonesArray("R3BCaloRawHit",5);
  TBranch *branchRawHit = caloTree->GetBranch("CaloRawHit");
  if(branchRawHit) branchRawHit->SetAddress(&rawHitCA);
  
  //Crystal Hits
  TClonesArray* crystalHitCA;  
  R3BCaloCrystalHit** crystalHit;
  crystalHitCA = new TClonesArray("R3BCaloCrystalHit",5);
  TBranch *branchCrystalHit = caloTree->GetBranch("CaloCrystalHit");
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
      rawHit = new R3BCaloRawHit*[rawHitsPerEvent];
      for(Int_t j=0;j<rawHitsPerEvent;j++){
	rawHit[j] = new R3BCaloRawHit;
	rawHit[j] = (R3BCaloRawHit*) rawHitCA->At(j);      
      }
    }
    if(crystalHitsPerEvent>0) {
      crystalHit = new R3BCaloCrystalHit*[crystalHitsPerEvent];
      for(Int_t j=0;j<crystalHitsPerEvent;j++){
	crystalHit[j] = new R3BCaloCrystalHit;
	crystalHit[j] = (R3BCaloCrystalHit*) crystalHitCA->At(j);      
      }
    }
    
    //filling info
    if(rawHitsPerEvent>0) {
      for(Int_t h=0;h<rawHitsPerEvent;h++){
	hEnergy->Fill(rawHit[h]->GetEnergy());
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
  
  TCanvas* c1 = new TCanvas("Info","Info",0,0,720,900);
	c1->SetFillColor(0);
	c1->SetFrameFillColor(0);

	//MC TRACK CANVAS
	c1->cd();
	hEnergy->Draw();

}
	   
