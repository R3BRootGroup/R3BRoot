void R3BNeuLANDTranslator::MarkSignals_MinimizeArray()
{
    // This function marks the primary signals based on a
    // minimization procedure. It needs the primary tracks to be built
    // and it needs the primary interaction points to be assigned.
    // We use an array-minimization to select the best possible
    // matches. No minimum distance, or hard-cuts are used.
    
    // Reset:
    ThisDetectedMultiplicity = 0;
    
    // Get the number of primary interactions and the number of signals:
    Int_t nPrims = ThisNeuLANDMultiplicity;
    Int_t nSignals = fArraySignals->GetEntries();
    R3BSignal* Signal;
    
    // Next, only proceed if there are primary interaction points and signals:
    if ((nSignals>0)&&(nPrims>0))
    {
        // Then, declare what we need:
        Double_t Minimum = 1e98;
        Int_t SignalIndex = -1;
        Int_t PrimIndex = -1;
        Int_t MinimizeTimes = 0;
        Double_t PrimT;
        Double_t PrimX;
        Double_t PrimY;
        Double_t PrimZ;
        Double_t SignalT;
        Double_t SignalX;
        Double_t SignalY;
        Double_t SignalZ;
        
        // Declare our minimization array:
        Double_t** DistanceArray = new Double_t*[nPrims];
        
        for (Int_t n = 0; n<nPrims; ++n)
        {
            DistanceArray[n] = new Double_t[nSignals];
        }
        
        // Next, loop over both the primary points and the signals
        // to fill the array:
        for (Int_t n = 0; n<nPrims; ++n)
        {
            // Retrieve the primary point position:
            PrimX = PrimIntPoints.at(n).X();
            PrimY = PrimIntPoints.at(n).Y();
            PrimZ = PrimIntPoints.at(n).Z();
            PrimT = PrimIntPoints.at(n).T();
                
            // Next, loop over the signals:
            for (Int_t k = 0; k<nSignals; ++k)
            {
                // Retrieve the current signal:
                Signal = (R3BSignal*) fArraySignals->At(k);
                
                // Get its position:
                SignalX = Signal->GetPositionX();
                SignalY = Signal->GetPositionY();
                SignalZ = Signal->GetPositionZ();
                SignalT = Signal->GetTime();
                
                // Then, compute the distance:
                DistanceArray[n][k] = 0.0;
                if (MinimizationMarking_IncludeTime==kTRUE) {DistanceArray[n][k] = DistanceArray[n][k] + ceff*ceff*(SignalT-PrimT)*(SignalT-PrimT);}
                DistanceArray[n][k] = DistanceArray[n][k] + (SignalX-PrimX)*(SignalX-PrimX);
                DistanceArray[n][k] = DistanceArray[n][k] + (SignalY-PrimY)*(SignalY-PrimY);
                DistanceArray[n][k] = DistanceArray[n][k] + (SignalZ-PrimZ)*(SignalZ-PrimZ);
                DistanceArray[n][k] = TMath::Sqrt(DistanceArray[n][k]);
        
                // Done. close the blocks:
            }
        }
        
        // Next, find overall minimum of the matrix and do it nPrims-times:
        // However, if nSignals is smaller than nPrims (which might happen
        // on rare occasions, we should only do it nSignals-times:
        MinimizeTimes = nPrims;
        if (nPrims>nSignals) {MinimizeTimes = nSignals;}
        
        for (Int_t time = 0; time<MinimizeTimes; ++time)
        {
            // Search for one overall minimum:
            Minimum = 1e98;
            SignalIndex = -1;
            PrimIndex = -1;
        
            for (Int_t n = 0; n<nPrims; ++n)
            {
                for (Int_t k = 0; k<nSignals; ++k)
                {
                    if (DistanceArray[n][k]<Minimum)
                    {
                        Minimum = DistanceArray[n][k];
                        SignalIndex = k;
                        PrimIndex = n;
                    }
                }   
            }
        
            // This now means that SignalIndex and PrimIndex should be linked.
            // So mark the corresponding signal. However, 
            if ((SignalIndex>=0)&&(PrimIndex>=0))
            {
                if ((MinimizationMarking_AllowMaxDist==kFALSE)||((MinimizationMarking_AllowMaxDist==kTRUE)&&(Minimum<MinimizationMarking_MaxDistance)))
                {
                    // Then, mark the signal:
                    Signal = (R3BSignal*) fArraySignals->At(SignalIndex);
                    Signal->SetPrimarySim();
                }
                else
                {
                    // Do not mark anything.
                }
                    
                // Next, we must eliminate this link from the matrix search. Remember that we should
                // also remove it from the search if it failed due to the distance limit, because 
                // otherwise, we keep being caught in the same overall minimum.
                for (Int_t n = 0; n<nPrims; ++n)
                {
                    DistanceArray[n][SignalIndex] = 1e99; // NOTE: bigger then the start-value of Minimum!
                }
            
                for (Int_t k = 0; k<nSignals; ++k)
                {
                    DistanceArray[PrimIndex][k] = 1e99; // NOTE: bigger then the start-value of Minimum!
                }
            }
            else
            {
                cout << "NeuLANDTranslator FAILURE: We could not find a minimum at time = " << time << "!\n";
                cout << "                 : nPrims = " << nPrims << " | nSignals = " << nSignals << " in event " << EventCounter << "\n";
            }
            
            // And next, we can repeat the search process again.
        }
        
        // Delete the Distance Array:
        delete DistanceArray;
    }
        
    // Marking is now done. Hence, determine the detected multiplicity:
    for (Int_t k = 0; k<nSignals; ++k)
    {
        Signal = (R3BSignal*) fArraySignals->At(k);
        if (Signal->IsPrimarySim()==kTRUE) {ThisDetectedMultiplicity = ThisDetectedMultiplicity + 1;}
    }
    
    // NOTE: Due to the minimization using an array instead of just finding the best sutor for each 
    // primary interaction point, we avoid 'doubles': trying to mark the same signal multiple times 
    // for different primary neutron interaction points.
    
    // Done.
}
