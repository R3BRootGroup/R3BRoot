void R3BMultMatrix::UpdatePerfectMatrices()
{
    // Update function for some of the Neutron Separation Matrices.
    Int_t nClusters_NeuLAND = fArrayCluster->GetEntries();
    Int_t nClusters_NEBULA = 0;
    if (UseNEBULA==kTRUE) {nClusters_NEBULA = fArrayNEBULACluster->GetEntries();}
    
    // Deal with perfect matrices:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        if ((DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
        {
            MultMatrix_Max[DetectedMultiplicity][GunMultiplicity] = MultMatrix_Max[DetectedMultiplicity][GunMultiplicity] + 1.0;
            MultMatrix_Max[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Max[MaxMultiplicity+1][GunMultiplicity] + 1.0;
        }
    }
    
    if (UseNEBULA==kTRUE)
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NEBULA>1)))
        {
            if ((NEBDetectedMultiplicity>=0)&&(NEBDetectedMultiplicity<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Max_NEBULA[NEBDetectedMultiplicity][GunMultiplicity] = MultMatrix_Max_NEBULA[NEBDetectedMultiplicity][GunMultiplicity] + 1.0;
                MultMatrix_Max_NEBULA[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Max_NEBULA[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            }
        }
        
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
        {
            if (((DetectedMultiplicity+NEBDetectedMultiplicity)>=0)&&((DetectedMultiplicity+NEBDetectedMultiplicity)<=MaxMultiplicity)&&(GunMultiplicity>=0)&&(GunMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_Max_Combined[(DetectedMultiplicity+NEBDetectedMultiplicity)][GunMultiplicity] = MultMatrix_Max_Combined[(DetectedMultiplicity+NEBDetectedMultiplicity)][GunMultiplicity] + 1.0;
                MultMatrix_Max_Combined[MaxMultiplicity+1][GunMultiplicity] = MultMatrix_Max_Combined[MaxMultiplicity+1][GunMultiplicity] + 1.0;
            }
        }
    }
}
