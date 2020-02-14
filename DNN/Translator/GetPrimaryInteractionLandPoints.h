void R3BNeuLANDTranslator::GetPrimaryInteractionLandPoints()
{
    // Call only after BuildPrimaryTracks. It identifies the 
    // primary interaction points in NeuLAND from the primary tracks.
    // using the MC neulandPoints in NeuLAND.
    // This means, that it requires the generation of the array IsPrimTrack.
    
    // Reset multiplicities:
    ThisNeuLANDMultiplicity = 0;
    ThisNEBULAMultiplicity = 0;
    
    // Reset prim. interaction vector:
    PrimIntPoints.clear();
    PrimIntPoints_TrackID.clear();
    
    // Declare what we need:
    Int_t nTracks = fArrayMCPoints->GetEntries();
    Int_t nPoints = fArrayPoints->GetEntries();
    if (UseNEBULA==kTRUE) {nPoints = nPoints + fArrayNEBPoints->GetEntries();}
    Int_t ThisTrackID;
    Bool_t PrimTest;
    Double_t ThisMinTOF_Point;
    Int_t ThisMinTOFIndex_Point;
    R3BMCTrack* ThisTrack;
    R3BNeulandPoint* ThisPoint;
    TLorentzVector PrimPoint;
    TVector3 PrimPos;
    
    // Assign arrays:
    MinTOF = new Double_t[nTracks];
    BarIndices = new Int_t[nTracks];
    Int_t* Point_PrimIndex = new Int_t[nPoints];
    Double_t* Point_Times = new Double_t[nPoints];
         
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        MinTOF[ktrack] = 1e99;
        BarIndices[ktrack] = -1;
    }
    
    for (Int_t kpoint = 0; kpoint<nPoints; ++kpoint)
    {
        Point_PrimIndex[kpoint] = -1;
        Point_Times[kpoint] = 1e99;
    }    
    
    // Next, loop over all MC LandPoints and trace them 
    // back to their primary track.
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
    }
       
    // The next step is to loop over all primary tracks 
    // and per track, identify the minimum TOF NeuLAND point that traced back to it.
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
                // Only take points along that 
                // traced back to this track:
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
            
            // So now, ThisMinTOFIndex_Point holds the min TOF 
            // NeuLAND point index corresponding to the current primary 
            // track (if there is such a point).
            
            // So use that MC point as the primary interaction point:
            if ((ThisMinTOFIndex_Point>=0)&&(ThisMinTOFIndex_Point<nPoints))
            {
                // Retrieve the point:
                if (ThisMinTOFIndex_Point<fArrayPoints->GetEntries()) {ThisPoint = (R3BNeulandPoint*) fArrayPoints->At(ThisMinTOFIndex_Point);}
                else if (UseNEBULA==kTRUE) {ThisPoint = (R3BNeulandPoint*) fArrayNEBPoints->At(ThisMinTOFIndex_Point - fArrayPoints->GetEntries());}
                else {cout << "### NeuLAND Translator ERROR: Without NEBULA, ThisMinTOFIndex_Point should not exceed fArrayPoints->GetEntries()!\n";}

                // Obtain the corresponding TraciKID:
                ThisTrackID = ThisPoint->GetTrackID();
                
                // Fill up the required arrays:
                MinTOF[ktrack] = ThisPoint->GetTime();
                BarIndices[ktrack] = ThisPoint->GetDetectorID(); // NOTE: ranges 1-3000!
                
                // Next, store its position:
                PrimPos = ThisPoint->GetPosition();
                PrimPoint.SetT(ThisPoint->GetTime());
                PrimPoint.SetX(PrimPos.X());
                PrimPoint.SetY(PrimPos.Y());
                PrimPoint.SetZ(PrimPos.Z());
                PrimIntPoints.push_back(PrimPoint);
                PrimIntPoints_TrackID.push_back(Point_PrimIndex[ThisMinTOFIndex_Point]);
                
                if (ThisMinTOFIndex_Point<fArrayPoints->GetEntries()) {ThisNeuLANDMultiplicity = ThisNeuLANDMultiplicity + 1;}
                else if (UseNEBULA==kTRUE) {ThisNEBULAMultiplicity = ThisNEBULAMultiplicity + 1;}
                
                // Done.
            }
        }
    }
    
    // Assign NeuLAND Multiplicity:
    if (UseNEBULA==kFALSE) {ThisNeuLANDMultiplicity = PrimIntPoints.size();}
    
    // Delete Temporary Arrays:
    delete Point_PrimIndex;
    delete Point_Times;
    
    // Done.
}
