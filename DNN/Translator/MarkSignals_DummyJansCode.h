void R3BNeuLANDTranslator::MarkSignals_DummyJansCode()
{
    // Dummy function to check errors:
    ThisDetectedMultiplicity = 0;
    Int_t NprimClusters = fArrayJanClusters->GetEntries();
    Int_t nSignals = fArraySignals->GetEntries();
    R3BSignal* ThisSignal;
    
    Int_t nMarks = NprimClusters;
    if (nMarks>nSignals) {nMarks = nSignals;}
    
    for (Int_t k = 0; k<nMarks; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        ThisSignal->SetPrimarySim();
        ThisDetectedMultiplicity = ThisDetectedMultiplicity + 1;
    }
    
    // Done.
}
