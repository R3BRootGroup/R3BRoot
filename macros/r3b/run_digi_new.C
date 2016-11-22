void run_digi_new()
{
    // ----- Files ---------------------------------------------------------------
    TString inFile = "r3bsim_new.root";
    TString parFile = "r3bpar_new.root";
    TString outFile = "r3bhits_new.root";
    // ---------------------------------------------------------------------------

    // ----- Timer ---------------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ---------------------------------------------------------------------------

    // ----- Digitization --------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inFile);
    run->SetOutputFile(outFile);
    // ---------------------------------------------------------------------------

    // ----- Connect the Digitization Task ---------------------------------------
    R3BCaloHitFinder* califa_digitizer = new R3BCaloHitFinder();
    run->AddTask(califa_digitizer);
    
    // mTOF
    R3BmTofDigitizer* mtof_digitizer = new R3BmTofDigitizer();
    run->AddTask(mtof_digitizer);

    // STaRTrack
    R3BSTaRTraHitFinder* tra_digitizer = new R3BSTaRTraHitFinder();
    run->AddTask(tra_digitizer);
    
    // MFI
    R3BMfiDigitizer* mfi_digitizer = new R3BMfiDigitizer();
    run->AddTask(mfi_digitizer);
    
    // PSP
    R3BPspDigitizer* psp_digitizer = new R3BPspDigitizer();
    run->AddTask(psp_digitizer);
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
