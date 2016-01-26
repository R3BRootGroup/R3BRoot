
void run_sample_data()
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = -1;                                // number of events to read, -1 - untill CTRL+C
    const Int_t trigger = -1;                            // 1 - onspill, 2 - offspill. -1 - all

    TString outputFileName = "output_raw_land.root";     // name of output file
    const Int_t refresh = 100000;                        // refresh rate for saving control histograms
    TString parFileName = "params_raw_land.root";        // name of parameter file

    const char *landMappingName = "cfg_neuland_s438b.hh";   // mapping file
    const Int_t nBarsPerPlane = 50;                         // number of scintillator bars per plane
    const Int_t updateRate = 150000;
    const Int_t minStats = 10000;                           // minimum number of entries for TCAL calibration
    const Int_t nModules = 800;                             // number of photomultipliers (for TCAL calibration)

    // Create source with unpackers ----------------------------------------------
    FairLmdSource* source = new FairLmdSource();
    source->AddFile("sample_data_2.lmd");

    R3BEventHeaderUnpack *event_unpack = new R3BEventHeaderUnpack();
    source->AddUnpacker(event_unpack);

    // NeuLAND MBS parameters -------------------------------
    Short_t type = 94;
    Short_t subType = 9400;
    Short_t procId = 12;
    Short_t subCrate = 0;
    Short_t control = 3;
    source->AddUnpacker(new R3BLandUnpack(type, subType, procId, subCrate, control));
    // ------------------------------------------------------

    // LOS MBS parameters -----------------------------------
    type = 88;
    subType = 8800;
    procId = 12;
    subCrate = 1;
    control = 9;
    source->AddUnpacker(new R3BLosUnpack(type, subType, procId, subCrate, control));
    // ------------------------------------------------------
    // ---------------------------------------------------------------------------

    // Create online run ---------------------------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile(outputFileName.Data());
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
    R3BLandTcalFill* tcalFill = new R3BLandTcalFill("TcalFill");
    tcalFill->SetUpdateRate(updateRate);
    tcalFill->SetMinStats(minStats);
    tcalFill->SetTrigger(trigger);
    tcalFill->SetNofModules(nModules, 50);
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

	if(ana->GetNItemsTotal() > 8000)
	{
		cout << " Test passed" << endl;
		cout << " All ok " << endl;
	}
}

