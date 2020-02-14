void R3BNeuLANDTranslator::BuildPrimaryTracks()
{
    // Identifies the primary tracks from the R3BMCTracks.
    // it also marks the tracks and stores them as TLorentzVectors
    // in the OutputFile.
    
    // Reset Gun Multiplicity:
    ThisGunMultiplicity = 0;
    
    // Declare what we need:
    Int_t nTracks = fArrayMCPoints->GetEntries();
    Int_t PrimTrackIndex = 0;
    Int_t RecoilTrackIndex = 0;
    Int_t ThisPDG;
    Int_t ThisMotherID;
    Double_t StartPoint_X;
    Double_t StartPoint_Y;
    Double_t StartPoint_Z;
    Double_t p0;
    R3BMCTrack* ThisTrack;
    TLorentzVector* ThisPrimTrack;
    
    // Regenerate the marker for the primary tracks:
    IsPrimTrack = new Bool_t[nTracks];
    
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        IsPrimTrack[ktrack] = kFALSE;
    }
    
    // Next, loop over all MC tracks to find the primary ones:
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        // Retrieve the current track:
        ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ktrack);
        
        // Retrieve the track properties:
        ThisPDG = ThisTrack->GetPdgCode();
        ThisMotherID = ThisTrack->GetMotherId();
        StartPoint_X = ThisTrack->GetStartX();
        StartPoint_Y = ThisTrack->GetStartY();
        StartPoint_Z = ThisTrack->GetStartZ();
        
        // See if the track starting point is in the correct area and the PDG is correct too:
        if ((StartPoint_X>(TargetX-Target_PrimVolume_Thickness))&&(StartPoint_X<(TargetX+Target_PrimVolume_Thickness))&&
            (StartPoint_Y>(TargetY-Target_PrimVolume_Thickness))&&(StartPoint_Y<(TargetY+Target_PrimVolume_Thickness))&&
            (StartPoint_Z>(TargetZ-Target_PrimVolume_Thickness))&&(StartPoint_Z<(TargetZ+Target_PrimVolume_Thickness))&&
            (ThisPDG==ParticlePDG))
        {
            // Then, we have a primary track:
            ThisGunMultiplicity = ThisGunMultiplicity + 1;
        
            // Store this track:
            new ((*fArrayMCNeutronTracks)[PrimTrackIndex]) TLorentzVector();
            ThisPrimTrack = (TLorentzVector*) fArrayMCNeutronTracks->At(PrimTrackIndex);
            
            ThisPrimTrack->SetX(ThisTrack->GetPx()*1000.0);
            ThisPrimTrack->SetY(ThisTrack->GetPy()*1000.0);
            ThisPrimTrack->SetZ(ThisTrack->GetPz()*1000.0);
    
            p0 = ThisPrimTrack->X()*ThisPrimTrack->X() + ThisPrimTrack->Y()*ThisPrimTrack->Y() + ThisPrimTrack->Z()*ThisPrimTrack->Z() + ParticleMass*ParticleMass;
            p0 = TMath::Sqrt(p0);
            ThisPrimTrack->SetT(p0);
            
            // Update counters & markers:
            PrimTrackIndex = PrimTrackIndex + 1;
            IsPrimTrack[ktrack] = kTRUE;
        
            // NOTE: So whether a track is primary or not, is given by IsPrimTrack.
        }
        else if ((StartPoint_X>(TargetX-Target_PrimVolume_Thickness))&&(StartPoint_X<(TargetX+Target_PrimVolume_Thickness))&&
            (StartPoint_Y>(TargetY-Target_PrimVolume_Thickness))&&(StartPoint_Y<(TargetY+Target_PrimVolume_Thickness))&&
            (StartPoint_Z>(TargetZ-Target_PrimVolume_Thickness))&&(StartPoint_Z<(TargetZ+Target_PrimVolume_Thickness))&&
            (ThisPDG!=ParticlePDG))
        {
            // Then, we also have a primary track, but it is not a neutron. so we will save it, but we will not count it!
            
            // Store this track:
            new ((*fArrayMCRecoilTracks)[RecoilTrackIndex]) TLorentzVector();
            ThisPrimTrack = (TLorentzVector*) fArrayMCRecoilTracks->At(RecoilTrackIndex);
            
            ThisPrimTrack->SetX(ThisTrack->GetPx()*1000.0);
            ThisPrimTrack->SetY(ThisTrack->GetPy()*1000.0);
            ThisPrimTrack->SetZ(ThisTrack->GetPz()*1000.0);
    
            p0 = ThisPrimTrack->X()*ThisPrimTrack->X() + ThisPrimTrack->Y()*ThisPrimTrack->Y() + ThisPrimTrack->Z()*ThisPrimTrack->Z() + 1e6*ThisTrack->GetMass()*ThisTrack->GetMass();
            p0 = TMath::Sqrt(p0);
            ThisPrimTrack->SetT(p0);
            
            // Update index:
            RecoilTrackIndex = RecoilTrackIndex + 1;
        }
    } 
}
