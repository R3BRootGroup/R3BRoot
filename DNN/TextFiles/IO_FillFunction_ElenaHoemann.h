void R3BTextFileGenerator::IO_FillFunction_ElenaHoemann()
{
    // fills vectors InputNeurons & OutputNeurons on a specific 
    // method with values of the TClonesArrays. This function 
    // fills input and output neurons according to the work
    // of Elena Hoemann. We use different options in this function:
    // Bool_t Include_HorPaddleCoordinate;
    // Bool_t Include_ZeroMult_Output;
    // Bool_t Include_ZeroMultEvents;
    // Bool_t Include_Multiplicity_MisAligned_Events; (Can only be excluded when SimulationData_IsAvailable==kTRUE ==> when that data is available)
    // Bool_t Include_SigneHitClusters;
    
    // We need the array of complete clusters to make this work.
    if ((Clusters_IsLinked[CurrentDigiRun]==kFALSE)||
        ((NeutronTracks_IsLinked==kFALSE)&&(SimulationData_IsAvailable==kTRUE))||
        (TheScorers->ContainsNoErrors()==kFALSE)||
        ((Mults_AreLinked[CurrentDigiRun]==kFALSE)&&(SimulationData_IsAvailable==kTRUE)))
    {
        if (Clusters_IsLinked[CurrentDigiRun]==kFALSE) {ErrorMessage("The IO ElenaHoemann fillfunction needs fArrayClusters to work!");}
        if (NeutronTracks_IsLinked==kFALSE)            {ErrorMessage("The IO ElenaHoemann fillfunction needs fArrayNeutronTracks to work!");}
        if (TheScorers->ContainsNoErrors()==kFALSE)    {ErrorMessage("The IO ElenaHoemann fillfunction needs the scorers without any errors!");}
        if (Mults_AreLinked[CurrentDigiRun]==kFALSE)   {ErrorMessage("The IO ElenaHoemann fillfunction needs all 3 fArray-Multiplicities to work!");}
    }
    else
    {
        // Check that the number of inputs and outputs is properly defined:
        Int_t CorrectNumber_Inputs = 2+4*NDoublePlanes*NbarsPerPlane;
        if ((Include_HorPaddleCoordinate==kFALSE)&&(Include_SignalScores==kFALSE)&&(Include_SignalBetas==kFALSE)) {CorrectNumber_Inputs = 2+4*NDoublePlanes*NbarsPerPlane;}
        if ((Include_HorPaddleCoordinate==kTRUE)&&(Include_SignalScores==kFALSE)&&(Include_SignalBetas==kFALSE))  {CorrectNumber_Inputs = 2+6*NDoublePlanes*NbarsPerPlane;}
        if ((Include_HorPaddleCoordinate==kFALSE)&&(Include_SignalScores==kTRUE)&&(Include_SignalBetas==kFALSE))  {CorrectNumber_Inputs = 2+6*NDoublePlanes*NbarsPerPlane;}
        if ((Include_HorPaddleCoordinate==kTRUE)&&(Include_SignalScores==kTRUE)&&(Include_SignalBetas==kFALSE))   {CorrectNumber_Inputs = 2+8*NDoublePlanes*NbarsPerPlane;}
        if ((Include_HorPaddleCoordinate==kFALSE)&&(Include_SignalScores==kFALSE)&&(Include_SignalBetas==kTRUE))  {CorrectNumber_Inputs = 2+6*NDoublePlanes*NbarsPerPlane;}
        if ((Include_HorPaddleCoordinate==kTRUE)&&(Include_SignalScores==kFALSE)&&(Include_SignalBetas==kTRUE))   {CorrectNumber_Inputs = 2+8*NDoublePlanes*NbarsPerPlane;}
        if ((Include_HorPaddleCoordinate==kFALSE)&&(Include_SignalScores==kTRUE)&&(Include_SignalBetas==kTRUE))   {CorrectNumber_Inputs = 2+8*NDoublePlanes*NbarsPerPlane;}
        if ((Include_HorPaddleCoordinate==kTRUE)&&(Include_SignalScores==kTRUE)&&(Include_SignalBetas==kTRUE))    {CorrectNumber_Inputs = 2+10*NDoublePlanes*NbarsPerPlane;}
        if (Include_TotalScores==kTRUE) {CorrectNumber_Inputs = CorrectNumber_Inputs + 2;}
        
        Int_t CorrectNumber_Outputs = MaxMultiplicity;
        if (Include_ZeroMult_Output==kTRUE) {CorrectNumber_Outputs = MaxMultiplicity+1;}

        if ((N_inputs==CorrectNumber_Inputs)&&(N_outputs==CorrectNumber_Outputs))
        {
            // Then everything is OK. We have to generate a vector with ElenaHits for 
            // the Full NeuLAND and then, we have to assign the digis to these hits:
            
            // Retrieve the multiplicities:
            ObjInteger* ThisObjInt;
            Int_t StoredGunMultiplicity = 0;
            Int_t StoredNeuLANDMultiplicity = 0;
            Int_t StoredDetectedMultiplicity = 0;
            
            if (SimulationData_IsAvailable==kTRUE)
            {
                ThisObjInt = (ObjInteger*) fArrayGunMult[CurrentDigiRun]->At(0);
                StoredGunMultiplicity = ThisObjInt->GetNumber();
            
                ThisObjInt = (ObjInteger*) fArrayNeuMult[CurrentDigiRun]->At(0);
                StoredNeuLANDMultiplicity = ThisObjInt->GetNumber();
            
                ThisObjInt = (ObjInteger*) fArrayDetMult[CurrentDigiRun]->At(0);
                StoredDetectedMultiplicity = ThisObjInt->GetNumber();
            }
            
            // Firt, create an empty vector:
            Int_t nElenaHits = 2*NDoublePlanes*NbarsPerPlane;
            std::vector<ElenaHit> AllElenaHits;
            AllElenaHits.resize(nElenaHits);
            
            for (Int_t k = 0; k<nElenaHits; ++k)
            {
                AllElenaHits.at(k).nPaddle = k+1; // starts at 1!
                AllElenaHits.at(k).HitMeanTime = 0.0;
                AllElenaHits.at(k).HitMeanEnergy = 0.0;
                AllElenaHits.at(k).HitMeanLocation = 0.0;
                AllElenaHits.at(k).SignalScore = 0.0;
                AllElenaHits.at(k).Beta = 0.0;
                AllElenaHits.at(k).PaddleFired = kFALSE;
            }
            
            // Next, we must fill in the signals in these hits:
            Int_t nClusters = fArrayClusters[CurrentDigiRun]->GetEntries();
            Int_t ClusterSize = 0;
            Int_t DetectedMultiplicity = 0;
            R3BSignalCluster* ThisCluster;
            R3BSignal* ThisSignal;
            Int_t ThisSignalPaddleIndex;
            Double_t TotalEnergy = 0.0;
            Double_t TotalSignalScore = 0.0;
            Double_t TotalClusterScore = 0.0;
            
            // Loop over all clusters:
            if (nClusters>0)
            {
                for (Int_t kclus = 0; kclus<nClusters; ++kclus)
                {
                    // retrieve the current cluster:
                    ThisCluster = (R3BSignalCluster*) fArrayClusters[CurrentDigiRun]->At(kclus);
                
                    // Get the cluster size:
                    ClusterSize = ThisCluster->GetSize();
                
                    // Update the detected multiplicity:
                    DetectedMultiplicity = DetectedMultiplicity + ThisCluster->GetPrimaryMultSim();
                    
                    // Update the total energy deposition:
                    TotalEnergy = TotalEnergy + ThisCluster->GetEnergy();
                    
                    // Update the total cluster score:
                    TotalClusterScore = TotalClusterScore + TheScorers->ObtainClusterScore(ThisCluster);
                    
                    // Next, loop over the signals inside the cluster:
                    if (ClusterSize>0)
                    {
                        if ((Include_SigneHitClusters==kTRUE)||(ClusterSize>1))
                        {
                            for (Int_t ksig = 0; ksig<ClusterSize; ++ksig)
                            {
                                // Retrieve the current signal:
                                ThisSignal = (R3BSignal*) ThisCluster->Atp(ksig);
                                
                                // Update the signal score:
                                TotalSignalScore = TotalSignalScore + TheScorers->ObtainSignalScore(ThisSignal);
                                
                                // Add the signal to the Elena hit:
                                ThisSignalPaddleIndex = ThisSignal->GetCrystalIndex() - 1; // in NeuLANDTranslator, this was defined to start at 1!
                                AllElenaHits.at(ThisSignalPaddleIndex).nPaddle = ThisSignal->GetCrystalIndex();
                                AllElenaHits.at(ThisSignalPaddleIndex).HitMeanTime = ThisSignal->GetTime();
                                AllElenaHits.at(ThisSignalPaddleIndex).HitMeanEnergy = ThisSignal->GetEnergy();
                                AllElenaHits.at(ThisSignalPaddleIndex).PaddleFired = kTRUE;
                                AllElenaHits.at(ThisSignalPaddleIndex).HitMeanLocation = GetPMTPossition(ThisSignal);
                                AllElenaHits.at(ThisSignalPaddleIndex).Beta = ThisSignal->GetBeta(Target_T,Target_X, Target_Y,Target_Z);
                                AllElenaHits.at(ThisSignalPaddleIndex).SignalScore = TheScorers->ObtainSignalScore(ThisSignal);
                                
                                // Done.
                            }
                        }
                    }
                }
            }
            
            // Insert FailSafes:
            if (DetectedMultiplicity>MaxMultiplicity)       {DetectedMultiplicity = MaxMultiplicity;}
            if (StoredDetectedMultiplicity>MaxMultiplicity) {StoredDetectedMultiplicity = MaxMultiplicity;}
            if (StoredNeuLANDMultiplicity>MaxMultiplicity)  {StoredNeuLANDMultiplicity = MaxMultiplicity;}
            if (StoredGunMultiplicity>MaxMultiplicity)      {StoredGunMultiplicity = MaxMultiplicity;}
                        
            // Retrieve the particle-gun multiplicity:
            Int_t GunMultiplicity = 0;
            
            if (SimulationData_IsAvailable==kTRUE)
            {
                GunMultiplicity = fArrayNeutronTracks->GetEntries();
            }
            
            // Now, perform a check if this event should be filled in into the input neurons and output neurons:
            Bool_t FillCheck = kTRUE;
            
            if (PreselectionMethod=="Elena")
            {
                if (SimulationData_IsAvailable==kTRUE)
                {
                    if ((StoredDetectedMultiplicity!=StoredNeuLANDMultiplicity)&&(Include_Multiplicity_MisAligned_Events==kFALSE)) {FillCheck = kFALSE;}
                    // if ((StoredDetectedMultiplicity!=StoredGunMultiplicity)&&(Include_Multiplicity_MisAligned_Events==kFALSE)) {FillCheck = kFALSE;}
                    if ((StoredDetectedMultiplicity==0)&&(Include_ZeroMultEvents==kFALSE)) {FillCheck = kFALSE;}
                    
                    if (FillCheck==kFALSE)
                    {
                        cout << "ELENA pre-selection: Event " << EventCounter/nDigiRuns << " was discarded.\n";
                    }
                }
            }
            else
            {
                // Then, do the method as we always have done it:
                if (SimulationData_IsAvailable==kTRUE)
                {
                    if (Include_ZeroMultEvents==kFALSE)
                    {
                        if (DetectedMultiplicity==0) 
                        {
                            FillCheck = kFALSE;
                        
                            if (CurrentDigiRun==0)
                            {
                                cout << " Event " << EventCounter/nDigiRuns << " was discarded because NeuLAND detected multiplicity ZERO.\n";
                            }
                        }
                    }
            
                    if (Include_Multiplicity_MisAligned_Events==kFALSE)
                    {
                        if (DetectedMultiplicity!=GunMultiplicity) 
                        {
                            FillCheck = kFALSE;
                        
                            if (CurrentDigiRun==0)
                            {
                                cout << " Event " << EventCounter/nDigiRuns << " was discarded because the NeuLAND detected multiplicity && the ParticleGun produced multiplicity were not equal!\n";
                            }
                        }
                    }
                    
                    if (ComputeSingleClusterRatio==kTRUE)
                    {
                        if ((nClusters==1)&&(DetectedMultiplicity==1))
                        {
                            FillCheck = kFALSE;
                            
                            cout << " Event " << EventCounter/nDigiRuns << " was discarded because it was a single-cluster event.\n";
                        }
                    }
                }
            }
            
            // Now, we have the ElenaHits and they are already properly ordered. So now, we must fill in the neurons.
            // Declare what we need:
            Double_t NeuronInputs_EdepSignal;
            Double_t NeuronInputs_TimeSignal;
            Double_t NeuronInputs_PosSignal;
            Double_t NeuronInputs_SignalScore;
            Double_t NeuronInputs_SignalBeta;
            
            // Test whether we should actually fill in anything:
            if (FillCheck==kTRUE)
            {
                // Decide whether or not we normalize:
                if (ExecuteNormalization==kFALSE)
                {
                    InputNeurons.push_back((Int_t) nClusters);
                    InputNeurons.push_back(TotalEnergy);
                    
                    if (Include_TotalScores==kTRUE)
                    {
                        InputNeurons.push_back(TotalClusterScore);
                        InputNeurons.push_back(TotalSignalScore);
                    }
            
                    for (Int_t k = 0; k<nElenaHits; ++k)
                    {
                        InputNeurons.push_back(AllElenaHits.at(k).HitMeanTime);
                        InputNeurons.push_back(AllElenaHits.at(k).HitMeanEnergy);
                        
                        if (Include_HorPaddleCoordinate==kTRUE)
                        {
                            InputNeurons.push_back(AllElenaHits.at(k).HitMeanLocation);
                        }
                        
                        if (Include_SignalScores==kTRUE)
                        {
                            InputNeurons.push_back(AllElenaHits.at(k).SignalScore);
                        }
                        
                        if (Include_SignalBetas==kTRUE)
                        {
                            InputNeurons.push_back(AllElenaHits.at(k).Beta);
                        }
                    }
                }
                else
                {
                    // Then, first Learn the normalizations:
                    LearnAdvancedNorms->AdaptOldScale("nClusters",(Int_t) nClusters);
                    LearnAdvancedNorms->AdaptOldScale("TotalEnergy",TotalEnergy);
                    
                    if (Include_TotalScores==kTRUE)
                    {
                        LearnAdvancedNorms->AdaptOldScale("TotalClusterScore",TotalClusterScore);
                        LearnAdvancedNorms->AdaptOldScale("TotalSignalScore",TotalSignalScore);
                    }
                
                    for (Int_t k = 0; k<nElenaHits; ++k)
                    {
                        if (AllElenaHits.at(k).PaddleFired==kTRUE)
                        {
                            LearnAdvancedNorms->AdaptOldScale(TimeNormalization_NormName.Data(),AllElenaHits.at(k).HitMeanTime);
                            LearnAdvancedNorms->AdaptOldScale("SignalEnergy",AllElenaHits.at(k).HitMeanEnergy);
                            
                            if (Include_HorPaddleCoordinate==kTRUE)
                            {
                                LearnAdvancedNorms->AdaptOldScale("X",AllElenaHits.at(k).HitMeanLocation);
                            }
                            
                            if (Include_SignalScores==kTRUE)
                            {
                                LearnAdvancedNorms->AdaptOldScale("SingleSignalScore",AllElenaHits.at(k).SignalScore);
                            }
                            
                            if (Include_SignalBetas==kTRUE)
                            {
                                LearnAdvancedNorms->AdaptOldScale("RelativisticBeta",AllElenaHits.at(k).Beta);
                            }
                        }
                    }
                
                    // And then, apply the normalizations:
                    InputNeurons.push_back(TheNormalization->Normalize("nClusters",(Int_t) nClusters));
                    InputNeurons.push_back(TheNormalization->Normalize("TotalEnergy",TotalEnergy));
                    
                    if (Include_TotalScores==kTRUE)
                    {
                        InputNeurons.push_back(TheNormalization->Normalize("TotalClusterScore",TotalClusterScore));
                        InputNeurons.push_back(TheNormalization->Normalize("TotalSignalScore",TotalSignalScore));
                    }
            
                    for (Int_t k = 0; k<nElenaHits; ++k)
                    {
                        // Note that we add zeros in between here too, and those zeros should not be normalized:
                        if (AllElenaHits.at(k).PaddleFired==kTRUE)
                        {
                            // Compute normalized values:
                            NeuronInputs_EdepSignal = TheNormalization->Normalize("SignalEnergy",AllElenaHits.at(k).HitMeanEnergy);
                            NeuronInputs_TimeSignal = TheNormalization->Normalize(TimeNormalization_NormName.Data(),AllElenaHits.at(k).HitMeanTime);
                            
                            if (Include_HorPaddleCoordinate==kTRUE) 
                            {
                                NeuronInputs_PosSignal = TheNormalization->Normalize("X",AllElenaHits.at(k).HitMeanLocation);
                            }
                            else
                            {
                                NeuronInputs_PosSignal = 0.5*(Input_ISNO+Input_ISYES);
                            }
                            
                            if (Include_SignalScores==kTRUE) 
                            {
                                NeuronInputs_SignalScore = TheNormalization->Normalize("SingleSignalScore",AllElenaHits.at(k).SignalScore);
                            }
                            else
                            {
                                NeuronInputs_SignalScore = 0.5*(Input_ISNO+Input_ISYES);
                            }
                            
                            if (Include_SignalBetas==kTRUE) 
                            {
                                NeuronInputs_SignalBeta = TheNormalization->Normalize("RelativisticBeta",AllElenaHits.at(k).Beta);
                            }
                            else
                            {
                                NeuronInputs_SignalBeta = 0.5*(Input_ISNO+Input_ISYES);
                            }
                            
                            // See if we have to cut off tails:
                            if (Eliminate_Tails_InNormalization==kTRUE)
                            {
                                if ((NeuronInputs_EdepSignal<(Input_ISNO-0.1*(Input_ISYES-Input_ISNO)))||(NeuronInputs_EdepSignal>(Input_ISYES+0.1*(Input_ISYES-Input_ISNO)))||
                                    (NeuronInputs_TimeSignal<(Input_ISNO-0.1*(Input_ISYES-Input_ISNO)))||(NeuronInputs_TimeSignal>(Input_ISYES+0.1*(Input_ISYES-Input_ISNO)))||
                                    (NeuronInputs_PosSignal<(Input_ISNO-0.1*(Input_ISYES-Input_ISNO)))||(NeuronInputs_PosSignal>(Input_ISYES+0.1*(Input_ISYES-Input_ISNO)))||
                                    (NeuronInputs_SignalScore<(Input_ISNO-0.1*(Input_ISYES-Input_ISNO)))||(NeuronInputs_SignalScore>(Input_ISYES+0.1*(Input_ISYES-Input_ISNO)))||
                                    (NeuronInputs_SignalBeta<(Input_ISNO-0.1*(Input_ISYES-Input_ISNO)))||(NeuronInputs_SignalBeta>(Input_ISYES+0.1*(Input_ISYES-Input_ISNO))))
                                {
                                    NeuronInputs_EdepSignal = Input_ISNO;
                                    NeuronInputs_TimeSignal = Input_ISNO;
                                    NeuronInputs_PosSignal = Input_ISNO;
                                    NeuronInputs_SignalScore = Input_ISNO;
                                    NeuronInputs_SignalBeta = Input_ISNO;
                                }
                            }
                            
                            // Fill in input neurons:
                            InputNeurons.push_back(NeuronInputs_TimeSignal);
                            InputNeurons.push_back(NeuronInputs_EdepSignal);
                            if (Include_HorPaddleCoordinate==kTRUE) {InputNeurons.push_back(NeuronInputs_PosSignal);}
                            if (Include_SignalScores==kTRUE) {InputNeurons.push_back(NeuronInputs_SignalScore);}
                            if (Include_SignalBetas==kTRUE) {InputNeurons.push_back(NeuronInputs_SignalBeta);}
                        }
                        else
                        {
                            // Then, just fill in zeros:
                            InputNeurons.push_back(Input_ISNO);
                            InputNeurons.push_back(Input_ISNO);
                            if (Include_HorPaddleCoordinate==kTRUE) {InputNeurons.push_back(Input_ISNO);}
                            if (Include_SignalScores==kTRUE)        {InputNeurons.push_back(Input_ISNO);}
                            if (Include_SignalBetas==kTRUE)         {InputNeurons.push_back(Input_ISNO);}
                        }
                    }
                }
            
                // Next, fill in output neurons:
                if (Include_ZeroMult_Output==kTRUE)
                {
                    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
                    {
                        OutputNeurons.push_back(Output_ISNO);
                    }
                    OutputNeurons.at(DetectedMultiplicity) = Output_ISYES;
                }
                else
                {
                    for (Int_t k = 0; k<MaxMultiplicity; ++k)
                    {
                        OutputNeurons.push_back(Output_ISNO);
                    }
                    
                    if (DetectedMultiplicity!=0)
                    {
                        OutputNeurons.at(DetectedMultiplicity-1) = Output_ISYES;
                    }
                }
            } // closeing of the block for removing certain events.
            else
            {
                // Then, clean the full vectors, so it is easily recognised:
                InputNeurons.clear();
                OutputNeurons.clear();
            }
        }
        else
        {
            if (N_outputs!=CorrectNumber_Outputs) {ErrorMessage("For the Elena Hoemann network, we need always <MaxMultiplicity> or <MaxMultiplicity+1> outputs!");}
            if (N_inputs!=CorrectNumber_Inputs)   {ErrorMessage("For the Elena Hoemann network, we need always need N_inputs=2+<4or6>*NDoublePlanes*NbarsPerPlane inputs!");}
        }
    }
}
