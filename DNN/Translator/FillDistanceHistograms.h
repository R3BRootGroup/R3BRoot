struct SecondaryTracks
{
    Int_t PDG;
    Double_t Theta;
    Double_t Ekin;
    Double_t Distance;
    Int_t BarIndex;
};

void R3BNeuLANDTranslator::FillDistanceHistograms()
{    
    // First,loop over all primary interaction points, and find the strictly secondary tracks
    // that originate there:
    Int_t nTracks = fArrayMCPoints->GetEntries();
    Int_t nSignals = fArraySignals->GetEntries();
    Int_t nPoints = fArrayPoints->GetEntries();
    Int_t nPrims = PrimIntPoints.size();
    R3BNeulandPoint* ThisPoint;
    R3BMCTrack* ThisTrack;
    R3BSignal* ThisSignal;
    TVector3 Momentum;
    TVector3 Position;
    SecondaryTracks OneTrackInfo;
    Int_t ThisTrackID;
    Int_t ThisMotherID;
    Int_t Point_PrimIndex;
    Int_t ThisSignalIndex;
    Double_t StartT;
    Double_t StartX;
    Double_t StartY;
    Double_t StartZ;
    Double_t px;
    Double_t py;
    Double_t pz;
    Double_t Theta;
    Double_t Ekin;
    Double_t Distance;
    Double_t MinDist;
    Int_t MinDistIndex;
    Bool_t TestCondition;
    Bool_t PrimTest;
    
    // Declare the correct tags:
    std::vector<std::vector<SecondaryTracks>> PDG_Tags;
    PDG_Tags.resize(nPrims);
    for (Int_t kprim=0; kprim>nPrims; ++kprim) {PDG_Tags.at(kprim).clear();}
    
    // Next, loop over all MC tracks to find the primary ones:
    for (Int_t ktrack = 0; ktrack<nTracks; ++ktrack)
    {
        // Retrieve the current track:
        ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ktrack);
        
        // See,if it is a strictly secondary track:
        ThisMotherID = ThisTrack->GetMotherId();
        
        if ((ThisMotherID>=0)&&(ThisMotherID<nTracks))
        {
            if (IsPrimTrack[ThisMotherID]==kTRUE)
            {
                // Retrieve the required information:
                StartT = ThisTrack->GetStartT();
                StartX = ThisTrack->GetStartX();
                StartY = ThisTrack->GetStartY();
                StartZ = ThisTrack->GetStartZ();
                px = ThisTrack->GetPx();
                py = ThisTrack->GetPy();
                pz = ThisTrack->GetPz();
                Momentum.SetX(px);
                Momentum.SetY(py);
                Momentum.SetZ(pz);
                Theta = Momentum.Theta()*180.0/TMath::Pi(); // NOTE: conversion to degrees!
                Ekin = (ThisTrack->GetEnergy() - ThisTrack->GetMass())*1000.0; // NOTE: Conversion to MeV!
                
                OneTrackInfo.PDG = ThisTrack->GetPdgCode();
                OneTrackInfo.Theta = Theta;
                OneTrackInfo.Ekin = Ekin;
                OneTrackInfo.BarIndex = InWhichNeuLANDBar(StartX,StartY,StartZ);
                
                // Retrieve index of the corresponding primary interaction point:
                MinDistIndex = -1;
                
                for (Int_t kprim = 0; kprim<nPrims; ++kprim)
                {
                    if (PrimIntPoints_TrackID.at(kprim)==ThisMotherID)
                    {
                        MinDistIndex = kprim;
                    }
                }
                
                // Add it to the proper primary track:
                if (MinDistIndex>=0)
                {
                    // Compute distance:
                    Distance = 0.0;
                    Distance = Distance + (StartX - PrimIntPoints.at(MinDistIndex).X())*(StartX - PrimIntPoints.at(MinDistIndex).X());
                    Distance = Distance + (StartY - PrimIntPoints.at(MinDistIndex).Y())*(StartY - PrimIntPoints.at(MinDistIndex).Y());
                    Distance = Distance + (StartZ - PrimIntPoints.at(MinDistIndex).Z())*(StartZ - PrimIntPoints.at(MinDistIndex).Z());
                    Distance = TMath::Sqrt(Distance);
                    OneTrackInfo.Distance = Distance;

                    // Store data:
                    PDG_Tags.at(MinDistIndex).push_back(OneTrackInfo);
                }
                
                // Done.
            }
        }
    }
                
    // Compute distances:
    std::vector<Int_t> SignalIndex;
    SignalIndex.resize(nPrims);
    for (Int_t kprim = 0; kprim<nPrims; ++kprim) {SignalIndex.at(kprim) = -1;}
    
    std::vector<Bool_t> Claimed;
    Claimed.resize(nSignals);
    for (Int_t ksig = 0; ksig<nSignals; ++ksig) {Claimed.at(ksig) = kFALSE;}
    
    for (Int_t kprim = 0; kprim<nPrims; ++kprim)
    {
        MinDist = 1e99;
        MinDistIndex = -1;
            
        for (Int_t ksig = 0; ksig<nSignals; ++ksig)
        {
            ThisSignal = (R3BSignal*) fArraySignals->At(ksig);
        
            if ((ThisSignal->IsPrimarySim()==kTRUE)&&(Claimed.at(ksig)==kFALSE))
            {
                Distance = 0.0;
                Distance = Distance + (ThisSignal->GetPositionX() - PrimIntPoints.at(kprim).X())*(ThisSignal->GetPositionX() - PrimIntPoints.at(kprim).X());
                Distance = Distance + (ThisSignal->GetPositionY() - PrimIntPoints.at(kprim).Y())*(ThisSignal->GetPositionY() - PrimIntPoints.at(kprim).Y());
                Distance = Distance + (ThisSignal->GetPositionZ() - PrimIntPoints.at(kprim).Z())*(ThisSignal->GetPositionZ() - PrimIntPoints.at(kprim).Z());
                Distance = TMath::Sqrt(Distance);
                
                if (Distance<MinDist)
                {
                    MinDist = Distance;
                    MinDistIndex = ksig;
                }
            }
        }
        
        if (MinDistIndex!=-1)
        {
            SignalIndex.at(kprim) = MinDistIndex;
            Claimed.at(MinDistIndex) = kTRUE;
            PrimToSignal_Distance->Fill(MinDist);
            
            if ((MinDist>20.0)&&(DistanceCheck==kTRUE))
            {
                cout << "EventCounter = " << EventCounter << " && Secondary tracks from prim. track " << kprim << " with MinDist = " << MinDist << ":\n";
                
                for (Int_t k = 0; k<PDG_Tags.at(kprim).size(); ++k)
                {
                    cout << "Track: " << k << ": PDG = " << PDG_Tags.at(kprim).at(k).PDG << " | Ekin[MeV] = " << PDG_Tags.at(kprim).at(k).Ekin << " | Theta[deg] = " << PDG_Tags.at(kprim).at(k).Theta << " | Dist(PrimPoint,SecStart) = " << PDG_Tags.at(kprim).at(k).Distance << " | BarIndex = " << PDG_Tags.at(kprim).at(k).BarIndex << "\n";
                }
                
                cout << "\n";
            } 
            
            TestCondition = kFALSE;
            for (Int_t k = 0; k<PDG_Tags.at(kprim).size(); ++k)
            {
                if ((PDG_Tags.at(kprim).at(k).PDG==TheNuclei->GetPDGCode("proton"))&&(PDG_Tags.at(kprim).at(k).Ekin>10.0)&&(PDG_Tags.at(kprim).at(k).Distance<10.0))
                {
                    TestCondition = kTRUE;
                }
            }
            
            if (TestCondition==kTRUE)
            {
                PrimToSignal_Distance_Conditional->Fill(MinDist);
            }
        }
    }
    
    // Make event prints for a single event:
    if ((EventCounter==243)&&(DistanceCheck==kTRUE))
    {
        // Fill histograms:
        for (Int_t kpoint = 0; kpoint<nPoints; ++kpoint)
        {
            ThisPoint = (R3BNeulandPoint*) fArrayPoints->At(kpoint);
            Position = ThisPoint->GetPosition();
            MCPrint->Fill(Position.X(),Position.Y(),Position.Z());
            
            // Find coresponding signal:
            ThisSignalIndex = -1;
            
            for (Int_t ksig = 0; ksig<nSignals; ++ksig)
            {
                ThisSignal = (R3BSignal*) fArraySignals->At(ksig);
                
                if (ThisSignal->GetCrystalIndex()==ThisPoint->GetDetectorID())
                {
                    ThisSignalIndex = ksig;
                }
            }
            
            // Trace the point back:
            ThisTrackID = ThisPoint->GetTrackID();
            ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ThisTrackID);
            PrimTest = kFALSE;
            Point_PrimIndex = -1;
        
            while ((ThisTrackID>=0)&&(PrimTest==kFALSE))
            {   
                // Test if it is a primary track:
                if (IsPrimTrack[ThisTrackID]==kTRUE)
                {
                    // Then, we are done:
                    PrimTest = kTRUE;
                    Point_PrimIndex = ThisTrackID;
                }
            
                // Else, start tracing back:
                ThisTrackID = ThisTrack->GetMotherId();
            
                if (ThisTrackID>=0)
                {
                    ThisTrack = (R3BMCTrack*) fArrayMCPoints->At(ThisTrackID);
                }
            }
            
            // Fill the appropriate histograms:
            for (Int_t kprim = 0; kprim<nPrims; ++kprim)
            {
                ThisTrackID = PrimIntPoints_TrackID.at(kprim);
                if (Point_PrimIndex==ThisTrackID)
                {
                    MCPrint_Indu[kprim]->Fill(Position.X(),Position.Y(),Position.Z());
                    
                    if (ThisSignalIndex>=0)
                    {
                        ThisSignal = (R3BSignal*) fArraySignals->At(ThisSignalIndex);
                        SignalPrint_Indu[kprim]->Fill(ThisSignal->GetPositionX(),ThisSignal->GetPositionY(),ThisSignal->GetPositionZ());
                        // NOTE: a signal can appear in multiple print-histograms, because it can trace back to multiple tracks.
                        // However, the signal can only be appointed to a single primary track. And in computing the distance histograms, 
                        // we remove the claimed signals! But backtracing will try to mark (invainly) the same signal twice.
                        // That causes the tails in the conditional distance histograms!
                    }
                }
            }
        }
        
        for (Int_t ksig = 0; ksig<nSignals; ++ksig)
        {
            ThisSignal = (R3BSignal*) fArraySignals->At(ksig);
            SignalPrint->Fill(ThisSignal->GetPositionX(),ThisSignal->GetPositionY(),ThisSignal->GetPositionZ());
        }
        
        // Add all primary tracks:
        Double_t** xpoints = new Double_t*[nPrims];
        Double_t** ypoints = new Double_t*[nPrims];
        Double_t** zpoints = new Double_t*[nPrims];
        PrimLines.resize(nPrims);
        
        for (Int_t kprim = 0; kprim<nPrims; ++kprim)
        {
            xpoints[kprim] = new Double_t[2];
            ypoints[kprim] = new Double_t[2];
            zpoints[kprim] = new Double_t[2];
            
            xpoints[kprim][0] = TargetX;
            ypoints[kprim][0] = TargetY;
            zpoints[kprim][0] = TargetZ;
            
            xpoints[kprim][1] = PrimIntPoints.at(kprim).X();
            ypoints[kprim][1] = PrimIntPoints.at(kprim).Y();
            zpoints[kprim][1] = PrimIntPoints.at(kprim).Z();
            
            PrimLines.at(kprim) = new TPolyLine3D(2,&xpoints[kprim][0],&ypoints[kprim][0],&zpoints[kprim][0]);
            PrimLines.at(kprim)->SetLineColor(2);
            PrimLines.at(kprim)->SetLineWidth(2.0);
        }
    }
   
    // Done.
}

// Distance between Primary neutron interaction points & signals: why are they large:
// 1) the first interaction may not produce a charged track at sufficient energy, or may produce no new track at all (elastic scattering)
// 2) the produced proton could be created at the edge of a bar, or in dead material, and follow an 'unlucky' flight path.
//    if this happens, it can move through the gabs of a few bars before being detected. The dead material can be circumvented by
//    modifying the geometry, but due to the discrete nature of the Monte Carlo simulations and the energy thresholds & segmented structure of NeuLAND,
//    these effects will never completely vanish.
// 3) pile-up effects. They can occur both in the digitizer, as in the backtracing. With multiple neutron tracks,
//    a signal may contain contributions from multiple tracks. The backtracing procedure cannot mark that same signal
//    twice, and since we only use each signal once in the computation of the distance histograms, the second track is forced to use a signal
//    that actually does not belong to it. This effect can be circumvented by removing the claiming-demand in the calculation here, but
//    it is best studied by only shooting a single neutron at a time on NeuLAND, so that pile-up effects from the digitization are also eliminated.
