Bool_t R3BTextFileGenerator::LinkMultiplicities()
{
    // Retrieves the TClonesArrays for the 3 Signals for all digi runs.
    Bool_t Answer = kFALSE;
    
    // Reset all Boolians:
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        Mults_AreLinked[k] = kFALSE;
    }
    
    // Next, create the TClonesArrays:
    fArrayGunMult = new TClonesArray*[nDigiRuns];
    fArrayNeuMult = new TClonesArray*[nDigiRuns];
    fArrayDetMult = new TClonesArray*[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        fArrayGunMult[k] = new TClonesArray("ObjInteger");
        fArrayNeuMult[k] = new TClonesArray("ObjInteger");
        fArrayDetMult[k] = new TClonesArray("ObjInteger");
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
        if ((TClonesArray*)ioman->GetObject("Gun_Multiplicity"+OutputNameTag) == nullptr)
        {
            ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No Gun_Multiplicity" + OutputNameTag + "!");
            RetrieveTest = kFALSE;
        }
        else
        {
            if ((TClonesArray*)ioman->GetObject("NeuLAND_Multiplicity"+OutputNameTag) == nullptr)
            {
                ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No NeuLAND_Multiplicity" + OutputNameTag + "!");
                RetrieveTest = kFALSE;
            }
            else
            {
                if ((TClonesArray*)ioman->GetObject("Detected_Multiplicity"+OutputNameTag) == nullptr)
                {
                    ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No Detected_Multiplicity" + OutputNameTag + "!");
                    RetrieveTest = kFALSE;
                }
                else
                {
                    fArrayGunMult[k] = (TClonesArray*) ioman->GetObject("Gun_Multiplicity"+OutputNameTag);
                    fArrayNeuMult[k] = (TClonesArray*) ioman->GetObject("NeuLAND_Multiplicity"+OutputNameTag);
                    fArrayDetMult[k] = (TClonesArray*) ioman->GetObject("Detected_Multiplicity"+OutputNameTag);
                    Mults_AreLinked[k] = kTRUE;
                }
            }
        }
    }
    
    // Then, return the answer:
    if (RetrieveTest==kTRUE) {Answer = kTRUE;}
    return Answer;
}

Bool_t R3BTextFileGenerator::AreMultsLinked(Int_t const Index)
{
    Bool_t Answer = kFALSE;
    
    if ((Index>=0)&&(Index<nDigiRuns))
    {
        Answer = Mults_AreLinked[Index];
    }
    else
    {
        Bool_t Test = kTRUE;
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            if (Mults_AreLinked[k]==kFALSE)
            {
                Test = kFALSE;
            }
        }
        
        if (Test==kTRUE) {Answer = kTRUE;}
    }
    
    return Answer;
}
