Bool_t R3BNeuLANDTranslator::NeuClusters_AreIdentical(R3BNeulandCADCluster* First, R3BNeulandCADCluster* Second)
{
    // Begin by comparing the sizes:
    if (First->GetSize()!=Second->GetSize())
    {
        return kFALSE;
    }
    else
    {
        // Then, obtain the cluster size:
        Int_t ClusterSize = First->GetSize();
    
        // Obtain the hits of the first cluster:
        std::vector<R3BNeulandCADHit> FirstHitSet = First->GetHits();
    
        // And also of the second one:
        std::vector<R3BNeulandCADHit> SecondHitSet = Second->GetHits();
        
        // Then, add up the quadratic distances in space & time 
        // between all the hits:
        Double_t TotalDistance = 0.0;
        
        TVector3 FirstPosition;
        TVector3 SecondPosition;
        Double_t FirstTime;
        Double_t SecondTime;
        
        // Loop over the clusters:
        for (Int_t k = 0; k<ClusterSize; ++k)
        {
            // Get position & time:
            FirstPosition = FirstHitSet.at(k).GetPosition();
            SecondPosition = SecondHitSet.at(k).GetPosition();
            FirstTime = FirstHitSet.at(k).GetT();
            SecondTime = SecondHitSet.at(k).GetT();
            
            // Add up the Distance:
            TotalDistance = TotalDistance + ceff*ceff*(FirstTime - SecondTime)*(FirstTime - SecondTime);
            TotalDistance = TotalDistance + (FirstPosition.X() - SecondPosition.X())*(FirstPosition.X() - SecondPosition.X());
            TotalDistance = TotalDistance + (FirstPosition.Y() - SecondPosition.Y())*(FirstPosition.Y() - SecondPosition.Y());
            TotalDistance = TotalDistance + (FirstPosition.Z() - SecondPosition.Z())*(FirstPosition.Z() - SecondPosition.Z());
        }
        
        // Take sqrt of toal:
        TotalDistance = TMath::Sqrt(TotalDistance);
        
        // Then, make a decision:
        if (TotalDistance>0.1) {return kFALSE;}
        else {return kTRUE;}
    }
}
