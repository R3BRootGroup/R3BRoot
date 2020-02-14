void R3BNeuLANDTranslator::MarkSignals_BackTracing()
{
    // This function marks the primary signals based on
    // backtracing. It needs the primary tracks to be built
    // because it requires the IsPrimTrack-array to be build.
    
    // Reset the current multiplicity:
    ThisDetectedMultiplicity = 0;
    
    // Declare what we need:
    Int_t nPoints = fArrayPoints->GetEntries();
    if (UseNEBULA==kTRUE) {nPoints = nPoints + fArrayNEBPoints->GetEntries();}
    Int_t nSignals = fArraySignals->GetEntries();
    Int_t nSignals_NEBULA = 0; 
    if (UseNEBULA==kTRUE) {nSignals_NEBULA = fArrayNEBULASignals->GetEntries();}
    Int_t nTracks = fArrayMCPoints->GetEntries();
    R3BNeulandPoint* ThisPoint;
    R3BSignal* ThisSignal;
    R3BMCTrack* ThisTrack;
    Bool_t PrimTest;
    Int_t ThisTrackID;
    Double_t ThisMinTOF_Point;
    Int_t ThisMinTOFIndex_Point;
    Int_t ThisBarIndex;
    Double_t MinDist;
    Double_t Distance;
    Int_t MinDist_Index;
    Int_t MinDist_Counter;
    Bool_t MinDist_Check;
    
    // Declare markers for the points:
    Bool_t* Point_IsValid = new Bool_t[nPoints];
    Int_t* Point_SignalIndex = new Int_t[nPoints];
    Int_t* Point_SignalIndex_NEBULA = new Int_t[nPoints];
    Int_t* Point_PrimIndex = new Int_t[nPoints];
    Double_t* Point_Times = new Double_t[nPoints];
    
    for (Int_t kpoint = 0; kpoint<nPoints; ++kpoint)
    {
        Point_IsValid[kpoint] = kFALSE;
        Point_SignalIndex[kpoint] = -1;
        Point_SignalIndex_NEBULA[kpoint] = -1;
        Point_PrimIndex[kpoint] = -1;
        Point_Times[kpoint] = 1e99;
    }
    
    // Then, loop over all MC points:
    for (Int_t kpoint = 0; kpoint<nPoints; ++kpoint)
    {
        // Retrieve the current point:
        if (kpoint<fArrayPoints->GetEntries()) {ThisPoint = (R3BNeulandPoint*) fArrayPoints->At(kpoint);}
        else if (UseNEBULA==kTRUE) {ThisPoint = (R3BNeulandPoint*) fArrayNEBPoints->At(kpoint - fArrayPoints->GetEntries());}
        else {cout << "### NeuLAND Translator ERROR: Without NEBULA, kpoint should not exceed fArrayPoints->GetEntries()!\n";}
        
        // Store its TOF:
        Point_Times[kpoint] = ThisPoint->GetTime(); // Are already nanoseconds!
        
        // Retrieve the corresponding track ID:
        ThisTrackID = ThisPoint->GetTrackID();
        
        // Retrieve the corresponding MC track:
        ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ThisTrackID);
        
        // Trace it back to its primary track:
        PrimTest = kFALSE;
        
        while ((ThisTrackID>=0)&&(PrimTest==kFALSE))
        {
            // Test if it is a primary track:
            if (IsPrimTrack[ThisTrackID]==kTRUE)
            {
                // Then, we are done:
                PrimTest = kTRUE;
                Point_PrimIndex[kpoint] = ThisTrackID;
            }
            
            // Else, start tracing back:
            ThisTrackID = ThisTrack->GetMotherId();
            
            if (ThisTrackID>=0)
            {
                ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ThisTrackID);
            }
        }
        
        // NOTE: NOT each NeuLAND point should come from a prim track.
        // We can also have background points that did not come from 
        // the target. But the while-loop will always end. We enter 
        // with ThisTrackID>=0 & PrimTest==kFALSE by definition.
        // Then, if we happen to have the correct track immediately,
        // we set PrimTest to kTRUE, so we will NOT have another while-step.
        // and we mark the point with the proper index. Else, we take 
        // one step back. If we hit -1 without putting PrimTest to kTRUE,
        // the kpoint will not be assigned to a primary track.
        
        // Next, we will search for a matching signal. Get the bar index:
        ThisBarIndex = ThisPoint->GetDetectorID(); // NOTE: ranges 1-3000, just like signal->GetCrystalIndex!
        
        // Then, loop over NeuLAND signals to find a corresponding one:
        for (Int_t ksignal = 0; ksignal<nSignals; ++ksignal)
        {
            // Retrieve the current signal:
            ThisSignal = (R3BSignal*) fArraySignals->At(ksignal);
            
            // Next, compare the bars:
            if (ThisSignal->GetCrystalIndex()==ThisBarIndex)
            {
                // Then, the point is valid, but only if 
                // the MC point also is located in NeuLAND:
                if (kpoint<fArrayPoints->GetEntries())
                {
                    Point_IsValid[kpoint] = kTRUE;
                    Point_SignalIndex[kpoint] = ksignal;
                    Point_SignalIndex_NEBULA[kpoint] = -1;
                }
                
                // NOTE: there is only one signal per bar & each 
                // point has only one bar, so there is no way 
                // to mess this up.
            }
        }
        
        // Same for NEBULA signals:
        if (UseNEBULA==kTRUE)
        {
            for (Int_t ksignal = 0; ksignal<nSignals_NEBULA; ++ksignal)
            {
                // Retrieve the current signal:
                ThisSignal = (R3BSignal*) fArrayNEBULASignals->At(ksignal);
            
                // Next, compare the bars:
                if (ThisSignal->GetCrystalIndex()==ThisBarIndex)
                {
                    // Then, the point is valid, but only if 
                    // the MC point also is located in NEBULA:
                    if (kpoint>=fArrayPoints->GetEntries())
                    {
                        Point_IsValid[kpoint] = kTRUE;
                        Point_SignalIndex[kpoint] = -1;
                        Point_SignalIndex_NEBULA[kpoint] = ksignal;
                    }
                
                    // NOTE: there is only one signal per bar & each 
                    // point has only one bar, so there is no way 
                    // to mess this up.
                }
            }
        }
        
        // Done. points are now marked as valid and traced back to their
        // primaries.
    }
       
    // The next step is now, to loop over all primary tracks inside
    // NeuLAND and per track, identify the minimum TOF NeuLAND point:
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        // See if the current track is a prim. track. We do NOT check whether
        // it interacted in NeuLAND yes/no, because if we do, we may skip 
        // landpoints that do not come from the generation of a new track.
        if (IsPrimTrack[ktrack]==kTRUE)
        {
            // Then, loop over all points and search for min TOF:
            ThisMinTOF_Point = 1e99;
            ThisMinTOFIndex_Point = -1;
            
            for (Int_t kpoint = 0; kpoint<nPoints; ++kpoint)
            {
                // Only take valid points along and points that 
                // traced back to this track:
                if (Point_IsValid[kpoint]==kTRUE)
                {
                    if (Point_PrimIndex[kpoint]==ktrack)
                    {
                        // search for min TOF:
                        if (Point_Times[kpoint]<ThisMinTOF_Point)
                        {
                            ThisMinTOF_Point = Point_Times[kpoint];
                            ThisMinTOFIndex_Point = kpoint;
                                
                            // close all blocks:
                        }
                    }
                }
            }
            
            // So now, ThisMinTOFIndex_Point holds the min TOF 
            // NeuLAND (or NEBULA) point index corresponding to the current primary 
            // track (if there is such a point). Point_SignalIndex at 
            // ThisMinTOFIndex_Point then holds the proper signal to mark.
            // But then, the question is whether we should take the NEBULA of the NeuLAND
            // signal. But one of them should always be -1, so that is not too difficult.
            
            // Check for a valid MC index:
            if ((ThisMinTOFIndex_Point>=0)&&(ThisMinTOFIndex_Point<nPoints))
            {
                // Check for valid Signal-index:
                if (((Point_SignalIndex[ThisMinTOFIndex_Point]>=0)&&
                     (Point_SignalIndex[ThisMinTOFIndex_Point]<nSignals)&&
                     (UseNEBULA==kFALSE))||
                    
                    ((UseNEBULA==kTRUE)&&
                     ((Point_SignalIndex[ThisMinTOFIndex_Point]>=0)&&
                      (Point_SignalIndex[ThisMinTOFIndex_Point]<=nSignals))||
                     ((Point_SignalIndex_NEBULA[ThisMinTOFIndex_Point]>=0)&&
                      (Point_SignalIndex_NEBULA[ThisMinTOFIndex_Point]<nSignals_NEBULA))))
                {
                    // Then, retrieve the proper signal:
                    if ((UseNEBULA==kFALSE)||((UseNEBULA==kTRUE)&&(Point_SignalIndex_NEBULA[ThisMinTOFIndex_Point]==-1)))
                    {
                        // Then, just take the regular NeuLAND signal:
                        ThisSignal = (R3BSignal*) fArraySignals->At(Point_SignalIndex[ThisMinTOFIndex_Point]);
                    }
                    else if ((UseNEBULA==kTRUE)&&(Point_SignalIndex[ThisMinTOFIndex_Point]==-1))
                    {
                        // Then, we must take a NEBULA signal:
                        ThisSignal = (R3BSignal*) fArrayNEBULASignals->At(Point_SignalIndex_NEBULA[ThisMinTOFIndex_Point]);
                    }
                    else
                    {
                        cout << "### NeuLAND Translator ERROR: This choice for marking a signal should never occur!\n";
                    }
                    
                    // NOTE: it may occur at this point that ThisSignal was already marked as a primary signal.
                    // this is a rare occasion (about 2%), but this happens if 2 primary neutrons have their
                    // primary interaction both within the same NeuLAND bar. In this case, it is impossible 
                    // for NeuLAND to disentangle the 2 primary neutron hits. In terms of multiplicity, we
                    // could count this signal double and move on, but in practise, the two particle 
                    // showers will completely overlap, so only based on the magnitude of the QDC signals
                    // one might see that there is a double shower. Not for the rest of the pattern.
                    // And due to the giant overlap of the calorimetry, the magnitude of the QDC signals
                    // will not be very reliable. Hence, I choose to consider such events as lost and I
                    // will not do something to recover these events within this algorithm.
                
                    // A double does not necessarily have to have an overlap in particle showers. It is also
                    // possible that the two neutrons hit the same bar at different places. But if that
                    // happens, one of the prim. int. points is inherently lost. So then, we have a problem too.
                
                    // Next, search for the minimum distance between this signal and the primary neutron 
                    // interaction points:
                    MinDist = 1e99;
                    MinDist_Index = -1;
                    MinDist_Counter = 0;
                    MinDist_Check = kFALSE;
                
                    if (PrimIntPoints.size()>0)
                    {
                        for (Int_t kint = 0; kint<PrimIntPoints.size(); ++kint)
                        {
                            // Compute the distance:
                            Distance = 0.0;
                            Distance = Distance + (PrimIntPoints.at(kint).X() - ThisSignal->GetPositionX())*(PrimIntPoints.at(kint).X() - ThisSignal->GetPositionX());
                            Distance = Distance + (PrimIntPoints.at(kint).Y() - ThisSignal->GetPositionY())*(PrimIntPoints.at(kint).Y() - ThisSignal->GetPositionY());
                            Distance = Distance + (PrimIntPoints.at(kint).Z() - ThisSignal->GetPositionZ())*(PrimIntPoints.at(kint).Z() - ThisSignal->GetPositionZ());
                            if (MinimizationMarking_IncludeTime==kTRUE) 
                            {
                                Distance = Distance + (PrimIntPoints.at(kint).T() - ThisSignal->GetTime())*(PrimIntPoints.at(kint).T() - ThisSignal->GetTime())*ceff*ceff;
                            }
                            Distance = TMath::Sqrt(Distance);
                        
                            // Search for the minimum:
                            if (Distance<MinDist)
                            {
                                MinDist = Distance;
                                MinDist_Index = kint;
                                MinDist_Counter = MinDist_Counter + 1;
                            }
                        }
                    }
                
                    // Next, only mark the matching signal as a primary, if the min.
                    // distance is below a certain threshold, when that threshold has to be used:
                    if ((MinimizationMarking_AllowMaxDist==kFALSE)||((MinimizationMarking_AllowMaxDist==kTRUE)&&(MinDist<MinimizationMarking_MaxDistance)))
                    {
                        // Then, mark the signal:
                        ThisSignal->SetPrimarySim();
                    }
                }
            }
        }
    }
    
    // Next, obtain the detected multiplicities:
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        if (ThisSignal->IsPrimarySim()==kTRUE) {ThisDetectedMultiplicity = ThisDetectedMultiplicity + 1;}    
    }
    
    if (UseNEBULA==kTRUE)
    {
        for (Int_t k = 0; k<nSignals_NEBULA; ++k)
        {
            ThisSignal = (R3BSignal*) fArrayNEBULASignals->At(k);
            if (ThisSignal->IsPrimarySim()==kTRUE) {ThisNEBDetectedMultiplicity = ThisNEBDetectedMultiplicity + 1;}    
        }
    }
    
    // Delete the temporary arrays:
    delete Point_IsValid;
    delete Point_SignalIndex;
    delete Point_SignalIndex_NEBULA;
    delete Point_Times;
    delete Point_PrimIndex;
    
}
