
void run(Int_t runNumber , Int_t cosmicRunNumber)
{
    TStopwatch timer;
    timer.Start();

    TString strRunNumber = "run";
    strRunNumber += runNumber;
    TString strCosmicRunNumber = "run";
    strCosmicRunNumber += cosmicRunNumber;    
    TString dirIn1 = "/Users/kresan/data/s438b/data/";
    TString dirIn2 = "/Users/kresan/data/s438b/data/";
    TString dirOut = "/Users/kresan/data/s438b/data/";

    TString inputFileName1 = dirIn2 + strRunNumber + "_tcal_land.root";             		// name of input file
    TString parFileName1   = dirIn1 + "params_" + strRunNumber + "_raw.root";  			// name of parameter file
    TString parFileName2   = dirIn1 + "params_" + strCosmicRunNumber + "_cosmic1.root";         // name of parameter file
    TString outputFileName = dirOut + strRunNumber + "_digi.root";                  		// name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(inputFileName1));
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // ----- Runtime DataBase info -----------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parIo1 = new FairParRootFileIo(kParameterMerged);
    parIo1->open(parFileName1);
    rtdb->setFirstInput(parIo1);
    FairParRootFileIo* parIo2 = new FairParRootFileIo(kParameterMerged);
    parIo2->open(parFileName2);
    rtdb->setSecondInput(parIo2);
    rtdb->addRun(runNumber);
    rtdb->getContainer("NeulandHitPar");
    rtdb->setInputVersion(runNumber, (char*)"NeulandHitPar", 1, 2);
    // ---------------------------------------------------------------------------

    // R3BNeulandCal2Hit ---------------------------------------------------------
    R3BNeulandCal2Hit* neulandCal2Hit= new R3BNeulandCal2Hit("NeulandCal2Hit", 0);
    neulandCal2Hit->SetFirstPlaneHorisontal();
    neulandCal2Hit->SetDistanceToTarget(1300.);
    run->AddTask(neulandCal2Hit);
    // ---------------------------------------------------------------------------
    
    // R3BLosCal2Hit -------------------------------------------------------------
    R3BLosCal2Hit* loscal2hit = new R3BLosCal2Hit("R3BLosCal2Hit",0);
    run->AddTask(loscal2hit);
    // ---------------------------------------------------------------------------

    // Analysis ------------------------------------------------------------------
    R3BNeulandHitHist* ana = new R3BNeulandHitHist("LandAna",1);
    ana->SetNofBars(400);
    ana->SetFirstPlaneHorisontal();
    ana->SetTimeOffset(1516);
    ana->SetDistance(14);
    run->AddTask(ana);
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
    cout << "Parameter file is " << parFileName1 << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

