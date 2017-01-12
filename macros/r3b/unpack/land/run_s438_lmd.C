
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = -1;                               // number of events to read, -1 - untill CTRL+C
    TString inDir = "/Volumes/Data/kresan/s438/lmd/";   // directory with lmd files
    TString outDir = "/Volumes/Data/kresan/s438/data/"; // output directory
    TString histDir = "/Users/kresan/Sites/";           // web-server directory

    TString outputFileName = outDir + runNumber + "_raw.root";                 // name of output file
    TString histFileName = histDir + "hist_s438_" + runNumber + "_raw.root";   // name of file with control histograms
    const Int_t refresh = 100000;                                              // refresh rate for saving control histograms
    TString parFileName = outDir + "params_" + runNumber + "_raw.root";        // name of parameter file
    //const Long64_t maxSize = 1 * 1024 * 1024 * 1024;                         // 1 GByte       // file split size

    const char *landMappingName = "cfg_neuland_s438.hh";   // mapping file
    const Int_t nBarsPerPlane = 50;
    const Int_t updateRate = 150000;
    const Int_t minStats = 10000;
    const Int_t nModules = 200;

    // Create source with unpackers ----------------------------------------------
    Int_t iFile = 0;
    Int_t kFile = 0;
    if(runNumber.Contains("run180"))
    {
        iFile = 796;
        kFile = 810;
    }
    else if(runNumber.Contains("run108"))
    {
        iFile = 325;
        kFile = 328;
    }
    else if(runNumber.Contains("run107"))
    {
        iFile = 322;
        kFile = 325;
    }
    FairLmdSource* source = new FairLmdSource();
    char strName[1000];
    for(Int_t i = iFile; i < kFile; i++)
    {
        sprintf(strName, "%s%s_%4d.lmd", inDir.Data(), runNumber.Data(), i);
        for(Int_t j = 0; j < 1000; j++) if(' ' == strName[j]) strName[j] = '0';
        cout << strName << endl;
        source->AddFile(strName);
    }

    R3BEventHeaderUnpack *event_unpack = new R3BEventHeaderUnpack();
    source->AddUnpacker(event_unpack);

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
    //source->AddUnpacker(new R3BLosUnpack(type, subType, procId, subCrate, control));
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
    R3BNeulandMapped2CalPar* tcalFill = new R3BNeulandMapped2CalPar("NeulandMapped2Cal");
    tcalFill->SetUpdateRate(updateRate);
    tcalFill->SetMinStats(minStats);
    tcalFill->SetTrigger(2);
    tcalFill->SetNofModules(nModules, 40);
    run->AddTask(tcalFill);

    R3BLosTcalFill* losTcalFill = new R3BLosTcalFill("LosTcalFill");
    losTcalFill->SetUpdateRate(updateRate);
    losTcalFill->SetMinStats(minStats);
    losTcalFill->SetNofModules(20);
    //run->AddTask(losTcalFill);
    // ---------------------------------------------------------------------------

    // Add analysis task ---------------------------------------------------------
    R3BLandRawAna* ana = new R3BLandRawAna("LandRawAna", 1);
    run->AddTask(ana);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    //((TTree*)gFile->Get("cbmsim"))->SetMaxTreeSize(maxSize);
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Runtime data base ---------------------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(parFileName);
    rtdb->setOutput(parOut);
    rtdb->print();
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(nev, 0);
    rtdb->saveOutput();
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

