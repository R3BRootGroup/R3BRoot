void R3BNeuLANDTranslator::TranslateSignals(TClonesArray* fDigiArray, TClonesArray* fSignalArray)
{
    // This function fills the TClonesArray fSignalArray
    // by using fDigiArray. It is a simple 1-1 corespondance.
    // Both the Simulation and the Experimental markings of the
    // signals will be put to kFALSE. These markings 
    // have to be dealt with by different functions.
    Int_t nDigis = fDigiArray->GetEntries();
    
    if (nDigis==0) {} // If there are no signals, it is pointless to process them!
    else 
    {
        for (Int_t k = 0; k<nDigis; ++k)
        {
            new ((*fSignalArray)[k]) R3BSignal();
        }        
    
        // Next, we will transfer the information 1-by-1:
        R3BNeulandDigi* Digi;
        R3BSignal* Signal;
        TVector3 Position;
        Double_t Energy;
        Double_t Time;
        Int_t Paddle;
    
        for (Int_t k = 0; k<nDigis; ++k)
        {
            // Retrieve individual digi and signal:
            Digi = (R3BNeulandDigi*) fDigiArray->At(k);
            Signal = (R3BSignal*) fSignalArray->At(k);
        
            // And next, retrieve digi info:
            Position = Digi->GetPosition();
            Energy = Digi->GetE();
            Time = Digi->GetT();
            Paddle = Digi->GetPaddle(); // NOTE: ranges from 1-3000!
            
            // Check that it indeed starts at 1 and ends at 3000:
            if (Paddle<=0) {cout << "### NeuLANDTranslator-Error: Paddle index was interpreted wrong!\n";}
            if (Paddle>nDoublePlanes*nPaddlesPerPlane*2) {cout << "### NeuLANDTranslator-Error: Paddle index found to be too large!\n";}
        
            // And put this into the signal:
            Signal->SetPositionX(Position.X());
            Signal->SetPositionY(Position.Y());
            Signal->SetPositionZ(Position.Z());
            Signal->SetTime(Time);
            Signal->SetEnergy(Energy);
            Signal->SetCrystalIndex(Paddle);
        
            // Set default labels:
            Signal->SetNonPrimarySim();
            Signal->SetNonPrimaryExp();
        }
    }
}
