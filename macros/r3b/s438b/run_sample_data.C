
typedef struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_raw_nnp_onion_t nnp;
} EXT_STR_h101;

void run_sample_data()
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = -1;                                // number of events to read, -1 - untill CTRL+C
    const Int_t trigger = -1;                            // 1 - onspill, 2 - offspill. -1 - all

    TString filename = "sample_data_2.lmd";
    TString outputFileName = "output_raw_land.root";     // name of output file
    const Int_t refresh = 100000;                        // refresh rate for saving control histograms
    TString parFileName = "params_raw_land.root";        // name of parameter file

    const Int_t updateRate = 150000;
    const Int_t minStats = 10000;                           // minimum number of entries for TCAL calibration
    const Int_t nModules = 800;                             // number of photomultipliers (for TCAL calibration)

    // Create source with unpackers ----------------------------------------------
    TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
    TString ucesb_dir = getenv("UCESB_DIR");
    TString ucesb_path = ucesb_dir + "/../upexps/s438b/s438b";
    
    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);
    source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BNeulandTacquilaReader((EXT_STR_h101_raw_nnp*)&ucesb_struct.nnp, offsetof(EXT_STR_h101, nnp)));
    // ---------------------------------------------------------------------------

    // Create online run ---------------------------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile(outputFileName.Data());
    run->SetRunId(1111);
    // ---------------------------------------------------------------------------

    // Create ALADIN field map ---------------------------------------------------
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    Double_t fMeasCurrent = 2500.; // I_current [A]
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(1.);
    run->SetField(magField);
    // ---------------------------------------------------------------------------

    // TCAL ----------------------------------------------------------------------
    R3BNeulandMapped2CalPar* tcalFill = new R3BNeulandMapped2CalPar("TcalFill");
    tcalFill->SetUpdateRate(updateRate);
    tcalFill->SetMinStats(minStats);
    tcalFill->SetTrigger(trigger);
    tcalFill->SetNofModules(nModules);
    run->AddTask(tcalFill);
    // ---------------------------------------------------------------------------

    // Add analysis task ---------------------------------------------------------
    R3BNeulandMappedHist* ana = new R3BNeulandMappedHist("LandRawAna", 1);
    run->AddTask(ana);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
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
    if(nev < 0)
    {
        run->Run(nev, 0);
    }
    else
    {
        run->Run(0, nev);
    }
    rtdb->saveOutput();
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Parameter file is " << parFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;

	if(ana->GetNItemsTotal() > 3800)
	{
		cout << " Test passed" << endl;
		cout << " All ok " << endl;
	}
}

