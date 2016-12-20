
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    const Int_t trigger = -1;              // 1 - onspill, 2 - offspill. -1 - all

    TString dirIn1 = "/u/mheil/data/neuland_tamex/mapped/";
    TString dirIn2 = "/u/mheil/data/neuland_tamex/par/";
    TString dirOut = "/u/mheil/data/neuland_tamex/tcal/";
    TString inputFileName1 = dirIn1 + runNumber + "_mapped_neuland.root";              // name of input file
    TString parFileName    = dirIn2 + "params_" + runNumber + "_mapped_neuland.root";  // name of parameter file
    TString outputFileName = dirOut + runNumber + "_tcal_neuland.root";             // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inputFileName1.Data());
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
    R3BNeulandTcal* neulandTcal = new R3BNeulandTcal("LandTcal", 1);
    neulandTcal->SetTrigger(trigger);
    neulandTcal->SetNofModules(1, 50, 0);
    run->AddTask(neulandTcal);

/*
    R3BLosTcal* losTcal = new R3BLosTcal("LosTcal", 1);
    losTcal->SetNofModules(20);
    run->AddTask(losTcal);
 */
    // ---------------------------------------------------------------------------

<<<<<<< HEAD
    R3BNeulandCalTest *testTcal = new R3BNeulandCalTest("TestTcal", 1);
=======
    R3BLandTcalTest *testTcal = new R3BLandTcalTest("TestTcal", 1);
>>>>>>> Added: Tamex reader. Geometry and macro for s2018.
    run->AddTask(testTcal);

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

