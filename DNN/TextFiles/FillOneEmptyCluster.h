void R3BTextFileGenerator::FillOneEmptyCluster()
{
    // Function to write one single cluster to the InputNeurons and OutputNeurons.
    // The function will write many cluster properties to the inputs: all Scoring+
    // quantities. By replacing the totalscore-sorting by a DNN, correlations
    // between these parameters can be taken along.
    
    // Begin by filling in zeros in the input:
    InputNeurons.push_back(Input_ISNO);
    InputNeurons.push_back(Input_ISNO);
    InputNeurons.push_back(Input_ISNO);
    InputNeurons.push_back(Input_ISNO);
                        
    if (TheScorers->ClusterScore_DepositedEnergy_IsUsed()==kTRUE)  {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_Size_IsUsed()==kTRUE)             {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_KineticEnergy_IsUsed()==kTRUE)    {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_RelativisticBeta_IsUsed()==kTRUE) {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_Rvalue_IsUsed()==kTRUE)           {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_LogR_IsUsed()==kTRUE)             {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_TOF_IsUsed()==kTRUE)              {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_SpaceLength_IsUsed()==kTRUE)      {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_SpaceRadius_IsUsed()==kTRUE)      {InputNeurons.push_back(Input_ISNO);}
    if (TheScorers->ClusterScore_TimeLength_IsUsed()==kTRUE)       {InputNeurons.push_back(Input_ISNO);}
    
    // Next, also fill in zero in the output neurons:    
    if (nOutputFlags_PerCluster==1)
    {
        // This case is a little special:
        OutputNeurons.push_back(Output_ISNO);
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
        OutputNeurons.at(CurrentSize) = Output_ISYES;
        
        // Done.
    }
    else
    {
        ErrorMessage("nOutputFlags_PerCluster Cannot be a negative number!");
    }
    
    // Done.
}
