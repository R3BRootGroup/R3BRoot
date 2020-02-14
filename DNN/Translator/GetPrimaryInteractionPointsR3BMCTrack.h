void R3BNeuLANDTranslator::GetPrimaryInteractionPointsR3BMCTrack()
{
    // Call only after BuildPrimaryTracks. It identifies the 
    // primary interaction points in NeuLAND from the primary tracks.
    // This means, that it requires the generation of the array IsPrimTrack.
    
    // Reset multiplicities:
    ThisNeuLANDMultiplicity = 0;
    
    // Reset prim. interaction vector:
    PrimIntPoints.clear();
    PrimIntPoints_TrackID.clear();
    
    // Declare what we need:
    Int_t nTracks = fArrayMCPoints->GetEntries();
    Int_t ThisPDG;
    Int_t ThisMotherID;
    Int_t ThisBarIndex;
    Double_t StartPoint_T;
    Double_t StartPoint_X;
    Double_t StartPoint_Y;
    Double_t StartPoint_Z;
    Double_t ThisTOF;
    R3BMCTrack* ThisTrack;
    TLorentzVector ThisPoint;
    
    // Assign arrays:
    MinTOF = new Double_t[nTracks];
    BarIndices = new Int_t[nTracks];
    Double_t* Tpos = new Double_t[nTracks];
    Double_t* Xpos = new Double_t[nTracks];
    Double_t* Ypos = new Double_t[nTracks];
    Double_t* Zpos = new Double_t[nTracks];
        
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        MinTOF[ktrack] = 1e99;
        BarIndices[ktrack] = -1;
        Tpos[ktrack] = 0.0;
        Xpos[ktrack] = 0.0;
        Ypos[ktrack] = 0.0;
        Zpos[ktrack] = 0.0;
    }
    
    // Next, loop over all tracks again. If 
    // 1) its starting point is in NeuLAND,
    // 2) its mother track is a primary track
    // then, the starting point of the current track is a primary interaction.
    // however, we should only take the min TOF point prim interaction,
    // because we can get at most 1 prim. inetraction per prim. track.
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        // Retrieve the current track:
        ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ktrack);
            
        // Retrieve the track properties:
        ThisPDG = ThisTrack->GetPdgCode();
        ThisMotherID = ThisTrack->GetMotherId();
        StartPoint_T = ThisTrack->GetStartT()*1e9; // NOTE: Conversion to nanoseconds
        StartPoint_X = ThisTrack->GetStartX();
        StartPoint_Y = ThisTrack->GetStartY();
        StartPoint_Z = ThisTrack->GetStartZ();
        ThisPDG = ThisTrack->GetPdgCode();
            
        // Check that the mother is a primary track:
        if ((ThisMotherID>=0)&&(ThisMotherID<nTracks))
        {
            if (IsPrimTrack[ThisMotherID]==kTRUE)
            {
                // Next, check that the starting point 
                // is inside NeuLAND:
                ThisBarIndex = InWhichNeuLANDBar(StartPoint_X,StartPoint_Y,StartPoint_Z);
                   
                if ((ThisBarIndex>=0)&&(ThisBarIndex<(nPaddlesPerPlane*nDoublePlanes*2)))
                {
                    // Next, get the TOF:
                    ThisTOF = ThisTrack->GetStartT()*1e9; // NOTE: Conversion to nanoseconds
                    
                    // And search for the minimum:
                    if (ThisTOF<MinTOF[ThisMotherID])
                    {
                        MinTOF[ThisMotherID] = ThisTOF;
                        BarIndices[ThisMotherID] = ThisBarIndex; // NOTE: ranges 1-3000!
                        Tpos[ThisMotherID] = StartPoint_T;
                        Xpos[ThisMotherID] = StartPoint_X;
                        Ypos[ThisMotherID] = StartPoint_Y;
                        Zpos[ThisMotherID] = StartPoint_Z;
                            
                        // NOTE: So if a primary track has MinTOF<1e98, it interacted in NeuLAND and we also know
                        // the bar of its first interaction. Unfortunately, this first interaction does not always 
                        // correspond to a signal in a bar.
                        
                        // Done. close all blocks:
                    }
                }
            }
        }
    }
        
    // Then, after we assigned prim. interaction points to the primary tracks,
    // loop over those same tracks again to count the NeuLAND multiplicity and store
    // the primary interaction points:
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ktrack);
            
        // check that it is a primary track:
        if (IsPrimTrack[ktrack]==kTRUE)
        {
            // Check that it interacted in NeuLAND:
            if (MinTOF[ktrack]<1e98)
            {
                // Then, update the NeuLAND multiplicity:
                ThisNeuLANDMultiplicity = ThisNeuLANDMultiplicity + 1;
                
                // And store its interaction point:
                ThisPoint.SetT(Tpos[ktrack]);
                ThisPoint.SetX(Xpos[ktrack]);
                ThisPoint.SetY(Ypos[ktrack]);
                ThisPoint.SetZ(Zpos[ktrack]);
                PrimIntPoints.push_back(ThisPoint);
                PrimIntPoints_TrackID.push_back(ktrack);
            }
        }
    }
    
    // Delete temporary arrays:
    delete Tpos;
    delete Xpos;
    delete Ypos;
    delete Zpos;
}
