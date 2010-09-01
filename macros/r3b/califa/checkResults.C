//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis results
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 01/09/10
//         Comments:
//			Checks the primary, crystalHits and caloHits characteristics.
//			User configurable for additional studies.
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l checkResults.C
//                         
//
//  -------------------------------------------------------------------------

void checkResults() {
	// ----    Debug option   -------------------------------------------------
	gDebug = 0;
	// ------------------------------------------------------------------------
	
	// ----  Load libraries   -------------------------------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	
	gSystem->Load("libGenVector");
	
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libMCStack");
	gSystem->Load("libField");
	gSystem->Load("libGen");
	
	//----  Load R3B specific libraries ---------------------------------------
	gSystem->Load("libR3Bbase");
	//gSystem->Load("libR3BGen");
	gSystem->Load("libR3BPassive");
	gSystem->Load("libR3BData");
	gSystem->Load("libR3BCal");
	gSystem->Load("libR3BCalo");
	//gSystem->Load("libR3BDch");
	//gSystem->Load("libR3BGfi");
	//gSystem->Load("libR3BLand");
	//gSystem->Load("libR3BmTof");
	//gSystem->Load("libR3BTof");
	//gSystem->Load("libR3BTra");
	//gSystem->Load("libR3BChimera");
	//gSystem->Load("libELILuMon");
	
	
	gROOT->SetStyle("Default");
	//gStyle->SetOptTitle(0);
	//gStyle->SetOptStat(0);
	//gStyle->SetOptFit(0);
	
	TH1F* h1_T = new TH1F("h1_T","Primary PDG Code",2300,0,2299);
	TH1F* h2_T = new TH1F("h2_T","Primary Energy (MeV)",200,0,3); //Change this maximum energy
	TH1F* h3_T = new TH1F("h3_T","Primary Theta",200,0,3.2);
	TH1F* h4_T = new TH1F("h4_T","Primary Phi",200,-3.2,3.2);
	TH1F* h1_Cry = new TH1F("h1_Cry","Crystal ID",3840,0,3840);
	TH1F* h2_Cry = new TH1F("h2_Cry","Crystal Energy (MeV)",200,0,3); //Change this maximum energy
	TH1F* h3_Cry = new TH1F("h3_Cry","Crystal Type",30,0,30);
	TH1F* h4_Cry = new TH1F("h4_Cry","Crystal Copy",200,0,200);
	TH1F* h1_Cal = new TH1F("h1_Cal","Nb of Crystals in hit",20,0,20);
	TH1F* h2_Cal = new TH1F("h2_Cal","Hit Energy (MeV)",200,0,3); //Change this maximum energy
	TH1F* h3_Cal = new TH1F("h3_Cal","Hit Theta",200,0,3.2);
	TH1F* h4_Cal = new TH1F("h4_Cal","Hit Phi",200,-3.2,3.2);
	TH1F* h1_TMul = new TH1F("h1_TMul","Multiplicities",20,0,20);
	TH1F* h1_CryMul = new TH1F("h1_CryMul","Multiplicities",20,0,20);
	TH1F* h1_CalMul = new TH1F("h1_CalMul","Multiplicities",20,0,20);
	TH1F* h2_CC = new TH1F("h2_CC","Reconstructed Primary Energy (MeV)",2000,0,3); //Change this maximum energy
	TH1F* h3_CC = new TH1F("h3_CC","Theta resolution",400,-0.5,0.5);
	TH1F* h4_CC = new TH1F("h4_CC","Phi resolution",400,-0.5,0.5);
	//Additional user defined histos
	TH2F* h1_User = new TH2F("h1_User","Double hits, ... where?",400,-2.5,2.5,400,-2.5,2.5);
	
	h1_CryMul->SetLineColor(kRed);
	h1_CalMul->SetLineColor(kBlue);

	TFile *file0 = TFile::Open("califaAna.root");
	TFile *file1 = TFile::Open("r3bsim.root");

	TTree* TCalo = (TTree*)file0->Get("cbmsim");
	TTree* TCrystal = (TTree*)file1->Get("cbmsim");
	
	//Crystal Hits (input)
	TClonesArray* crystalHitCA;  
	R3BCaloCrystalHit** crystalHit;
	crystalHitCA = new TClonesArray("R3BCaloCrystalHit",5);
	TBranch *branchCrystalHit = TCrystal->GetBranch("CrystalHit");
	branchCrystalHit->SetAddress(&crystalHitCA);

	//Calo Hits (output)
	TClonesArray* caloHitCA;  
	R3BCaloHit** caloHit;
	caloHitCA = new TClonesArray("R3BCaloHit",5);
	TBranch *branchCaloHit = TCalo->GetBranch("CaloHit");
	branchCaloHit->SetAddress(&caloHitCA);
	
	//MCTrack(input)
	TClonesArray* MCTrackCA;  
	R3BMCTrack** track;
	MCTrackCA = new TClonesArray("R3BMCTrack",5);
	TBranch *branchMCTrack = TCrystal->GetBranch("MCTrack");
	branchMCTrack->SetAddress(&MCTrackCA);
	
	Long64_t nevents = TCrystal->GetEntries();
		
	Int_t crystalHitsPerEvent=0;
	Int_t caloHitsPerEvent=0;
	Int_t MCtracksPerEvent=0;
	Int_t primary=0;
	TVector3 momentum;
	Double_t corEnergy=0;
	Int_t tirame=0;
	
	for(Int_t i=0;i<nevents;i++){
	    if(i%1000 == 0) printf("Event:%d\n",i);
				
		crystalHitCA->Clear();
		caloHitCA->Clear();
		MCTrackCA->Clear();

		TCrystal->GetEvent(i);
		TCalo->GetEvent(i);
		
		crystalHitsPerEvent = crystalHitCA->GetEntries(); 
		caloHitsPerEvent = caloHitCA->GetEntries(); 
		MCtracksPerEvent = MCTrackCA->GetEntries();
		
		if(crystalHitsPerEvent>0) {
			crystalHit = new R3BCaloCrystalHit*[crystalHitsPerEvent];
			for(Int_t j=0;j<crystalHitsPerEvent;j++){
				crystalHit[j] = new R3BCaloCrystalHit;
				crystalHit[j] = (R3BCaloCrystalHit*) crystalHitCA->At(j);      
			}
		}
		if(caloHitsPerEvent>0) {
			caloHit = new R3BCaloHit*[caloHitsPerEvent];
			for(Int_t j=0;j<caloHitsPerEvent;j++){
				caloHit[j] = new R3BCaloHit;
				caloHit[j] = (R3BCaloHit*) caloHitCA->At(j);      
			}
		}		
		if(MCtracksPerEvent>0) {
			track = new R3BMCTrack*[MCtracksPerEvent];
			for(Int_t j=0;j<MCtracksPerEvent;j++){
				track[j] = new R3BMCTrack;
				track[j] = (R3BMCTrack*) MCTrackCA->At(j);      
			}
		}
				
		// Use this part to select on the primary 
		// (or in other feature of any hit, if properly modified)
		/*
		tirame=0;
		for(Int_t h=0;h<MCtracksPerEvent;h++){
			if(track[h]->GetMotherId()<0){
				track[h]->GetMomentum(momentum);	
				if(momentum.Theta()<1.8 || momentum.Theta()>2.2) tirame=1;
				//if(momentum.Phi()<-0.8 || momentum.Phi()>-0.5) tirame=1;
			}
		}
		if(tirame) continue;	
		 */
		
		for(Int_t h=0;h<crystalHitsPerEvent;h++){
			h1_Cry->Fill(crystalHit[h]->GetCrystalId());
			h2_Cry->Fill(crystalHit[h]->GetEnergy()*1000);
			h3_Cry->Fill(crystalHit[h]->GetCrystalType());
			h4_Cry->Fill(crystalHit[h]->GetCrystalCopy());
		}
		for(Int_t h=0;h<caloHitsPerEvent;h++){
			h1_Cal->Fill(caloHit[h]->GetNbOfCrystalHits());	
			h2_Cal->Fill(caloHit[h]->GetEnergy()*1000);	
			h3_Cal->Fill(caloHit[h]->GetTheta());	
			h4_Cal->Fill(caloHit[h]->GetPhi());
			h2_CC->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,0.82));
			
		}
		for(Int_t h=0;h<MCtracksPerEvent;h++)
			if(track[h]->GetMotherId()<0){
				track[h]->GetMomentum(momentum);	
				h1_T->Fill(track[h]->GetPdgCode());
				h2_T->Fill(track[h]->GetEnergy()*1000-track[h]->GetMass()*1000);
				h3_T->Fill(momentum.Theta());
				h4_T->Fill(momentum.Phi());
				primary++;
				if(primary==1){
					if(caloHitsPerEvent==1){
						h3_CC->Fill(momentum.Theta() - caloHit[h]->GetTheta());
						h4_CC->Fill(momentum.Phi() - caloHit[h]->GetPhi());
					}
				}
				
			}
		h1_CryMul->Fill(crystalHitsPerEvent);
		h1_CalMul->Fill(caloHitsPerEvent);
		h1_TMul->Fill(primary);
		primary=0;
		
		//User defined additions
		if(caloHitsPerEvent==2){
			h1_User->Fill(caloHit[1]->GetTheta()-caloHit[0]->GetTheta(),
						  caloHit[1]->GetPhi()-caloHit[0]->GetPhi());
		}
			
		if(crystalHitsPerEvent) delete [] crystalHit;
		if(caloHitsPerEvent) delete [] caloHit;
		if(MCtracksPerEvent) delete [] track;

	}
	TCanvas* c1 = new TCanvas("MCTrack","MCTrack",0,0,720,400);
	TCanvas* c2 = new TCanvas("crystalHit","crystalHit",720,0,720,400);
	TCanvas* c3 = new TCanvas("caloHit","caloHit",0,435,720,400);
	TCanvas* c4 = new TCanvas("correlations","correlations",720,435,720,400);
	TCanvas* c5 = new TCanvas("User","User defined",200,300,720,400);

	c1->Divide(2,2);
	c1->cd(1); h1_T->Draw();
	c1->cd(2); h2_T->Draw();
	c1->cd(3); h3_T->Draw();
	c1->cd(4); h4_T->Draw();
	c2->Divide(2,2);
	c2->cd(1); h1_Cry->Draw();
	c2->cd(2); h2_Cry->Draw();
	c2->cd(3); h3_Cry->Draw();
	c2->cd(4); h4_Cry->Draw();
	c3->Divide(2,2);
	c3->cd(1); h1_Cal->Draw();
	c3->cd(2); h2_Cal->Draw();
	c3->cd(3); h3_Cal->Draw();
	c3->cd(4); h4_Cal->Draw();
	c4->Divide(2,2);
	TVirtualPad * c4_1 = c4->cd(1);   
	c4_1->SetLogy();
	h1_TMul->Draw(); h1_CalMul->Draw("SAME"); h1_CryMul->Draw("SAME"); 
	c5->cd();
	h1_User->Draw();
	
	TLatex Tl;
	Tl.SetTextSize(0.06);	Tl.DrawLatex(14, 10000, "Primary");
	Tl.SetTextSize(0.06);   Tl.SetTextColor(2);  Tl.DrawLatex(14, 3500, "Crystal");
	Tl.SetTextSize(0.06);   Tl.SetTextColor(4);  Tl.DrawLatex(14, 1000, "Calorimeter");
	
	TVirtualPad * c4_2 = c4->cd(2);	
	c4_2->SetLogy();
	h2_CC->Draw();
	
	c4->cd(3); h3_CC->Draw();
	c4->cd(4); h4_CC->Draw();
	
	
	
	
}
		
		
Double_t GetCMEnergy(Double_t theta, Double_t energy, Double_t beta=0.82){
	//
	// Calculating the CM energy from the lab energy and the polar angle
	//
	Double_t gamma = 1/sqrt(1-beta*beta);
	//Lorenzt boost correction
	//E' = gamma E + beta gamma P|| = gamma E + beta gamma P cos(theta) 
	//In photons E=P
	Double_t energyCorrect = gamma*energy - beta*gamma*energy*cos(theta);
			
	return energyCorrect;
}
		
			
