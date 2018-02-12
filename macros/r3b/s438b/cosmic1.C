
void run(Int_t runNumber)
{
    TStopwatch timer;
    timer.Start();
    
    TString strRunNumber = "run";
    strRunNumber += runNumber;
    TString dirIn1 = "/Users/kresan/data/s438b/data/";
    TString dirIn2 = "/Users/kresan/data/s438b/tcal/";
    TString dirOut = "/Users/kresan/data/s438b/cosmic1/";
    TString inputFileName1 = dirIn2 + strRunNumber + "_tcal.root";              	// name of input file
    TString parFileName    = dirIn1 + "params_" + strRunNumber + "_raw.root";  		// name of parameter file
    TString outParFileName = dirOut + "params_" + strRunNumber + "_cosmic1.root";  	// name of parameter file
    TString outputFileName = dirOut + strRunNumber + "_cosmic1.root";             	// name of output file

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
    FairParRootFileIo* parIo2 = new FairParRootFileIo(kParameterMerged);
    parIo2->open(outParFileName);
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo2);
    rtdb->getContainer("LandTCalPar")->setChanged();
    // ---------------------------------------------------------------------------

    // Cosmic1----------------------------------------------------------------------
    R3BNeulandCal2HitPar* cosmic1 = new R3BNeulandCal2HitPar("cosmic1");
    cosmic1->SetPlanes(8);
    cosmic1->SetErrorTH(1.0);
    cosmic1->SetDeviationTH(1.0);
    cosmic1->SetMinEventQDC(100);
    run->AddTask(cosmic1);
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

