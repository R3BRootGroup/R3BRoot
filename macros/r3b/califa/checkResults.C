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
//     BUT FIRST, select in the //SETTINGS section the simulation features 
//	(the macro will plot and text information as a function of these settings)
//  -------------------------------------------------------------------------

void checkResults() {
	char title0[250];
	char title1[250];
	//SETTINGS 
	Double_t calVersion = 5.0;       //Calorimeter version (=7.05 or 7.07)
	Double_t Eproj = 2.0;              //Gamma Energy in projectile frame in MeV 
	Int_t totalEvents = 100000;        //Events
	Int_t multiplicity = 1;           //Multiplicity (particles per event)
	
	Double_t threshold=0.050;		  //Threshold in MeV
	Int_t ExpRes=5;			          //Exp. Resol in MeV

	//SOME PREVIOUS RESULT (plot first the energy corrected and check it)!
	Double_t mean = 1.98519e+02;       //Mean of energy peak fit
	Double_t sigma = 1.36854e+00;      //Sigma of energy peak fit
	Double_t sigmaPol = 1.84747e-02;   //Sigma of polar angle fit
	
	//FOR THE HISTOGRAMS AND PLOTS:
	Double_t maxE = 2.3;               //Maximum energy in MeV in the histos
	
	sprintf(title0,"%s","/Users/hapol/r3broot/macros/r3b/califa/califaAna.root");  	
 	sprintf(title1,"%s","/Users/hapol/r3broot/macros/r3b/califa/r3bsim.root");  	
	TFile *file0 = TFile::Open(title0);
	TFile *file1 = TFile::Open(title1);
	
	Double_t beta=0.82;
	
	//END OF THE SETTING AREA
	 
	Double_t minThetaBarrel=0 , maxThetaBarrel=0;
	if(calVersion==7.05){
		minThetaBarrel= 32.9;    //Angular coverture of BARREL 7.05
		maxThetaBarrel= 134.7;   //Angular coverture of BARREL 7.05
	}
	else if(calVersion==7.07){
		minThetaBarrel= 32.4;  //Angular coverture of BARREL 7.07
		maxThetaBarrel= 134.2; //Angular coverture of BARREL 7.07
	}
	else{
		minThetaBarrel= 35;  //??
		maxThetaBarrel= 130; //??
	}
	
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
	TH1F* h2_CC = new TH1F("h2_CC","Reconstructed (Lorenzt corrected) Primary Energy (MeV)",2000,0,maxE); //Change this maximum energy
	TH1F* h3_CC = new TH1F("h3_CC","Theta resolution if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH1F* h4_CC = new TH1F("h4_CC","Phi resolution if(caloHitsPerEvent==1)",400,-0.25,0.25);
	TH2F* h1_CC2 = new TH2F("h1_CC2","Multiplicity vs. polar angle",200,0,3.2,36,0,12);
	TH2F* h2_CC2 = new TH2F("h2_CC2","Energy reconstructed vs. polar angle",50,0,3.2,400,0,maxE); //Change this maximum energy
	TH2F* h3_CC2 = new TH2F("h3_CC2","Theta resolution vs. polar angle",100,0,3.2,100,-0.25,0.25);
	TH2F* h4_CC2 = new TH2F("h4_CC2","Phi resolution vs. polar angle",100,0,3.2,100,-0.25,0.25);

	TH1F* h1_EF = new TH1F("h1_EF","Efficiency vs. polar angle",200,0,3.2);
	//TH1F* h2_EF = new TH1F("h2_EF","Eventos vs. polar angle",1000,0,3.2);

	//Additional user defined histos
	TH2F* h1_User = new TH2F("h1_User","Double hits, ... where?",400,-2.5,2.5,400,-2.5,2.5);

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
	if(nevents!= totalEvents * multiplicity) 
		cout << "WARNING: The number of events ("<< nevents 
			<<") in the tree is not totalEvents*multiplicity ("<< totalEvents<<"*"<<multiplicity<<") !!"<<endl;

	Int_t crystalHitsPerEvent=0;
	Int_t caloHitsPerEvent=0;
	Int_t MCtracksPerEvent=0;
	Int_t primary=0;
	TVector3 momentum;
	Double_t corEnergy=0;
	Int_t tirame=0;
	
	for(Int_t i=0;i<nevents;i++){
	    if(i%1000 == 0) printf("Event:%i\n",i);
				
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
				
		//loop in crystal Hits
		for(Int_t h=0;h<crystalHitsPerEvent;h++){
			h1_Cry->Fill(crystalHit[h]->GetCrystalId());
			if(crystalHit[h]->GetEnergy()*1000>threshold)
				h1_Cry_count->Fill(crystalHit[h]->GetCrystalId());
			h2_Cry->Fill(crystalHit[h]->GetEnergy()*1000);
			h3_Cry->Fill(crystalHit[h]->GetCrystalType());
			h4_Cry->Fill(crystalHit[h]->GetCrystalCopy());
		}
		//loop in calorimeter Hits
		for(Int_t h=0;h<caloHitsPerEvent;h++){
			h1_Cal->Fill(caloHit[h]->GetNbOfCrystalHits());	
			h2_Cal->Fill(caloHit[h]->GetEnergy()*1000);	
			h3_Cal->Fill(caloHit[h]->GetTheta());	
			h4_Cal->Fill(caloHit[h]->GetPhi());
			h2_CC->Fill(GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
			h2_CC2->Fill(caloHit[h]->GetTheta(),GetCMEnergy(caloHit[h]->GetTheta(),caloHit[h]->GetEnergy()*1000,beta));
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
				if(primary==1){
					if(caloHitsPerEvent==1){  
						//only present if multiplicity=1 and it is well reconstructed (only one calorimeter Hit)
						h3_CC->Fill(momentum.Theta() - caloHit[h]->GetTheta());
						h4_CC->Fill(momentum.Phi() - caloHit[h]->GetPhi());
						h1_CC2->Fill(momentum.Theta(),crystalHitsPerEvent);
						h3_CC2->Fill(momentum.Theta(),momentum.Theta() - caloHit[h]->GetTheta());
						h4_CC2->Fill(momentum.Theta(),momentum.Phi() - caloHit[h]->GetPhi());
						h1_EF->Fill(caloHit[h]->GetTheta());
					}
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

	
	//MC TRACK CANVAS
	c1->cd();
	c1->Divide(2,2);
	c1->cd(1);	h1_T->Draw();   MCTrack_1->SetFrameBorderMode(0);
	c1->cd(2);	h2_T->Draw();	MCTrack_2->SetFrameBorderMode(0);
	c1->cd(3);	h3_T->Draw();	MCTrack_3->SetFrameBorderMode(0);
	TLine* line1 = new TLine(minThetaBarrel*TMath::Pi()/180.,0,minThetaBarrel*TMath::Pi()/180.,10000);
	line1->SetLineStyle(2);
	line1->Draw();
	line1->DrawLine(maxThetaBarrel*TMath::Pi()/180.,0.,maxThetaBarrel*TMath::Pi()/180.,10000);
	c1->cd(4);	h4_T->Draw();	MCTrack_4->SetFrameBorderMode(0);
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
	c2->cd(1); h1_Cry->Draw();	crystalHit_1->SetFrameBorderMode(0);
	c2->cd(2); h2_Cry->Draw();	crystalHit_2->SetFrameBorderMode(0);	crystalHit_2->SetLogy();
	c2->cd(3); h3_Cry->Draw();	crystalHit_3->SetFrameBorderMode(0);
	c2->cd(4); h4_Cry->Draw();	crystalHit_4->SetFrameBorderMode(0);
	c2->cd();
	TPad* c2_2 = new TPad("c2_2", "c2_2",0.,0.,0.9,0.03);	
	c2_2->Draw();
	c2_2->cd();
	title = new TLatex(0.1,0.2,"Crystal Hits: information in the CALIFA crystals (only BARREL)");
	title->SetTextSize(0.6);
	title->Draw();
	c2->cd();
	
	
	//CALORIMETER HIT CANVAS	
	c3->cd();
	c3->Divide(2,2);
	c3->cd(1); h1_Cal->Draw();	caloHit_1->SetFrameBorderMode(0);
	c3->cd(2); h2_Cal->Draw();	caloHit_2->SetFrameBorderMode(0);	caloHit_2->SetLogy();
	c3->cd(3); h3_Cal->Draw();	caloHit_3->SetFrameBorderMode(0);
	TLine* line2 = new TLine(minThetaBarrel*TMath::Pi()/180.,0,minThetaBarrel*TMath::Pi()/180.,10000);
	line2->SetLineStyle(2);
	line2->Draw();
	line2->DrawLine(maxThetaBarrel*TMath::Pi()/180.,0.,maxThetaBarrel*TMath::Pi()/180.,10000);
	c3->cd(4); h4_Cal->Draw();	caloHit_4->SetFrameBorderMode(0);
	c3->cd();
	TPad* c3_2 = new TPad("c3_2", "c3_2",0.,0.,0.9,0.03);	
	c3_2->Draw();
	c3_2->cd();
	title = new TLatex(0.1,0.2,"Calorimeter Hits: reconstruction from the crystals using the HitFinder (BARREL)");
	title->SetTextSize(0.6);
	title->Draw();
	c3->cd();
	
	
	//CORRELATIONS CANVAS	
	c4->cd();
	c4->Divide(2,2);
	h1_CryMul->SetLineColor(kRed);
	h1_CalMul->SetLineColor(kBlue);
	c4->cd(1);   
	h1_TMul->Draw(); h1_CalMul->Draw("SAME"); h1_CryMul->Draw("SAME");    
	correlations_1->SetFrameBorderMode(0);	correlations_1->SetLogy();

	TLatex Tl;
	Tl.SetTextSize(0.06);	Tl.DrawLatex(12, 10000, "Primary");
	Tl.SetTextSize(0.06);   Tl.SetTextColor(2);  Tl.DrawLatex(12, 3500, "Crystal");
	Tl.SetTextSize(0.06);   Tl.SetTextColor(4);  Tl.DrawLatex(12, 1000, "Calorimeter");
	
	c4->cd(2);
	h2_CC->Fit("gaus","","",Eproj-0.05*Eproj,Eproj+0.05*Eproj); 
	h2_CC->Draw(); 
	correlations_2->SetFrameBorderMode(0); correlations_2->SetLogy();
	c4->cd(3); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
	h3_CC->Fit("gaus","","",-0.03,0.03); 
	h3_CC->Draw(); 
	correlations_3->SetFrameBorderMode(0);
	c4->cd(4); gStyle->SetOptStat(1); gStyle->SetOptFit(1); 
	h4_CC->Fit("gaus","","",-0.06,0.06); 
	h4_CC->Draw(); 
	correlations_4->SetFrameBorderMode(0); 
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
	c6->cd(1);	correlations2_1->SetFrameBorderMode(0);	h1_CC2->Draw("ZCOL");
	c6->cd(2);	correlations2_2->SetFrameBorderMode(0);	h2_CC2->Draw("ZCOL"); correlations2_2->SetLogz();
	c6->cd(3);	correlations2_3->SetFrameBorderMode(0); h3_CC2->Draw("ZCOL"); correlations2_3->SetLogz();
	c6->cd(4);	correlations2_4->SetFrameBorderMode(0); h4_CC2->Draw("ZCOL"); correlations2_4->SetLogz();
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
	
	c5->cd(1);	User_1->SetFrameBorderMode(0); h1_User->Draw();
	c5->cd(2);	User_2->SetFrameBorderMode(0); h3_T->Draw();h1_EF->Draw("same");
	c5->cd(3);	User_3->SetFrameBorderMode(0); h1_EF->Draw();
	c5->cd(4);	User_4->SetFrameBorderMode(0); //h2_EF->Draw();
	c5->cd();

	
	//DATA
	//Previous calculations
	Int_t particlesInBarrel = h3_T->Integral(minThetaBarrel,maxThetaBarrel);  //angular coverture of BARREL

	Double_t resolution = (2.35*sigma*100)/mean;
	Int_t binLeft2Sigma = h2_CC->FindBin(mean-2*sigma);
	Int_t binRight2Sigma = h2_CC->FindBin(mean+2*sigma);
	Int_t photopeakParticles2Sigma =h2_CC->Integral(binLeft2Sigma,binRight2Sigma);
	Int_t binLeft3Sigma = h2_CC->FindBin(mean-3*sigma);
	Int_t binRight3Sigma = h2_CC->FindBin(mean+3*sigma);
	Int_t photopeakParticles3Sigma = h2_CC->Integral(binLeft3Sigma,binRight3Sigma);
	
	//Checking the total entries in the BARREL region
	cout << endl << endl 
		 << "CALIFA version: " << calVersion 
	<< "      Particle energy: " << Eproj 
	<< "      Events:  " << totalEvents  << endl;	
	cout << "Multiplicity: " <<  multiplicity << " particles/event" << endl;
	cout << "Particles emitted: totalEvents * multiplicity = " << totalEvents *  multiplicity << " total particles " << endl;
	cout << "Particles in the Barrel = " << particlesInBarrel << " between angles " << minThetaBarrel << " and " << maxThetaBarrel << endl;
	cout << "Mean number of crystals with signal per event: " << h1_CryMul->GetMean() << endl;
	cout << "Mean number of crystals with signal per particle in Barrel: " << h1_CryMul->GetMean()*totalEvents/particlesInBarrel<< endl;
	cout << "Mean number of calorimeter hits per event: " << h1_CalMul->GetMean() << endl;
	cout << "Mean number of calorimeter hits per particle in Barrel: " << h1_CalMul->GetMean()*totalEvents/particlesInBarrel << endl;
  	cout << "Particles in barrel / all particles (%): " << (Double_t)particlesInBarrel*100/(Double_t)(totalEvents*multiplicity) << endl<< endl;

	cout << "Photopeak efficiency studies"<< endl;
	cout << "Number of particles in the photopeak +- two sigmas (95.44%): " << photopeakParticles2Sigma << endl;     
	cout << "Number of particles in the photopeak +- two sigmas (99.74%): " << photopeakParticles3Sigma << endl;
	cout << "Photopeak efficiency: Np(2sigma)/particles (%) = " << (Double_t)photopeakParticles2Sigma*100/(Double_t)(totalEvents*multiplicity) << endl ;
	cout << "Photopeak efficiency: Np(3sigma)/particles (%) = " << (Double_t)photopeakParticles3Sigma*100/(Double_t)(totalEvents*multiplicity) << endl ;
	cout << "Photopeak efficiency: Np(2sigma)/particlesInBarrel (%) = " << (Double_t)photopeakParticles2Sigma*100/(Double_t)particlesInBarrel << endl ;
	cout << "Photopeak efficiency: Np(3sigma)/particlesInBarrel (%) = " << (Double_t)photopeakParticles3Sigma*100/(Double_t)particlesInBarrel << endl << endl << endl ;

	TCanvas *ctext = new TCanvas("ctext","Texto",0,0,600,800);
	ctext->SetFrameFillColor(0);
   	ctext->SetFillColor(0);

 	char oT1[250];char oT2[250];char oT3[250];char oT4[250];char oT5[250];char oT6[250];char oT99[250];
	char oT7[250];char oT8[250];char oT9[250];char oT10[250];char oT11[250];char oT12[250]; //output Text
	char oT13[250];char oT14[250];char oT15[250];char oT16[250];char oT17[250];char oT18[250];char oT19[250];
	char oT20[250];char oT21[250];char oT22[250];char oT23[250];char oT24[250];char oT25[250];char oT26[250];
	
  	sprintf(oT1,"%s%f%s%f%s%i","CALIFA version: ",calVersion,";  Particle energy: ",Eproj," MeV;  Events: ",totalEvents);  	
	sprintf(oT2,"%s%i%s%f%s%i%s","Multiplicity: ",multiplicity,".   Threshold: ",threshold," MeV.    Exp. resolution: ", ExpRes,"%");
  	sprintf(oT3,"%s%i%s","Particles emitted: totalEvents * multiplicity = ",totalEvents *  multiplicity," total particles ");
  	sprintf(oT4,"%s%i%s%f%s%f","Particles in the Barrel: ",particlesInBarrel," between angles ",minThetaBarrel," and ",maxThetaBarrel);
  	sprintf(oT5,"%s%f","Mean number of crystals with signal per event: ",h1_CryMul->GetMean());
  	sprintf(oT6,"%s%f","Mean number of crystals with signal per particle in Barrel: ",h1_CryMul->GetMean()*totalEvents*multiplicity/particlesInBarrel);
  	sprintf(oT7,"%s%f","Mean number of Barrel hits per event : ",h1_CalMul->GetMean());
  	sprintf(oT8,"%s%f","Mean number of Barrel hits per particle in Barrel: ",h1_CalMul->GetMean()*totalEvents*multiplicity/particlesInBarrel);
  	sprintf(oT9,"%s%f","Particles in Barrel / all particles (%): ",(Double_t)particlesInBarrel*100/(Double_t)(totalEvents*multiplicity));
  	sprintf(oT99,"%s%f","Barrel hits / Barrel particles (%): ",(Double_t)h1_Cal->GetEntries()*100/(Double_t)particlesInBarrel);
	sprintf(oT10,"%s","Photopeak efficiencies"); 
  	//sprintf(oT11,"%s%i","Particles in the photopeak +- two sigmas (95.44%): ",photopeakParticles2Sigma); 
   	sprintf(oT12,"%s%i","Photopeak entries (+-3 sigmas, 99.74%): ",photopeakParticles3Sigma); 
   	//sprintf(oT13,"%s%f","Np(2sigma)/particles (%): ",(Double_t)photopeakParticles2Sigma*100/(Double_t)(totalEvents*multiplicity));
	sprintf(oT14,"%s%f","Photopeak entries/particles (%): ",(Double_t)photopeakParticles3Sigma*100/(Double_t)(totalEvents*multiplicity));
	//sprintf(oT15,"%s%f","Np(2sigma)/particlesInBarrel (%): ",(Double_t)photopeakParticles2Sigma*100/(Double_t)particlesInBarrel);
	sprintf(oT16,"%s%f","Photopeak entries/particlesInBarrel (%): ",(Double_t)photopeakParticles3Sigma*100/(Double_t)particlesInBarrel);
	sprintf(oT17,"%s","Photopeak and angle resolution"); 
  	sprintf(oT18,"%s%f","FWHM (%): ",sigma*235./mean); 
	sprintf(oT19,"%s%f%s%f","Polar angle resolution (sigma in rads, degs): ",sigmaPol,", ",sigmaPol*180/3.141597 ); 
	sprintf(oT20,"%s%i%s%i","Total crystals with signal: ",h1_Cry->GetEntries(), "  above threshold: ",h1_Cry_count->GetEntries()); 
	sprintf(oT21,"%s%i","Total calorimeter hits: ",h1_Cal->GetEntries()); 
	//sprintf(oT22,"%s","Hit finder sum over a window of 0.25rad (14.3deg) around each hit with large E."); 
	sprintf(oT22,"%s","Hit finder sum over all calorimeter crystals."); 

  	TLatex l;
  	l.SetTextSize(0.025);
  	l.DrawLatex(0.1,0.92,oT1);
  	l.DrawLatex(0.1,0.89,oT2);
  	l.DrawLatex(0.1,0.86,oT3);
  	l.DrawLatex(0.1,0.82,oT4);
	
  	l.DrawLatex(0.1,0.68,oT5);
  	l.DrawLatex(0.1,0.65,oT6);
  	l.DrawLatex(0.1,0.62,oT7);
  	l.DrawLatex(0.1,0.59,oT8);
  	l.DrawLatex(0.1,0.56,oT9);
	l.DrawLatex(0.1,0.53,oT99);

  	l.DrawLatex(0.1,0.47,oT10);
  	//l.DrawLatex(0.1,0.47,oT11);
  	l.DrawLatex(0.1,0.44,oT12);
  	//l.DrawLatex(0.1,0.41,oT13);
  	l.DrawLatex(0.1,0.41,oT14);
	//l.DrawLatex(0.1,0.35,oT15);
	l.DrawLatex(0.1,0.38,oT16);
	l.DrawLatex(0.1,0.30,oT17);
	l.DrawLatex(0.1,0.27,oT18);
	l.DrawLatex(0.1,0.24,oT19);
	l.DrawLatex(0.1,0.79,oT20);
	l.DrawLatex(0.1,0.76,oT21);
	l.DrawLatex(0.1,0.14,oT22);

	l.DrawLatex(0.1,0.10,title0);
	l.DrawLatex(0.1,0.07,title1);

  	//ctext->Print("text.gif");
  	//ctext->Print();
  	ctext->Draw();
  
	ctext->Modified();
	ctext->cd();

 	//OUTPUT FILE
  	ctext.Print("output.ps("); 
	c1.Print("output.ps"); 	
  	c2.Print("output.ps"); 
  	c3.Print("output.ps");
  	c4.Print("output.ps");
  	c6.Print("output.ps)");

  	/*ctext.Print("out_10MeV_version7_05_multiplicity1.pdf("); 
	c1.Print("out_10MeV_version7_05_multiplicity1.pdf"); 	
  	c2.Print("out_10MeV_version7_05_multiplicity1.pdf"); 
  	c3.Print("out_10MeV_version7_05_multiplicity1.pdf");
  	c4.Print("out_10MeV_version7_05_multiplicity1.pdf)");*/
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
		
			
