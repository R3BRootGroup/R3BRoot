//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 12/04/2012
//         Comments:
//			Runs the CALIFA Hit Finder. Outputs a root file with 
//			a collection (TClonesArray) of R3BCalifaHits
//
//  -------------------------------------------------------------------------
//
//   Usage: 
//        > root -l -b -q  'califaAna_batch.C(nEvents, fGeoVer, fThres, fExpRes, fDelPolar, fDelAzimuthal)'
//                         
//
//  -------------------------------------------------------------------------

void califaAna_batch(Int_t nEvents=1, Int_t fGeoVer=1, Double_t fThres=0.000050, 
					 Double_t fExpRes=5., Double_t fDelPolar=3.2, Double_t fDelAzimuthal=3.2) {
	        
        cout << "Running califaAna_batch with arguments:" <<endl;
        cout << "Number of events: " << nEvents <<endl;
        cout << "CALIFA geo version: " << fGeoVer <<endl;
        cout << "Threshold: " << fThres <<endl<<endl;
	cout << "Experimental resolution: " << fExpRes <<endl<<endl;

	
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
	
        FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
        FairParRootFileIo*  parIo1 = new FairParRootFileIo();
        parIo1->open("r3bpar.root");
        rtdb->setFirstInput(parIo1);
        rtdb->print();

	fRun->SetSource(new FairFileSource("r3bsim.root"));
	fRun->SetOutputFile("califaAna.root");
	
	// -----  Analysis routines for CALIFA	
	
	R3BCalifaCrystalCal2Hit* califaHF = new R3BCalifaCrystalCal2Hit();
	//Selecting the geometry version
	// 0- CALIFA 5.0, including BARREL and ENDCAP.
	// 1- CALIFA 7.05, only BARREL
	// 2- CALIFA 7.07, only BARREL
	// 3- CALIFA 7.09, only BARREL (ongoing work)
	// 4- CALIFA 7.17, only ENDCAP (in CsI[Tl])
	// 5- CALIFA 7.07+7.17, 
	// 6- CALIFA 7.09+7.17, (ongoing work)
	// 10- CALIFA 8.11, only BARREL (ongoing work) 
	// ...
	califaHF->SelectGeometryVersion(fGeoVer);          
	//califaHF->SelectGeometryVersion(10);          
	califaHF->SetDetectionThreshold(fThres);             //50 KeV  [fThres in GeV]
	califaHF->SetExperimentalResolution(fExpRes);        //5% at 1 MeV
	califaHF->SetAngularWindow(fDelPolar,fDelAzimuthal); //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter]

	fRun->AddTask(califaHF);
	
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
