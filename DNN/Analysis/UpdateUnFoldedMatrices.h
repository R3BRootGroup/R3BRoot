void R3BMultMatrix::UpdateUnFoldedMatrices()
{
    // Update function for some of the Neutron Separation Matrices.
    Int_t nClusters_NeuLAND = fArrayCluster->GetEntries();
    Int_t nClusters_NEBULA = 0;
    if (UseNEBULA==kTRUE) {nClusters_NEBULA = fArrayNEBULACluster->GetEntries();}
    
    // Deal with UnFolded matrices:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        if ((TDRMultiplicity>=0)&&(TDRMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_TDR_UnFolded[TDRMultiplicity][DetectedMultiplicity] = MultMatrix_TDR_UnFolded[TDRMultiplicity][DetectedMultiplicity] + 1.0;
            MultMatrix_TDR_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_TDR_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
        }
    
        if ((DNNMultiplicity>=0)&&(DNNMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_DNN_UnFolded[DNNMultiplicity][DetectedMultiplicity] = MultMatrix_DNN_UnFolded[DNNMultiplicity][DetectedMultiplicity] + 1.0;
            MultMatrix_DNN_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_DNN_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
        }
    
        if ((BetaNeuLANDMultiplicity>=0)&&(BetaNeuLANDMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_Beta_UnFolded[BetaNeuLANDMultiplicity][DetectedMultiplicity] = MultMatrix_Beta_UnFolded[BetaNeuLANDMultiplicity][DetectedMultiplicity] + 1.0;
            MultMatrix_Beta_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_Beta_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
        }
    
        if ((SingleNeuLANDMultiplicity>=0)&&(SingleNeuLANDMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_Single_UnFolded[SingleNeuLANDMultiplicity][DetectedMultiplicity] = MultMatrix_Single_UnFolded[SingleNeuLANDMultiplicity][DetectedMultiplicity] + 1.0;
            MultMatrix_Single_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_Single_UnFolded[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
        }
    }
    
    if (UseNEBULA==kTRUE)
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NEBULA>1)))
        {
            if ((BetaNEBULAMultiplicity>=0)&&(BetaNEBULAMultiplicity<=MaxMultiplicity)&&(NEBDetectedMultiplicity>=0)&&(NEBDetectedMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Beta_UnFolded_NEBULA[BetaNEBULAMultiplicity][NEBDetectedMultiplicity] = MultMatrix_Beta_UnFolded_NEBULA[BetaNEBULAMultiplicity][NEBDetectedMultiplicity] + 1.0;
                MultMatrix_Beta_UnFolded_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] = MultMatrix_Beta_UnFolded_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] + 1.0;
            }
        
            if ((SingleNEBULAMultiplicity>=0)&&(SingleNEBULAMultiplicity<=MaxMultiplicity)&&(NEBDetectedMultiplicity>=0)&&(NEBDetectedMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Single_UnFolded_NEBULA[SingleNEBULAMultiplicity][NEBDetectedMultiplicity] = MultMatrix_Single_UnFolded_NEBULA[SingleNEBULAMultiplicity][NEBDetectedMultiplicity] + 1.0;
                MultMatrix_Single_UnFolded_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] = MultMatrix_Single_UnFolded_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] + 1.0;
            }
        }
        
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
        {
            if ((BetaCombinedMultiplicity>=0)&&(BetaCombinedMultiplicity<=MaxMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)>=0)&&((DetectedMultiplicity+NEBDetectedMultiplicity)<=MaxMultiplicity))
            {
                MultMatrix_Beta_UnFolded_Combined[BetaCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Beta_UnFolded_Combined[BetaCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
                MultMatrix_Beta_UnFolded_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Beta_UnFolded_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
            }
        
            if ((SingleCombinedMultiplicity>=0)&&(SingleCombinedMultiplicity<=MaxMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)>=0)&&((DetectedMultiplicity+NEBDetectedMultiplicity)<=MaxMultiplicity))
            {
                MultMatrix_Single_UnFolded_Combined[SingleCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Single_UnFolded_Combined[SingleCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
                MultMatrix_Single_UnFolded_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Single_UnFolded_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
            }
        }
    }
}
