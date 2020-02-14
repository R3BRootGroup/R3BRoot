Bool_t R3BTextFileGenerator::LinkClusters()
{
    // Retrieves the TClonesArray for the Clusters for all digi runs.
    Bool_t Answer = kFALSE;
    
    // Reset all Boolians:
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        Clusters_IsLinked[k] = kFALSE;
    }
    
    // Next, create the TClonesArray:
    fArrayClusters = new TClonesArray*[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        fArrayClusters[k] = new TClonesArray("R3BSignalCluster");
    }
    
    // Next, attempt to retrieve all arrays:
    Bool_t RetrieveTest = kTRUE;
    TString st = "";
    TString kstr = "";
    TString OutputNameTag = "";
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        // Define the OutputNameTags:
        kstr = st.Itoa(k,10);
        OutputNameTag = "_DigiRun_" + kstr;
        if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
        
        // Retrieve the Clusters:
        if ((TClonesArray*)ioman->GetObject("Clusters"+OutputNameTag) == nullptr)
        {
            ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No R3BSignalClusters" + OutputNameTag + "!");
            RetrieveTest = kFALSE;
        }
        else
        {
            fArrayClusters[k] = (TClonesArray*) ioman->GetObject("Clusters"+OutputNameTag);
            Clusters_IsLinked[k] = kTRUE;
        }
    }
    
    // Then, return the answer:
    if (RetrieveTest==kTRUE) {Answer = kTRUE;}
    return Answer;
}

Bool_t R3BTextFileGenerator::AreClustersLinked(Int_t const Index)
{
    Bool_t Answer = kFALSE;
    
    if ((Index>=0)&&(Index<nDigiRuns))
    {
        Answer = Clusters_IsLinked[Index];
    }
    else
    {
        Bool_t Test = kTRUE;
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            if (Clusters_IsLinked[k]==kFALSE)
            {
                Test = kFALSE;
            }
        }
        
        if (Test==kTRUE) {Answer = kTRUE;}
    }
    
    return Answer;
}
