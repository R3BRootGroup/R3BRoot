
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    TString dirIn1 = "/media/vadim/Externe/GSI/data/";
    TString dirIn2 = "/media/vadim/Externe/GSI/data/";
    TString dirOut = "/media/vadim/Externe/GSI/data/";
    TString inputFileName1 = dirIn2 + runNumber + "_tcal.root";             // name of input file
    TString parFileName    = dirIn1 + "params_" + runNumber + "_raw.root";  // name of parameter file
    TString outputFileName = dirOut + runNumber + "_digi.root";            // name of output file

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
    // ---------------------------------------------------------------------------

    // Tdiff calibration ---------------------------------------------------------
    R3BLandTSync* landTSync= new R3BLandTSync("LandTSync", 1);
    //landTSync->SetFirstPlaneHorisontal();
    run->AddTask(landTSync);
    // ---------------------------------------------------------------------------

    // Analysis ------------------------------------------------------------------
//     R3BLandAna* landAna = new R3BLandAna("LandAna", 1);
//     landAna->SetNofBars(400);
//     landAna->SetFirstPlaneHorisontal();
//     run->AddTask(landAna);
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
    cout << "Parameter file is " << parFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

