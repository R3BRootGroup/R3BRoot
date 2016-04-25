
void run(TString runNumber, TString refRun)
{
    TStopwatch timer;
    timer.Start();

    TString dirIn1 = "/Users/kresan/data/s438b/data/";
    TString dirIn2 = "/Users/kresan/data/s438b/tcal/";
    TString dirOut = "/Users/kresan/data/s438b/digi/";
    TString inputFileName1 = dirIn2 + runNumber + "_tcal_land.root";             // name of input file
    TString parFileName1   = dirIn1 + "params_" + runNumber + "_raw_land.root";  // name of parameter file
    TString parFileName2   = dirIn1 + "params_" + refRun + "_raw.root";          // name of parameter file
    TString outputFileName = dirOut + runNumber + "_digi.root";                  // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inputFileName1.Data());
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // ----- Runtime DataBase info -----------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    parIo1->open(parFileName1);
    rtdb->setFirstInput(parIo1);
    FairParRootFileIo* parIo2 = new FairParRootFileIo();
    parIo2->open(parFileName2);
    rtdb->setFirstInput(parIo2);
    // ---------------------------------------------------------------------------

    // cal2hit ---------------------------------------------------------
    R3BNeulandCal2Hit* neulandCal2Hit= new R3BNeulandCal2Hit("NeulandCal2Hit", 1);
    neulandCal2Hit->SetFirstPlaneHorisontal();
    neulandCal2Hit->SetDistanceToTarget(1300.);
    run->AddTask(neulandCal2Hit);
    // ---------------------------------------------------------------------------

    // Analysis ------------------------------------------------------------------
    R3BLandAna* landAna = new R3BLandAna("LandAna", 1);
    landAna->SetNofBars(400);
    landAna->SetFirstPlaneHorisontal();
    landAna->SetMinimalLOSSignals(3);
    landAna->SetTimeOffset(1530.);
    run->AddTask(landAna);
    // ---------------------------------------------------------------------------
    
    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run();
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Parameter file is " << parFileName1 << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

