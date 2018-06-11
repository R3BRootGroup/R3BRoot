/* Additional info:
 * To generate the header file for a different number of febex modules,
 * add then in $UCESB_DIR/upexps/califalisbon16/califa.spec file and use:
 *
 * $UCESB_DIR/upexps/califalisbon16/califa --ntuple=RAW,STRUCT_HH,ext_h101_raw_califa_febex.h
 *
 * Put this header file into the 'r3bsource' directory and recompile
 * (present file is valid for 9 modules and 16 channels for module)
 * */

typedef struct EXT_STR_h101_t {
  EXT_STR_h101_unpack_t unpack;
  EXT_STR_h101_CALIFA_t califa;
} EXT_STR_h101;

void califa_febex3_calibParFinder() {
  TStopwatch timer;
  timer.Start();
  
  const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  
  /* Create source using ucesb for input ------------------ */
  
  TString filename = "/media/mbsdaq/extDisk/data/168_2018-06-07_12-38-32/data_0001.lmd";
  TString outputFileName = "./data_0001.root";
  
  TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
  TString ucesb_dir = getenv("UCESB_DIR");
  
  TString ucesb_path = ucesb_dir + "/../upexps/califaKrakow17/califa";
  
  EXT_STR_h101 ucesb_struct;
  
  R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
					      ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
  source->SetMaxEvents(nev);
  
  source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct,
					offsetof(EXT_STR_h101, unpack)));
  source->AddReader(new R3BCalifaFebexReader((EXT_STR_h101_CALIFA*)&ucesb_struct.califa,
					     offsetof(EXT_STR_h101, califa)));
  /* ------------------------------------------------------ */
  
  /* Create online run ------------------------------------ */
  FairRunOnline* run = new FairRunOnline(source);
  //run->SetRunId(1495624105);
  run->SetRunId(1513078509);
  run->SetOutputFile(outputFileName);
  
  /* Add analysis task ------------------------------------ */
  
  //R3BCalifaMapped2CrystalCalPar ----
  TArrayF* EnergythePeaks = new TArrayF();
  Float_t e1=1332.5;
  Float_t e2=1173.2;
  EnergythePeaks->Set(2);
  EnergythePeaks->AddAt(e1,0);
  EnergythePeaks->AddAt(e2,1);
  
  R3BCalifaMapped2CrystalCalPar* CalPar = new R3BCalifaMapped2CrystalCalPar();  
  CalPar->SetNumCrystals(128); //2 petals*64cry
  CalPar->SetMinStadistics(1000);
  CalPar->SetNumParameterFit(2);//OPTIONAL by default 2
  CalPar->SetCalRange_left(250);  
  CalPar->SetCalRange_right(600);
  CalPar->SetCalRange_bins(350);    
  CalPar->SetSigma(3.0);
  CalPar->SetThreshold(0.0001);      
  CalPar->SetEnergyPeaks(EnergythePeaks);
  run->AddTask(CalPar);

  
  /* Initialize ------------------------------------------- */
  run->Init();
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  /* ------------------------------------------------------ */
  
  /* Runtime data base ------------------------------------ */
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  /*R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();*/
  
  //Choose Root or Ascii file	
  //1-Root file with the Calibartion Parameters
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open("Califa_CalibParam.root");
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  
  //2-Ascii file with the Calibartion Parameters
  /*FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
    parIo1->open("Califa_CalibParam.par","out");
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    rtdb->print();*/
  /* ------------------------------------------------------ */
  
  /* Run -------------------------------------------------- */
  run->Run(nev,0);
  /*rtdb->saveOutput();*/
  delete run;
  /* ------------------------------------------------------ */

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is " << outputFileName << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
       << endl << endl;
}
