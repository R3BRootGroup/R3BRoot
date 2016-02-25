
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = -1;                                // number of events to read, -1 - untill CTRL+C
    const Int_t trigger = 2;                             // 1 - onspill, 2 - offspill. -1 - all
    TString inDir = "/Users/kresan/data/s438b/lmd/";     // directory with lmd files
    TString outDir = "/Users/kresan/data/s438b/data/";   // output directory
    TString histDir = "/Users/kresan/Sites/";            // web-server directory

    TString outputFileName = outDir + runNumber + "_raw.root";                  // name of output file
    TString histFileName = histDir + "hist_s438b_" + runNumber + "_raw.root";   // name of file with control histograms
    const Int_t refresh = 100000;                                               // refresh rate for saving control histograms
    TString parFileName = outDir + "params_" + runNumber + "_raw.root";         // name of parameter file
    //const Long64_t maxSize = 1 * 1024 * 1024 * 1024;                          // 1 GByte       // file split size

    const char *landMappingName = "cfg_neuland_s438b.hh";   // mapping file
    const Int_t nBarsPerPlane = 50;                         // number of scintillator bars per plane

    // Create source with unpackers ----------------------------------------------
    FairLmdSource* source = new FairLmdSource();
    source->AddPath(inDir, runNumber+"*");

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
}

