
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const char *landMappingName = "cfg_neuland_s438.hh";   // mapping file
    const Int_t nBarsPerPlane = 50;
    const Int_t updateRate = 150000;
    const Int_t minStats = 10000;
    const Int_t nModules = 200;

    TString dirIn = "/Volumes/Data/kresan/s438/data/";
    TString dirOut = "/Volumes/Data/kresan/s438/tcal/";
    TString inputFileName = dirIn + runNumber + "_tcal.root";           // name of input file
    TString parFileName = dirIn + "params_" + runNumber + "_tcal.root"; // name of parameter file
    TString outputFileName = dirOut + runNumber + "_tcal_1.root";       // name of output file
    Int_t nFiles;
    if (0 == runNumber.CompareTo("r122"))
    {
        nFiles = 22;
    }
    else if (0 == runNumber.CompareTo("r126"))
    {
        nFiles = 2;
    }

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inputFileName.Data());
    for (Int_t i = 1; i < nFiles; i++)
    {
        inputFileName = dirIn + runNumber + "_tcal_";
        inputFileName += i;
        inputFileName += ".root";
        run->AddFile(inputFileName.Data());
    }
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // Runtime data base ---------------------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();

    // Set the SQL based IO as first input
    FairParTSQLIo* input_db = new FairParTSQLIo();
    input_db->SetShutdown(kTRUE);
    input_db->open();
    rtdb->setFirstInput(input_db);
    rtdb->setOutput(input_db);

    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(parFileName);
    rtdb->setSecondInput(parOut);
    rtdb->saveOutput();
    rtdb->print();
    // ---------------------------------------------------------------------------

    // Channel mapping -----------------------------------------------------------
    R3BLandMapping* map = new R3BLandMapping();
    map->SetFileName(landMappingName);
    map->SetNofBarsPerPlane(nBarsPerPlane);
    run->AddTask(map);
    // ---------------------------------------------------------------------------

    // TCAL ----------------------------------------------------------------------
    R3BNeulandMapped2CalPar* landTcalFill = new R3BNeulandMapped2CalPar("NeulandMapped2Cal");
    landTcalFill->SetUpdateRate(updateRate);
    landTcalFill->SetMinStats(minStats);
    landTcalFill->SetNofModules(nModules, 40);
    run->AddTask(landTcalFill);

    R3BLosTcalFill* losTcalFill = new R3BLosTcalFill("LosTcalFill");
    losTcalFill->SetUpdateRate(updateRate);
    losTcalFill->SetMinStats(minStats);
    losTcalFill->SetNofModules(20);
    run->AddTask(losTcalFill);
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

