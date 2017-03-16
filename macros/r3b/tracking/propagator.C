
void propagator()
{
    TString inFile = "sim.root";
    TString parFile = "par.root";
    TString outFile = "tracking.root";
    
    // Create analysis run -----------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inFile.Data());
    run->SetOutputFile(outFile.Data());
    // -------------------------------------------------------------------------
    
    // Runtime DataBase info ---------------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    parIo1->open(parFile.Data());
    rtdb->setFirstInput(parIo1);
    // -------------------------------------------------------------------------

    // Propagation task --------------------------------------------------------
    R3BPropagate* prop = new R3BPropagate("R3BPropagate", kTRUE);
    run->AddTask(prop);
    // -------------------------------------------------------------------------
    
    // Initialize --------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // -------------------------------------------------------------------------
    
    TStopwatch timer;
    timer.Start();
    
    // Run ---------------------------------------------------------------------
    run->Run(100);
    delete run;
    // -------------------------------------------------------------------------
    
    timer.Stop();
    cout << timer.RealTime() << "  " << timer.CpuTime() << endl;
}

