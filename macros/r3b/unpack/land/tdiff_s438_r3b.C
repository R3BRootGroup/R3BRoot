
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    TString dirIn1 = "/Volumes/Data/kresan/s438/data/";
    TString dirIn2 = "/Volumes/Data/kresan/s438/tcal/";
    TString dirOut = "/Volumes/Data/kresan/s438/digi/";
    TString tdiffParName = "tdiff_run107.dat";
    TString inputFileName1 = dirIn2 + runNumber + "_tcal.root";             // name of input file
    TString parFileName    = dirIn1 + "params_" + runNumber + "_tcal.root"; // name of parameter file
    TString outputFileName = dirOut + runNumber + "_digi.root";             // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inputFileName1.Data());
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

    // Tdiff calibration ---------------------------------------------------------
    R3BLandTdiff* landTdiff = new R3BLandTdiff("LandTdiff", 1);
    landTdiff->SetTdiffParName(tdiffParName.Data());
    run->AddTask(landTdiff);
    // ---------------------------------------------------------------------------

    // Analysis ------------------------------------------------------------------
    R3BLandAna* landAna = new R3BLandAna("LandAna", 1);
    run->AddTask(landAna);
    // ---------------------------------------------------------------------------
    
    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(0, 100000);
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Parameter file is " << parFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

