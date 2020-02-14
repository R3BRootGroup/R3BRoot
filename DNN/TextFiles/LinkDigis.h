Bool_t R3BTextFileGenerator::LinkDigis()
{
    // Retrieves the TClonesArray for the Digis for all digi runs.
    Bool_t Answer = kFALSE;
    
    // Reset all Boolians:
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        Digis_IsLinked[k] = kFALSE;
    }
    
    // Next, create the TClonesArray:
    fArrayDigis = new TClonesArray*[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        fArrayDigis[k] = new TClonesArray("R3BNeulandDigi");
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
        
        // Retrieve the Digis:
        if ((TClonesArray*)ioman->GetObject("NeulandDigis"+OutputNameTag) == nullptr)
        {
            ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No NeulandDigis" + OutputNameTag + "!");
            RetrieveTest = kFALSE;
        }
        else
        {
            fArrayDigis[k] = (TClonesArray*) ioman->GetObject("NeulandDigis"+OutputNameTag);
            Digis_IsLinked[k] = kTRUE;
        }
    }
    
    // Then, return the answer:
    if (RetrieveTest==kTRUE) {Answer = kTRUE;}
    return Answer;
}

Bool_t R3BTextFileGenerator::AreDigisLinked(Int_t const Index)
{
    Bool_t Answer = kFALSE;
    
    if ((Index>=0)&&(Index<nDigiRuns))
    {
        Answer = Digis_IsLinked[Index];
    }
    else
    {
        Bool_t Test = kTRUE;
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            if (Digis_IsLinked[k]==kFALSE)
            {
                Test = kFALSE;
            }
        }
        
        if (Test==kTRUE) {Answer = kTRUE;}
    }
    
    return Answer;
}
