void R3BTextFileGenerator::NormClassDefinition()
{
    // Defines the normalization classes. Call this function
    // in the Init()-fnction of the FairTask.
    
    // Extract the data-driven time normalizations in the case of training:
    Double_t DataDriven_MeanTime = 0.0;
    Double_t DataDriven_ScaleTime = 1.0;
    
    if (ValidationMode==kFALSE)
    {
        TFile* InputFile = (TFile*) ioman->GetInFile();
        TH1D* AllTimes;
        AllTimes = (TH1D*) InputFile->Get("AllTimes");  
        if ((AllTimes==0)||(AllTimes==nullptr)) {AllTimes = (TH1D*) InputFile->Get("AllTimes_Merged");}
        TH1D* PrimTimes;
        PrimTimes = (TH1D*) InputFile->Get("PrimTimes"); 
        if ((PrimTimes==0)||(PrimTimes==nullptr)) {PrimTimes = (TH1D*) InputFile->Get("PrimTimes_Merged");}
        
        if (DataTimeNormaliation_UsePrimTimes==kFALSE)
        {
            DataDriven_MeanTime = AllTimes->GetMean();
            DataDriven_ScaleTime = 2.0*AllTimes->GetStdDev();
        }
        else
        {
            DataDriven_MeanTime = PrimTimes->GetMean();
            DataDriven_ScaleTime = 2.0*PrimTimes->GetStdDev();
        }
    
        // Check for the travel time:
        Double_t Distance = TMath::Sqrt(TMath::Power(TMath::Abs(Inputs->GetInputDouble("ParticleGun_x_position","cm") - Inputs->GetInputDouble("TARGET_center_x_position","cm")),2.0) + 
                                    TMath::Power(TMath::Abs(Inputs->GetInputDouble("ParticleGun_y_position","cm") - Inputs->GetInputDouble("TARGET_center_y_position","cm")),2.0) + 
                                    TMath::Power(TMath::Abs(Inputs->GetInputDouble("ParticleGun_z_position","cm") - Inputs->GetInputDouble("TARGET_center_z_position","cm")),2.0));
    
        Distance = Distance + TMath::Sqrt(TMath::Power(TMath::Abs(Inputs->GetInputDouble("NeuLAND_center_x_position","cm") - Inputs->GetInputDouble("TARGET_center_x_position","cm")),2.0) + 
                                      TMath::Power(TMath::Abs(Inputs->GetInputDouble("NeuLAND_center_y_position","cm") - Inputs->GetInputDouble("TARGET_center_y_position","cm")),2.0) + 
                                      TMath::Power(TMath::Abs(Inputs->GetInputDouble("NeuLAND_front_z_position","cm")  - Inputs->GetInputDouble("TARGET_center_z_position","cm")),2.0));
    
        Double_t BeamBeta = Inputs->GetAvgBeamBeta();
        Double_t cLight = 29.9792458;
        Double_t TravelTime = Distance/(cLight*BeamBeta);
    
        if (TravelTime<(DataDriven_MeanTime-DataDriven_ScaleTime))
        {
            Double_t LeftTime = TravelTime - 0.1*DataDriven_ScaleTime;
            Double_t RightTime = DataDriven_MeanTime + DataDriven_ScaleTime;
            DataDriven_MeanTime = 0.5*(LeftTime+RightTime);
            DataDriven_ScaleTime = 0.5*TMath::Abs(RightTime-LeftTime);
        }
    
        // Write them to a file:
        std::ofstream TimeFile = std::ofstream(ParentOutputPath + "/TimeFile.txt", std::ofstream::out);
        TimeFile << DataDriven_MeanTime << "\n";
        TimeFile << DataDriven_ScaleTime << "\n";
        TimeFile.close();
    }
    else
    {
        // Then, collect them from a file, if we do not have training:
        ifstream TimeFile;
        TimeFile.open(ParentOutputPath + "/TimeFile.txt");
        
        if (TimeFile.is_open())
        {
            TimeFile >> DataDriven_MeanTime;
            TimeFile >> DataDriven_ScaleTime;
        }
        
        TimeFile.close();
    }
    
    cout << "\n\n ==> Time Normalization: Mean [ns] = " << DataDriven_MeanTime << " && Scale [ns] = " << DataDriven_ScaleTime << "\n\n";
    
    // Check inputs:
    if (Inputs->IsLinked()==kTRUE)
    {
        // Then, we are OK.
        
        // Declare application of advanced norms:
        ApplyAdvancedNorms = new NormalizationClass();
        ApplyAdvancedNorms->LinkInputClass(Inputs);
        ApplyAdvancedNorms->LinkScorers(TheScorers);
        ApplyAdvancedNorms->SetDataDrivenTimes(DataDriven_MeanTime,DataDriven_ScaleTime);
        
        // Read the file:
        Bool_t ReadResult = ApplyAdvancedNorms->ReadNormFile(FileName);
        
        if (ReadResult==kTRUE)
        {
            // Then, we are OK:
            if (ExecuteNormalization==kTRUE) {ApplyAdvancedNorms->PerformNormalization();}
            else                             {ApplyAdvancedNorms->SkipNormalization();}
        }
        else
        {
            // Then, we use an emergency situation:
            ApplyAdvancedNorms->ClearParameters();
            ApplyAdvancedNorms->BlankDefaultNorms(Input_ISNO,Input_ISYES);
            ApplyAdvancedNorms->SkipNormalization();
        }
        
        // Declare learning of advanced norms:
        LearnAdvancedNorms = new NormalizationClass();
        LearnAdvancedNorms->LinkInputClass(Inputs);
        LearnAdvancedNorms->SetDataDrivenTimes(DataDriven_MeanTime,DataDriven_ScaleTime);
        LearnAdvancedNorms->LinkScorers(TheScorers);
        LearnAdvancedNorms->BlankDefaultNorms(Input_ISNO,Input_ISYES);
        LearnAdvancedNorms->SkipNormalization();
        
        // And declare the simple default norms:
        DefaultNorms = new NormalizationClass();
        DefaultNorms->LinkInputClass(Inputs);
        DefaultNorms->SetDataDrivenTimes(DataDriven_MeanTime,DataDriven_ScaleTime);
        DefaultNorms->LinkScorers(TheScorers);
        DefaultNorms->DefaultNorms(Input_ISNO,Input_ISYES);
        if (ExecuteNormalization==kTRUE) {DefaultNorms->PerformNormalization();}
        else                             {DefaultNorms->SkipNormalization();}
        
        // And declare the simple default norms:
        ElenaNorms = new NormalizationClass();
        ElenaNorms->LinkInputClass(Inputs);
        ElenaNorms->SetDataDrivenTimes(DataDriven_MeanTime,DataDriven_ScaleTime);
        ElenaNorms->LinkScorers(TheScorers);
        ElenaNorms->ElenaNorms(Input_ISNO,Input_ISYES);
        if (ExecuteNormalization==kTRUE) {ElenaNorms->PerformNormalization();}
        else                             {ElenaNorms->SkipNormalization();}
    }
    else
    {
        ErrorMessage("You cannot define the Normalization Class without a properly linked Inputs-Class!");
    }
}
