void R3BTextFileGenerator::IO_FillFunction_3to6_TradMed_BabyNetwork()
{
    // fills vectors InputNeurosn & OutputNeurons on a specific 
    // method with values of the TClonesArrays.
    
    // Check whether the number of inputs and outputs are the 
    // correct values:
    Int_t nDesiredInputs = 3;
    if (IncludeTarget==kTRUE) {nDesiredInputs = nDesiredInputs + 4;}
    
    if ((N_inputs==nDesiredInputs)&&(N_outputs==(MaxMultiplicity+1)))
    {
        // Then, check whether the clusters are actually provided to us:
        if (Clusters_IsLinked[CurrentDigiRun]==kTRUE)
        {
            // Extract number of clusters:
            Int_t nClusters = fArrayClusters[CurrentDigiRun]->GetEntries();
            Double_t nClusters_d = (Int_t) nClusters;
    
            // Extract total energy deposition, true multiplicity & avg. ClusterSize;
            Int_t TrueMultiplicity = 0;
            Int_t ThisClusterSize;
            Double_t EdepTotal = 0.0;
            Double_t ThisClusterSize_d = 0.0;
            Double_t AvgClusterSize = 0.0;
            R3BSignalCluster* TheCluster;
    
            for (Int_t k = 0; k<nClusters; ++k)
            {
                TheCluster = (R3BSignalCluster*) fArrayClusters[CurrentDigiRun]->At(k);
                EdepTotal = EdepTotal + TheCluster->GetEnergy();
                TrueMultiplicity = TrueMultiplicity + TheCluster->GetPrimaryMultSim();
                
                ThisClusterSize = TheCluster->GetSize();
                ThisClusterSize_d = (Int_t) ThisClusterSize;
                AvgClusterSize = AvgClusterSize + ThisClusterSize_d;
            }
            
            if (nClusters_d>0.0)
            {
                AvgClusterSize = AvgClusterSize/nClusters_d;
            }
            else
            {
                AvgClusterSize = 0.0;
            }
        
            // Fill our inputs:
            if (IncludeTarget==kTRUE)
            {
                InputNeurons.push_back(Target_T);
                InputNeurons.push_back(Target_X);
                InputNeurons.push_back(Target_Y);
                InputNeurons.push_back(Target_Z);
            }
            
            // Decide upon normalization of the other inputs:
            if (ExecuteNormalization==kFALSE)
            {
                InputNeurons.push_back(nClusters_d);
                InputNeurons.push_back(EdepTotal);
                InputNeurons.push_back(AvgClusterSize);
            }
            else
            {
                // Then, it gets harder. First, learn normalization:
                LearnAdvancedNorms->AdaptOldScale("nClusters",nClusters_d);
                LearnAdvancedNorms->AdaptOldScale("TotalEnergy",EdepTotal);
                LearnAdvancedNorms->AdaptOldScale("ClusterSize",AvgClusterSize);
                
                // Then, fill in normalized values:
                InputNeurons.push_back(TheNormalization->Normalize("nClusters",nClusters_d));
                InputNeurons.push_back(TheNormalization->Normalize("TotalEnergy",EdepTotal));
                InputNeurons.push_back(TheNormalization->Normalize("ClusterSize",AvgClusterSize));
            }
        
            // Fill our outputs:
            for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
            {
                OutputNeurons.push_back(Output_ISNO);
            }
            
            if ((TrueMultiplicity>=0)&&(TrueMultiplicity<=MaxMultiplicity))
            {
                OutputNeurons.at(TrueMultiplicity) = Output_ISYES;
            }
            else
            {
                TString st = "";
                TString IndexStr = st.Itoa(EventCounter,10);
                ErrorMessage("Somehow we found in Event Index "+IndexStr+" a multiplcity higher than the max!");
            }
        
            // Done.
        }
        else
        {
            ErrorMessage("The 3to6 IO-function requires a linked fArrayClusters!");
        }
    }
    else
    {
        ErrorMessage("The 3to6 IO-function requires n_inputs==2 & N_outputs==Max.Mult.+1!");
    }
}
