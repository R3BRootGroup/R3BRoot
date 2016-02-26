
void run(TString runNumber)
{
    TStopwatch timer;
    timer.Start();

    TString dirIn1 = "/Volumes/Data/kresan/s438b/data/";
    TString dirOut = "/Volumes/Data/kresan/s438b/data/";
    TString inputFileName1 = dirIn1 + runNumber + "_tcal.root";              // name of input file
    TString parFileName    = dirIn1 + "params_" + runNumber + "_raw.root";  // name of parameter file
    TString outputFileName = dirOut + runNumber + "_cosmic1.root";             // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(inputFileName1.Data());
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // ----- Runtime DataBase info -----------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parIo1 = new FairParRootFileIo(kParameterMerged);
    parIo1->open(parFileName);
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    // ---------------------------------------------------------------------------

    // Cosmic1----------------------------------------------------------------------
    R3BLandCosmic1* cosmic1 = new R3BLandCosmic1("cosmic1");
    cosmic1->SetMaxPaddleDistFromLine(1);
    cosmic1->SetPlanes(8);
    run->AddTask(cosmic1);
    // ---------------------------------------------------------------------------
    
    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run();
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

