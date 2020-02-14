void R3BMultMatrix::UpdateElenaMatrices()
{
    // Update function for some of the Neutron Separation Matrices.
    Int_t nClusters_NeuLAND = fArrayCluster->GetEntries();
    Int_t nClusters_NEBULA = 0;
    if (UseNEBULA==kTRUE) {nClusters_NEBULA = fArrayNEBULACluster->GetEntries();}
    
    // Elena Efficiency matrix:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        if ((NeuLANDMultiplicity==DetectedMultiplicity)&&(DetectedMultiplicity!=0))
        {
            if ((DNNMultiplicity>=0)&&(DNNMultiplicity<=MaxMultiplicity)&&(DetectedMultiplicity>=0)&&(DetectedMultiplicity<=MaxMultiplicity))
            {
                MultMatrix_DNN_ElenaEvents[DNNMultiplicity][DetectedMultiplicity] = MultMatrix_DNN_ElenaEvents[DNNMultiplicity][DetectedMultiplicity] + 1.0;
                MultMatrix_DNN_ElenaEvents[MaxMultiplicity+1][DetectedMultiplicity] = MultMatrix_DNN_ElenaEvents[MaxMultiplicity+1][DetectedMultiplicity] + 1.0;
            }
        }
    }
}
