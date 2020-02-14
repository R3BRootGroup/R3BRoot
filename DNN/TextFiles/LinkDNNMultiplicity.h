Bool_t R3BTextFileGenerator::LinkDNNMultiplicity()
{
    // Retrieves the TClonesArrays for the 3 Signals for all digi runs.
    Bool_t Answer = kFALSE;
    
    // Reset all Boolians:
    DNNMults_AreLinked = new Bool_t[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        DNNMults_AreLinked[k] = kFALSE;
    }
    
    // Next, create the TClonesArrays:
    fArrayDNNMult = new TClonesArray*[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        fArrayDNNMult[k] = new TClonesArray("ObjInteger");
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
        
        // Retrieve the multiplicities:
        if ((TClonesArray*)ioman->GetObject("DNN_Multiplicity"+OutputNameTag) == nullptr)
        {
            ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No DNN_Multiplicity" + OutputNameTag + "!");
            RetrieveTest = kFALSE;
        }
        else
        {
            fArrayDNNMult[k] = (TClonesArray*) ioman->GetObject("DNN_Multiplicity"+OutputNameTag);
            DNNMults_AreLinked[k] = kTRUE;
        }
    }
    
    // Then, return the answer:
    if (RetrieveTest==kTRUE) {Answer = kTRUE;}
    return Answer;
}

Bool_t R3BTextFileGenerator::AreDNNMultsLinked(Int_t const Index)
{
    Bool_t Answer = kFALSE;
    
    if ((Index>=0)&&(Index<nDigiRuns))
    {
        Answer = DNNMults_AreLinked[Index];
    }
    else
    {
        Bool_t Test = kTRUE;
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            if (DNNMults_AreLinked[k]==kFALSE)
            {
                Test = kFALSE;
            }
        }
        
        if (Test==kTRUE) {Answer = kTRUE;}
    }
    
    return Answer;
}
