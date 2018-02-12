
void run(Int_t runNumber, Int_t cosmicRunNumber, Int_t pulsarRunNumber)
{
    TStopwatch timer;
    timer.Start();

    TString strRunNumber = "run";
    strRunNumber += runNumber;
    TString strCosmicRunNumber = "run";
    strCosmicRunNumber += cosmicRunNumber;
    TString strPulsarRunNumber = "run";
    strPulsarRunNumber += pulsarRunNumber;
    const Int_t nModules = 800;
    const Int_t trigger = 1;              // 1 - onspill, 2 - offspill. -1 - all

    TString dirIn1 = "/Users/kresan/data/s438b/data/";
    TString dirOut = "/Users/kresan/data/s438b/tcal/";
    TString inputFileName1 = dirIn1 + strRunNumber + "_raw.root";              			// name of input file
    TString parFileName    = dirIn1 + "params_" + strRunNumber + "_raw.root";  			// name of parameter file
TString cosmicParFileName    = dirIn1 + "params_" + strCosmicRunNumber + "_cosmic1.root";  	// name of cosmic parameter file
    TString pulsarParFileName    = dirIn1 + "params_" + strPulsarRunNumber + "_QCal.root";  	// name of pulsar parameter file
    TString outputFileName = dirOut + strRunNumber + "_tcal_land.root";             		// name of output file

    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(inputFileName1));
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // ----- Runtime DataBase info -----------------------------------------------
    Bool_t kParameterMerged = kTRUE;
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo(kParameterMerged);
    TList *parlist = new TList();
    parlist->Add(new TObjString(parFileName));
    parlist->Add(new TObjString(cosmicParFileName));
    parlist->Add(new TObjString(pulsarParFileName));
    parIo1->open(parlist);
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    rtdb->addRun(runNumber);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(runNumber, (char*)"LandTCalPar", 1, 1);    
    rtdb->getContainer("NeulandQCalPar");
    rtdb->setInputVersion(runNumber, (char*)"NeulandQCalPar", 1, 1);
    rtdb->getContainer("NeulandHitPar");
    rtdb->setInputVersion(runNumber, (char*)"NeulandHitPar", 1, 1);
    // ---------------------------------------------------------------------------

    // Time calibration ----------------------------------------------------------
    R3BNeulandMapped2Cal* landTcal = new R3BNeulandMapped2Cal("LandTcal", 1);
    landTcal->SetTrigger(trigger);
    landTcal->SetNofModules(nModules);
    landTcal->EnableWalk(kTRUE);
    run->AddTask(landTcal);

    R3BLosMapped2Cal* losTcal = new R3BLosMapped2Cal("LosTcal", 1);
    losTcal->SetNofModules(20, 4);
    run->AddTask(losTcal);
    // ---------------------------------------------------------------------------

    //R3BNeulandCalTest *testTcal = new R3BNeulandCalTest("TestTcal", 1);
    //run->AddTask(testTcal);

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

