void R3BTextFileGenerator::IO_FillFunction_ScoringPlus()
{
    // Function to fill the clusterproperties of the scoringplus 
    // method into a network, so that the network can replace the 
    // histogram division, and therefore find correlations.
    
    // begin by testing if the network structure is OK:
    Int_t N_DesiredInputs = ((TheScorers->GetNumberOfClusterScores()) + 4)*NmaxClusters;
    Int_t N_DesiredOutputs = NmaxClusters*nOutputFlags_PerCluster;
    
    // Compare with inputs:
    if (Step2_MultiplicityChoice=="Perfect") {Step2_UseDetectedMultiplicity = kTRUE;}
    
    if (!((N_inputs==N_DesiredInputs)&&
          (N_outputs==N_DesiredOutputs)&&
          (Step2_IsSelected==kTRUE)&&
          ((Select_Determined_Multiplicity>0)&&(Select_Determined_Multiplicity<=MaxMultiplicity))&&
          (Clusters_IsLinked[CurrentDigiRun]==kTRUE)&&
          (((Mults_AreLinked[CurrentDigiRun]==kTRUE)&&(ValidationMode==kFALSE))||((DNNMults_AreLinked[CurrentDigiRun]==kTRUE)&&(ValidationMode==kTRUE)))&&
          (TheScorers->ContainsNoErrors()==kTRUE)))
    {
        if (N_inputs!=N_DesiredInputs) {ErrorMessage("The number of Input neurons should be the number of used clusterscores + 4 times the max. number of clusters!");}
        if (N_outputs!=N_DesiredOutputs) {ErrorMessage("The number of Output neurons should be the max. number of clusters!");}
        if (Step2_IsSelected==kFALSE) {ErrorMessage("This function is only supposed to be used for the second reconstruction step, because it requires the multiplicity!");}
        if ((Select_Determined_Multiplicity<=0)||(Select_Determined_Multiplicity>MaxMultiplicity)) {ErrorMessage("The selected multiplicity was not simulated!");}
        if (Clusters_IsLinked[CurrentDigiRun]==kFALSE) {ErrorMessage("This function needs the clusters to be linked!");}
        if ((Mults_AreLinked[CurrentDigiRun]==kFALSE)&&(ValidationMode==kFALSE)) {ErrorMessage("Under training mode, this function needs to link to the Gun, NeuLAND & Detected Multiplicity!");}
        if ((DNNMults_AreLinked[CurrentDigiRun]==kFALSE)&&(ValidationMode==kTRUE)) {ErrorMessage("Under Validation mode, this function needs to link to the DNN predicted Multiplicity!");}
        if (TheScorers->ContainsNoErrors()==kFALSE) {ErrorMessage("This function needs the scorers to have been properly initialized & collected!");}
    }
    else
    {
        // Then, we can move on. We will first sort the clusters according to their total score 
        // and then offer them to the DNN. The inputs are all the score-values (not scores, but cluster properties) and T,X,Y,Z of the cluster head.
        
        // begin by clearin the output vectors:
        InputNeurons.clear();
        OutputNeurons.clear();
        
        // Next, define the preselection:
        Bool_t Preselection_AllowEvent = kTRUE;
        Int_t StoredGunMultiplicity;
        Int_t StoredNeuLANDMultiplicity;
        Int_t StoredDetectedMultiplicity;
        Int_t PedictedDNNMultiplicity;
        
        // Next, decide upon the case:
        if (ValidationMode==kFALSE)
        {
            // Then, obtain the 3 MC multiplicities:
            ObjInteger* ThisObjInt;
            
            ThisObjInt = (ObjInteger*) fArrayGunMult[CurrentDigiRun]->At(0);
            StoredGunMultiplicity = ThisObjInt->GetNumber();
            
            ThisObjInt = (ObjInteger*) fArrayNeuMult[CurrentDigiRun]->At(0);
            StoredNeuLANDMultiplicity = ThisObjInt->GetNumber();
            
            ThisObjInt = (ObjInteger*) fArrayDetMult[CurrentDigiRun]->At(0);
            StoredDetectedMultiplicity = ThisObjInt->GetNumber();
            
            // And subject them to testing:
            if (StoredGunMultiplicity!=Select_Determined_Multiplicity)      {Preselection_AllowEvent = kFALSE;}
            if (StoredNeuLANDMultiplicity!=Select_Determined_Multiplicity)  {Preselection_AllowEvent = kFALSE;}
            if (StoredDetectedMultiplicity!=Select_Determined_Multiplicity) {Preselection_AllowEvent = kFALSE;}
        }
        else
        {
            // Then, we must rely on the DNN multiplicity:
            ObjInteger* ThisObjInt = (ObjInteger*) fArrayDNNMult[CurrentDigiRun]->At(0);
            PedictedDNNMultiplicity = ThisObjInt->GetNumber();
            
            // But we can also try to access the other multiplicities:
            if (SimulationData_IsAvailable==kTRUE)
            {
                ThisObjInt = (ObjInteger*) fArrayGunMult[CurrentDigiRun]->At(0);
                StoredGunMultiplicity = ThisObjInt->GetNumber();
            
                ThisObjInt = (ObjInteger*) fArrayNeuMult[CurrentDigiRun]->At(0);
                StoredNeuLANDMultiplicity = ThisObjInt->GetNumber();
            
                ThisObjInt = (ObjInteger*) fArrayDetMult[CurrentDigiRun]->At(0);
                StoredDetectedMultiplicity = ThisObjInt->GetNumber();
            }
            
            // And subject it to testing:
            if (Step2_UseDetectedMultiplicity==kFALSE)
            {
                if (PedictedDNNMultiplicity!=Select_Determined_Multiplicity) {Preselection_AllowEvent = kFALSE;}
            }
            else
            {
                if (StoredDetectedMultiplicity!=Select_Determined_Multiplicity) {Preselection_AllowEvent = kFALSE;}
            }
        }
        
        // Then, only continue if the preselection worked:
        if (Preselection_AllowEvent==kTRUE)
        {
            // Then, the next step is to sort the clusters according to their total score:
            Int_t nClusters = fArrayClusters[CurrentDigiRun]->GetEntries();
            std::vector<ClusterSortStructure> TheClusters;
            TheClusters.clear();
            TheClusters.resize(nClusters);
            R3BSignalCluster* ThisCluster;
            Double_t ThisClusterScore = 0.0;
    
            for (Int_t k = 0; k<nClusters; ++k)
            {
                // Compute Scores:
                ThisCluster = (R3BSignalCluster*) fArrayClusters[CurrentDigiRun]->At(k);
                ThisClusterScore = TheScorers->ObtainClusterScore(ThisCluster);
                TheClusters.at(k).TheCluster = ThisCluster;
                TheClusters.at(k).TheScore = ThisClusterScore;
            }   
    
            // Sort to total score:
            std::sort(TheClusters.begin(), TheClusters.end(), ClusterSortFunction_TxT);
            
            // Find largest primary cluster index after the sorting.
            Int_t nPrimClusters = 0;
            
            for (Int_t k = 0; k<nClusters; ++k)
            {
                if (TheClusters.at(k).TheCluster->GetPrimaryMultSim()>0) {nPrimClusters = k+1;}
            }
            
            // Fill in control histograms:
            TotalClusterScore_All->Fill((Int_t) nClusters);
            TotalClusterScore_Prim->Fill((Int_t) nPrimClusters);
            
            // Cut off the vector after the maximum number of signals is reached:
            if (CutOff_ClusterVector==kTRUE)
            {
                if (TheClusters.size()>NmaxClusters) {TheClusters.resize(NmaxClusters);}
            }
            
            // And then re-shuffle the order:
            if ((ReShuffle_ClusterVector==kTRUE)&&(ValidationMode==kFALSE))
            {
                std::random_shuffle(TheClusters.begin(), TheClusters.end(), RandomSortFunction);
            }
            
            // Check, if the cutoff-cluster actually still contains the correct muliplicity:
            Int_t CutOffMultiplicity = 0;
            
            for (Int_t k = 0; k<TheClusters.size(); ++k)
            {
                if (TheClusters.at(k).TheCluster->GetPrimaryMultSim()>0) {CutOffMultiplicity = CutOffMultiplicity + 1;}
            }
            
            if (Kickout_EventsWithLostPrimClusters==kFALSE) {CutOffMultiplicity = StoredDetectedMultiplicity;}
            
            if ((ValidationMode==kTRUE)||((ValidationMode==kFALSE)&&(CutOffMultiplicity==StoredDetectedMultiplicity)))
            {
                // Then, loop over the clusters:
                for (Int_t kclus = 0; kclus<NmaxClusters; ++kclus)
                {
                    // Check that the cluster actually exists:
                    if (kclus<nClusters)
                    {
                        // Then, fill in the current cluster:
                        FillOneCluster(TheClusters.at(kclus).TheCluster);
                    }
                    else
                    {
                        // Fill in an empty cluster:
                        FillOneEmptyCluster();
                    }
                        
                    // Now, filling is done. Close all blocks:
                }
            }
        }
    }
}
