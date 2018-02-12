void DchDigi(){

  // Input files
   //TString inFile="/lustre/land/jmargan/simulation/Ox_2p_en5/r3bsim.root";
   //TString parFile="/lustre/land/jmargan/simulation/Ox_2p_en5/r3bpar.root";
   TString inFile="/u/fwamers/Promotion/L_Analysis/s318/qfs_analysis/ne17/sim_r3broot/output/r3bsim.root";
   TString parFile="/u/fwamers/Promotion/L_Analysis/s318/qfs_analysis/ne17/sim_r3broot/output/r3bpar.root";

  // Output file         
  //TString outFile ="/lustre/land/jmargan/simulation/test/histo_2p.root";
  TString outFile ="/u/fwamers/Promotion/L_Analysis/s318/qfs_analysis/ne17/sim_r3broot/output/digi1_test.root";


  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetSource(new FairFileSource(inFile));
  fRun->SetOutputFile(outFile);



//  // Verbosity Mode level
//  // (0=quiet, 1=event level, 2=track level, 3=debug)
//  Int_t iVerbose = 1;
//  //Connect the Digitization  Task
//  R3BTargetDigitizer* Target  = new R3BTargetDigitizer();
//  R3BTraDigitizer* Tra  = new R3BTraDigitizer();
//  R3BGfiDigitizer* Gfi  = new R3BGfiDigitizer();
//  R3BmTofDigitizer* mTof  = new R3BmTofDigitizer();
//  R3BDchDigitizer* dch  = new R3BDchDigitizer();
//  R3BTofDigitizer* Tof  = new R3BTofDigitizer();
////  R3BTof2pDigitizer* Tof2p  = new R3BTof2pDigitizer();
//        
//
//  fRun->AddTask(Target);
//  fRun->AddTask(Tra);
//  fRun->AddTask(Gfi);
//  fRun->AddTask(mTof);
//  fRun->AddTask(dch);
//  fRun->AddTask(Tof);  
////  fRun->AddTask(Tof2p);
   


// Verbosity Mode level
// (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  //Connect the Digitization  Task
  //  R3BTargetDigitizer* Target  = new R3BTargetDigitizer();
  R3BTarget2pDigitizer* Target2p  = new R3BTarget2pDigitizer();  
  //  R3BTraDigitizer* Tra  = new R3BTraDigitizer();
  R3BTraFraDigitizer* TraFra  = new R3BTraFraDigitizer();
  R3BTra2pDigitizer* Tra2p  = new R3BTra2pDigitizer();
  R3BGfiDigitizer* Gfi  = new R3BGfiDigitizer();
  R3BmTofDigitizer* mTof  = new R3BmTofDigitizer();
  //  R3BDchDigitizer* dch  = new R3BDchDigitizer();
  R3BDch2pDigitizer* dch2p  = new R3BDch2pDigitizer();
  //  R3BTofDigitizer* Tof  = new R3BTofDigitizer();
  R3BTof2pDigitizer* Tof2p  = new R3BTof2pDigitizer();


  //  fRun->AddTask(Target);
  fRun->AddTask(Target2p);
  //  fRun->AddTask(Tra);
  fRun->AddTask(TraFra);
  fRun->AddTask(Tra2p);
  fRun->AddTask(Gfi);
  fRun->AddTask(mTof);
  //  fRun->AddTask(dch);
  fRun->AddTask(dch2p);
  //  fRun->AddTask(Tof);  
  fRun->AddTask(Tof2p);


  // Runtime DataBase info
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  // Load the Root Geometry
  //fRun->LoadGeometry();
  
  // Number of events to process
  Int_t nEvents = 100;
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  delete fRun;

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file writen:  "    << outFile << endl;
  cout << "Parameter file writen " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
