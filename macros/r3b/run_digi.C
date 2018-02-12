
void RemoveGeoManager();

void run_digi()
{
    // ----- Files ---------------------------------------------------------------
    TString inFile = "sim.root";
    TString parFile = "par.root";
    TString outFile = "digi.root";
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
    R3BCalifaCrystalCal2Hit* califa_digitizer = new R3BCalifaCrystalCal2Hit();
    run->AddTask(califa_digitizer);

    // STaRTrack

    //R3BStartrackHitFinder* tra_digitizer = new R3BStartrackHitFinder();  // R3BStartrackHitFinder() is obsolete
    R3BStartrackDigit* tra_digitizer = new R3BStartrackDigit();

    run->AddTask(tra_digitizer);

    // PSP
    R3BPspDigitizer* psp_digitizer = new R3BPspDigitizer();
    run->AddTask(psp_digitizer);

    // Fi4,5,6
    R3BFi4Digitizer* Fi4_digitizer = new R3BFi4Digitizer(0.001,0.01,1);
    run->AddTask(Fi4_digitizer);

    // sfi
    R3BsfiDigitizer* sfi_digitizer = new R3BsfiDigitizer(0.001,0.01);
    run->AddTask(sfi_digitizer);

    // dTOF
    R3BdTofDigitizer* dtof_digitizer = new R3BdTofDigitizer();
    run->AddTask(dtof_digitizer);
    dtof_digitizer->SetSigma_y( 1. );
    dtof_digitizer->SetSigma_t( 0.03 );
    dtof_digitizer->SetSigma_ELoss( 0.014  ); //0.0015
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

    RemoveGeoManager();
}

/**
 * \ function RemoveGeoManager
 * There are some problems when deleting our geometries. In some cases
 * or combinations of geometries there is a double free of some memory
 * which results in a crash of ROOT. To avoid this we have patched one
 * ROOT class. With the newest ROOT6 version this isn't done any longer.
 * As a workaround to avoid the crash we delete two TObjArrays ourself
 * and then call the destructor of the TGeoManager at the end of the
 * macro. To simplify this one also can use this function.
 */
void RemoveGeoManager()
{
  if (gROOT->GetVersionInt() >= 60602) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
}
