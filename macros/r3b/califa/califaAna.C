//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis
//         Author: Hector Alvarez <hector.alvarez@usc.es>,
//                 Michael Bendel <michael.bendel@tum.de>
//         Last Update: 20/10/14
//         Comments:
//			Runs the CALIFA Hit Finder. Outputs a root file with 
//			a collection (TClonesArray) of R3BCalifaHitData
//
//  -------------------------------------------------------------------------
//
//   Usage: 
//        > root -l califaAna.C
//                         
//
//  -------------------------------------------------------------------------

void califaAna(Int_t nEvents = 1) {
	
	// Input and output files
 
        // Input file: simulation
	TString InFile = "r3bsim.root";

	// Input file: parameters
	TString parFile = "r3bpar.root";

	// Output file
	TString OutFile = "califaAna.root";

	
	// In general, the following parts need not be touched
	// ========================================================================

	// ----    Debug option   -------------------------------------------------
	gDebug = 0;
	// ------------------------------------------------------------------------
	
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------
	

	
	// -----   Create analysis run   ----------------------------------------
	FairRunAna* fRun = new FairRunAna();
	fRun->SetSource(new FairFileSource(InFile));
	fRun->SetOutputFile(OutFile.Data());

	// -----   Runtime database   ---------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parIn = new FairParRootFileIo(kTRUE);
	parIn->open(parFile.Data());
	rtdb->setFirstInput(parIn);
	rtdb->print();



	// -----  Analysis routines for CALIFA	
	R3BCalifaCrystalCal2Hit* califaHF = new R3BCalifaCrystalCal2Hit();
        TMap fDetList = new TMap();
        califaHF->SetGeometryFileName(((TObjString*)&fDetList.GetValue("CALIFA"))->GetString().Data());
	//Selecting the geometry version
	// 0- CALIFA 5.0, including BARREL and ENDCAP.
	// 1- CALIFA 7.05, only BARREL
	// 2- CALIFA 7.07, only BARREL
	// 3- CALIFA 7.09, only BARREL (ongoing work)
	// 4- CALIFA 7.17, only ENDCAP (in CsI[Tl])
	// 5- CALIFA 7.07+7.17, 
	// 6- CALIFA 7.09+7.17, (ongoing work)
	// 10- CALIFA 8.00, (ongoing work) 
	// ...
	// 16- CALIFA 8.11 + cc0.2 (ongoing work)
	califaHF->SelectGeometryVersion(16);   
	califaHF->SetClusteringAlgorithm(1,0);
	califaHF->SetDetectionThreshold(0.000050);  //50 KeV
	califaHF->SetExperimentalResolution(6.);    //6% at 1 MeV
	califaHF->SetComponentResolution(.25);      //sigma = 0.25 MeV
	califaHF->SetPhoswichResolution(3.,5.);     //percent @ 1 MeV for LaBr and LaCl 
	califaHF->SetAngularWindow(3.2,3.2);        //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter]

	fRun->AddTask(califaHF);
	
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
