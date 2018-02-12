
void run(Int_t runNumber, Int_t refRun)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nModules = 800;
    const Int_t trigger = 2;              // 1 - onspill, 2 - offspill. -1 - all

    TString strRunNumber = "run";
    strRunNumber += runNumber;
    TString strRefRun = "run";
    strRefRun += refRun;
    TString dirIn1 = "/Users/kresan/data/s438b/data/";
    TString dirOut = "/Users/kresan/data/s438b/tcal/";
    TString inputFileName1 = dirIn1 + strRunNumber + "_raw.root";              // name of input file
    TString parFileName    = dirIn1 + "params_" + strRunNumber + "_raw.root";  // name of parameter file
    TString parFileName2   = dirIn1 + "params_" + strRefRun + "_raw.root";     // name of parameter file
    TString outputFileName = dirOut + strRunNumber + "_tcal.root";             // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(inputFileName1));
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // ----- Runtime DataBase info -----------------------------------------------
    Bool_t kParameterMerged = kTRUE;
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo(kParameterMerged);
    parIo1->open(parFileName);
    rtdb->setFirstInput(parIo1);
    FairParRootFileIo* parIo2 = new FairParRootFileIo(kParameterMerged);
    parIo2->open(parFileName2);
    rtdb->setSecondInput(parIo2);
    rtdb->addRun(runNumber);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(runNumber, (char*)"LandTCalPar", 1, 2);
    rtdb->print();
    // ---------------------------------------------------------------------------

    // Time calibration ----------------------------------------------------------
    R3BNeulandMapped2Cal* landTcal = new R3BNeulandMapped2Cal("LandTcal", 1);
    landTcal->SetTrigger(trigger);
    landTcal->SetNofModules(nModules);
    landTcal->SetPulserMode(kTRUE);
    landTcal->EnableWalk(kFALSE);
    run->AddTask(landTcal);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    rtdb->print();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run();
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

