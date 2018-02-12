
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nModules = 200;

    TString dirIn1 = "/Volumes/Data/kresan/s438/data/";
    TString dirOut = "/Volumes/Data/kresan/s438/tcal/";
    TString inputFileName1 = dirIn1 + runNumber + "_raw.root";              // name of input file
    TString parFileName    = dirIn1 + "params_" + runNumber + "_raw.root";  // name of parameter file
    TString outputFileName = dirOut + runNumber + "_tcal.root";             // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(inputFileName1));
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

    // Time calibration ----------------------------------------------------------
    R3BNeulandMapped2Cal* landTcal = new R3BNeulandMapped2Cal("NeulandMapped2Cal", 1);
    landTcal->SetNofModules(nModules, 40);
    landTcal->SetTrigger(2);
    run->AddTask(landTcal);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
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

