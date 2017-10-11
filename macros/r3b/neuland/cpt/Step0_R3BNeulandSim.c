void Step0_R3BNeulandSim(
   const UInt_t particle_id,
   const UInt_t num_events,
   const Double_t momentum,
   const TString base_name,
   const TString base_path = "."
)
{
   TStopwatch timer;
   timer.Start();

   // System paths
   const TString working_directory = getenv("VMCWORKDIR");
   gSystem->Setenv("GEOMPATH",   working_directory + "/geometry");
   gSystem->Setenv("CONFIG_DIR", working_directory + "/gconfig");

   // Output files
   const TString out_file = base_path + "/sim." + base_name + ".root";
   const TString par_file = base_path + "/par." + base_name + ".root";

   // Basic simulation setup
   FairRunSim *run = new FairRunSim();
   run->SetName("TGeant3");
   run->SetOutputFile(out_file);
   run->SetMaterials("media_r3b.geo");

   //run->SetUserConfig("g4R3bConfig.C");
   //run->SetUserCuts("SetCuts.C");

   // Geometry: Cave
   FairModule *cave = new R3BCave("CAVE");
   cave->SetGeometryFileName("r3b_cave.geo");
   run->AddModule(cave);

   // Geometry: Neuland
   R3BDetector *land = new R3BLand("Land", kTRUE);
   land->SetVerboseLevel(0);
   land->SetGeometryFileName("neuland_v12a_14m.geo.root");
   run->AddModule(land);


   // Primary particle generator
   FairBoxGenerator *boxGen = new FairBoxGenerator(particle_id);
   boxGen->SetThetaRange(0., 1.);
   boxGen->SetPhiRange(0., 360.);
   boxGen->SetPRange(momentum, momentum);
   boxGen->SetXYZ(0., 0., 0.);
   boxGen->SetDebug(1);
   FairPrimaryGenerator *primGen = new FairPrimaryGenerator();
   primGen->AddGenerator(boxGen);
   run->SetGenerator(primGen);


   // Further setup options and initialization
   FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
   run->SetStoreTraj(kTRUE);
   run->Init();


   // Connect runtime parameter file
   FairParRootFileIo *par_file_io = new FairParRootFileIo(kTRUE);
   par_file_io->open(par_file);
   FairRuntimeDb *rtdb = run->GetRuntimeDb();
   rtdb->setOutput(par_file_io);
   rtdb->saveOutput();


   // Simulate
   run->Run(num_events);


   // Report
   timer.Stop();
   cout << "Macro finished succesfully."    << endl;
   cout << "Output file is "    << out_file << endl;
   cout << "Parameter file is " << par_file << endl;
   cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << "s" << endl << endl;
}
