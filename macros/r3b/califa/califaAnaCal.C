
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    //const char *landMappingName = "cfg_neuland_s438.hh";   // mapping file

    TString inputFileName = "XXX.root";           // name of input file
    TString parFileName    = "YYY.root"; // name of parameter file
    TString outputFileName = "ZZZ.root";          // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inputFileName.Data());
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // ----- Runtime DataBase info -----------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    parIo1->open(parFileName);
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);

    // Set the SQL IO as second input
    FairParTSQLIo* inp = new FairParTSQLIo();
    inp->SetVerbosity(1);
    inp->open();
    rtdb->setFirstInput(inp);

    rtdb->saveOutput();
    // ---------------------------------------------------------------------------

    // Calibration ----------------------------------------------------------
    R3BCaloCal* caloCal = new R3BCaloCal("CaloCal", 0);
    run->AddTask(caloCal);
    // ---------------------------------------------------------------------------

    // Histograms ----------------------------------------------------------------
    R3BCaloCrystalAna* caloAna = new R3BCaloCrystalAna("CaloCrystalAna", 1);
    run->AddTask(caloCrystalAna);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(0, 10000);
    // ---------------------------------------------------------------------------

    delete run;

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Parameter file is " << parFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

