//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B STARTRACKER Analysis
//         Author: Nick Ashwood <n.i.ashwood@bham.ac.uk>
//         Last Update: 28/05/14 Marc Labiche
//         Comments: Adapted from califaAna.C
//			Runs the STARTRACKER Hit Finder. Outputs a root file with 
//			a collection (TClonesArray) of R3BTraHits
//
//  -------------------------------------------------------------------------
//
//   Usage: 
//        > root -l startrackerAna.C
//                         
//
//  -------------------------------------------------------------------------

void StarTrackAna(Int_t nEvents = 1) {
	
	/*TString dir = getenv("VMCWORKDIR");
	
	TString r3b_geomdir = dir + "/geometry";
	gSystem->Setenv("GEOMPATH",r3b_geomdir.Data());
	
	TString r3b_confdir = dir + "gconfig";
	gSystem->Setenv("CONFIG_DIR",r3b_confdir.Data());
	*/
	
	// Input and output files
	//TString InFile = "r3bsim.root";
	//TString OutFile = "startrackerAna.root";
	
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
	//gSystem->Load("libR3BXBall");
	//gSystem->Load("libR3BCalifa");
	//gSystem->Load("libR3BDch");
	//gSystem->Load("libR3BGfi");
	//gSystem->Load("libR3BLand");
	//gSystem->Load("libR3BmTof");
	//gSystem->Load("libR3BTof");
	//gSystem->Load("libR3BSTaRTra");  // obsolete since 2017
	gSystem->Load("libR3BStartrack");
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
	// -----  Analysis routines for STARTRACKER	
	
	fRun->SetSource(new FairFileSource("r3bsim.root"));
	fRun->SetOutputFile("startrackerAna.root");
	
	//fRun->LoadGeometry(); //Needed for what??
	
	R3BSTaRTraHitFinder* startraHF = new R3BSTaRTraHitFinder();  // !!! obsolete (replaced by R3BStartrackDigit)
	//Selecting the geometry version
	// 0- STARTRACKER 5.0.
	// ...
	//startraHF->SelectGeometryVersion(15);          
	startraHF->SetDetectionThreshold(0.000050);//50 KeV
	startraHF->SetExperimentalResolution(0.00005);  // 50 keV

	fRun->AddTask(startraHF);
	
	// Number of events to process
	Int_t nEvents = 100000;

	fRun->Init();                     
	fRun->Run(0, nEvents);
	
    delete fRun;

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
