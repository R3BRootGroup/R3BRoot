// This header contains the functions to determine the
// multiplicity of the traditional method reconstruction
// using the perfect multiplicity from the simulations:

// Function to extract the true multiplicity inside the Exec-function:
Int_t R3B_TradMed_NeutronTracker::PerfectMultiplicity()
{
    // Declare the answer:
    Int_t TheMultiplicity = 0;
    
    // Next, declare what ne need:
    Int_t nClusters = fArrayCluster->GetEntries();
    R3BSignalCluster* TheCluster;
    
    // Loop over the clusters:
    for (Int_t k = 0; k<nClusters; ++k)
    {
        // Extract the current cluster:
        TheCluster = (R3BSignalCluster*) fArrayCluster->At(k);
        
        // Add the true multiplicity to the total:
        TheMultiplicity = TheMultiplicity + TheCluster->GetPrimaryMultSim();
    }   
    
    // Done. Return the answer:
    return TheMultiplicity;
}
    
