
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = 300;                               // number of events to read, -1 - untill CTRL+C
    const Int_t trigger = 1;                             // 1 - onspill, 2 - offspill. -1 - all
    TString inDir = "/Volumes/Data/kresan/s438b/lmd/";   // directory with lmd files

    TString outputFileName = runNumber + "_raw.root";                  // name of output file
    TString parFileName = "params_" + runNumber + "_raw.root";         // name of parameter file

    // Create source with unpackers ----------------------------------------------
    FairLmdSource* source = new FairLmdSource();
    source->AddPath(inDir, runNumber+"*");

    R3BEventHeaderUnpack *event_unpack = new R3BEventHeaderUnpack();
    source->AddUnpacker(event_unpack);
    // ---------------------------------------------------------------------------

    
    // TFW MBS parameters -------------------------------
    Short_t type = 88;
    Short_t subType = 8800;
    Short_t procId = 12;
    Short_t subCrate = 2;
    Short_t control = 9;
    source->AddUnpacker(new R3BTofUnpack(type, subType, procId, subCrate, control));
    // ------------------------------------------------------
    
    
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

    // Initialize ----------------------------------------------------------------
    run->Init();
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
    run->Run(0, nev);
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

