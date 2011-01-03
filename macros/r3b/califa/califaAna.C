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
//
//  -------------------------------------------------------------------------

void califaAna(Int_t nEvents = 1) {
	
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
	
	
	// -----   Create analysis run   ----------------------------------------
	FairRunAna* fRun = new FairRunAna();
	
	
	 TFile* file = new TFile("r3bpar.root");
	file->Get("FairBaseParSet"); 
	
	// -----   Runtime database   ---------------------------------------------
	/*
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parIn = new FairParRootFileIo();
	parIn->open("r3bpar.root");
	rtdb->print();
	*/
	// -----  Analysis routines for CALIFA	
	
	fRun->SetInputFile("r3bsim.root");
	fRun->SetOutputFile("califaAna.root");
	
	//fRun->LoadGeometry(); //Needed for what??
	
	R3BCaloHitFinder* caloHF = new R3BCaloHitFinder();
	caloHF->SelectGeometryVersion(0);          // 0 for version 5.0, 1 for version 7.05, 2 for version 7.07
	caloHF->SetDetectionThreshold(0.000050);   //50 KeV
	caloHF->SetExperimentalResolution(5.);    //5% at 1 MeV
	fRun->AddTask(caloHF);
	
	// Number of events to process
	Int_t nEvents = 100000;

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
