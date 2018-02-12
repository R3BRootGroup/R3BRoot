void run(Int_t runNumber)
{
    TStopwatch timer;
    timer.Start();
    
    TString strRunNumber = "run";
    strRunNumber += runNumber; 
    TString inDir = "/Users/kresan/data/s438b/data/";     // directory with lmd files
    TString outDir = "/Users/kresan/data/s438b/data/";   // output directory  
    TString inputFileName1 = inDir + strRunNumber + "_raw.root";              // name of input file
    TString parFileName    = inDir + "params_" + strRunNumber + "_QCal.root";  // name of parameter file
    TString outputFileName = inDir + strRunNumber + "_QCal.root";             // name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(inputFileName1));
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
    R3BNeulandMapped2QCalPar* pedpar = new R3BNeulandMapped2QCalPar();
    pedpar->SetPlanes(8);
    run->AddTask(pedpar);
    // ---------------------------------------------------------------------------
    
    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run();
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

