//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 01/09/10
//         Comments:
//			Runs the CALIFA Hit Finder. Outputs a root file with 
//			a collection (TClonesArray) of R3BCaloHits
//
//  -------------------------------------------------------------------------
//
//   Usage: 
//        > root -l califaAna.C
//                         
// Comments:In this case the macro is prepared for gamma radiation in phoswich-endcap IEM-CSIC Madrid, CLF717_Geometry_PhoswichEndcap.geo (José Sánchez del Río Sáez) 24/12/2012

//  -------------------------------------------------------------------------

void califaAna_Gamma_0_5MeV(Int_t nEvents = 3000 , 
		TString file_r3bsim    = "r3bsim_Gamma_0_5MeV.root",
		TString file_r3bpar    = "r3bpar_Gamma_0_5MeV.root",
		TString file_califaAna = "califaAna_Gamma_0_5MeV.root"
		) {
	
	/*TString dir = getenv("VMCWORKDIR");
	
	TString r3b_geomdir = dir + "/geometry";
	gSystem->Setenv("GEOMPATH",r3b_geomdir.Data());
	
	TString r3b_confdir = dir + "gconfig";
	gSystem->Setenv("CONFIG_DIR",r3b_confdir.Data());
	*/
	
	// Input and output files
	//TString InFile = "r3bsim.root";
	//TString OutFile = "califaAna.root";
	
	// In general, the following parts need not be touched
	// ========================================================================
	
	// ----    Debug option   -------------------------------------------------
	gDebug = 0;
	// ------------------------------------------------------------------------
	
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
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
	gSystem->Load("libR3BPassive");
	gSystem->Load("libR3BData");
	gSystem->Load("libR3BCal");
	gSystem->Load("libR3BCalo");
	
	
	// -----   Create analysis run   ----------------------------------------
	FairRunAna* fRun = new FairRunAna();
	
	///*****************************************
	//TString file_r3bpar = "r3bpar.root";
	 TFile* file = new TFile(file_r3bpar);
	file->Get("FairBaseParSet"); 
	///*****************************************

	// -----   Runtime database   ---------------------------------------------
	/*
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parIn = new FairParRootFileIo();
	parIn->open("r3bpar.root");
	rtdb->print();
	*/
	// -----  Analysis routines for CALIFA	
	
	///**************************************************	
	//TString file_r3bsim = "r3bsim.root";
	//TString file_califaAna = "califaAna.root";
	fRun->SetInputFile(file_r3bsim);
	fRun->SetOutputFile(file_califaAna);
	///**************************************************
	
	//fRun->LoadGeometry(); //Needed for what??
	
	R3BCaloHitFinder* caloHF = new R3BCaloHitFinder();
	//Selecting the geometry version
	// 0- CALIFA 5.0, including BARREL and ENDCAP.
	// 1- CALIFA 7.05, only BARREL
	// 2- CALIFA 7.07, only BARREL
	// 4- CALIFA 7.17, only ENDCAP (in CsI[Tl])  or phoswich endcap IEM-CSIC Madrid (LaBr - LaCl)(CLF717_Geometry_PhoswichEndcap_1.geo & ""2.geo ) J. Sanchez del Rio (24-10-2012)
	// 5- CALIFA 7.07+7.17 or Phoswich endcap IEM Madrid + Barrel CLF717 or CLF811  (CLF717_Geometry_PhoswichEndcap_3.geo & ""_4.geo) J. Sanchez del Rio (24-10-2012) 
	// 5- CALIFA 7.07+7.17, 
	// 6- CALIFA 7.09+7.17, (ongoing work) Phoswich endcap IEM Madrid + Barrel CLF709  (CLF717_Geometry_PhoswichEndcap_5.geo) J.Sanchez del Rio (24-10-2012)
	// 10- CALIFA 8.00, (ongoing work) 
	// ...


	caloHF->SelectGeometryVersion(10);          

	//caloHF->SelectGeometryVersion(5);  
	caloHF->SetDetectionThreshold(0.000050);//50 KeV
	caloHF->SetExperimentalResolution(5.);  //5% at 1 MeV
	caloHF->SetAngularWindow(3.2,3.2);      //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter]

	fRun->AddTask(caloHF);
	
	// Number of events to process
	//Int_t nEvents =100;

	fRun->Init();                     
	fRun->Run(0, nEvents);
	
	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
	
	
	
}
