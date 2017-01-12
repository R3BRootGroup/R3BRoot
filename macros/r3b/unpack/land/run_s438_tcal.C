
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = 1000000;                       // number of events to read, -1 - untill CTRL+C
    TString dir = "/Volumes/Data/kresan/s438/data/"; // output directory
    TString histDir = "/Users/kresan/webdocs/";      // web-server directory

    const char* node = "lxi047"; // remote-event server

    TString outputFileName = dir + runNumber + "_tcal.root";                  // name of output file
    TString histFileName = histDir + "hist_s438_" + runNumber + "_tcal.root"; // name of file with control histograms
    const Int_t refresh = 10000;                                              // refresh rate for saving control histograms
    TString parFileName = dir + "params_" + runNumber + "_tcal.root";         // name of parameter file
    const Long64_t maxSize = 100 * 1024 * 1024;                               // 100 MByte             // file split size

    const char *landMappingName = "cfg_neuland_s438.hh";   // mapping file
    const Int_t nBarsPerPlane = 50;
    const Int_t updateRate = 150000;
    const Int_t minStats = 10000;
    const Int_t nModules = 200;

    // Create source with unpackers ----------------------------------------------
    FairRemoteSource* source = new FairRemoteSource(node);

    // NeuLAND MBS parameters -------------------------------
    Short_t type = 94;
    Short_t subType = 9400;
    Short_t procId = 10;
    Short_t subCrate = 1;
    Short_t control = 3;
    source->AddUnpacker(new R3BLandUnpack(type, subType, procId, subCrate, control));
    // ------------------------------------------------------

    // LOS MBS parameters -----------------------------------
    type = 88;
    subType = 8800;
    procId = 10;
    subCrate = 7;
    control = 5;
    source->AddUnpacker(new R3BLosUnpack(type, subType, procId, subCrate, control));
    // ------------------------------------------------------
    // ---------------------------------------------------------------------------

    // Create online run ---------------------------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile(outputFileName.Data());
    run->SetGenerateHtml(kTRUE, histFileName.Data(), refresh);
    // ---------------------------------------------------------------------------

    // Create ALADIN field map ---------------------------------------------------
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    Double_t fMeasCurrent = 2500.; // I_current [A]
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(1.);
    run->SetField(magField);
    // ---------------------------------------------------------------------------

    // Channel mapping -----------------------------------------------------------
    R3BLandMapping* map = new R3BLandMapping();
    map->SetFileName(landMappingName);
    map->SetNofBarsPerPlane(nBarsPerPlane);
    run->AddTask(map);
    // ---------------------------------------------------------------------------

    // TCAL ----------------------------------------------------------------------
    R3BNeulandMapped2CalPar* tcalFill = new R3BNeulandMapped2CalPar("TcalFill");
    tcalFill->SetUpdateRate(updateRate);
    tcalFill->SetMinStats(minStats);
    tcalFill->SetNofModules(nModules, 20);
    run->AddTask(tcalFill);

    R3BLosTcalFill* losTcalFill = new R3BLosTcalFill("LosTcalFill");
    losTcalFill->SetUpdateRate(updateRate);
    losTcalFill->SetMinStats(minStats);
    losTcalFill->SetNofModules(20);
    run->AddTask(losTcalFill);
    // ---------------------------------------------------------------------------

    // Add analysis task ---------------------------------------------------------
    R3BLandRawAna* ana = new R3BLandRawAna("LandRawAna", 1);
    run->AddTask(ana);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    ((TTree*)gFile->Get("cbmsim"))->SetMaxTreeSize(maxSize);
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Runtime data base ---------------------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();

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

    // Run -----------------------------------------------------------------------
    run->Run(nev, 0);
    delete run;
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
