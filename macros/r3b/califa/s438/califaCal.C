//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 25/09/14
//         Comments:
//			Runs the CALIFA Hit Finder. Outputs a root file with 
//			a collection (TClonesArray) of R3BCaloHits
//
//  -------------------------------------------------------------------------
//
//   Usage: 
//        > root -l califaCal.C
//                         
//
//  -------------------------------------------------------------------------

void califaCal(Int_t nEvents = -1) {
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // -----   Create analysis run   ----------------------------------------
  FairRunAna* fRun = new FairRunAna();
  fRun->SetInputFile("./data/na22_low_range_30mev_raw.root");
  fRun->SetOutputFile("./data/na22_low_range_30mev_cal.root");
  

    // -----   Runtime database   ---------------------------------------------
  //FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  //R3BCaloDUCalPar* duPar = (R3BCaloDUCalPar*)rtdb->getContainer("CaloDUCalPar");

  //FairParAsciiFileIo* parIn = new FairParAsciiFileIo();
  //parIn->open("/Users/hapol/CODES/r3broot/macros/r3b/califa/params.txt");
  //rtdb->setFirstInput(parIn);
  //rtdb->print();


  //Crystal calibration
  R3BCaloCal* cal = new R3BCaloCal();
  fRun->AddTask(cal);
  

  fRun->Init();       
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
             
  fRun->Run(0, 10000);
  
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
