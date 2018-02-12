void run_aladin_digi()
{
    // ----- Files ---------------------------------------------------------------
    TString inFile = "aladin_sim.root";
    TString parFile = "aladin_par.root";
    TString outFile = "aladin_digi.root";
    // ---------------------------------------------------------------------------

    // ----- Timer ---------------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ---------------------------------------------------------------------------

    // ----- Digitization --------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(inFile));
    run->SetOutputFile(outFile);
    // ---------------------------------------------------------------------------

    // ----- Connect the Digitization Task ---------------------------------------
    // TOF
    R3BTof2pDigitizer* tof_digitizer = new R3BTof2pDigitizer();
    run->AddTask(tof_digitizer);

    // mTOF
    R3BmTofDigitizer* mtof_digitizer = new R3BmTofDigitizer();
    run->AddTask(mtof_digitizer);

    // DCH
    R3BDch2pDigitizer* dch_2pdigitizer = new R3BDch2pDigitizer();
    run->AddTask(dch_2pdigitizer);

    // DCH
    R3BDchDigitizer* dch_digitizer = new R3BDchDigitizer(1);
    run->AddTask(dch_digitizer);

    // Tracker
    R3BTra2pDigitizer* tra_digitizer = new R3BTra2pDigitizer();
    run->AddTask(tra_digitizer);

    // GFI
    R3BGfiDigitizer* gfi_digitizer = new R3BGfiDigitizer();
    run->AddTask(gfi_digitizer);

    // MFI
    R3BMfiDigitizer* mfi_digitizer = new R3BMfiDigitizer();
    run->AddTask(mfi_digitizer);
    // ---------------------------------------------------------------------------

    // ----- Runtime DataBase info -----------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    parIo1->open(parFile.Data());
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    // ---------------------------------------------------------------------------

    // ----- Intialise and run ---------------------------------------------------
    run->Init();
    run->Run();
    delete run;
    // ---------------------------------------------------------------------------

    // ----- Finish --------------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file writen:  " << outFile << endl;
    cout << "Parameter file writen " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    cout << " Test passed" << endl;
    cout << " All ok " << endl;
    cout << " Digitization successful." << endl;
    // ---------------------------------------------------------------------------
}
