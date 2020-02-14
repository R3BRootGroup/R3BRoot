void R3BASCIIFileGenerator::InitializeSnFiles()
{
    // Links the class parameters to the proper Sn-datafiles
    // and does more initialization tasks so that these files 
    // can be read.
    
    // Begin by testing if the Inputs-class is properly linked:
    SnFiles_Initialized = kFALSE;
    
    if (Inputs->IsLinked()==kFALSE)
    {
        ErrorMessage("You need to provide a suitable Inputs-class if you want to initialize the Sn datafiles!");
    }
    else
    {
        // Then, obtain the required inputs:
        TString SnFilePath = Inputs->GetInputString("TheVMCFolder") + "/DNN/SnFiles/";
        TString GunType = Inputs->GetInputString("ParticleGun_ASCII_GunChoice");
        Double_t BeamEnergy = Inputs->GetAvgBeamEnergyPerNucleon();
        TString DesignatedEnergy = "";
        TString RelativeEnergy = "";
        
        if ((TMath::Abs(BeamEnergy-200.0)<0.1)&&(GunType=="SnData_100keV"))
        {
            SnFilePath = SnFilePath + "/event_200AMeV_100keV/";
            DesignatedEnergy = "200";
            RelativeEnergy = "100";
        }
        else if ((TMath::Abs(BeamEnergy-600.0)<0.1)&&(GunType=="SnData_100keV"))
        {
            SnFilePath = SnFilePath + "/event_600AMeV_100keV/";
            DesignatedEnergy = "600";
            RelativeEnergy = "100";
        }
        else if ((TMath::Abs(BeamEnergy-1000.0)<0.1)&&(GunType=="SnData_100keV"))
        {
            SnFilePath = SnFilePath + "/event_1000AMeV_100keV/";
            DesignatedEnergy = "1000";
            RelativeEnergy = "100";
        }
        else if ((TMath::Abs(BeamEnergy-200.0)<0.1)&&(GunType=="SnData_500keV"))
        {
            SnFilePath = SnFilePath + "/event_200AMeV_500keV/";
            DesignatedEnergy = "200";
            RelativeEnergy = "500";
        }
        else if ((TMath::Abs(BeamEnergy-600.0)<0.1)&&(GunType=="SnData_500keV"))
        {
            SnFilePath = SnFilePath + "/event_600AMeV_500keV/";
            DesignatedEnergy = "600";
            RelativeEnergy = "500";
        }
        else if ((TMath::Abs(BeamEnergy-1000.0)<0.1)&&(GunType=="SnData_500keV"))
        {
            SnFilePath = SnFilePath + "/event_1000AMeV_500keV/";
            DesignatedEnergy = "1000";
            RelativeEnergy = "500";
        }
        else
        {
            if ((GunType=="SnData_100keV")||(GunType=="SnData_500keV")) {ErrorMessage("Make sure that the average beam BeamEnergy per nucleon equals 200, 600 or 1000 MeV/u.");}
            else {ErrorMessage("You can only use the Sn datafiles if the GunType equals SnData_100keV or SnData_500keV.");}
        }
        
        // now, generate the arrays depending on the particle gun multiplicity:
        Include_SnNucleus = Inputs->GetInputBoolian("ParticleGun_IncludeSnNucleus_SnDataFiles");
        RandomRotateNeutrons = Inputs->GetInputBoolian("ParticleGun_RandomRotateNeutrons_SnDataFiles");
        ParticleGun_Multiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
        R3BRoot_nEvents = Inputs->GetInputInteger("R3BRoot_nEvents");
        StartReading = Inputs->GetInputInteger("ParticleGun_StartEventReading_SnDataFiles");
        EndReading = Inputs->GetInputInteger("ParticleGun_EndEventReading_SnDataFiles");
        nReadedFiles = ParticleGun_Multiplicity;
        
        // Properly process the start & end points:
        if (StartReading<0) {StartReading = 0;}
        if (StartReading>9999) {StartReading = 0;}
        if (EndReading<0) {EndReading = 9999;}
        if (EndReading>9999) {EndReading = 9999;}
        
        // Take multithreading into account:
        if (TotalNumberOfThreads>1)
        {
            if (EndReading<(StartReading-1+TotalNumberOfThreads)) 
            {
                EndReading = StartReading + TotalNumberOfThreads - 1;
            }
            if (EndReading>9999)
            {
                StartReading = StartReading - (EndReading-9999);
                EndReading = 9999;
            }
        }
        else
        {
            if (EndReading<StartReading) {EndReading = StartReading;}
        }
        
        SnDataFiles = new std::ifstream[nReadedFiles];
        SnDataFileName.resize(nReadedFiles);
        CurrentCounters.resize(nReadedFiles);
        StartCounters.resize(nReadedFiles);
        FinalCounters.resize(nReadedFiles);
        
        // Next, define the filenames and counters:
        Int_t SnA = 132;
        Int_t ThisA = SnA;
        TString ThisFilesName = "";
        TString st = "";
        Bool_t OpenTest = kTRUE;
        
        for (Int_t k = 0; k<nReadedFiles; ++k)
        {
            // Create the proper filename:
            ThisA = SnA - (k+1);
            ThisFilesName = st.Itoa(ThisA,10) + "Sn_" + st.Itoa(k+1,10) + "n_" + DesignatedEnergy + "AMeV_" + RelativeEnergy + "keV.dat";
            
            // Pass it to the fileName:
            SnDataFileName.at(k) = SnFilePath + ThisFilesName;
            
            // Try to open the file:
            SnDataFiles[k].open(SnDataFileName.at(k).Data());
            
            // Test if it worked:
            if (!(SnDataFiles[k].is_open()))
            {
                ErrorMessage("We could not open Sn DataFile <"+SnDataFileName.at(k)+">!");
                OpenTest = kFALSE;
            }
            
            // Initialize the counters:
            if (TotalNumberOfThreads>1) 
            {
                CurrentCounters[k] = CurrentThread; // NOTE: This starts at zero!
                StartCounters[k] = StartReading;
                FinalCounters[k] = EndReading;
            }
            else 
            {
                CurrentCounters[k] = 0;
                StartCounters[k] = StartReading;
                FinalCounters[k] = EndReading;
            }
        }
        
        // If everything opened succesfully, we can give the success statement:
        if (OpenTest==kTRUE) {SnFiles_Initialized = kTRUE;}
        
        // Done.
    }
}
