void R3BNeuLANDTranslator::MarkSignals_UseJansCode()
{
    // This function marks the primary signals based on the output of
    // Jan Mayers code. It does not need anything like primary tracks or
    // primary interaction points, because it gets its information 
    // from outside.
    
    // Reset:
    ThisDetectedMultiplicity = 0;
    Int_t TestDetMult = 0;
    Int_t TotalPrimClusters = 0;
    Int_t TotalMinSearches = 0;
    
    // Next, declare what we need:
    R3BNeulandCADHit ClusterHead;
    R3BNeulandCADCluster* ThisNeuCluster;
    R3BNeulandCADCluster* OtherNeuCluster;
    R3BSignal* ThisSignal;
    TVector3 Pos;
    Int_t nPrimClusters = fArrayJanClusters->GetEntries();
    Int_t nSignals = fArraySignals->GetEntries();
    Bool_t Clusters_Equality_Test;
    Double_t MinDist;
    Double_t Distance;
    Double_t Time;
    Int_t MinDistIndex;
    Int_t MinDistCounter;
    
    // We start by identifying double clusters in the array. The doubles
    // will be eliminated and the first one is given a higher counter:
    Bool_t* PrimCluster_IsValid = new Bool_t[nPrimClusters];
    Int_t* PrimCluster_nPrimHits = new Int_t[nPrimClusters];
        
    // Fill the arrays by default (no doubles) values:
    for (Int_t kprimclus = 0; kprimclus<nPrimClusters; ++kprimclus)
    {
        PrimCluster_IsValid[kprimclus] = kTRUE; 
        PrimCluster_nPrimHits[kprimclus] = 1;
    }
    
    // Then, loop over the clusters:
    for (Int_t kprimclus = 0; kprimclus<nPrimClusters; ++kprimclus)
    {
        // begin by testing if the cluster is valid (meaning: no double)
        // Else, we can skip the test:
        if (PrimCluster_IsValid[kprimclus]==kTRUE)
        {
            // Then, retrieve the current cluster:
            ThisNeuCluster = (R3BNeulandCADCluster*) fArrayJanClusters->At(kprimclus);
                
            // Next, loop over all clusters that we have not yet made a comparison with:
            for (Int_t n = kprimclus+1; n<nPrimClusters; ++n)
            {
                // again, test if this other cluster was already ruled out:
                if (PrimCluster_IsValid[n]==kTRUE)
                {
                    // Then, also retrieve this cluster:
                    OtherNeuCluster = (R3BNeulandCADCluster*) fArrayJanClusters->At(n);
                            
                    // And then, check if they are equal:
                    Clusters_Equality_Test = NeuClusters_AreIdentical(ThisNeuCluster,OtherNeuCluster);
                            
                    if (Clusters_Equality_Test==kTRUE)
                    {
                        // then, the second cluster becomes invalid and the first one
                        // should be counted double. note as well that this will rule 
                        // out the second cluster from entering in other equality tests 
                        // as well, which is precisely what we want.
                        PrimCluster_IsValid[n] = kFALSE;
                        PrimCluster_nPrimHits[kprimclus] = PrimCluster_nPrimHits[kprimclus] + PrimCluster_nPrimHits[n];
                        PrimCluster_nPrimHits[n] = 0;
                                
                        // End blocks:
                    }
                }
            }
        }
    }

    // Next, to be consistent with Elena, we will remove clusters appearing too often: MaxClusterMult
    for (Int_t kprimclus = 0; kprimclus<nPrimClusters; ++kprimclus)
    {
        if (PrimCluster_nPrimHits[kprimclus]>MaxClusterMult)
        {
            PrimCluster_nPrimHits[kprimclus] = MaxClusterMult;
        }
        TotalPrimClusters = TotalPrimClusters + PrimCluster_nPrimHits[kprimclus];
    }
    
    // Then, we will loop over each cluster and mark the closest unmarked signal 
    // to it as a primary hit:
    for (Int_t kprimclus = 0; kprimclus<nPrimClusters; ++kprimclus)
    {
        // check if the cluster is valid:
        if (PrimCluster_IsValid[kprimclus]==kTRUE)
        {
            // Then, retrieve the current cluster:
            ThisNeuCluster = (R3BNeulandCADCluster*) fArrayJanClusters->At(kprimclus);
            
            // Next, retrieve the cluster head (TOF, as Jan Mayer did it):
            ClusterHead = ThisNeuCluster->GetFirstHit();
            
            // And its position and time:
            Pos = ClusterHead.GetPosition();
            Time = ClusterHead.GetT();
            
            // Then, find a minimum as often as required:
            for (Int_t nMinimize = 0; nMinimize<PrimCluster_nPrimHits[kprimclus]; ++nMinimize)
            {
                // Reset:
                TotalMinSearches = TotalMinSearches + 1;
                MinDist = 1e99;
                MinDistIndex = -1;
                MinDistCounter = 0;
                
                // Loop over all signals:
                for (Int_t ksignal = 0; ksignal<nSignals; ++ksignal)
                {
                    // retrieve the current signal:
                    ThisSignal = (R3BSignal*) fArraySignals->At(ksignal);
                    
                    // only continue with the search over unmarked signals:
                    if (ThisSignal->IsPrimarySim()==kFALSE)
                    {
                        // Compute distance:
                        Distance = 0.0;
                        Distance = Distance + (ThisSignal->GetPositionX() - Pos.X())*(ThisSignal->GetPositionX() - Pos.X());
                        Distance = Distance + (ThisSignal->GetPositionY() - Pos.Y())*(ThisSignal->GetPositionY() - Pos.Y());
                        Distance = Distance + (ThisSignal->GetPositionZ() - Pos.Z())*(ThisSignal->GetPositionZ() - Pos.Z());
                        Distance = Distance + ceff*ceff*(ThisSignal->GetTime() - Time)*(ThisSignal->GetTime() - Time);
                        Distance = TMath::Sqrt(Distance);
                        
                        // Search for minimum, but ONLY consider unmarked signals:
                        if (Distance<MinDist)
                        {
                            MinDist = Distance;
                            MinDistIndex = ksignal;
                            MinDistCounter = MinDistCounter + 1;
                        }
                    }
                }
                
                // Next, if we find a minimum, mark it:
                if (MinDistCounter>0)
                {
                    ThisSignal = (R3BSignal*) fArraySignals->At(MinDistIndex);
                    ThisSignal->SetPrimarySim();
                    TestDetMult = TestDetMult + 1;
                }
                
                // Done. Close all blocks:
            }
        }
    }
    
    // Next, obtain the detected multiplicity:
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        if (ThisSignal->IsPrimarySim()==kTRUE) {ThisDetectedMultiplicity = ThisDetectedMultiplicity + 1;}
    }
    
    // Show a little warning (1.2% of cases):
    if (nPrimClusters!=ThisDetectedMultiplicity)
    {
        cout << " Event: " << EventCounter << " | Detected Multiplicity = " << ThisDetectedMultiplicity << " && #prim. clusters = " << nPrimClusters << " && nSignals = " << nSignals << " && TotalPrimClusters = " << TotalPrimClusters << " && TotalMinSearches = " << TotalMinSearches << "\n";
    }
    
    // Another warning:
    if (TestDetMult!=ThisDetectedMultiplicity)
    {
        cout << "==> ERROR: Event: " << EventCounter << " | Detected Multiplicity = " << ThisDetectedMultiplicity << " which should have been " << TestDetMult << "!\n";
    }
    
    // Done.
    delete PrimCluster_IsValid;
    delete PrimCluster_nPrimHits;
}
