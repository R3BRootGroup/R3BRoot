void R3BMultMatrix::UpdateAlignedMatrices()
{
    // Update function for some of the Neutron Separation Matrices.
    Int_t nClusters_NeuLAND = fArrayCluster->GetEntries();
    Int_t nClusters_NEBULA = 0;
    if (UseNEBULA==kTRUE) {nClusters_NEBULA = fArrayNEBULACluster->GetEntries();}
    
    // Deal with Aligned matrices:
    
    // And the matrices without misaligned events:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        if ((GunMultiplicity==DetectedMultiplicity)&&(DetectedMultiplicity!=0))
        {
            if ((TDRMultiplicity>=0)&&(TDRMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_TDR_AlignedEvents[TDRMultiplicity][DetectedMultiplicity] = MultMatrix_TDR_AlignedEvents[TDRMultiplicity][DetectedMultiplicity] + 1.0;
                MultMatrix_TDR_AlignedEvents[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_TDR_AlignedEvents[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
            }
    
            if ((DNNMultiplicity>=0)&&(DNNMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_DNN_AlignedEvents[DNNMultiplicity][DetectedMultiplicity] = MultMatrix_DNN_AlignedEvents[DNNMultiplicity][DetectedMultiplicity] + 1.0;
                MultMatrix_DNN_AlignedEvents[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_DNN_AlignedEvents[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
            }
        
            if ((BetaNeuLANDMultiplicity>=0)&&(BetaNeuLANDMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Beta_Aligned[BetaNeuLANDMultiplicity][DetectedMultiplicity] = MultMatrix_Beta_Aligned[BetaNeuLANDMultiplicity][DetectedMultiplicity] + 1.0;
                MultMatrix_Beta_Aligned[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_Beta_Aligned[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
            }
        
            if ((SingleNeuLANDMultiplicity>=0)&&(SingleNeuLANDMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Single_Aligned[SingleNeuLANDMultiplicity][DetectedMultiplicity] = MultMatrix_Single_Aligned[SingleNeuLANDMultiplicity][DetectedMultiplicity] + 1.0;
                MultMatrix_Single_Aligned[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_Single_Aligned[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
            }
        }
    }
    
    if (UseNEBULA==kTRUE)
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NEBULA>1)))
        {
            if ((GunMultiplicity==NEBDetectedMultiplicity)&&(NEBDetectedMultiplicity!=0))
            {        
                if ((BetaNEBULAMultiplicity>=0)&&(BetaNEBULAMultiplicity<=MaxMultiplicity)&&(NEBDetectedMultiplicity>=0)&&(NEBDetectedMultiplicity<=MaxMultiplicity))
                {
                    MultMatrix_Beta_Aligned_NEBULA[BetaNEBULAMultiplicity][NEBDetectedMultiplicity] = MultMatrix_Beta_Aligned_NEBULA[BetaNEBULAMultiplicity][NEBDetectedMultiplicity] + 1.0;
                    MultMatrix_Beta_Aligned_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] = MultMatrix_Beta_Aligned_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] + 1.0;
                }
            }
        
            if ((GunMultiplicity==NEBDetectedMultiplicity)&&(NEBDetectedMultiplicity!=0))
            {        
                if ((SingleNEBULAMultiplicity>=0)&&(SingleNEBULAMultiplicity<=MaxMultiplicity)&&(NEBDetectedMultiplicity>=0)&&(NEBDetectedMultiplicity<=MaxMultiplicity))
                {
                    MultMatrix_Single_Aligned_NEBULA[SingleNEBULAMultiplicity][NEBDetectedMultiplicity] = MultMatrix_Single_Aligned_NEBULA[SingleNEBULAMultiplicity][NEBDetectedMultiplicity] + 1.0;
                    MultMatrix_Single_Aligned_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] = MultMatrix_Single_Aligned_NEBULA[MaxMultiplicity+1][NEBDetectedMultiplicity] + 1.0;
                }
            }
        }
        
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
        {
            if ((GunMultiplicity==(DetectedMultiplicity+NEBDetectedMultiplicity))&&((DetectedMultiplicity+NEBDetectedMultiplicity)!=0))
            {        
                if ((BetaCombinedMultiplicity>=0)&&(BetaCombinedMultiplicity<=MaxMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)>=0)&&((DetectedMultiplicity+NEBDetectedMultiplicity)<=MaxMultiplicity))
                {
                    MultMatrix_Beta_Aligned_Combined[BetaCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Beta_Aligned_Combined[BetaCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
                    MultMatrix_Beta_Aligned_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Beta_Aligned_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
                }
            }
        
            if ((GunMultiplicity==(DetectedMultiplicity+NEBDetectedMultiplicity))&&((DetectedMultiplicity+NEBDetectedMultiplicity)!=0))
            {        
                if ((SingleCombinedMultiplicity>=0)&&(SingleCombinedMultiplicity<=MaxMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)>=0)&&((DetectedMultiplicity+NEBDetectedMultiplicity)<=MaxMultiplicity))
                {
                    MultMatrix_Single_Aligned_Combined[SingleCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Single_Aligned_Combined[SingleCombinedMultiplicity][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
                    MultMatrix_Single_Aligned_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] = MultMatrix_Single_Aligned_Combined[MaxMultiplicity+1][(DetectedMultiplicity+NEBDetectedMultiplicity)] + 1.0;
                }
            }
        }
    }
}
