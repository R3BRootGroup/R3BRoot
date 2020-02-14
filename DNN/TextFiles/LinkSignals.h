Bool_t R3BTextFileGenerator::LinkSignals()
{
    // Retrieves the TClonesArray for the Signals for all digi runs.
    Bool_t Answer = kFALSE;
    
    // Reset all Boolians:
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        Signals_IsLinked[k] = kFALSE;
    }
    
    // Next, create the TClonesArray:
    fArraySignals = new TClonesArray*[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        fArraySignals[k] = new TClonesArray("R3BSignal");
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
        
        // Retrieve the Signals:
        if ((TClonesArray*)ioman->GetObject("Signals"+OutputNameTag) == nullptr)
        {
            ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No R3BSignals" + OutputNameTag + "!");
            RetrieveTest = kFALSE;
        }
        else
        {
            fArraySignals[k] = (TClonesArray*) ioman->GetObject("Signals"+OutputNameTag);
            Signals_IsLinked[k] = kTRUE;
        }
    }
    
    // Then, return the answer:
    if (RetrieveTest==kTRUE) {Answer = kTRUE;}
    return Answer;
}

Bool_t R3BTextFileGenerator::AreSignalsLinked(Int_t const Index)
{
    Bool_t Answer = kFALSE;
    
    if ((Index>=0)&&(Index<nDigiRuns))
    {
        Answer = Signals_IsLinked[Index];
    }
    else
    {
        Bool_t Test = kTRUE;
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            if (Signals_IsLinked[k]==kFALSE)
            {
                Test = kFALSE;
            }
        }
        
        if (Test==kTRUE) {Answer = kTRUE;}
    }
    
    return Answer;
}
