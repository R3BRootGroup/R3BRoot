inline void ConnectParFileToRuntimeDb(const TString par_file, FairRuntimeDb* rtdb)
{
  FairParRootFileIo* io = new FairParRootFileIo();
  io->open(par_file);
  rtdb->setFirstInput(io);
  rtdb->setOutput(io);
  rtdb->saveOutput();
}


void MCMonitoring(
  const UInt_t num_events,
  const TString base_name,
  const TString base_path = "."
)
{
  TStopwatch timer;
  timer.Start();


  const TString sim_file = base_path + "/r3bsim." + base_name + ".root";
  const TString par_file = base_path + "/r3bpar." + base_name + ".root";
  const TString out_file = base_path + "/mcmon."  + base_name + ".root";

  FairRunAna* run = new FairRunAna();
  run->SetInputFile(sim_file);
  run->SetOutputFile(out_file);
  ConnectParFileToRuntimeDb(par_file, run->GetRuntimeDb());

  run->AddTask(new R3BNeulandMCMon());

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
