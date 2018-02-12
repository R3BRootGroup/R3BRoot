
void run(Int_t runNumber)
{
    TStopwatch timer;
    timer.Start();

    TString strRunNumber = "run";
    strRunNumber += runNumber;
    TString dirIn1 = "/Users/kresan/data/s438b/data/";
    TString dirIn2 = "/Users/kresan/data/s438b/data/";
    TString dirOut = "/Users/kresan/data/s438b/data/";
    TString inputFileName1 = dirIn2 + strRunNumber + "_tcal.root";             		// name of input file
    TString parFileName    = dirIn1 + "params_" + strRunNumber + "_cosmic1.root";  	// name of parameter file
    TString outputFileName = dirOut + strRunNumber + "_digi.root";            		// name of output file

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
    rtdb->addRun(runNumber);   
    rtdb->getContainer("NeulandHitPar");
    rtdb->setInputVersion(runNumber, (char*)"NeulandHitPar", 1, 2);
    // ---------------------------------------------------------------------------

    // cal2hit ---------------------------------------------------------
    R3BNeulandCal2Hit* neulandCal2Hit= new R3BNeulandCal2Hit("NeulandCal2Hit", 1);
    //neulandCal2Hit->SetFirstPlaneHorisontal();
    run->AddTask(neulandCal2Hit);
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

