inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void NeuLAND_ExpTranslation(Bool_t const ThisIsBackground = kFALSE)
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Retrieve the required inputs:
    TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
    TString SignalFile = TheOutputPath + Inputs->GetInputString("NeuLAND_SignalFile");
    TString ParFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_ParFile");
    Double_t AvgBeamEnergy = Inputs->GetAvgBeamEnergyPerNucleon();
    TString DataFolder = "250MeV";
    if (TMath::Abs(AvgBeamEnergy-109.1)<10.0) {DataFolder = "110MeV";}
    
    // Define the task:
    R3BExpTranslator_7LiPN_250MeV* Translator = new R3BExpTranslator_7LiPN_250MeV();
    //Translator->SetTestMode();
    Translator->SetRealMode();
    if (ThisIsBackground==kTRUE) {Translator->SetBackgroundMode();}
    else                         {Translator->SetSignalMode();}
    Translator->LinkInputClass(Inputs);
    Translator->ConnectToInputFiles(DataFolder);
    Int_t nEvents = Translator->GetNumberOfEvents();
    Translator->UpdateInputs();
    
    // Then, execute the macro if the Inputs Manager has no errors:
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Activate the timer:
        TStopwatch timer;
        timer.Start();

        // Create the Master FairTask:
        FairRunAna* run = new FairRunAna();
        run->SetOutputFile(SignalFile);
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
            
        // Add the translators to the Mother FairTask:
        run->AddTask(Translator);

        // Run:
        run->Init();
        cout << "\n\n ===> THE RUN WILL CONTAIN " << nEvents << " EVENTS!\n\n";
        run->Run(0,nEvents);

        // Finish the timer:
        timer.Stop();

        // Give final output:
        cout << endl;
        cout << "Macro finished succesfully!" << endl;
        cout << "Output file written: " << SignalFile << endl;
        cout << "Parameter file written: " << ParFile << endl;
        cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
        cout << endl;
    }
    else
    {
        // Then, print what has gone wrong:
        Inputs->PrintAllErrors();
    }
}
