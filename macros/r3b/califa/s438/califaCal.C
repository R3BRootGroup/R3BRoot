//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Analysis
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 30/09/14
//         Comments:
//	       	Runs the CALIFA calibration. Outputs a root file with 
//     		a collection (TClonesArray) of R3BCaloCrystalHits
//
//  -------------------------------------------------------------------------
//
//   Usage: 
//        > root -l califaCal.C
//        ROOT> .L califaCal.C
//        ROOT>  califaCal(inputFile,outputFile,parRootFile,nevents)
//                         
//   where
//    inputFile is the input root file with the R3BCalorRawHits 
//    outputFile is the output root file where the R3BCaloCrystalHits will 
//               be created
//    parRootFile is the parameters ROOT file 
//    nevents is the number of events to calibrate (0 if all)
// 
//    Batch is also an option, e.g.
//    root -l -b -q 'califaCal.C("./data/totcal/900mv_30mev__raw.root","./data/totcal/900mv_30mev_cal.root","califa_allPars.root")';
//
 -------------------------------------------------------------------------

void califaCal(TString inputFile, TString outputFile, 
	       TString parRootFile, Int_t nEvents = 0) {
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // -----   Create analysis run   ----------------------------------------
  FairRunAna* fRun = new FairRunAna();
  fRun->SetInputFile(inputFile);
  fRun->SetOutputFile(outputFile);
  
  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parRootIn; 
  FairParAsciiFileIo* parAsciiIn; 
  
  // if(parAsciiFile) {
  //   parAsciiIn = new FairParAsciiFileIo();
  //   parAsciiIn->open(parAsciiFile);
  //   rtdb->setFirstInput(parAsciiIn);
  //   if(parRootFile) {
  //     parRootIn = new FairParRootFileIo();
  //     parRootIn->open(parRootFile);
  //     rtdb->setSecondInput(parRootIn);
  //   }
  // }
  //else if(parRootFile) {
  parRootIn = new FairParRootFileIo();
  parRootIn->open(parRootFile);
  rtdb->setFirstInput(parRootIn);
    //}
  //rtdb->setOutput(parIn);
  //rtdb->saveOutput();
  rtdb->print();
  
  /*TList *lPars = rtdb->getListOfContainers();
  for (Int_t i=0;i<lPars->GetEntries();i++) {
    FairParSet* apar = (FairParSet*) lPars->At(i);
    apar->Print();
  }    
  R3BCaloCalPar* par = (R3BCaloCalPar*)rtdb->getContainer("CaloCalPar");  
  if(par) par->Print();
  else cout << "NO PAR!!!!" << endl << endl;

  R3BCaloDUCalPar* du =  par->GetDUCalParAt(0);
  if(du) du->Print();
  else cout << "NO DU!!!!" << endl << endl;
  */

  //Crystal calibration
  R3BCaloCal* cal = new R3BCaloCal();
  fRun->AddTask(cal);
  

  fRun->Init();       
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
             
  fRun->Run(0,nEvents);
  
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
