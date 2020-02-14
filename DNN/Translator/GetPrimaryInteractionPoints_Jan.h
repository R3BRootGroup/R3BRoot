void R3BNeuLANDTranslator::GetPrimaryInteractionPoints_Jan()
{
    // This function identifies the primary interaction points in
    // NeuLAND using the outcome of Jan Mayers
    // NeulandPrimaryNeutronInteractionPoints.
    
    // Reset multiplicities:
    ThisNeuLANDMultiplicity = 0;
    
    // Reset prim. interaction vector:
    PrimIntPoints.clear();
    PrimIntPoints_TrackID.clear();
    
    // Declare what we need:
    Int_t nPrims = fArrayPrims->GetEntries();
    Int_t nTracks = fArrayMCPoints->GetEntries();
    Double_t StartPoint_T;
    Double_t StartPoint_X;
    Double_t StartPoint_Y;
    Double_t StartPoint_Z;
    Double_t PrimX;
    Double_t PrimY;
    Double_t PrimZ;
    Double_t PrimT;
    Double_t Distance;
    Double_t MinDist;
    Int_t ThisBarIndex;
    Int_t ThisMotherID;
    Int_t ThisTrackIndex;
    Int_t ThisTrackCounter;
    Int_t ThisTrackID;
    FairMCPoint* Primary;
    R3BMCTrack* ThisTrack;
    TLorentzVector ThisPoint;
    
    // Assign arrays:
    MinTOF = new Double_t[nTracks];
    BarIndices = new Int_t[nTracks];
        
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        MinTOF[ktrack] = 1e99;
        BarIndices[ktrack] = -1;
    }
    
    // Loop over all of Jan Mayers tracks:
    for (Int_t kprim = 0; kprim<nPrims; ++kprim)
    {
        // Retrieve the current track:
        Primary = (FairMCPoint*) fArrayPrims->At(kprim);
        ThisTrackID = Primary->GetTrackID();
        
        // Retrieve its position:
        PrimX = Primary->GetX();
        PrimY = Primary->GetY();
        PrimZ = Primary->GetZ();
        PrimT = Primary->GetTime(); // is already in nanoseconds!
        
        // Retrieve its Bar index:
        ThisBarIndex = InWhichNeuLANDBar(PrimX,PrimY,PrimZ);
        
        // Reset the corresponding MC track index:
        ThisTrackIndex = -1;
        ThisTrackCounter = 0;
        MinDist = 1e99;
        
        // Match this point to the R3BMCTracks:
        for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
        {
            // Retrieve the current track:
            ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ktrack);
            
            // Retrieve its start position:
            StartPoint_T = ThisTrack->GetStartT()*1e9; // NOTE: Conversion to nanoseconds
            StartPoint_X = ThisTrack->GetStartX();
            StartPoint_Y = ThisTrack->GetStartY();
            StartPoint_Z = ThisTrack->GetStartZ();
            
            // Retrieve MotherID (which is supposed to be a primary track
            // if all went well):
            ThisMotherID = ThisTrack->GetMotherId();
            
            // Compute the distance to the primary track:
            Distance = 0.0;
            Distance = Distance + ceff*ceff*(PrimT-StartPoint_T)*(PrimT-StartPoint_T);
            Distance = Distance + (PrimX-StartPoint_X)*(PrimX-StartPoint_X);
            Distance = Distance + (PrimY-StartPoint_Y)*(PrimY-StartPoint_Y);
            Distance = Distance + (PrimZ-StartPoint_Z)*(PrimZ-StartPoint_Z);
            Distance = TMath::Sqrt(Distance);
            
            // Corrupt distance if the motherID is not a primary track:
            if (ThisMotherID<0) {Distance = 1e99;}
            if (ThisMotherID>=nTracks) {Distance = 1e99;}
            if ((ThisMotherID>=0)&&(ThisMotherID<nTracks)) {if (IsPrimTrack[ThisMotherID]==kFALSE) {Distance = 1e99;}}
            
            // Next, find the minimum:
            if (Distance<MinDist)
            {
                MinDist = Distance;
                ThisTrackIndex = ktrack;
                ThisTrackCounter = ThisTrackCounter + 1;
            }
            
            // Next, close the track loop:
        }
        
        // Then, now that we have linked Jan Mayers tracks to the MC-tracks, we can fill in the rest of the arrays:
        if ((ThisTrackCounter>0)&&(MinDist<0.1))
        {
            // Then, we are OK:
            ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ThisTrackIndex);
            ThisMotherID = ThisTrack->GetMotherId();
            MinTOF[ThisMotherID] = ThisTrack->GetStartT()*1e9; // NOTE: Conversion to nanoseconds!
            BarIndices[ThisMotherID] = ThisBarIndex; // NOTE: ranges 1-3000!
        }
        else
        {
            cout << "R3BNeuLANDTranslator ERROR: No match found for: primary track = " << kprim << " | event = " << EventCounter << " | Min. Distance = " << MinDist << "\n";
            // NOTE: This error does not mean something serious. It just means that the arrays could not be filled, but multiplicities are unaltered!
        }
        
        // Next, store the interaction point and assign the NeuLAND multiplicity:
        ThisNeuLANDMultiplicity = ThisNeuLANDMultiplicity + 1;
                
        // And store its interaction point:
        ThisPoint.SetT(PrimT);
        ThisPoint.SetX(PrimX);
        ThisPoint.SetY(PrimY);
        ThisPoint.SetZ(PrimZ);
        PrimIntPoints.push_back(ThisPoint);
        PrimIntPoints_TrackID.push_back(ThisTrackID);
        
        // Then, close the loop over primary tracks:
    }
    
    // Done.
}
