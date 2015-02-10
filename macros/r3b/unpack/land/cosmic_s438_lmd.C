
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nBars = 100;

    TString dirIn1 = "/Volumes/Data/kresan/s438/data/";
    TString dirIn2 = "/Volumes/Data/kresan/s438/tcal/";
    TString dirOut = "/Volumes/Data/kresan/s438/cosmic/";
    TString tdiffParName = "tdiff_" + runNumber + ".dat";
    TString inputFileName1 = dirIn2 + runNumber + "_tcal.root";             // name of input file
    TString parFileName    = dirIn1 + "params_" + runNumber + "_raw.root";  // name of parameter file
    TString outputFileName = dirOut + runNumber + "_cosmic.root";           // name of output file

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
    rtdb->saveOutput();
    // ---------------------------------------------------------------------------

    // Tdiff calibration ---------------------------------------------------------
    R3BLandTdiffFill* landTdiffFill = new R3BLandTdiffFill("LandTdiffFill", 1);
    landTdiffFill->SetNofBars(nBars);
    landTdiffFill->SetTdiffParName(tdiffParName.Data());
    run->AddTask(landTdiffFill);
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

