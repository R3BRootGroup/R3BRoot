// Initialization function
Bool_t AllScorers::Initialize()
{
    // Declare the answer:
    Bool_t Answer = kFALSE;
    
    // Here, we create all scorers that we use. Begin by checking that 
    // our inputs class is OK:
    if (Inputs->IsLinked()==kFALSE)
    {
        ErrorMessage("We cannot initialize the scorers without a linked Inputs-class!");
    }
    else
    {
        // Then, obtain the required parameters:
        OutputPath = Inputs->GetInputString("TheOutputPath");
        OutputFile = "NeuLAND_TheScorersFile.root"; // Must be predefined for Validate.sh script!
        BeamEperN = Inputs->GetAvgBeamEnergyPerNucleon();
        BeamBeta = Inputs->GetAvgBeamBeta();
        ParticleGun_StartX = Inputs->GetInputDouble("ParticleGun_x_position","cm");
        ParticleGun_StartY = Inputs->GetInputDouble("ParticleGun_y_position","cm");
        ParticleGun_StartZ = Inputs->GetInputDouble("ParticleGun_z_position","cm");
        TargetT = Inputs->GetTargetTime();
        TargetX = Inputs->GetInputDouble("TARGET_center_x_position","cm");
        TargetY = Inputs->GetInputDouble("TARGET_center_y_position","cm");
        TargetZ = Inputs->GetInputDouble("TARGET_center_z_position","cm");
        NeuLAND_Center_X = Inputs->GetInputDouble("NeuLAND_center_x_position","cm");
        NeuLAND_Center_Y = Inputs->GetInputDouble("NeuLAND_center_y_position","cm");
        NeuLAND_Front_Z = Inputs->GetInputDouble("NeuLAND_front_z_position","cm");
        NeuLAND_RotX = Inputs->GetInputDouble("NeuLAND_x_rotation_angle","degree");
        NeuLAND_RotY = Inputs->GetInputDouble("NeuLAND_y_rotation_angle","degree");
        NeuLAND_RotZ = Inputs->GetInputDouble("NeuLAND_z_rotation_angle","degree");
        NeuLAND_Start_Time = Inputs->GetNeuLANDStartTime();
        NeuLAND_Scintillator_Totalthickness = Inputs->GetTotalPaddleThickness();
        NeuLAND_nBarsPerPlane = Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane");
        NeuLAND_nDoublePlanes = Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
        NeuLAND_nBarsTotal = NeuLAND_nBarsPerPlane*NeuLAND_nDoublePlanes*2;
        ScoringResolution = Inputs->GetInputInteger("NeuLAND_ScoringHistogramResolution");
        ParticleType_ToBeDetected = Inputs->GetInputString("NeuLAND_ParticleType_ToBeDetected");
        ParticleMass = TheNuclei->GetMass(ParticleType_ToBeDetected,"MeV");
        SelectPrimHitInCluster = Inputs->GetInputString("NeuLAND_Scoring_HowToFind_PrimSignalWithinCluster");
        ScoreCombination = Inputs->GetInputString("NeuLAND_Scoring_Combination_of_Individual_Scores");
        UseClusterScore_DepositedEnergy = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_DepositedEnergy");
        UseClusterScore_KineticEnergy = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_KineticEnergy");
        UseClusterScore_RelativisticBeta = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_RelativisticBeta");
        UseClusterScore_TOF = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_TOF");
        UseClusterScore_Size = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_Size");
        UseClusterScore_Rvalue = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_Rvalue");
        UseClusterScore_LogR = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_LogR");
        UseClusterScore_SpaceLength = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_SpaceLength");
        UseClusterScore_TimeLength = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_TimeLength");
        UseClusterScore_SpaceRadius = Inputs->GetInputBoolian("NeuLAND_Scoring_UseClusterScore_SpaceRadius");
        UseSignalScore_DepositedEnergy = Inputs->GetInputBoolian("NeuLAND_Scoring_UseSignalScore_DepositedEnergy");
        UseSignalScore_KineticEnergy = Inputs->GetInputBoolian("NeuLAND_Scoring_UseSignalScore_KineticEnergy");
        UseSignalScore_RelativisticBeta = Inputs->GetInputBoolian("NeuLAND_Scoring_UseSignalScore_RelativisticBeta");
        UseSignalScore_TOF = Inputs->GetInputBoolian("NeuLAND_Scoring_UseSignalScore_TOF");
        UseSignalScore_Rvalue = Inputs->GetInputBoolian("NeuLAND_Scoring_UseSignalScore_Rvalue");
        
        if (ThisDetector=="NEBULA")
        {
            OutputFile = "NEBULA_TheScorersFile.root"; // Must be predefined for Validate.sh script!
            Key = "NEBULA";
            
            NeuLAND_Center_X = Inputs->GetInputDouble("NEBULA_center_x_position","cm");
            NeuLAND_Center_Y = Inputs->GetInputDouble("NEBULA_center_y_position","cm");
            NeuLAND_Front_Z = Inputs->GetInputDouble("NEBULA_front_z_position","cm");
            NeuLAND_RotX = Inputs->GetInputDouble("NEBULA_x_rotation_angle","degree");
            NeuLAND_RotY = Inputs->GetInputDouble("NEBULA_y_rotation_angle","degree");
            NeuLAND_RotZ = Inputs->GetInputDouble("NEBULA_z_rotation_angle","degree");
            NeuLAND_Start_Time = Inputs->GetNEBULAStartTime();
            NeuLAND_Scintillator_Totalthickness = Inputs->GetNEBULATotalPaddleThickness();
            NeuLAND_nBarsPerPlane = Inputs->GetInputInteger("NEBULA_Number_of_Bars_in_OnePlane");
            NeuLAND_nDoublePlanes = Inputs->GetInputInteger("NEBULA_Number_of_DoublePlaneModules");
            NeuLAND_nBarsTotal = NeuLAND_nBarsPerPlane*NeuLAND_nDoublePlanes*2;
            ParticleType_ToBeDetected = Inputs->GetInputString("NEBULA_ParticleType_ToBeDetected");
            ParticleMass = TheNuclei->GetMass(ParticleType_ToBeDetected,"MeV");
        }
        
        if (Inputs->ContainsNoErrors()==kTRUE)
        {
            // Next, generate the scorers once at a time:
            SignalScorers.clear();
            ScoringClass* ThisSignalScorer;
            
            ThisSignalScorer = new ScoringClass();
            ThisSignalScorer->SetKey(Key);
            ThisSignalScorer->SetName("Energy");
            ThisSignalScorer->MarkScorerForSignals();
            ThisSignalScorer->SetPlotLinearScale();
            ThisSignalScorer->GenerateSpectrum(ScoringResolution,0.0,40.0);
            SignalScorers.push_back(ThisSignalScorer);
            
            ThisSignalScorer = new ScoringClass();
            ThisSignalScorer->SetKey(Key);
            ThisSignalScorer->SetName("TOF");
            ThisSignalScorer->MarkScorerForSignals();
            ThisSignalScorer->SetPlotLinearScale();
            ThisSignalScorer->GenerateSpectrum(ScoringResolution,NeuLAND_Start_Time*0.9,NeuLAND_Start_Time+1.5*((Int_t) NeuLAND_nDoublePlanes));
            SignalScorers.push_back(ThisSignalScorer);
            
            ThisSignalScorer = new ScoringClass();
            ThisSignalScorer->SetKey(Key);
            ThisSignalScorer->SetName("Beta");
            ThisSignalScorer->MarkScorerForSignals();
            ThisSignalScorer->SetPlotLinearScale();
            ThisSignalScorer->GenerateSpectrum(ScoringResolution,BeamBeta-0.1,BeamBeta+0.05);
            SignalScorers.push_back(ThisSignalScorer);
            
            ThisSignalScorer = new ScoringClass();
            ThisSignalScorer->SetKey(Key);
            ThisSignalScorer->SetName("Ekin");
            ThisSignalScorer->MarkScorerForSignals();
            ThisSignalScorer->SetPlotLinearScale();
            ThisSignalScorer->GenerateSpectrum(ScoringResolution,BeamEperN*0.8,BeamEperN*1.1);
            SignalScorers.push_back(ThisSignalScorer);
            
            ThisSignalScorer = new ScoringClass();
            ThisSignalScorer->SetKey(Key);
            ThisSignalScorer->SetName("Rvalue");
            ThisSignalScorer->MarkScorerForSignals();
            ThisSignalScorer->SetPlotLinearScale();
            ThisSignalScorer->GenerateSpectrum(ScoringResolution,0.0,0.005);
            SignalScorers.push_back(ThisSignalScorer);
            
            // ------------------------------
            
            ClusterScorers.clear();
            ScoringClass* ThisClusterScorer;
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("Energy");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLogScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,0.0,BeamEperN*1.0);
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("Size");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLogScale();
            ThisClusterScorer->GenerateSpectrum(2*NeuLAND_nDoublePlanes,0.0,2.0*((Int_t) NeuLAND_nDoublePlanes));
            ClusterScorers.push_back(ThisClusterScorer);
     
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("TOF");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLinearScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,NeuLAND_Start_Time*0.9,NeuLAND_Start_Time+1.5*((Int_t) NeuLAND_nDoublePlanes));
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("Beta");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLinearScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,BeamBeta-0.1,BeamBeta+0.05);
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("Ekin");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLinearScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,BeamEperN*0.8,BeamEperN*1.1);
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("Rvalue");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLogScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,0.0,0.01);
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("LogR");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLinearScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,0.0,10.0);
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("SpaceLength");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLogScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,0.0,((Int_t) NeuLAND_nDoublePlanes)*NeuLAND_Scintillator_Totalthickness*2.0);
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("TimeLength");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLogScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,0.0,15.0);
            ClusterScorers.push_back(ThisClusterScorer);
            
            ThisClusterScorer = new ScoringClass();
            ThisClusterScorer->SetKey(Key);
            ThisClusterScorer->SetName("SpaceRadius");
            ThisClusterScorer->MarkScorerForClusters();
            ThisClusterScorer->SetPlotLogScale();
            ThisClusterScorer->GenerateSpectrum(ScoringResolution,0.0,100.0);
            ClusterScorers.push_back(ThisClusterScorer);

            // ------------------------------
            
            // Then, create the output file and assign it to each scorer.
            // NOTE: we must make sure that we do not overwrite our scorers if they already exist!
            TString FileName = OutputPath + "/" + OutputFile;
            cout << " ==> FileName = " << FileName.Data() << "\n";
            
            FileStat_t stom;
            Int_t test = gSystem->GetPathInfo(FileName.Data(),stom);
            TFile* f;
    
            if (test==0)
            {
                // Then the file already exists:
                f = new TFile(FileName.Data(),"update");
            }
            else
            {
                // recreate the file:
                f = new TFile(FileName.Data(),"recreate");
            }
            
            // Then, assign it to the scorers:
            for (Int_t k = 0; k<SignalScorers.size(); ++k)
            {
                SignalScorers.at(k)->AssignOutputFile(f);
            }
            
            for (Int_t k = 0; k<ClusterScorers.size(); ++k)
            {
                ClusterScorers.at(k)->AssignOutputFile(f);
            }
            
            // ------------------------------
            
            // Then, give the success statement:
            Scorers_Initialized = kTRUE;
            
            // And test all scorers for errors:
            for (Int_t k = 0; k<SignalScorers.size(); ++k)
            {
                if (SignalScorers.at(k)->ContainsNoErrors()==kFALSE) {Scorers_Initialized = kFALSE;}
            }
            
            for (Int_t k = 0; k<ClusterScorers.size(); ++k)
            {
                if (ClusterScorers.at(k)->ContainsNoErrors()==kFALSE) {Scorers_Initialized = kFALSE;}
            }
            
            // Assign the answer:
            Answer = Scorers_Initialized;
        }
    }
    
    // return the answer:
    if (Answer==kFALSE) {ErrorMessage("The scorers were not properly initialized!");}
    return Answer;
}
