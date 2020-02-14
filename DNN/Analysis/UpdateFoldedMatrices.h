void R3BMultMatrix::UpdateFoldedMatrices()
{
    // Update function for some of the Neutron Separation Matrices.
    Int_t nClusters_NeuLAND = fArrayCluster->GetEntries();
    Int_t nClusters_NEBULA = 0;
    if (UseNEBULA==kTRUE) {nClusters_NEBULA = fArrayNEBULACluster->GetEntries();}
    
    /// Deal with Folded matrices:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        if ((TDRMultiplicity>=0)&&(TDRMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_TDR_Folded[TDRMultiplicity][GunMultiplicity] = MultMatrix_TDR_Folded[TDRMultiplicity][GunMultiplicity] + 1.0;
        
            if (((DetectedMultiplicity>=TDRMultiplicity)&&(DetectedMultiplicity<=GunMultiplicity))||((DetectedMultiplicity<=GunMultiplicity)&&(DetectedMultiplicity>=TDRMultiplicity)))
            {
                MultMatrix_TDR_Folded_Restricted[TDRMultiplicity][GunMultiplicity] = MultMatrix_TDR_Folded_Restricted[TDRMultiplicity][GunMultiplicity] + 1.0;
            }
        
            MultMatrix_TDR_Folded[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_TDR_Folded[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            MultMatrix_TDR_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_TDR_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] + 1.0;
        }
    
        if ((DNNMultiplicity>=0)&&(DNNMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_DNN_Folded[DNNMultiplicity][GunMultiplicity] = MultMatrix_DNN_Folded[DNNMultiplicity][GunMultiplicity] + 1.0;
        
            if (((DetectedMultiplicity>=DNNMultiplicity)&&(DetectedMultiplicity<=GunMultiplicity))||((DetectedMultiplicity<=GunMultiplicity)&&(DetectedMultiplicity>=DNNMultiplicity)))
            {
                MultMatrix_DNN_Folded_Restricted[DNNMultiplicity][GunMultiplicity] = MultMatrix_DNN_Folded_Restricted[DNNMultiplicity][GunMultiplicity] + 1.0;
            }
        
            MultMatrix_DNN_Folded[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_DNN_Folded[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            MultMatrix_DNN_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_DNN_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] + 1.0;
        }
    
        if ((BetaNeuLANDMultiplicity>=0)&&(BetaNeuLANDMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_Beta_Folded[BetaNeuLANDMultiplicity][GunMultiplicity] = MultMatrix_Beta_Folded[BetaNeuLANDMultiplicity][GunMultiplicity] + 1.0;
        
            if (((DetectedMultiplicity>=BetaNeuLANDMultiplicity)&&(DetectedMultiplicity<=GunMultiplicity))||((DetectedMultiplicity<=GunMultiplicity)&&(DetectedMultiplicity>=BetaNeuLANDMultiplicity)))
            {
                MultMatrix_Beta_Folded_Restricted[BetaNeuLANDMultiplicity][GunMultiplicity] = MultMatrix_Beta_Folded_Restricted[BetaNeuLANDMultiplicity][GunMultiplicity] + 1.0;
            }
        
            MultMatrix_Beta_Folded[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Beta_Folded[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            MultMatrix_Beta_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Beta_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] + 1.0;
        }
    
        if ((SingleNeuLANDMultiplicity>=0)&&(SingleNeuLANDMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_Single_Folded[SingleNeuLANDMultiplicity][GunMultiplicity] = MultMatrix_Single_Folded[SingleNeuLANDMultiplicity][GunMultiplicity] + 1.0;
       
            if (((DetectedMultiplicity>=SingleNeuLANDMultiplicity)&&(DetectedMultiplicity<=GunMultiplicity))||((DetectedMultiplicity<=GunMultiplicity)&&(DetectedMultiplicity>=SingleNeuLANDMultiplicity)))
            {
                MultMatrix_Single_Folded_Restricted[SingleNeuLANDMultiplicity][GunMultiplicity] = MultMatrix_Single_Folded_Restricted[SingleNeuLANDMultiplicity][GunMultiplicity] + 1.0;
            }
        
            MultMatrix_Single_Folded[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Single_Folded[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            MultMatrix_Single_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Single_Folded_Restricted[MaxMultiplicity+1][GunMultiplicity] + 1.0;
        }
    }
    
    if (UseNEBULA==kTRUE)
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NEBULA>1)))
        {
            if ((BetaNEBULAMultiplicity>=0)&&(BetaNEBULAMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Beta_Folded_NEBULA[BetaNEBULAMultiplicity][GunMultiplicity] = MultMatrix_Beta_Folded_NEBULA[BetaNEBULAMultiplicity][GunMultiplicity] + 1.0;
            
                if (((NEBDetectedMultiplicity>=BetaNEBULAMultiplicity)&&(NEBDetectedMultiplicity<=GunMultiplicity))||((NEBDetectedMultiplicity<=GunMultiplicity)&&(NEBDetectedMultiplicity>=BetaNEBULAMultiplicity)))
                {
                    MultMatrix_Beta_Folded_Restricted_NEBULA[BetaNEBULAMultiplicity][GunMultiplicity] = MultMatrix_Beta_Folded_Restricted_NEBULA[BetaNEBULAMultiplicity][GunMultiplicity] + 1.0;
                }
            
                MultMatrix_Beta_Folded_NEBULA[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Beta_Folded_NEBULA[MaxMultiplicity+1][GunMultiplicity] + 1.0;
                MultMatrix_Beta_Folded_Restricted_NEBULA[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Beta_Folded_Restricted_NEBULA[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            }
        
            if ((SingleNEBULAMultiplicity>=0)&&(SingleNEBULAMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Single_Folded_NEBULA[SingleNEBULAMultiplicity][GunMultiplicity] = MultMatrix_Single_Folded_NEBULA[SingleNEBULAMultiplicity][GunMultiplicity] + 1.0;
            
                if (((NEBDetectedMultiplicity>=SingleNEBULAMultiplicity)&&(NEBDetectedMultiplicity<=GunMultiplicity))||((NEBDetectedMultiplicity<=GunMultiplicity)&&(NEBDetectedMultiplicity>=SingleNEBULAMultiplicity)))
                {
                    MultMatrix_Single_Folded_Restricted_NEBULA[SingleNEBULAMultiplicity][GunMultiplicity] = MultMatrix_Single_Folded_Restricted_NEBULA[SingleNEBULAMultiplicity][GunMultiplicity] + 1.0;
                }
            
                MultMatrix_Single_Folded_NEBULA[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Single_Folded_NEBULA[MaxMultiplicity+1][GunMultiplicity] + 1.0;
                MultMatrix_Single_Folded_Restricted_NEBULA[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Single_Folded_Restricted_NEBULA[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            }
        }        
        
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
        {
            if ((BetaCombinedMultiplicity>=0)&&(BetaCombinedMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Beta_Folded_Combined[BetaCombinedMultiplicity][GunMultiplicity] = MultMatrix_Beta_Folded_Combined[BetaCombinedMultiplicity][GunMultiplicity] + 1.0;
                
                if ((((DetectedMultiplicity+NEBDetectedMultiplicity)>=BetaCombinedMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)<=GunMultiplicity))||(((DetectedMultiplicity+NEBDetectedMultiplicity)<=GunMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)>=BetaCombinedMultiplicity)))
                {
                    MultMatrix_Beta_Folded_Restricted_Combined[BetaCombinedMultiplicity][GunMultiplicity] = MultMatrix_Beta_Folded_Restricted_Combined[BetaCombinedMultiplicity][GunMultiplicity] + 1.0;
                }
            
                MultMatrix_Beta_Folded_Combined[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Beta_Folded_Combined[MaxMultiplicity+1][GunMultiplicity] + 1.0;
                MultMatrix_Beta_Folded_Restricted_Combined[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Beta_Folded_Restricted_Combined[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            }
        
            if ((SingleCombinedMultiplicity>=0)&&(SingleCombinedMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Single_Folded_Combined[SingleCombinedMultiplicity][GunMultiplicity] = MultMatrix_Single_Folded_Combined[SingleCombinedMultiplicity][GunMultiplicity] + 1.0;
            
                if ((((DetectedMultiplicity+NEBDetectedMultiplicity)>=SingleCombinedMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)<=GunMultiplicity))||(((DetectedMultiplicity+NEBDetectedMultiplicity)<=GunMultiplicity)&&((DetectedMultiplicity+NEBDetectedMultiplicity)>=SingleCombinedMultiplicity)))
                {
                    MultMatrix_Single_Folded_Restricted_Combined[SingleCombinedMultiplicity][GunMultiplicity] = MultMatrix_Single_Folded_Restricted_Combined[SingleCombinedMultiplicity][GunMultiplicity] + 1.0;
                }
            
                MultMatrix_Single_Folded_Combined[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Single_Folded_Combined[MaxMultiplicity+1][GunMultiplicity] + 1.0;
                MultMatrix_Single_Folded_Restricted_Combined[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Single_Folded_Restricted_Combined[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            }
        }
    }
}
