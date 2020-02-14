void R3BTextFileGenerator::FillOneCluster(R3BSignalCluster* ThisCluster)
{
    // Function to write one single cluster to the InputNeurons and OutputNeurons.
    // The function will write many cluster properties to the inputs: all Scoring+
    // quantities. By replacing the totalscore-sorting by a DNN, correlations
    // between these parameters can be taken along.
    
    // Decide upon INPUT normalization first:
    if (ExecuteNormalization==kFALSE)
    {
        // Then, it is easy. Simply fill.
        
        // First, do T, X, Y, Z of the cluster head:
        InputNeurons.push_back(TheScorers->GetPrimaryClusterHit(ThisCluster)->GetTime());
        InputNeurons.push_back(TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionX());
        InputNeurons.push_back(TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionY());
        InputNeurons.push_back(TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionZ());
        
        // Then, go about all of the cluster scoring properties:
        if (TheScorers->ClusterScore_DepositedEnergy_IsUsed()==kTRUE)  {InputNeurons.push_back(TheScorers->ComputeClusterScoreValue_Energy(ThisCluster));}
        if (TheScorers->ClusterScore_Size_IsUsed()==kTRUE)             {InputNeurons.push_back(TheScorers->ComputeClusterScoreValue_Size(ThisCluster));}
        if (TheScorers->ClusterScore_KineticEnergy_IsUsed()==kTRUE)    {InputNeurons.push_back(TheScorers->ComputeClusterScoreValue_Ekin(ThisCluster));}
        if (TheScorers->ClusterScore_RelativisticBeta_IsUsed()==kTRUE) {InputNeurons.push_back(TheScorers->ComputeClusterScoreValue_Beta(ThisCluster));}
        if (TheScorers->ClusterScore_Rvalue_IsUsed()==kTRUE)           {InputNeurons.push_back(TheScorers->ComputeClusterScoreValue_Rvalue(ThisCluster));}
        if (TheScorers->ClusterScore_LogR_IsUsed()==kTRUE)             {InputNeurons.push_back(TheScorers->ComputeClusterScoreValue_LogR(ThisCluster));}
        if (TheScorers->ClusterScore_TOF_IsUsed()==kTRUE)              {InputNeurons.push_back(TheScorers->ComputeClusterScoreValue_TOF(ThisCluster));}
        if (TheScorers->ClusterScore_SpaceLength_IsUsed()==kTRUE)      {InputNeurons.push_back(TheScorers->ComputeClusterSpaceLength(ThisCluster));}
        if (TheScorers->ClusterScore_SpaceRadius_IsUsed()==kTRUE)      {InputNeurons.push_back(TheScorers->ComputeClusterSpaceRadius(ThisCluster));}
        if (TheScorers->ClusterScore_TimeLength_IsUsed()==kTRUE)       {InputNeurons.push_back(TheScorers->ComputeClusterTimeLength(ThisCluster));}
        
        // Done.
    }
    else
    {
        // Then, first learn the normalizations:
        LearnAdvancedNorms->AdaptOldScale(TimeNormalization_NormName.Data(),TheScorers->GetPrimaryClusterHit(ThisCluster)->GetTime());
        LearnAdvancedNorms->AdaptOldScale("X",TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionX());
        LearnAdvancedNorms->AdaptOldScale("Y",TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionY());
        LearnAdvancedNorms->AdaptOldScale("Z",TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionZ());
                
        if (TheScorers->ClusterScore_DepositedEnergy_IsUsed()==kTRUE)  {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_Energy",TheScorers->ComputeClusterScoreValue_Energy(ThisCluster));}
        if (TheScorers->ClusterScore_Size_IsUsed()==kTRUE)             {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_Size",TheScorers->ComputeClusterScoreValue_Size(ThisCluster));}
        if (TheScorers->ClusterScore_KineticEnergy_IsUsed()==kTRUE)    {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_Ekin",TheScorers->ComputeClusterScoreValue_Ekin(ThisCluster));}
        if (TheScorers->ClusterScore_RelativisticBeta_IsUsed()==kTRUE) {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_Beta",TheScorers->ComputeClusterScoreValue_Beta(ThisCluster));}
        if (TheScorers->ClusterScore_Rvalue_IsUsed()==kTRUE)           {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_Rvalue",TheScorers->ComputeClusterScoreValue_Rvalue(ThisCluster));}
        if (TheScorers->ClusterScore_LogR_IsUsed()==kTRUE)             {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_LogR",TheScorers->ComputeClusterScoreValue_LogR(ThisCluster));}
        if (TheScorers->ClusterScore_TOF_IsUsed()==kTRUE)              {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_TOF",TheScorers->ComputeClusterScoreValue_TOF(ThisCluster));}
        if (TheScorers->ClusterScore_SpaceLength_IsUsed()==kTRUE)      {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_SpaceLength",TheScorers->ComputeClusterSpaceLength(ThisCluster));}
        if (TheScorers->ClusterScore_SpaceRadius_IsUsed()==kTRUE)      {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_SpaceRadius",TheScorers->ComputeClusterSpaceRadius(ThisCluster));}
        if (TheScorers->ClusterScore_TimeLength_IsUsed()==kTRUE)       {LearnAdvancedNorms->AdaptOldScale("ClusterScorer_TimeLength",TheScorers->ComputeClusterTimeLength(ThisCluster));}

        // And then, apply them:
        InputNeurons.push_back(TheNormalization->Normalize(TimeNormalization_NormName.Data(),TheScorers->GetPrimaryClusterHit(ThisCluster)->GetTime()));
        InputNeurons.push_back(TheNormalization->Normalize("X",TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionX()));
        InputNeurons.push_back(TheNormalization->Normalize("Y",TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionY()));
        InputNeurons.push_back(TheNormalization->Normalize("Z",TheScorers->GetPrimaryClusterHit(ThisCluster)->GetPositionZ()));
                
        if (TheScorers->ClusterScore_DepositedEnergy_IsUsed()==kTRUE)  {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_Energy",TheScorers->ComputeClusterScoreValue_Energy(ThisCluster)));}
        if (TheScorers->ClusterScore_Size_IsUsed()==kTRUE)             {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_Size",TheScorers->ComputeClusterScoreValue_Size(ThisCluster)));}
        if (TheScorers->ClusterScore_KineticEnergy_IsUsed()==kTRUE)    {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_Ekin",TheScorers->ComputeClusterScoreValue_Ekin(ThisCluster)));}
        if (TheScorers->ClusterScore_RelativisticBeta_IsUsed()==kTRUE) {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_Beta",TheScorers->ComputeClusterScoreValue_Beta(ThisCluster)));}
        if (TheScorers->ClusterScore_Rvalue_IsUsed()==kTRUE)           {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_Rvalue",TheScorers->ComputeClusterScoreValue_Rvalue(ThisCluster)));}
        if (TheScorers->ClusterScore_LogR_IsUsed()==kTRUE)             {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_LogR",TheScorers->ComputeClusterScoreValue_LogR(ThisCluster)));}
        if (TheScorers->ClusterScore_TOF_IsUsed()==kTRUE)              {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_TOF",TheScorers->ComputeClusterScoreValue_TOF(ThisCluster)));}
        if (TheScorers->ClusterScore_SpaceLength_IsUsed()==kTRUE)      {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_SpaceLength",TheScorers->ComputeClusterSpaceLength(ThisCluster)));}
        if (TheScorers->ClusterScore_SpaceRadius_IsUsed()==kTRUE)      {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_SpaceRadius",TheScorers->ComputeClusterSpaceRadius(ThisCluster)));}
        if (TheScorers->ClusterScore_TimeLength_IsUsed()==kTRUE)       {InputNeurons.push_back(TheNormalization->Normalize("ClusterScorer_TimeLength",TheScorers->ComputeClusterTimeLength(ThisCluster)));}
    }
    
    // Next, also fill in the output neurons:    
    if (nOutputFlags_PerCluster==1)
    {
        // This case is a little special:
        if (ThisCluster->GetPrimaryMultSim()>0)
        {
            OutputNeurons.push_back(Output_ISYES);
        }
        else
        {
            OutputNeurons.push_back(Output_ISNO);
        }
    }
    else if (nOutputFlags_PerCluster>1)
    {
        // then, we need to supply multiple outputs.
        // First, obtain the current size:
        Int_t CurrentSize = OutputNeurons.size();
        
        // Then, fill in new defaults:
        for (Int_t k = 0; k<nOutputFlags_PerCluster; ++k)
        {
            OutputNeurons.push_back(Output_ISNO);
        }
        
        // Then, mark the correct case:
        if (ThisCluster->GetPrimaryMultSim()<nOutputFlags_PerCluster)
        {
            OutputNeurons.at(CurrentSize+ThisCluster->GetPrimaryMultSim()) = Output_ISYES;
        }
        else
        {
            OutputNeurons.at(CurrentSize+nOutputFlags_PerCluster-1) = Output_ISYES;
        }
        
        // Done.
    }
    else
    {
        ErrorMessage("nOutputFlags_PerCluster Cannot be a negative number!");
    }
    
    // Done.
}
