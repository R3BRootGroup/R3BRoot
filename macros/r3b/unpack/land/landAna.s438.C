
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const char *landMappingName = "cfg_neuland_s438.hh";   // mapping file
    const Int_t nBarsPerPlane = 50;
    const Int_t nModules = 200;

    TString dirIn1 = "/Volumes/Data/kresan/s438/data/";
    TString dirIn2 = "/Volumes/Data/kresan/s438/tcal/";
    TString dirOut = "./";//"/Volumes/Data/kresan/s438/ana/";
    TString inputFileName1 = dirIn1 + runNumber + "_tcal.root";           // name of input file
    TString inputFileName2 = dirIn2 + runNumber + "_tcal.root";
    TString parFileName    = dirIn1 + "params_" + runNumber + "_tcal.root"; // name of parameter file
    TString outputFileName = dirOut + runNumber + "_ana.root";          // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inputFileName1.Data());
    run->AddFriend(inputFileName2.Data());
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

    // Time calibration ----------------------------------------------------------
    R3BNeulandMapped2Cal* landTcal = new R3BNeulandMapped2Cal("LandTcal", 0);
    landTcal->SetNofModules(nModules, 40);
    run->AddTask(landTcal);

    R3BLosTcal* losTcal = new R3BLosTcal("LosTcal", 0);
    losTcal->SetNofModules(20);
    run->AddTask(losTcal);
    // ---------------------------------------------------------------------------

    // Histograms ----------------------------------------------------------------
    R3BLandAna* landAna = new R3BLandAna("LandAna", 1);
    run->AddTask(landAna);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(0, 10000);
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

