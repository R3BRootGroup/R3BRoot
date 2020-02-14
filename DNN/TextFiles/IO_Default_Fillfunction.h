void R3BTextFileGenerator::IO_Default_Fillfunction()
{
    // fills vectors InputNeurosn & OutputNeurons on a specific 
    // method with values of the TClonesArrays.
    
    // Check whether the number of inputs and outputs are the 
    // correct values:
    if ((N_inputs==2)&&(N_outputs==1))
    {
        // Then, check whether the clusters are actually provided to us:
        if (Clusters_IsLinked[CurrentDigiRun]==kTRUE)
        {
            // Extract number of clusters:
            Int_t nClusters = fArrayClusters[CurrentDigiRun]->GetEntries();
            Double_t nClusters_d = (Int_t) nClusters;
    
            // Extract total energy deposition and true multiplicity:
            Int_t TrueMultiplicity = 0;
            Double_t EdepTotal = 0.0;
            R3BSignalCluster* TheCluster;
    
            for (Int_t k = 0; k<nClusters; ++k)
            {
                TheCluster = (R3BSignalCluster*) fArrayClusters[CurrentDigiRun]->At(k);
                EdepTotal = EdepTotal + TheCluster->GetEnergy();
                TrueMultiplicity = TrueMultiplicity + TheCluster->GetPrimaryMultSim();
            }
        
            Double_t TrueMultiplicity_d = (Int_t) TrueMultiplicity;
        
            // Fill our inputs:
            InputNeurons.push_back(nClusters_d);
            InputNeurons.push_back(EdepTotal);
        
            // Fill our outputs:
            OutputNeurons.push_back(TrueMultiplicity_d);
        
            // Done.
        }
        else
        {
            ErrorMessage("The default IO-function requires a linked fArrayClusters!");
        }
    }
    else
    {
        ErrorMessage("The default IO-function requires n_inputs==2 & N_outputs==1!");
    }
}
