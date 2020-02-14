Bool_t R3BTextFileGenerator::LinkNeutronTracks()
{
    // Retrieves the TClonesArray for the MC Neutron Tracks.
    // We only use the first Digi Run here, because these tracks are all identical
    // anyway (at least, they should be!)
    Bool_t Answer = kFALSE;
    
    // Reset Boolian:
    NeutronTracks_IsLinked = kFALSE;
    
    // Next, create the TClonesArray:
    fArrayNeutronTracks = new TClonesArray("TLorentzVector");
    
    // Next, attempt to retrieve the array:
    if ((TClonesArray*)ioman->GetObject("MCNeutronTracks") == nullptr)
    {
        ErrorMessage("I/O-manager FATAL: R3BTextFileGenerator: No MCNeutronTracks!");
        Answer = kFALSE;
    }
    else
    {
        fArrayNeutronTracks = (TClonesArray*) ioman->GetObject("MCNeutronTracks");
        NeutronTracks_IsLinked = kTRUE;
        Answer = kTRUE;
    }
    
    // Then, return the answer:
    return Answer;
}

Bool_t R3BTextFileGenerator::AreNeutronsLinked() {return NeutronTracks_IsLinked;}
