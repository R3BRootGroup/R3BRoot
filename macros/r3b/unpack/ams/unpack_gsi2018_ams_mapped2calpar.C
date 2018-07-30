/* Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * ./201810_s444 --ntuple=RAW:SST,id=h101_AMS,ext_h101_ams.h
 *
 * at $UCESB_DIR/upexps/201810_s444
 *
 * Put this header file into the 'r3bsource' directory and recompile.
 * */

typedef struct EXT_STR_h101_t {
  EXT_STR_h101_unpack_t unpack;
  EXT_STR_h101_AMS_t ams;
} EXT_STR_h101;

void unpack_gsi2018_ams_mapped2calpar() {
  TStopwatch timer;
  timer.Start();
  
  //const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  
  /* Create source using ucesb for input ------------------ */
  
  TString filename = "~/lmd/ams.lmd";
  TString outputFileName = "data_0001.root";
  
  TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
  TString ucesb_dir = getenv("UCESB_DIR");
  
  TString ucesb_path = ucesb_dir + "/../upexps/201810_s444/201810_s444";
  
  EXT_STR_h101 ucesb_struct;
  
  R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
					      ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
  source->SetMaxEvents(nev);
  
  source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct,
					offsetof(EXT_STR_h101, unpack)));
  source->AddReader(new R3BAmsReader((EXT_STR_h101_AMS*)&ucesb_struct.ams,
					     offsetof(EXT_STR_h101, ams)));

  
  /* Create online run ------------------------------------ */
  FairRunOnline* run = new FairRunOnline(source);
  run->SetRunId(1513078509);
  run->SetOutputFile(outputFileName);

  
  /* Add analysis task ------------------------------------ */
  R3BAmsMapped2StripCalPar* CalPar = new R3BAmsMapped2StripCalPar();
  CalPar->SetNumDetectors(4); // Number of ams detectors 
  CalPar->SetNumStrips(1024); // 1024 strips per ams detector
  CalPar->SetNumStripsK(384); // 384 strips for the K-side
  CalPar->SetNumStripsS(640); // 640 strips for the S-side
  CalPar->SetMaxSigma(5);     // Max. sigma to mark dead strips: 5 ADC units
  CalPar->SetMinStadistics(1000);
  CalPar->SetCalRange_left(0);  
  CalPar->SetCalRange_right(2000);
  CalPar->SetCalRange_bins(500);
  run->AddTask(CalPar);


  /* Initialize ------------------------------------------- */
  run->Init();
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  //FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
  //FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
  /* ------------------------------------------------------ */
  

  /* Runtime data base ------------------------------------ */
  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  //Choose Root or Ascii file	
  //1-Root file with the Calibartion Parameters
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open("Ams_CalibParam.root");
  rtdb->setOutput(parOut);
  
  //2-Ascii file with the Calibartion Parameters
  /*FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open("Ams_CalibParam.par","out");
  rtdb->setOutput(parIo1);*/


  /* Run -------------------------------------------------- */
  run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);


  /* Save parameters (if needed) -------------------------- */
  rtdb->saveOutput();


  /* Finish ----------------------------------------------- */
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  std::cout << std::endl << std::endl;
  std::cout << "Macro finished succesfully." << std::endl;
  std::cout << "Output file is " << outputFileName << std::endl;
  std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
            << std::endl << std::endl;
  gApplication->Terminate();
}
