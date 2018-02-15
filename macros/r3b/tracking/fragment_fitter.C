
void fragment_fitter()
{
    TString inFile = "sim.root";
    TString parFile = "par.root";
    TString digiFile = "digi.root";
    TString outFile = "tracking.root";
    
    // Create analysis run -----------------------------------------------------
    FairRunAna* run = new FairRunAna();
    FairFileSource* source = new FairFileSource(inFile);
    source->AddFriend(digiFile);
    run->SetSource(source);
    run->SetOutputFile(outFile.Data());
    // -------------------------------------------------------------------------
    
    // Runtime DataBase info ---------------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    parIo1->open(parFile.Data());
    rtdb->setFirstInput(parIo1);
    // -------------------------------------------------------------------------

    // Propagation task --------------------------------------------------------
    R3BFragmentFitter* prop = new R3BFragmentFitter("R3BFragmentFitter", kTRUE);
    run->AddTask(prop);
    // -------------------------------------------------------------------------
    
    // Initialize --------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    // -------------------------------------------------------------------------
    
    TStopwatch timer;
    timer.Start();
    
    // Run ---------------------------------------------------------------------
    run->Run();
//    delete run;
    // -------------------------------------------------------------------------
    
    timer.Stop();
    cout << timer.RealTime() << "  " << timer.CpuTime() << endl;
}

