void R3BTextFileGenerator::IO_FillFunction_Signals_MultNetwork()
{
    // fills vectors InputNeurons & OutputNeurons on a specific 
    // method with values of the TClonesArrays.
    
    // we need the signal array to make this work. Hence:
    if (Signals_IsLinked[CurrentDigiRun]==kFALSE)
    {
        ErrorMessage("The IO signal fillfunction needs fArraySignals to work!");
    }
    else
    {
        // Next, we must make sure that we have the right number of inputs & outputs.
        // We put a max. on the number of signals, hence both the input and the
        // output needs to correspond to this. 1*max for output & 6*max for input:
        Int_t nDesiredInputs = 6*NmaxSignals;
        if (IncludeTarget==kTRUE) {nDesiredInputs = nDesiredInputs + 4;}
        
        if ((N_inputs==nDesiredInputs)&&(N_outputs==(MaxMultiplicity+1)))
        {
            // Then, we are OK with inputs. Begin by creating one big cluster from
            // all R3BSignals:
            Int_t nSignals = fArraySignals[CurrentDigiRun]->GetEntries();
            R3BSignal* ThisSignal;
            
            R3BSignalCluster* BigCluster = new R3BSignalCluster(nSignals);
            
            for (Int_t k = 0; k<nSignals; ++k)
            {
                ThisSignal = (R3BSignal*) fArraySignals[CurrentDigiRun]->At(k);
                BigCluster->SetSignalp(k,ThisSignal);
            }
            
            // Next, sort it by time:
            BigCluster->SortClusterTOF();
            
            // Next, fill in the inputs & outputs:
            Int_t NeuronSize = nSignals;
            if (nSignals>NmaxSignals) {NeuronSize = NmaxSignals;}
            
            // Include the target if necessary:
            if (IncludeTarget==kTRUE)
            {
                InputNeurons.push_back(Target_T);
                InputNeurons.push_back(Target_X);
                InputNeurons.push_back(Target_Y);
                InputNeurons.push_back(Target_Z);
            }
            
            // loop over the signals in the TOF-sorted cluster up to NeuronSize only!
            
            // Initialize multiplicity:
            Int_t TheMultiplicity = 0; 
            
            for (Int_t k = 0; k<NeuronSize; ++k)
            {
                // Extract the current signal:
                ThisSignal = (R3BSignal*) BigCluster->Atp(k);
                
                // Add up multiplicity:
                if (ThisSignal->IsPrimarySim()==kTRUE) {TheMultiplicity = TheMultiplicity + 1;}
                
                // Next, define the inputs:
                
                // Decide whether or not we have to normalize:
                if (ExecuteNormalization==kFALSE)
                {
                    // Define inputs:
                    InputNeurons.push_back(Input_ISYES);                // Tag: this signal actually exists.
                    InputNeurons.push_back(ThisSignal->GetEnergy());    // Energy in the signal.
                    InputNeurons.push_back(ThisSignal->GetTime());      // TOF of the signal.
                    InputNeurons.push_back(ThisSignal->GetPositionX()); // X-coordinate of the signal.
                    InputNeurons.push_back(ThisSignal->GetPositionY()); // Y-coordinate of the signal.
                    InputNeurons.push_back(ThisSignal->GetPositionZ()); // Z-coordinate of the signal.
                }
                else
                {
                    // learn normalizations:
                    LearnAdvancedNorms->AdaptOldScale("SignalEnergy",ThisSignal->GetEnergy());
                    LearnAdvancedNorms->AdaptOldScale(TimeNormalization_NormName.Data(),ThisSignal->GetTime());
                    LearnAdvancedNorms->AdaptOldScale("X",ThisSignal->GetPositionX());
                    LearnAdvancedNorms->AdaptOldScale("Y",ThisSignal->GetPositionY());
                    LearnAdvancedNorms->AdaptOldScale("Z",ThisSignal->GetPositionZ());
                    
                    // Apply normalizations:
                    InputNeurons.push_back(Input_ISYES);                // Tag: this signal actually exists.
                    InputNeurons.push_back(TheNormalization->Normalize("SignalEnergy",ThisSignal->GetEnergy())); // Energy in the signal.
                    InputNeurons.push_back(TheNormalization->Normalize(TimeNormalization_NormName.Data(),ThisSignal->GetTime()));              // TOF of the signal.
                    InputNeurons.push_back(TheNormalization->Normalize("X",ThisSignal->GetPositionX()));         // X-coordinate of the signal.
                    InputNeurons.push_back(TheNormalization->Normalize("Y",ThisSignal->GetPositionY()));         // Y-coordinate of the signal.
                    InputNeurons.push_back(TheNormalization->Normalize("Z",ThisSignal->GetPositionZ()));         // Z-coordinate of the signal.
                }
            }
            
            // Now, in case that we did not fill up all the input/output pairs:
            if (nSignals<NmaxSignals)
            {
                for (Int_t k = nSignals; k<NmaxSignals; ++k)
                {
                    // Define inputs: alos nothing:
                    InputNeurons.push_back(Input_ISNO); // Tag: this signal does NOT exist!
                    InputNeurons.push_back(0.0);  // Energy
                    InputNeurons.push_back(0.0);  // Time
                    InputNeurons.push_back(0.0);  // X
                    InputNeurons.push_back(0.0);  // Y
                    InputNeurons.push_back(0.0);  // Z
                }
            }
            
            // Fill in outputs:
            for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
            {
                OutputNeurons.push_back(Output_ISNO);
            }
            
            if ((TheMultiplicity>=0)&&(TheMultiplicity<=MaxMultiplicity))
            {
                OutputNeurons.at(TheMultiplicity) = Output_ISYES;
            }
            else
            {
                ErrorMessage("The simulated multiplicity went out of its boundaries!");
            }
            
            // Done. every new goes with a delete:
            delete BigCluster;
        }
        else
        {
            ErrorMessage("The IO signal fillfunction needs 6*CutOff inputs & 1*CutOff outputs!");
        }
    }
}
