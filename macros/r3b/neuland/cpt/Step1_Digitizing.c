inline void ConnectParFileToRuntimeDb(const TString par_file, FairRuntimeDb* rtdb)
{
  FairParRootFileIo* io = new FairParRootFileIo();
  io->open(par_file);
  rtdb->setFirstInput(io);
  rtdb->setOutput(io);
  rtdb->saveOutput();
}


void Step1_Digitizing(
  const UInt_t num_events,
  const TString base_name,
  const TString base_path = "."
)
{
  TStopwatch timer;
  timer.Start();


  const TString sim_file = base_path + "/sim." + base_name + ".root";
  const TString par_file = base_path + "/par." + base_name + ".root";
  const TString out_file = base_path + "/digi." + base_name + ".root";

  FairRunAna* run = new FairRunAna();
  run->SetSource(new FairFileSource(sim_file));
  run->SetOutputFile(out_file);
  ConnectParFileToRuntimeDb(par_file, run->GetRuntimeDb());

  /*R3BLandDigitizer *land_digitizer = new R3BLandDigitizer(0);
  land_digitizer->SetTOFRange(100.);
  run->AddTask(land_digitizer);*/

  run->AddTask(new R3BNeulandDigitizer());
  
  run->Init();
  run->Run(0, num_events);
  delete run;


  timer.Stop();

  cout << endl;
  cout << "Macro finished succesfully!" << endl;
  cout << "Output file writen: " << out_file << endl;
  cout << "Parameter file writen: " << par_file << endl;
  cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
  cout << endl;
}
