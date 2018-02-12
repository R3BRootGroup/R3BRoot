
void RemoveGeoManager();

void run_digi_Fi4()
{
    // ----- Files ---------------------------------------------------------------
    TString inFile = "sim2.root";
    TString parFile = "par2.root";
    TString outFile = "hits2.root";
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
    /*R3BCalifaCrystalCal2Hit* califa_digitizer = new R3BCalifaCrystalCal2Hit();
    run->AddTask(califa_digitizer);
    
    // mTOF
    R3BmTofDigitizer* mtof_digitizer = new R3BmTofDigitizer();
    run->AddTask(mtof_digitizer);

    // STaRTrack
    //R3BSTaRTraHitFinder* tra_digitizer = new R3BSTaRTraHitFinder(); // obsolete
    R3BStartrackDigit* tra_digitizer = new R3BStartrackDigit();
    run->AddTask(tra_digitizer);
    
    // MFI
    R3BMfiDigitizer* mfi_digitizer = new R3BMfiDigitizer();
    run->AddTask(mfi_digitizer);
    
    // PSP
    R3BPspDigitizer* psp_digitizer = new R3BPspDigitizer();
    run->AddTask(psp_digitizer);*/

    // Fi4,5,6
    R3BFi4Digitizer* Fi4_digitizer = new R3BFi4Digitizer(0.001,0.01,1);
    run->AddTask(Fi4_digitizer);
    
    // sfi
    R3BsfiDigitizer* sfi_digitizer = new R3BsfiDigitizer(0.001,0.01);
    run->AddTask(sfi_digitizer);
    
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
