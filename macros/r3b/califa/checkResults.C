//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis results
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 08/09/15 <pablo.cabanelas@usc.es>
//         Comments:
//			Checks the primary, crystalHits and caloHits characteristics.
//			User configurable for additional studies.
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l checkResults.C
//                         
//     BUT FIRST, select in the //SETTINGS section the simulation features 
//	(the macro will plot and text information as a function of these settings)
//  -------------------------------------------------------------------------

		
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


void checkResults() {

	char title1[250];
	
	//SETTINGS 
	char calVersion[50] = "v13_811";          //Calorimeter version (5.0, 7.05, 7.07, 7.09, 7.17, 7.07+7.17,7.09+7.17, 8.11)
	Double_t Eproj = 2.00;                    //Gamma Energy in projectile frame in MeV 
	Int_t totalEvents = 2000;                 //Events
	Int_t multiplicity = 1;                   //Multiplicity (particles per event)
	
	Double_t threshold=0.050;		  //Threshold in MeV
	Int_t ExpRes=5;			          //Exp. Resol in MeV

	//FOR THE HISTOGRAMS AND PLOTS:
	Double_t maxE = 5;                        //Maximum energy in MeV in the histos

 	sprintf(title1,"%s","sim_out.root");  	
	TFile *file1 = TFile::Open(title1);
	
	Double_t beta=0.82;
	
	//END OF THE SETTING AREA
	
	Bool_t BARREL= kFALSE;
	Bool_t ENDCAP= kFALSE;
	Double_t minThetaBarrel=0. , maxThetaBarrel=0.;
        Double_t minThetaEndCap=0. , maxThetaEndCap=0.;

	if(!strcmp(calVersion,"5.0")){
		cout << "Warning: Calorimeter version 5.0 is not supported in this macro! "<< endl;
	}
	if(!strcmp(calVersion,"7.05")){
		cout << "Using CALIFA version 7.05 "<< endl;
		minThetaBarrel= 32.9;    //Angular coverture of BARREL 7.05
		maxThetaBarrel= 134.7;   //Angular coverture of BARREL 7.05
		BARREL=kTRUE;
	}
	else if(!strcmp(calVersion,"7.07")){
		cout << "Using CALIFA version 7.07 "<< endl;
		minThetaBarrel= 32.4;  //Angular coverture of BARREL 7.07
		maxThetaBarrel= 134.2; //Angular coverture of BARREL 7.07
		BARREL=kTRUE;
	}
	else if(!strcmp(calVersion,"7.09")){
		cout << "Using CALIFA version 7.09 "<< endl;
		minThetaBarrel= 32.;  //Angular coverture of BARREL 7.09
		maxThetaBarrel= 135.; //Angular coverture of BARREL 7.09
		BARREL=kTRUE;
	}
	else if(!strcmp(calVersion,"7.17")){
		cout << "Using CALIFA version 7.17 "<< endl;
		minThetaEndCap= 9.59;  //Angular coverture of ENDCAP 7.17
		maxThetaEndCap= 32.10; //Angular coverture of ENDCAP 7.17
		ENDCAP=kTRUE;
	}
	else if(!strcmp(calVersion,"7.07+7.17")){
		cout << "Using CALIFA version 7.07+7.17 "<< endl;
		minThetaBarrel= 32.4;  //Angular coverture of BARREL 7.07
		maxThetaBarrel= 134.2; //Angular coverture of BARREL 7.07
		minThetaEndCap= 9.59;  //Angular coverture of ENDCAP 7.17
		maxThetaEndCap= 32.10; //Angular coverture of ENDCAP 7.17
		BARREL=kTRUE;
		ENDCAP=kTRUE;
	}
	else if(!strcmp(calVersion,"7.09+7.17")){
		cout << "Using CALIFA version 7.09+7.17 "<< endl;
		minThetaBarrel= 32.;  //Angular coverture of BARREL 7.09
		maxThetaBarrel= 135.; //Angular coverture of BARREL 7.09
		minThetaEndCap= 9.59;  //Angular coverture of ENDCAP 7.17
		maxThetaEndCap= 32.10; //Angular coverture of ENDCAP 7.17
		BARREL=kTRUE;		
		ENDCAP=kTRUE;
	}
	else if(!strcmp(calVersion,"8.11")){
		cout << "Using CALIFA version 8.11 "<< endl;
		minThetaBarrel= 43.16;  //Angular coverture of BARREL 8.11
		maxThetaBarrel= 135.35; //Angular coverture of BARREL 8.11
		BARREL=kTRUE;
	}	
	else if(!strcmp(calVersion,"8.11+iPhos1.01")){
		cout << "Using CALIFA version 8.11 + iPhos 1.01 "<< endl;
		minThetaBarrel= 32.;  //Angular coverture of BARREL 7.09
		maxThetaBarrel= 135.; //Angular coverture of BARREL 7.09
		minThetaEndCap= 9.59;  //Angular coverture of ENDCAP 7.17
		maxThetaEndCap= 32.10; //Angular coverture of ENDCAP 7.17
		BARREL=kTRUE;		
		ENDCAP=kTRUE;
	}
	else if(!strcmp(calVersion,"v13_811")){
		cout << "Using CALIFA version v13_8.11 "<< endl;
		minThetaBarrel= 43.16;   //Angular coverture of BARREL 8.11
		maxThetaBarrel= 135.35;  //Angular coverture of BARREL 8.11
		minThetaEndCap= 9.59;    //Angular coverture of ENDCAP
		maxThetaEndCap= 32.10;   //Angular coverture of ENDCAP
		BARREL=kTRUE;
		ENDCAP=kTRUE;
	}
	else cout << "Error: Calorimeter version not correctly defined! "<< endl;
		
	// ----    Debug option   -------------------------------------------------
	gDebug = 0;
	// ------------------------------------------------------------------------
		
	
	gROOT->SetStyle("Default");
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	
	
	//HISTOGRAMS DEFINITION
	TH1F* h1_T = new TH1F("h1_T","Primary PDG Code",2300,0,2300);
	TH1F* h2_T = new TH1F("h2_T","Primary Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_T = new TH1F("h3_T","Primary Theta",200,0,3.2);
	TH1F* h4_T = new TH1F("h4_T","Primary Phi",200,-3.2,3.2);
	TH1F* h1_Cry = new TH1F("h1_Cry","Crystal ID",5850,0,5850);
	TH1F* h1_Cry_count = new TH1F("h1_Cry_count","Crystal ID_count",2,0,3840);
	TH1F* h2_Cry = new TH1F("h2_Cry","Crystal Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cry = new TH1F("h3_Cry","Crystal Type",32,0,32);
	TH1F* h4_Cry = new TH1F("h4_Cry","Crystal Copy",514,0,514);
	TH1F* h1_Cal = new TH1F("h1_Cal","Nb of Crystals in hit",20,0,20);
	TH1F* h2_Cal = new TH1F("h2_Cal","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cal = new TH1F("h3_Cal","Hit Theta",200,0,3.2);
	TH1F* h4_Cal = new TH1F("h4_Cal","Hit Phi",200,-3.2,3.2);
	TH1F* h1_TMul = new TH1F("h1_TMul","Multiplicities",20,0,20);
	TH1F* h1_CryMul = new TH1F("h1_CryMul","Multiplicities",20,0,20);
	TH1F* h1_CalMul = new TH1F("h1_CalMul","Multiplicities",20,0,20);
	TH1F* h2_CC = new TH1F("h2_CC","Reconstructed (Lorenzt corrected) Primary Energy (MeV)",2000,0,3*maxE); //Change this maximum energy
	TH1F* h3_CC = new TH1F("h3_CC","Theta residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH1F* h4_CC = new TH1F("h4_CC","Phi residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH2F* h1_CC2 = new TH2F("h1_CC2","Multiplicity vs. polar angle",200,0,3.2,36,0,12);
	TH2F* h2_CC2 = new TH2F("h2_CC2","Energy reconstructed vs. polar angle",50,0,3.2,400,0,3*maxE); //Change this maximum energy
	TH2F* h3_CC2 = new TH2F("h3_CC2","Theta residuals vs. polar angle",100,0,3.2,100,-0.25,0.25);
	TH2F* h4_CC2 = new TH2F("h4_CC2","Phi residuals vs. polar angle",100,0,3.2,100,-0.25,0.25);

	TH1F* h1_EF = new TH1F("h1_EF","Efficiency vs. polar angle",200,0,3.2);

	
	//BARREL
	TH1F* h1_Cry_barrel = new TH1F("h1_Cry_barrel","Crystal ID",5850,0,5850);
	TH1F* h1_Cry_count_barrel = new TH1F("h1_Cry_count_barrel","Crystal ID_count",2,0,3840);
	TH1F* h2_Cry_barrel = new TH1F("h2_Cry_barrel","Crystal Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cry_barrel = new TH1F("h3_Cry_barrel","Crystal Type",32,0,32);
	TH1F* h4_Cry_barrel = new TH1F("h4_Cry_barrel","Crystal Copy",514,0,514);
	TH1F* h1_Cal_barrel = new TH1F("h1_Cal_barrel","Nb of Crystals in hit",20,0,20);
	TH1F* h2_Cal_barrel = new TH1F("h2_Cal_barrel","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cal_barrel = new TH1F("h3_Cal_barrel","Hit Theta",200,0,3.2);
	TH1F* h4_Cal_barrel = new TH1F("h4_Cal_barrel","Hit Phi",200,-3.2,3.2);
	TH1F* h1_TMul_barrel = new TH1F("h1_TMul_barrel","Multiplicities",20,0,20);
	TH1F* h1_CryMul_barrel = new TH1F("h1_CryMul_barrel","Multiplicities",20,0,20);
	TH1F* h1_CalMul_barrel = new TH1F("h1_CalMul_barrel","Multiplicities",20,0,20);
	TH1F* h2_CC_barrel = new TH1F("h2_CC_barrel","Reconstructed (Lorenzt corrected) Primary Energy (MeV)",2000,0,maxE); //Change this maximum energy
	TH1F* h3_CC_barrel = new TH1F("h3_CC_barrel","Theta residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH1F* h4_CC_barrel = new TH1F("h4_CC_barrel","Phi residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);

	
	//ENDCAP
	TH1F* h1_Cry_endcap = new TH1F("h1_Cry_endcap","Crystal ID",5850,0,5850);
	TH1F* h1_Cry_count_endcap = new TH1F("h1_Cry_count_endcap","Crystal ID_count",2,0,3840);
	TH1F* h2_Cry_endcap = new TH1F("h2_Cry_endcap","Crystal Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cry_endcap = new TH1F("h3_Cry_endcap","Crystal Type",32,0,32);
	TH1F* h4_Cry_endcap = new TH1F("h4_Cry_endcap","Crystal Copy",514,0,514);
	TH1F* h1_Cal_endcap = new TH1F("h1_Cal_endcap","Nb of Crystals in hit",20,0,20);
	TH1F* h2_Cal_endcap = new TH1F("h2_Cal_endcap","Hit Energy (MeV)",200,0,3*maxE); //Change this maximum energy
	TH1F* h3_Cal_endcap = new TH1F("h3_Cal_endcap","Hit Theta",200,0,3.2);
	TH1F* h4_Cal_endcap = new TH1F("h4_Cal_endcap","Hit Phi",200,-3.2,3.2);
	TH1F* h1_TMul_endcap = new TH1F("h1_TMul_endcap","Multiplicities",20,0,20);
	TH1F* h1_CryMul_endcap = new TH1F("h1_CryMul_endcap","Multiplicities",20,0,20);
	TH1F* h1_CalMul_endcap = new TH1F("h1_CalMul_endcap","Multiplicities",20,0,20);
	TH1F* h2_CC_endcap = new TH1F("h2_CC_endcap","Reconstructed (Lorenzt corrected) Primary Energy (MeV)",2000,0,maxE); //Change this maximum energy
	TH1F* h3_CC_endcap = new TH1F("h3_CC_endcap","Theta residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH1F* h4_CC_endcap = new TH1F("h4_CC_endcap","Phi residuals if(caloHitsPerEvent==1)",400,-0.25,0.25);

	
	//Additional user defined histos
	TH2F* h1_User = new TH2F("h1_User","Double hits, ... where?",400,-2.5,2.5,400,-2.5,2.5);

	TTree* TCrystal = (TTree*)file1->Get("cbmsim");
	
	//Crystal Hits (input)
	TClonesArray* crystalHitCA;  
	R3BCalifaCrystalCalDataSim** crystalHit;
	crystalHitCA = new TClonesArray("R3BCalifaCrystalCalDataSim",5);
	TBranch *branchCrystalHit = TCrystal->GetBranch("CalifaCrystalCalDataSim");
	branchCrystalHit->SetAddress(&crystalHitCA);

	//Calo Hits (output)
	TClonesArray* caloHitCA;  
	R3BCalifaHitDataSim** caloHit;
	caloHitCA = new TClonesArray("R3BCalifaHitDataSim",5);
	TBranch *branchCaloHit = TCrystal->GetBranch("CalifaHitDataSim");
	branchCaloHit->SetAddress(&caloHitCA);
	
	//MCTrack(input)
	TClonesArray* MCTrackCA;  
	R3BMCTrack** track;
	MCTrackCA = new TClonesArray("R3BMCTrack",5);
	TBranch *branchMCTrack = TCrystal->GetBranch("MCTrack");
	branchMCTrack->SetAddress(&MCTrackCA);
	
	Long64_t nevents = TCrystal->GetEntries();
	if(nevents!= totalEvents * multiplicity) 
		cout << "WARNING: The number of events ("<< nevents 
			<<") in the tree is not totalEvents*multiplicity ("<< totalEvents<<"*"<<multiplicity<<") !!"<<endl;

	Int_t crystalHitsPerEvent=0;
	Int_t caloHitsPerEvent=0;
	Int_t MCtracksPerEvent=0;
	Int_t crystalHitsPerEvent_barrel=0;
	Int_t crystalHitsPerEvent_endcap=0;
	Int_t caloHitsPerEvent_barrel=0;
	Int_t caloHitsPerEvent_endcap=0;
	
	Int_t primary=0;	
	Int_t primary_barrel=0;
	Int_t primary_endcap=0;

	TVector3 momentum;
	Double_t corEnergy=0;
	Int_t tirame=0;
	
	for(Int_t i=0;i<nevents;i++){

	    if(i%1000 == 0) printf("Event:%i\n",i);
				
		crystalHitCA->Clear();
		caloHitCA->Clear();
		MCTrackCA->Clear();

		TCrystal->GetEvent(i);
		
		crystalHitsPerEvent = crystalHitCA->GetEntries(); 
		caloHitsPerEvent = caloHitCA->GetEntries(); 
		MCtracksPerEvent = MCTrackCA->GetEntries();
		
		if(crystalHitsPerEvent>0) {
			crystalHit = new R3BCalifaCrystalCalDataSim*[crystalHitsPerEvent];
			for(Int_t j=0;j<crystalHitsPerEvent;j++){
				crystalHit[j] = new R3BCalifaCrystalCalDataSim;
				crystalHit[j] = (R3BCalifaCrystalCalDataSim*) crystalHitCA->At(j);      
			}
		}
		if(caloHitsPerEvent>0) {
			caloHit = new R3BCalifaHitDataSim*[caloHitsPerEvent];
			for(Int_t j=0;j<caloHitsPerEvent;j++){
				caloHit[j] = new R3BCalifaHitDataSim;
				caloHit[j] = (R3BCalifaHitDataSim*) caloHitCA->At(j);      
			}
		}		
		if(MCtracksPerEvent>0) {
			track = new R3BMCTrack*[MCtracksPerEvent];
			for(Int_t j=0;j<MCtracksPerEvent;j++){
				track[j] = new R3BMCTrack;
				track[j] = (R3BMCTrack*) MCTrackCA->At(j);      
			}
		}
				
		//loop in crystal Hits
		for(Int_t h=0;h<crystalHitsPerEvent;h++){
			h1_Cry->Fill(crystalHit[h]->GetCrystalId());
			if(crystalHit[h]->GetEnergy()*1000>threshold)
				h1_Cry_count->Fill(crystalHit[h]->GetCrystalId());
			h2_Cry->Fill(crystalHit[h]->GetEnergy()*1000);
			h3_Cry->Fill(crystalHit[h]->GetCrystalType());
			h4_Cry->Fill(crystalHit[h]->GetCrystalCopy());
			if(BARREL && crystalHit[h]->GetCrystalId()<3000){
				crystalHitsPerEvent_barrel++;
				h1_Cry_barrel->Fill(crystalHit[h]->GetCrystalId());
				if(crystalHit[h]->GetEnergy()*1000>threshold)
					h1_Cry_count_barrel->Fill(crystalHit[h]->GetCrystalId());
				h2_Cry_barrel->Fill(crystalHit[h]->GetEnergy()*1000);
				h3_Cry_barrel->Fill(crystalHit[h]->GetCrystalType());
				h4_Cry_barrel->Fill(crystalHit[h]->GetCrystalCopy());
			}
			if(ENDCAP && crystalHit[h]->GetCrystalId()>3000){
				crystalHitsPerEvent_endcap++;
				h1_Cry_endcap->Fill(crystalHit[h]->GetCrystalId());
				if(crystalHit[h]->GetEnergy()*1000>threshold)
					h1_Cry_count_endcap->Fill(crystalHit[h]->GetCrystalId());
				h2_Cry_endcap->Fill(crystalHit[h]->GetEnergy()*1000);
				h3_Cry_endcap->Fill(crystalHit[h]->GetCrystalType());
				h4_Cry_endcap->Fill(crystalHit[h]->GetCrystalCopy());				
			}
		}
		//loop in calorimeter Hits
		for(Int_t h=0;h<caloHitsPerEvent;h++){
			h1_Cal->Fill(caloHit[h]->GetNbOfCrystalHits());	
			h2_Cal->Fill(caloHit[h]->GetEnergy()*1000);	
			h3_Cal->Fill(caloHit[h]->GetTheta());	
			h4_Cal->Fill(caloHit[h]->GetPhi());
			h2_CC->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
			h2_CC2->Fill(caloHit[h]->GetTheta(),GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
			if(BARREL && caloHit[h]->GetTheta()>minThetaBarrel*TMath::Pi()/180.) {
				caloHitsPerEvent_barrel++;
				h1_Cal_barrel->Fill(caloHit[h]->GetNbOfCrystalHits());	
				h2_Cal_barrel->Fill(caloHit[h]->GetEnergy()*1000);	
				h3_Cal_barrel->Fill(caloHit[h]->GetTheta());	
				h4_Cal_barrel->Fill(caloHit[h]->GetPhi());
				h2_CC_barrel->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
			}
			if(ENDCAP && caloHit[h]->GetTheta()< maxThetaEndCap*TMath::Pi()/180.) {
				caloHitsPerEvent_endcap++;
				h1_Cal_endcap->Fill(caloHit[h]->GetNbOfCrystalHits());	
				h2_Cal_endcap->Fill(caloHit[h]->GetEnergy()*1000);	
				h3_Cal_endcap->Fill(caloHit[h]->GetTheta());	
				h4_Cal_endcap->Fill(caloHit[h]->GetPhi());
				h2_CC_endcap->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
			}
		}
		
		//loop in MC mother tracks
		for(Int_t h=0;h<MCtracksPerEvent;h++){
			if(track[h]->GetMotherId()<0){
				track[h]->GetMomentum(momentum);	
				h1_T->Fill(track[h]->GetPdgCode());
				h2_T->Fill(track[h]->GetEnergy()*1000-track[h]->GetMass()*1000);
				h3_T->Fill(momentum.Theta());
				h4_T->Fill(momentum.Phi());
				primary++;
				if(BARREL && momentum.Theta()>minThetaBarrel*TMath::Pi()/180.) primary_barrel++;
				if(ENDCAP && momentum.Theta()<maxThetaEndCap*TMath::Pi()/180.) primary_endcap++;
				if(primary==1){
					if(caloHitsPerEvent==1){  
						//only present if multiplicity=1 and it is well reconstructed (only one calorimeter Hit)
						h3_CC->Fill(momentum.Theta() - caloHit[h]->GetTheta());
						h4_CC->Fill(momentum.Phi() - caloHit[h]->GetPhi());
						h1_CC2->Fill(momentum.Theta(),crystalHitsPerEvent);
						h3_CC2->Fill(momentum.Theta(),momentum.Theta() - caloHit[h]->GetTheta());
						h4_CC2->Fill(momentum.Theta(),momentum.Phi() - caloHit[h]->GetPhi());
						h1_EF->Fill(caloHit[h]->GetTheta());
						if(BARREL && caloHit[h]->GetTheta()>minThetaBarrel*TMath::Pi()/180.){
							h3_CC_barrel->Fill(momentum.Theta() - caloHit[h]->GetTheta());
							h4_CC_barrel->Fill(momentum.Phi() - caloHit[h]->GetPhi());
						}
						if(ENDCAP && caloHit[h]->GetTheta()< maxThetaEndCap*TMath::Pi()/180.) {
							h3_CC_endcap->Fill(momentum.Theta() - caloHit[h]->GetTheta());
							h4_CC_endcap->Fill(momentum.Phi() - caloHit[h]->GetPhi());
						}
					}
					//else ... one could think in introduce some combinatorial here to see the 
					//real coincidences over the background of randoms...
				}
			}
		}

		h1_CryMul->Fill(crystalHitsPerEvent);
		h1_CalMul->Fill(caloHitsPerEvent);
		if(BARREL) h1_CryMul_barrel->Fill(crystalHitsPerEvent_barrel);
		if(BARREL) h1_CalMul_barrel->Fill(caloHitsPerEvent_barrel);
		if(ENDCAP) h1_CryMul_endcap->Fill(crystalHitsPerEvent_endcap);
		if(ENDCAP) h1_CalMul_endcap->Fill(caloHitsPerEvent_endcap);

		crystalHitsPerEvent_barrel=0;
		caloHitsPerEvent_barrel=0;
		crystalHitsPerEvent_endcap=0;
		caloHitsPerEvent_endcap=0;
										  
		h1_TMul->Fill(primary);
		if(BARREL) h1_TMul_barrel->Fill(primary_barrel);
		if(ENDCAP) h1_TMul_endcap->Fill(primary_endcap);

		primary=0;
		primary_barrel=0;
		primary_endcap=0;
		
		//User defined additions
		if(caloHitsPerEvent==2){
			h1_User->Fill(caloHit[1]->GetTheta()-caloHit[0]->GetTheta(),
						  caloHit[1]->GetPhi()-caloHit[0]->GetPhi());
		}

		if(crystalHitsPerEvent) delete[] crystalHit;
		if(caloHitsPerEvent)    delete[] caloHit;
		if(MCtracksPerEvent)    delete[] track;
	}

	TCanvas* c1 = new TCanvas("MCTrack","MCTrack",0,0,720,900);
	c1->SetFillColor(0);
	c1->SetFrameFillColor(0);
	TCanvas* c2 = new TCanvas("crystalHit","crystalHit",720,0,720,900);
	c2->SetFillColor(0);
	c2->SetFrameFillColor(0);
	TCanvas* c3 = new TCanvas("caloHit","caloHit",0,435,720,900);
	c3->SetFillColor(0);
	c3->SetFrameFillColor(0);
	TCanvas* c4 = new TCanvas("correlations","correlations",720,435,720,900);
	c4->SetFillColor(0);
	c4->SetFrameFillColor(0);
	TCanvas* c6 = new TCanvas("correlations2","correlations2",720,435,720,900);
	c6->SetFillColor(0);
	c6->SetFrameFillColor(0);
	TCanvas* c5 = new TCanvas("User","User defined",200,300,720,900);
	c5->SetFillColor(0);
	c5->SetFrameFillColor(0);
	
	TCanvas* c2_barrel;
	if(BARREL){
		c2_barrel = new TCanvas("crystalHit_barrel","crystalHit_barrel",720,0,720,900);
		c2_barrel->SetFillColor(0);
		c2_barrel->SetFrameFillColor(0);
	}
	TCanvas* c3_barrel;
	if(BARREL){
		c3_barrel = new TCanvas("caloHit_barrel","caloHit_barrel",0,435,720,900);
		c3_barrel->SetFillColor(0);
		c3_barrel->SetFrameFillColor(0);
	}
	TCanvas* c4_barrel;
	if(BARREL){
		c4_barrel = new TCanvas("correlations_barrel","correlations_barrel",720,435,720,900);
		c4_barrel->SetFillColor(0);
		c4_barrel->SetFrameFillColor(0);
	}
	TCanvas* c2_endcap;
	if(ENDCAP){
		c2_endcap = new TCanvas("crystalHit_endcap","crystalHit_endcap",720,0,720,900);
		c2_endcap->SetFillColor(0);
		c2_endcap->SetFrameFillColor(0);
	}
	TCanvas* c3_endcap;
	if(ENDCAP){
		c3_endcap = new TCanvas("caloHit_endcap","caloHit_endcap",0,435,720,900);
		c3_endcap->SetFillColor(0);
		c3_endcap->SetFrameFillColor(0);
	}
	TCanvas* c4_endcap;
	if(ENDCAP){
		c4_endcap = new TCanvas("correlations_endcap","correlations_endcap",720,435,720,900);
		c4_endcap->SetFillColor(0);
		c4_endcap->SetFrameFillColor(0);
	}
	
	//MC TRACK CANVAS
	c1->cd();
	c1->Divide(2,2);
	c1->cd(1);	h1_T->Draw();
	c1->cd(2);	h2_T->Draw();
	c1->cd(3);	h3_T->Draw();
	TLine* line1 = new TLine(0,0,0,0);
	line1->SetLineStyle(2);
	line1->Draw();
	if(BARREL){
		line1->DrawLine(minThetaBarrel*TMath::Pi()/180.,0,minThetaBarrel*TMath::Pi()/180.,10000);
		line1->DrawLine(maxThetaBarrel*TMath::Pi()/180.,0.,maxThetaBarrel*TMath::Pi()/180.,10000);
	}
	if(ENDCAP) {
		line1->DrawLine(minThetaEndCap*TMath::Pi()/180.,0.,minThetaEndCap*TMath::Pi()/180.,10000);
		line1->DrawLine(maxThetaEndCap*TMath::Pi()/180.,0.,maxThetaEndCap*TMath::Pi()/180.,10000);
	}
	c1->cd(4);	h4_T->Draw();
	c1->cd();
	c1_2 = new TPad("c1_2", "c1_2",0.,0.,0.9,0.03);	
	c1_2->Draw();
	c1_2->cd();
	TLatex *title = new TLatex(0.1,0.2,"Information from the primaries generated by the MC");
	title->SetTextSize(0.6);
	title->Draw();
	c1->cd();
	
	//CRYSTAL HIT CANVAS
	c2->cd();
	c2->Divide(2,2);
	c2->cd(1); h1_Cry->Draw();
	c2->cd(2); h2_Cry->Draw(); c2->cd(2)->SetLogy();
	c2->cd(3); h3_Cry->Draw();
	c2->cd(4); h4_Cry->Draw();
	c2->cd();
	TPad* c2_2 = new TPad("c2_2", "c2_2",0.,0.,0.9,0.03);	
	c2_2->Draw();
	c2_2->cd();
	title = new TLatex(0.1,0.2,"Crystal Hits: information in the CALIFA crystals");
	title->SetTextSize(0.6);
	title->Draw();
	c2->cd();
	
	
	//CALORIMETER HIT CANVAS	
	c3->cd();
	c3->Divide(2,2);
	c3->cd(1); h1_Cal->Draw();
	c3->cd(2); h2_Cal->Draw(); c3->cd(2)->SetLogy();
	c3->cd(3); h3_Cal->Draw();
	
	TLine* line2 = new TLine(0,0,0,0);
	line2->SetLineStyle(2);
	line2->Draw();
	if(BARREL){
		line2->DrawLine(minThetaBarrel*TMath::Pi()/180.,0,minThetaBarrel*TMath::Pi()/180.,10000);
		line2->DrawLine(maxThetaBarrel*TMath::Pi()/180.,0.,maxThetaBarrel*TMath::Pi()/180.,10000);
	}
	if(ENDCAP) {
		line2->DrawLine(minThetaEndCap*TMath::Pi()/180.,0.,minThetaEndCap*TMath::Pi()/180.,10000);
		line2->DrawLine(maxThetaEndCap*TMath::Pi()/180.,0.,maxThetaEndCap*TMath::Pi()/180.,10000);
	}
	
	c3->cd(4); h4_Cal->Draw();
	c3->cd();
	TPad* c3_2 = new TPad("c3_2", "c3_2",0.,0.,0.9,0.03);	
	c3_2->Draw();
	c3_2->cd();
	title = new TLatex(0.1,0.2,"Calorimeter Hits: reconstruction from the crystals using the HitFinder");
	title->SetTextSize(0.6);
	title->Draw();
	c3->cd();
	
	
	//CORRELATIONS CANVAS	
	c4->cd();
	c4->Divide(2,2);
	h1_CryMul->SetLineColor(kRed);
	h1_CalMul->SetLineColor(kBlue);
	c4->cd(1);   
	h1_TMul->GetYaxis()->SetRangeUser(1, 2*totalEvents*multiplicity);
	h1_TMul->Draw(); h1_CalMul->Draw("SAME"); h1_CryMul->Draw("SAME");    
	c4->cd(1)->SetLogy();

	TLatex Tl;
	Tl.SetTextSize(0.06);	Tl.DrawLatex(12, 10000, "Primary");
	Tl.SetTextSize(0.06);   Tl.SetTextColor(2);  Tl.DrawLatex(12, 3500, "Crystal");
	Tl.SetTextSize(0.06);   Tl.SetTextColor(4);  Tl.DrawLatex(12, 1000, "Calorimeter");
	
	Double_t mean = 0., sigma = 0., sigmaPol = 0.;
	Double_t mean_barrel = 0., sigma_barrel = 0.;
	Double_t mean_endcap = 0., sigma_endcap = 0.;

	c4->cd(2);
	h2_CC->Fit("gaus","","",Eproj-0.1*Eproj,Eproj+0.1*Eproj); 
	//getting the value of the fit parameters
	TF1 *myfunc = h2_CC->GetFunction("gaus");
	if(myfunc!=NULL) mean = myfunc->GetParameter(1); //value of 2st parameter (mean)
	if(myfunc!=NULL) sigma = myfunc->GetParameter(2); //value of 2st parameter (sigma)
	cout<< "TESTING mean: "<<mean <<",  sigma: "<< sigma<< endl;
	h2_CC->Draw(); 
	c4->cd(2)->SetLogy();
	c4->cd(3); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
	h3_CC->Fit("gaus","","",-0.03,0.03); 
	//getting the value of the fit parameters
	TF1 *myfuncPol = h3_CC->GetFunction("gaus");
	if(myfuncPol!=NULL) sigmaPol = myfuncPol->GetParameter(2); //value of 2st parameter (mean)
	cout<< "TESTING sigmaPol: "<< sigmaPol<< endl;
	h3_CC->Draw(); 
	c4->cd(4); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
	h4_CC->Fit("gaus","","",-0.06,0.06); 
	h4_CC->Draw(); 
	c4->cd();
	TPad* c4_2 = new TPad("c4_2", "c4_2",0.,0.,0.9,0.03);
	c4_2->Draw();
	c4_2->cd();
	title = new TLatex(0.1,0.2,"Calorimeter Hits: checking the resolution of the reconstruction");
	title->SetTextSize(0.6);
	title->Draw();
	c4->cd();
	

	//CORRELATIONS 2 CANVAS	
	c6->cd();
	c6->Divide(2,2);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	c6->cd(1);	h1_CC2->Draw("ZCOL");
	c6->cd(2);	h2_CC2->Draw("ZCOL"); c6->cd(2)->SetLogz();
	c6->cd(3);	h3_CC2->Draw("ZCOL"); c6->cd(3)->SetLogz();
	c6->cd(4);	h4_CC2->Draw("ZCOL"); c6->cd(4)->SetLogz();
	c6->cd();
	TPad* c6_2 = new TPad("c6_2", "c6_2",0.,0.,0.9,0.03);	
	c6_2->Draw();
	c6_2->cd();
	title = new TLatex(0.01,0.2,"Calorimeter Hits: checking the resolution of the reconstruction vs. the polar angle");
	title->SetTextSize(0.6);
	title->Draw();
	c6->cd();
	
	//USER CANVAS	
	c5->cd();	
	c5->Divide(2,2);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	
	c5->cd(1); h1_User->Draw();
	c5->cd(2); h3_T->Draw();
	c5->cd(3); h1_EF->Draw();
	c5->cd();

	if(BARREL) {
		//CRYSTAL HIT CANVAS -- ONLY BARREL!!!
		c2_barrel->cd();
		c2_barrel->Divide(2,2);
		c2_barrel->cd(1); h1_Cry_barrel->Draw();
		c2_barrel->cd(2); h2_Cry_barrel->Draw(); c2_barrel->cd(2)->SetLogy();
		c2_barrel->cd(3); h3_Cry_barrel->Draw();
		c2_barrel->cd(4); h4_Cry_barrel->Draw();
		c2_barrel->cd();
		TPad* c2_2_barrel = new TPad("c2_2_barrel", "c2_2_barrel",0.,0.,0.9,0.03);	
		c2_2_barrel->Draw();
		c2_2_barrel->cd();
		title = new TLatex(0.1,0.2,"Crystal Hits [BARREL]: information in the CALIFA crystals");
		title->SetTextSize(0.6);
		title->Draw();
		c2_barrel->cd();
	
	
		//CALORIMETER HIT CANVAS -- ONLY BARREL!!!	
		c3_barrel->cd();
		c3_barrel->Divide(2,2);
		c3_barrel->cd(1); h1_Cal_barrel->Draw();
		c3_barrel->cd(2); h2_Cal_barrel->Draw(); c3_barrel->cd(2)->SetLogy();
		c3_barrel->cd(3); h3_Cal_barrel->Draw();
	
		TLine* line2_barrel = new TLine(0,0,0,0);
		line2_barrel->SetLineStyle(2);
		line2_barrel->Draw();
		line2_barrel->DrawLine(minThetaBarrel*TMath::Pi()/180.,0,minThetaBarrel*TMath::Pi()/180.,10000);
		line2_barrel->DrawLine(maxThetaBarrel*TMath::Pi()/180.,0.,maxThetaBarrel*TMath::Pi()/180.,10000);
		if(ENDCAP) {
			line2_barrel->DrawLine(minThetaEndCap*TMath::Pi()/180.,0.,minThetaEndCap*TMath::Pi()/180.,10000);
			line2_barrel->DrawLine(maxThetaEndCap*TMath::Pi()/180.,0.,maxThetaEndCap*TMath::Pi()/180.,10000);
		}
	
		c3_barrel->cd(4); h4_Cal_barrel->Draw();
		c3_barrel->cd();
		TPad* c3_2_barrel = new TPad("c3_2_barrel", "c3_2_barrel",0.,0.,0.9,0.03);	
		c3_2_barrel->Draw();
		c3_2_barrel->cd();
		title = new TLatex(0.1,0.2,"Calorimeter Hits [BARREL]: reconstruction from the crystals using the HitFinder");
		title->SetTextSize(0.6);
		title->Draw();
		c3_barrel->cd();
	
	
		//CORRELATIONS CANVAS -- ONLY BARREL!!!
		c4_barrel->cd();
		c4_barrel->Divide(2,2);
		h1_CryMul_barrel->SetLineColor(kRed);
		h1_CalMul_barrel->SetLineColor(kBlue);
		c4_barrel->cd(1);   
		h1_TMul_barrel->GetYaxis()->SetRangeUser(1, 2*totalEvents*multiplicity);
		h1_TMul_barrel->Draw(); h1_CalMul_barrel->Draw("SAME"); h1_CryMul_barrel->Draw("SAME");    
		c4_barrel->cd(1)->SetLogy();
	
		TLatex Tl_barrel;
		Tl_barrel.SetTextSize(0.06);	Tl_barrel.DrawLatex(12, 10000, "Primary");
		Tl_barrel.SetTextSize(0.06);    Tl_barrel.SetTextColor(2);  Tl_barrel.DrawLatex(12, 3500, "Crystal");
		Tl_barrel.SetTextSize(0.06);    Tl_barrel.SetTextColor(4);  Tl_barrel.DrawLatex(12, 1000, "Calorimeter");
	
		c4_barrel->cd(2);
		h2_CC_barrel->Fit("gaus","","",Eproj-0.05*Eproj,Eproj+0.05*Eproj); 
		//getting the value of the fit parameters
		TF1 *myfunc_barrel = h2_CC_barrel->GetFunction("gaus");
		if(myfunc_barrel!=NULL) mean_barrel = myfunc_barrel->GetParameter(1); //value of 2st parameter (mean)
		if(myfunc_barrel!=NULL) sigma_barrel = myfunc_barrel->GetParameter(2); //value of 2st parameter (mean)
		cout<< "TESTING mean_barrel: "<<mean_barrel <<",  sigma_barrel: "<< sigma_barrel<< endl;
		h2_CC_barrel->Draw(); 
		c4_barrel->cd(2)->SetLogy();
		c4_barrel->cd(3); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
		h3_CC_barrel->Fit("gaus","","",-0.03,0.03); 
		h3_CC_barrel->Draw(); 
		c4_barrel->cd(4); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
		h4_CC_barrel->Fit("gaus","","",-0.06,0.06); 
		h4_CC_barrel->Draw(); 
		c4_barrel->cd();
		TPad* c4_2_barrel = new TPad("c4_2_barrel", "c4_2_barrel",0.,0.,0.9,0.03);	
		c4_2_barrel->Draw();
		c4_2_barrel->cd();
		title = new TLatex(0.1,0.2,"Calorimeter Hits [BARREL]: checking the resolution of the reconstruction");
		title->SetTextSize(0.6);
		title->Draw();
		c4_barrel->cd();
	}
	
	
	if(ENDCAP) {
		//CRYSTAL HIT CANVAS -- ONLY ENDCAP!!!
		c2_endcap->cd();
		c2_endcap->Divide(2,2);
		c2_endcap->cd(1); h1_Cry_endcap->Draw();
		c2_endcap->cd(2); h2_Cry_endcap->Draw(); c2_endcap->cd(2)->SetLogy();
		c2_endcap->cd(3); h3_Cry_endcap->Draw();
		c2_endcap->cd(4); h4_Cry_endcap->Draw();
		c2_endcap->cd();
		TPad* c2_2_endcap = new TPad("c2_2_endcap", "c2_2_endcap",0.,0.,0.9,0.03);	
		c2_2_endcap->Draw();
		c2_2_endcap->cd();
		title = new TLatex(0.1,0.2,"Crystal Hits [ENDCAP]: information in the CALIFA crystals");
		title->SetTextSize(0.6);
		title->Draw();
		c2_endcap->cd();
		
		
		//CALORIMETER HIT CANVAS -- ONLY ENDCAP!!!	
		c3_endcap->cd();
		c3_endcap->Divide(2,2);
		c3_endcap->cd(1); h1_Cal_endcap->Draw();
		c3_endcap->cd(2); h2_Cal_endcap->Draw(); c3_endcap->cd(2)->SetLogy();
		c3_endcap->cd(3); h3_Cal_endcap->Draw();
		
		TLine* line2_endcap = new TLine(0,0,0,0);
		line2_endcap->SetLineStyle(2);
		line2_endcap->Draw();
		line2_endcap->DrawLine(minThetaBarrel*TMath::Pi()/180.,0,minThetaBarrel*TMath::Pi()/180.,10000);
		line2_endcap->DrawLine(maxThetaBarrel*TMath::Pi()/180.,0.,maxThetaBarrel*TMath::Pi()/180.,10000);
		if(BARREL) {
			line2->DrawLine(minThetaBarrel*TMath::Pi()/180.,0,minThetaBarrel*TMath::Pi()/180.,10000);
			line2->DrawLine(maxThetaBarrel*TMath::Pi()/180.,0.,maxThetaBarrel*TMath::Pi()/180.,10000);
		}
		
		c3_endcap->cd(4); h4_Cal_endcap->Draw();
		c3_endcap->cd();
		TPad* c3_2_endcap = new TPad("c3_2_endcap", "c3_2_endcap",0.,0.,0.9,0.03);	
		c3_2_endcap->Draw();
		c3_2_endcap->cd();
		title = new TLatex(0.1,0.2,"Calorimeter Hits [ENDCAP]: reconstruction from the crystals using the HitFinder");
		title->SetTextSize(0.6);
		title->Draw();
		c3_endcap->cd();
		
		
		//CORRELATIONS CANVAS -- ONLY ENDCAP!!!
		c4_endcap->cd();
		c4_endcap->Divide(2,2);
		h1_CryMul_endcap->SetLineColor(kRed);
		h1_CalMul_endcap->SetLineColor(kBlue);
		c4_endcap->cd(1);   
		h1_TMul_endcap->GetYaxis()->SetRangeUser(1, 2*totalEvents*multiplicity);
		h1_TMul_endcap->Draw(); h1_CalMul_endcap->Draw("SAME"); h1_CryMul_endcap->Draw("SAME");    
		c4_endcap->cd(1)->SetLogy();
		
		TLatex Tl_endcap;
		Tl_endcap.SetTextSize(0.06);	Tl_endcap.DrawLatex(12, 10000, "Primary");
		Tl_endcap.SetTextSize(0.06);    Tl_endcap.SetTextColor(2);  Tl_endcap.DrawLatex(12, 3500, "Crystal");
		Tl_endcap.SetTextSize(0.06);    Tl_endcap.SetTextColor(4);  Tl_endcap.DrawLatex(12, 1000, "Calorimeter");
		
		c4_endcap->cd(2);
		h2_CC_endcap->Fit("gaus","","",Eproj-0.05*Eproj,Eproj+0.05*Eproj); 
		//getting the value of the fit parameters
		TF1 *myfunc_endcap = h2_CC_endcap->GetFunction("gaus");
		if(myfunc_endcap!=NULL) mean_endcap = myfunc_endcap->GetParameter(1); //value of 2st parameter (mean)
		if(myfunc_endcap!=NULL) sigma_endcap = myfunc_endcap->GetParameter(2); //value of 2st parameter (mean)
		cout<< "TESTING mean_endcap: "<<mean_endcap <<",  sigma_endcap: "<< sigma_endcap<< endl;
		h2_CC_endcap->Draw(); 
		c4_endcap->cd(2)->SetLogy();
		c4_endcap->cd(3); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
		h3_CC_endcap->Fit("gaus","","",-0.03,0.03); 
		h3_CC_endcap->Draw(); 
		c4_endcap->cd(4); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
		h4_CC_endcap->Fit("gaus","","",-0.06,0.06); 
		h4_CC_endcap->Draw(); 
		c4_endcap->cd();
		TPad* c4_2_endcap = new TPad("c4_2_endcap", "c4_2_endcap",0.,0.,0.9,0.03);	
		c4_2_endcap->Draw();
		c4_2_endcap->cd();
		title = new TLatex(0.1,0.2,"Calorimeter Hits[ENDCAP]: checking the resolution of the reconstruction");
		title->SetTextSize(0.6);
		title->Draw();
		c4_endcap->cd();
	}
	
	//IN CASE THE FIt IS NOT AUTOMATICALLY DONE PROPERLY...
	//SOME PREVIOUS RESULT (plot first the energy corrected and check it)!
	if(0){
		mean = 4.98171e+00;          //Mean of energy peak fit
		sigma = 9.16487e-02;         //Sigma of energy peak fit
		mean_barrel = 4.99329e+00;   //Mean of energy peak fit
		sigma_barrel = 7.58946e-02;  //Sigma of energy peak fit
		mean_endcap = 4.94582e+00;   //Mean of energy peak fit
		sigma_endcap = 1.16033e-01;  //Sigma of energy peak fit
		sigmaPol = 1.84747e-02;      //Sigma of polar angle fit
	}
	
	//DATA
	//Previous calculations
	Double_t particlesInBarrel = h3_T->Integral(minThetaBarrel,maxThetaBarrel);  //angular coverture of BARREL
	Double_t particlesInEndCap = h3_T->Integral(minThetaEndCap,maxThetaEndCap);  //angular coverture of ENDCAP

	Double_t resolution = (2.35*sigma*100)/mean;
	Int_t binLeft2Sigma = h2_CC->FindBin(mean-2*sigma);
	Int_t binRight2Sigma = h2_CC->FindBin(mean+2*sigma);
	Double_t photopeakParticles2Sigma = h2_CC->Integral(binLeft2Sigma,binRight2Sigma);
	Int_t binLeft3Sigma = h2_CC->FindBin(mean-3*sigma);
	Int_t binRight3Sigma = h2_CC->FindBin(mean+3*sigma);
	Double_t photopeakParticles3Sigma = h2_CC->Integral(binLeft3Sigma,binRight3Sigma);
	Double_t photopeakParticles2Sigma_barrel = 0., photopeakParticles2Sigma_endcap = 0.;
	Double_t photopeakParticles3Sigma_barrel = 0., photopeakParticles3Sigma_endcap = 0.;
        if(BARREL) {
		Double_t resolution_barrel = (2.35*sigma_barrel*100)/mean_barrel;
		Int_t binLeft2Sigma_barrel = h2_CC_barrel->FindBin(mean_barrel-2*sigma_barrel);
		Int_t binRight2Sigma_barrel = h2_CC_barrel->FindBin(mean_barrel+2*sigma_barrel);
		photopeakParticles2Sigma_barrel = h2_CC_barrel->Integral(binLeft2Sigma_barrel,binRight2Sigma_barrel);
		Int_t binLeft3Sigma_barrel = h2_CC_barrel->FindBin(mean_barrel-3*sigma_barrel);
		Int_t binRight3Sigma_barrel = h2_CC_barrel->FindBin(mean_barrel+3*sigma_barrel);
		photopeakParticles3Sigma_barrel = h2_CC_barrel->Integral(binLeft3Sigma_barrel,binRight3Sigma_barrel);
        }
        if(ENDCAP) {
		Double_t resolution_endcap = (2.35*sigma_endcap*100)/mean_endcap;
		Int_t binLeft2Sigma_endcap = h2_CC_endcap->FindBin(mean_endcap-2*sigma_endcap);
		Int_t binRight2Sigma_endcap = h2_CC_endcap->FindBin(mean_endcap+2*sigma_endcap);
		photopeakParticles2Sigma_endcap = h2_CC_endcap->Integral(binLeft2Sigma_endcap,binRight2Sigma_endcap);
		Int_t binLeft3Sigma_endcap = h2_CC_endcap->FindBin(mean_endcap-3*sigma_endcap);
		Int_t binRight3Sigma_endcap = h2_CC_endcap->FindBin(mean_endcap+3*sigma_endcap);
		photopeakParticles3Sigma_endcap = h2_CC_endcap->Integral(binLeft3Sigma_endcap,binRight3Sigma_endcap);
        }
	
	cout << endl << endl << "CALIFA version: " << calVersion 
	<< "      Particle energy: " << Eproj 
	<< "      Events:  " << totalEvents  << endl;	
	cout << "Multiplicity: " <<  multiplicity << " particles/event" << endl;
	cout << "Threshold: " <<  threshold << " MeV" << endl;
	cout << "Exp. resolution: " <<  ExpRes << "%" << endl;
	cout << "Particles emitted: totalEvents * multiplicity = " << totalEvents *  multiplicity << " total particles " << endl<< endl;
	cout << "Total crystals with signal: " << h1_Cry->GetEntries() << "  above threshold: " << h1_Cry_count->GetEntries() << endl; 
	cout << "Total calorimeter hits: " << h1_Cal->GetEntries() << endl << endl; 

	cout << "Mean number of crystals with signal per event: " << h1_CryMul->GetMean() << endl;
	cout << "Mean number of calorimeter hits per event: " << h1_CalMul->GetMean() << endl << endl;

	//Checking the total entries in the BARREL region HERE!!!!!!!!!!!!!!!!
	if(BARREL) {
		cout << "Particles in the Barrel = " << particlesInBarrel << " between angles " << minThetaBarrel << " and " << maxThetaBarrel << endl;
		cout << "Mean number of crystals with signal in Barrel: " << h1_CryMul_barrel->GetMean() << endl;
		cout << "Mean number of crystals with signal in Barrel per primary: " << h1_CryMul_barrel->Integral(2,50)/(Double_t)(totalEvents*multiplicity) << endl;
		cout << "Mean number of crystals with signal in Barrel per primary in Barrel: "
			<< (Double_t)h1_CryMul_barrel->GetMean()*totalEvents*multiplicity/(Double_t)particlesInBarrel << endl;
		cout << "Mean number of calorimeter hits in Barrel: " << h1_CalMul_barrel->GetMean() << endl;
		cout << "Mean number of calorimeter hits in Barrel per primary: " << h1_CalMul_barrel->Integral(2,50)/(Double_t)(totalEvents*multiplicity) << endl;
		cout << "Mean number of calorimeter hits in Barrel per primary in Barrel: "
		<< (Double_t)h1_CalMul_barrel->GetMean()*totalEvents*multiplicity/(Double_t)particlesInBarrel << endl;
		cout << "Particles in barrel / all particles: " << (Double_t)particlesInBarrel*100/(Double_t)(totalEvents*multiplicity) << "%" << endl<< endl;
	}
	if(ENDCAP){
		cout << "Particles in the EndCap = " << particlesInEndCap << " between angles " << minThetaEndCap << " and " << maxThetaEndCap << endl;
		cout << "Mean number of crystals with signal in EndCap: " << h1_CryMul_endcap->GetMean() << endl;
		cout << "Mean number of crystals with signal in EndCap per primary: " << h1_CryMul_endcap->Integral(2,50)/((Double_t)(totalEvents*multiplicity)) << endl;
		cout << "Mean number of crystals with signal in EndCap per primary in EndCap: " 
		<< (Double_t)h1_CryMul_endcap->GetMean()*totalEvents*multiplicity/(Double_t)particlesInEndCap << endl;
		cout << "Mean number of calorimeter hits in EndCap: " << h1_CalMul_endcap->GetMean() << endl;
		cout << "Mean number of calorimeter hits in EndCap per primary: " << h1_CalMul_endcap->Integral(2,50)/(Double_t)(totalEvents*multiplicity) << endl;
		cout << "Mean number of calorimeter hits in EndCap per primary in EndCap: "
		<< (Double_t)h1_CalMul_endcap->GetMean()*(Double_t)totalEvents*multiplicity/(Double_t)particlesInEndCap << endl;
		cout << "Particles in endcap / all particles: " << (Double_t)particlesInEndCap*100/(Double_t)(totalEvents*multiplicity) << "%" << endl<< endl;		
	}
	cout << "Photopeak efficiency studies"<< endl;
	cout << "Number of particles in the photopeak +- two sigmas (95.44%): " << photopeakParticles2Sigma << endl;     
	cout << "Number of particles in the photopeak +- two sigmas (99.74%): " << photopeakParticles3Sigma << endl;
	cout << "Photopeak efficiency: Np(2sigma)/particles (%) = " << (Double_t)photopeakParticles2Sigma*100/(Double_t)(totalEvents*multiplicity) << endl ;
	cout << "Photopeak efficiency: Np(3sigma)/particles (%) = " << (Double_t)photopeakParticles3Sigma*100/(Double_t)(totalEvents*multiplicity) << endl ;
	if(BARREL) cout << "Photopeak efficiency: Np(2sigma)/particlesInBarrel (%) = " << (Double_t)photopeakParticles2Sigma*100/(Double_t)particlesInBarrel << endl ;
	if(BARREL) cout << "Photopeak efficiency: Np(3sigma)/particlesInBarrel (%) = " << (Double_t)photopeakParticles3Sigma*100/(Double_t)particlesInBarrel << endl << endl << endl ;
	if(ENDCAP) cout << "Photopeak efficiency: Np(2sigma)/particlesInEndCap (%) = " << (Double_t)photopeakParticles2Sigma*100/(Double_t)particlesInEndCap << endl ;
	if(ENDCAP) cout << "Photopeak efficiency: Np(3sigma)/particlesInEndCap (%) = " << (Double_t)photopeakParticles3Sigma*100/(Double_t)particlesInEndCap << endl << endl << endl ;
	
	TCanvas *ctext = new TCanvas("ctext","Texto",0,0,600,800);
	ctext->SetFrameFillColor(0);
   	ctext->SetFillColor(0);

 	char oT1[250];char oT2[250];char oT3[250];char oT4[250];char oT5[250];char oT6[250];char oT99[250];
	char oT7[250];char oT8[250];char oT9[250];char oT10[250];char oT11[250];char oT12[250]; 
	char oT13[250];char oT14[250];char oT15[250];char oT16[250];char oT17[250];char oT18[250];char oT19[250];
	char oT20[250];char oT21[250];char oT22[250];char oT23[250];char oT24[250];char oT25[250];char oT26[250];
	char oT27[250];	char oT28[250];	char oT29[250];	char oT30[250];	char oT31[250];	char oT32[250];
	char oT33[250];	char oT34[250];	char oT35[250];	char oT36[250];	char oT37[250];	char oT38[250];
	
  	sprintf(oT1,"%s%s%s%f%s%i","CALIFA version: ",calVersion,";  Particle energy: ",Eproj," MeV;  Events: ",totalEvents);  	
	sprintf(oT2,"%s%i%s%f%s%i%s","Multiplicity: ",multiplicity,";   Threshold: ",threshold," MeV;    Exp. resolution: ", ExpRes,"%");
  	sprintf(oT3,"%s%i%s","Particles emitted: totalEvents * multiplicity = ",totalEvents *  multiplicity," total particles ");
	sprintf(oT4,"%s%f%s%f","Total crystals with signal: ",h1_Cry->GetEntries(), "  above threshold: ",h1_Cry_count->GetEntries()); 
	sprintf(oT5,"%s%f","Total calorimeter hits: ",h1_Cal->GetEntries()); 
  	sprintf(oT6,"%s%f","Mean number of crystals with signal per event: ",h1_CryMul->GetMean());
	sprintf(oT7,"%s%f","Mean number of calorimeter hits per event: ",h1_CalMul->GetMean());
	
	if(BARREL)sprintf(oT8,"%s%f%s%f%s%f","Particles in the Barrel: ",particlesInBarrel," between angles ",minThetaBarrel," and ",maxThetaBarrel);
	if(BARREL)sprintf(oT9,"%s%f","Mean number of crystals with signal in Barrel: ",h1_CryMul_barrel->GetMean());
  	if(BARREL)sprintf(oT10,"%s%f","Mean number of crystals with signal in Barrel per primary: ",(Double_t)h1_CryMul_barrel->Integral(2,50)/(Double_t)(totalEvents*multiplicity));
  	if(BARREL)sprintf(oT11,"%s%f","Mean number of crystals with signal in Barrel per primary in Barrel: ",(Double_t)h1_CryMul_barrel->GetMean()*totalEvents*multiplicity/(Double_t)particlesInBarrel);
	if(BARREL)sprintf(oT12,"%s%f","Mean number of calorimeter hits in Barrel: ",h1_CalMul_barrel->GetMean());
  	if(BARREL)sprintf(oT13,"%s%f","Mean number of calorimeter hits in Barrel per primary: ",(Double_t)h1_CalMul_barrel->Integral(2,50)/(Double_t)(totalEvents*multiplicity));
  	if(BARREL)sprintf(oT14,"%s%f","Mean number of calorimeter hits in Barrel per primary in Barrel: ",(Double_t)h1_CalMul_barrel->GetMean()*totalEvents*multiplicity/(Double_t)particlesInBarrel);
	if(BARREL)sprintf(oT15,"%s%f","Particles in barrel / all particles (%): ",(Double_t)particlesInBarrel*100/(Double_t)(totalEvents*multiplicity));

	if(ENDCAP)sprintf(oT16,"%s%f%s%f%s%f","Particles in the Endcap: ",particlesInEndCap," between angles ",minThetaEndCap," and ",maxThetaEndCap);
	if(ENDCAP)sprintf(oT17,"%s%f","Mean number of crystals with signal in Endcap: ",h1_CryMul_endcap->GetMean());
  	if(ENDCAP)sprintf(oT18,"%s%f","Mean number of crystals with signal in Endcap per primary: ",(Double_t)h1_CryMul_endcap->Integral(2,50)/(Double_t)(totalEvents*multiplicity));
  	if(ENDCAP)sprintf(oT19,"%s%f","Mean number of crystals with signal in Endcap per primary in Endcap: ",(Double_t)h1_CryMul_endcap->GetMean()*totalEvents*multiplicity/(Double_t)particlesInEndCap);
	if(ENDCAP)sprintf(oT20,"%s%f","Mean number of calorimeter hits in Endcap: ",h1_CalMul_endcap->GetMean());
  	if(ENDCAP)sprintf(oT21,"%s%f","Mean number of calorimeter hits in Endcap per primary: ",(Double_t)h1_CalMul_endcap->Integral(2,50)/(Double_t)(totalEvents*multiplicity));
  	if(ENDCAP)sprintf(oT22,"%s%f","Mean number of calorimeter hits in Endcap per primary in Endcap: ",(Double_t)h1_CalMul_endcap->GetMean()*totalEvents*multiplicity/(Double_t)particlesInEndCap);
	if(ENDCAP)sprintf(oT23,"%s%f","Particles in endcap / all particles (%): ",(Double_t)particlesInEndCap*100/(Double_t)(totalEvents*multiplicity));
	
	
	sprintf(oT24,"%s","Photopeak efficiencies"); 
  	sprintf(oT25,"%s%f","Photopeak entries (+-2 sigmas, 95.44%): ",photopeakParticles2Sigma); 
   	sprintf(oT26,"%s%f","Photopeak entries/particles (%): ",(Double_t)photopeakParticles2Sigma*100/(Double_t)(totalEvents*multiplicity));

	if(BARREL)sprintf(oT27,"%s","Photopeak efficiencies (BARREL)"); 
  	if(BARREL)sprintf(oT28,"%s%f","Photopeak entries (+-2 sigmas, 95.44%): ",photopeakParticles2Sigma_barrel); 
   	if(BARREL)sprintf(oT29,"%s%f","Photopeak entries/particles (%): ",photopeakParticles2Sigma_barrel*100/(Double_t)(totalEvents*multiplicity));
	
	if(ENDCAP)sprintf(oT30,"%s","Photopeak efficiencies (ENDCAP)"); 
  	if(ENDCAP)sprintf(oT31,"%s%f","Photopeak entries (+-2 sigmas 95.44%): ",photopeakParticles2Sigma_endcap); 
   	if(ENDCAP)sprintf(oT32,"%s%f","Photopeak entries/particles (%): ",(Double_t)photopeakParticles2Sigma_endcap*100/(Double_t)(totalEvents*multiplicity));
	
	sprintf(oT33,"%s","Photopeak and angle resolution"); 
  	sprintf(oT34,"%s%f","FWHM (%): ",sigma*235./mean);
	if(BARREL)sprintf(oT35,"%s%f","FWHM BARREL (%): ",sigma_barrel*235./mean_barrel);
	if(ENDCAP)sprintf(oT36,"%s%f","FWHM ENDCAP (%): ",sigma_endcap*235./mean_endcap);
	sprintf(oT37,"%s%f%s%f","Polar angle resolution (sigma in rads, degs): ",sigmaPol,", ",sigmaPol*180/3.141597 ); 
	sprintf(oT38,"%s","Hit finder sum over all calorimeter crystals."); 

  	TLatex l;
  	l.SetTextSize(0.021);
  	l.DrawLatex(0.08,0.95,oT1);
  	l.DrawLatex(0.08,0.92,oT2);
	
  	l.DrawLatex(0.08,0.89,oT3);
  	if(BARREL)l.DrawLatex(0.08,0.85,oT4);
	if(ENDCAP)l.DrawLatex(0.08,0.82,oT5);
	l.DrawLatex(0.08,0.79,oT6);
	l.DrawLatex(0.08,0.76,oT7);

  	if(BARREL)l.DrawLatex(0.08,0.71,oT8);
  	if(BARREL)l.DrawLatex(0.08,0.69,oT9);
  	if(BARREL)l.DrawLatex(0.08,0.67,oT10);
  	if(BARREL)l.DrawLatex(0.08,0.65,oT11);
  	if(BARREL)l.DrawLatex(0.08,0.63,oT12);
	if(BARREL)l.DrawLatex(0.08,0.61,oT13);
	if(BARREL)l.DrawLatex(0.08,0.59,oT14);
	if(BARREL)l.DrawLatex(0.08,0.57,oT15);

  	if(ENDCAP)l.DrawLatex(0.08,0.54,oT16);
  	if(ENDCAP)l.DrawLatex(0.08,0.52,oT17);
  	if(ENDCAP)l.DrawLatex(0.08,0.50,oT18);
  	if(ENDCAP)l.DrawLatex(0.08,0.48,oT19);
  	if(ENDCAP)l.DrawLatex(0.08,0.46,oT20);
	if(ENDCAP)l.DrawLatex(0.08,0.44,oT21);
	if(ENDCAP)l.DrawLatex(0.08,0.42,oT22);
	if(ENDCAP)l.DrawLatex(0.08,0.40,oT23);
	
	l.DrawLatex(0.08,0.36,oT24);
	l.DrawLatex(0.08,0.34,oT25);
	l.DrawLatex(0.08,0.32,oT26);
	
	if(BARREL)l.DrawLatex(0.08,0.29,oT27);
	if(BARREL)l.DrawLatex(0.08,0.27,oT28);
	if(BARREL)l.DrawLatex(0.08,0.25,oT29);
	
	if(ENDCAP)l.DrawLatex(0.08,0.22,oT30);
	if(ENDCAP)l.DrawLatex(0.08,0.20,oT31);
	if(ENDCAP)l.DrawLatex(0.08,0.18,oT32);
	
	l.DrawLatex(0.08,0.15,oT33);
	l.DrawLatex(0.08,0.13,oT34);
	if(BARREL)l.DrawLatex(0.08,0.11,oT35);
	if(ENDCAP)l.DrawLatex(0.08,0.09,oT36);
	l.DrawLatex(0.08,0.07,oT37);
	l.DrawLatex(0.08,0.05,oT38);
	
	l.SetTextSize(0.018);
	l.DrawLatex(0.08,0.01,title1);

  	ctext->Draw();
  
	ctext->Modified();
	ctext->cd();

 	//OUTPUT FILE
  	ctext->Print("output.ps("); 
	c1->Print("output.ps"); 	
  	c2->Print("output.ps");
	if(BARREL)c2_barrel->Print("output.ps");
	if(ENDCAP)c2_endcap->Print("output.ps");
  	c3->Print("output.ps");
	if(BARREL)c3_barrel->Print("output.ps");
	if(ENDCAP)c3_endcap->Print("output.ps");
  	c4->Print("output.ps");
	if(BARREL)c4_barrel->Print("output.ps");
	if(ENDCAP)c4_endcap->Print("output.ps");
  	c6->Print("output.ps)");

}
