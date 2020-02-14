void R3BNeuLANDTranslator::MarkSignals_NewTracing()
{
    // This function traces signals back to its respective primary track:
    Int_t nPoints = fArrayPoints->GetEntries();
    Int_t nSignals = fArraySignals->GetEntries();
    Int_t nTracks = fArrayMCPoints->GetEntries();
    R3BNeulandPoint* ThisPoint;
    R3BSignal* ThisSignal;
    R3BMCTrack* ThisTrack;
    Bool_t PrimTest;
    Bool_t SameTest;
    Bool_t TraceTest;
    Int_t ThisTrackID;
    Int_t TracedTrackID;
    
    // Store tracing:
    std::vector<std::vector<Int_t>> Traced_Tracks;
    Traced_Tracks.resize(nSignals);
    
    // Perform tracing:
    for (Int_t ksignal = 0; ksignal<nSignals; ++ksignal)
    {
        // reset vector:
        Traced_Tracks.at(ksignal).clear();
        
        // Retrieve current signal:
        ThisSignal = (R3BSignal*) fArraySignals->At(ksignal);
        
        // Next, loop over all MC points:
        for (Int_t kpoint = 0; kpoint<nPoints; ++kpoint)
        {
            // retrieve curent MC point:
            ThisPoint = (R3BNeulandPoint*) fArrayPoints->At(kpoint);
            
            // See if it matches the current bar:
            if (ThisPoint->GetDetectorID()==ThisSignal->GetCrystalIndex())
            {
                // If so, we must retrieve the corresponding MC track:
                ThisTrackID = ThisPoint->GetTrackID();
                ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ThisTrackID);
        
                // And trace it back to its primary track:
                PrimTest = kFALSE;
                TracedTrackID = -1;
        
                while ((ThisTrackID>=0)&&(PrimTest==kFALSE))
                {
                    // Test if it is a primary track:
                    if (IsPrimTrack[ThisTrackID]==kTRUE)
                    {
                        // Then, we are done:
                        PrimTest = kTRUE;
                        TracedTrackID = ThisTrackID;
                    }
            
                    // Else, start tracing back:
                    ThisTrackID = ThisTrack->GetMotherId();
            
                    if (ThisTrackID>=0)
                    {
                        ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ThisTrackID);
                    }
                }
                
                // Then, we must add it to the Traced_tracks:
                if (TracedTrackID>=0)
                {
                    // Test if it is already there:
                    SameTest = kFALSE;
                    
                    for (Int_t k = 0; k<Traced_Tracks.at(ksignal).size(); ++k)
                    {
                        if (TracedTrackID==Traced_Tracks.at(ksignal).at(k))
                        {
                            SameTest = kTRUE;
                        }
                    }
                    
                    if (SameTest==kFALSE)
                    {
                        Traced_Tracks.at(ksignal).push_back(TracedTrackID);
                    }
                }
                
                // Done.
            }
        }
    }
    
    // Now that we have the tracks to which the signals trace back, we must
    // somehow decide what the shower-head is per primary track. that is 
    // the signal that matches best to the MC first interaction point.
    // So let's do an array minimization:
    
    // Create and fill the array with distances:
    Int_t nPrimPoints = PrimIntPoints.size();
    
    Double_t** MinArray = new Double_t*[nPrimPoints];
    
    for (Int_t kprim = 0; kprim<nPrimPoints; ++kprim)
    {
        MinArray[kprim] = new Double_t[nSignals];
        
        for (Int_t ksignal = 0; ksignal<nSignals; ++ksignal)
        {
            // Retrieve current signal:
            ThisSignal = (R3BSignal*) fArraySignals->At(ksignal);
            
            // Fill array:
            MinArray[kprim][ksignal] = 0.0;
            MinArray[kprim][ksignal] = MinArray[kprim][ksignal] + (ThisSignal->GetPositionX() - PrimIntPoints.at(kprim).X())*(ThisSignal->GetPositionX() - PrimIntPoints.at(kprim).X());
            MinArray[kprim][ksignal] = MinArray[kprim][ksignal] + (ThisSignal->GetPositionY() - PrimIntPoints.at(kprim).Y())*(ThisSignal->GetPositionY() - PrimIntPoints.at(kprim).Y());
            MinArray[kprim][ksignal] = MinArray[kprim][ksignal] + (ThisSignal->GetPositionZ() - PrimIntPoints.at(kprim).Z())*(ThisSignal->GetPositionZ() - PrimIntPoints.at(kprim).Z());
            if (MinimizationMarking_IncludeTime==kTRUE)
            {
                MinArray[kprim][ksignal] = MinArray[kprim][ksignal] + ceff*ceff*(ThisSignal->GetTime() - PrimIntPoints.at(kprim).T())*(ThisSignal->GetTime() - PrimIntPoints.at(kprim).T());
            }
            MinArray[kprim][ksignal] = TMath::Sqrt(MinArray[kprim][ksignal]);
        }
    }
    
    // Next, perform the minimization:
    Double_t MinDist;
    Int_t MinDistIndex_Prim;
    Int_t MinDistIndex_Signal;
    Int_t MinDistCounter;
    Int_t nMinimizations = nPrimPoints;
    if (nPrimPoints>nSignals) {nMinimizations = nSignals;}
    
    for (Int_t kmin = 0; kmin<nMinimizations; ++kmin)
    {
        // Reset:
        MinDist = 1e90; 
        MinDistIndex_Prim = -1;
        MinDistIndex_Signal = -1;
        MinDistCounter = 0;
        
        // loop over the primary points:
        for (Int_t kprim = 0; kprim<nPrimPoints; ++kprim)
        {
            // and over the signals:
            for (Int_t ksignal = 0; ksignal<nSignals; ++ksignal)
            {
                // Next, exclude all signals that do NOT trace back to kprim:
                TraceTest = kFALSE;
                
                for (Int_t k = 0; k<Traced_Tracks.at(ksignal).size(); ++k)
                {
                    if (Traced_Tracks.at(ksignal).at(k)==PrimIntPoints_TrackID.at(kprim))
                    {
                        TraceTest = kTRUE;
                    }
                }
                
                if (TraceTest==kTRUE)
                {
                    // then, search for the minimum:
                    if (MinArray[kprim][ksignal]<MinDist)
                    {
                        MinDist = MinArray[kprim][ksignal];
                        MinDistIndex_Prim = kprim;
                        MinDistIndex_Signal = ksignal;
                        MinDistCounter = MinDistCounter + 1;
                    }
                }
            }
        }
        
        // Now, hopefully we have found a minimum:
        if (MinDistCounter>0)
        {
            // Then, we have succesfully located a shower-head:
            ThisSignal = (R3BSignal*) fArraySignals->At(MinDistIndex_Signal);
            
            // So, mark it:
            if ((MinimizationMarking_AllowMaxDist==kFALSE)||((MinimizationMarking_AllowMaxDist==kTRUE)&&(MinDist<MinimizationMarking_MaxDistance)))
            {
                ThisSignal->SetPrimarySim();
            }
            
            // And, eliminate this track and this signal from the rest of the search:
            for (Int_t kprim = 0; kprim<nPrimPoints; ++kprim)
            {
                MinArray[kprim][MinDistIndex_Signal] = 1e99;
            }
            
            for (Int_t ksignal = 0; ksignal<nSignals; ++ksignal)
            {
                MinArray[MinDistIndex_Prim][ksignal] = 1e99;
            }
            
            // Done.
        }
    }
    
    // Next, obtain the detected multiplicity:
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        if (ThisSignal->IsPrimarySim()==kTRUE) {ThisDetectedMultiplicity = ThisDetectedMultiplicity + 1;}
    }
    
    // and, delete temporary arrays:
    delete MinArray;
}

// NOTE: this function only traces back signals to their tracks. And then, it minimizes, restricted to the tracing.
// However, it does NOT check whether the min TOF MC point belongs to a signal or not. This is essential, because it
// is the min TOF point that we hope to reconstruct. This function just takes a signal that traces back, and then
// finds the best link. Almost like unrestricted minimization. However, Backtracing also does the min TOF point requirement.
// NewTracing also avoids double markings by an array minimization, but backtracing does not do that. This is important,
// as one signal may trace back to multiple primary tracks. 
