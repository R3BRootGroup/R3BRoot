Bool_t R3BTextFileGenerator::LinkFilteredClusters()
{
    // Retrieves the TClonesArray for the Filtered Clusters for all digi runs.
    Bool_t Answer = kFALSE;
    
    // Reset all Boolians:
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        Filtered_IsLinked[k] = kFALSE;
    }
    
    // Next, create the TClonesArray:
    fArrayFilteredClusters = new TClonesArray*[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        fArrayFilteredClusters[k] = new TClonesArray("R3BSimpleCluster");
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
        
        // Retrieve the Filtered Clusters:
        if ((TClonesArray*)ioman->GetObject("TranslatedClusters"+OutputNameTag) == nullptr)
        {
            ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No R3BSimpleClusters" + OutputNameTag + "!");
            RetrieveTest = kFALSE;
        }
        else
        {
            fArrayFilteredClusters[k] = (TClonesArray*) ioman->GetObject("TranslatedClusters"+OutputNameTag);
            Filtered_IsLinked[k] = kTRUE;
        }
    }
    
    // Then, return the answer:
    if (RetrieveTest==kTRUE) {Answer = kTRUE;}
    return Answer;
}

Bool_t R3BTextFileGenerator::AreFilteredClustersLinked(Int_t const Index)
{
    Bool_t Answer = kFALSE;
    
    if ((Index>=0)&&(Index<nDigiRuns))
    {
        Answer = Filtered_IsLinked[Index];
    }
    else
    {
        Bool_t Test = kTRUE;
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            if (Filtered_IsLinked[k]==kFALSE)
            {
                Test = kFALSE;
            }
        }
        
        if (Test==kTRUE) {Answer = kTRUE;}
    }
    
    return Answer;
}
