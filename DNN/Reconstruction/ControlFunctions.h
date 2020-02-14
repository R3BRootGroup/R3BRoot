// Functions to control the AllScorers-class:

// Retrieve a pointer to a specific scorer:
ScoringClass* AllScorers::RetrieveSignalScorer(TString const Name)
{
    ScoringClass* Answer = 0;
    Int_t Index = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<SignalScorers.size(); ++k)
    {
        if (SignalScorers.at(k)->GetName()==Name)
        {
            Counter = Counter + 1;
            Index = k;
        }
    }
    
    if (Counter==0)
    {
        ErrorMessage("The SignalScorer "+Name+" was not found in the list!");
    }
    else if (Counter>1)
    {
        ErrorMessage("You defined multiple SignalScorers with the same name <"+Name+">! That is not allowed!");
    }
    else
    {
        Answer = SignalScorers.at(Index);
    }
    
    return Answer;
}

ScoringClass* AllScorers::RetrieveClusterScorer(TString const Name)
{
    ScoringClass* Answer = 0;
    Int_t Index = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<ClusterScorers.size(); ++k)
    {
        if (ClusterScorers.at(k)->GetName()==Name)
        {
            Counter = Counter + 1;
            Index = k;
        }
    }
    
    if (Counter==0)
    {
        ErrorMessage("The SignalScorer "+Name+" was not found in the list!");
    }
    else if (Counter>1)
    {
        ErrorMessage("You defined multiple ClusterScorers with the same name <"+Name+">! That is not allowed!");
    }
    else
    {
        Answer = ClusterScorers.at(Index);
    }
    
    return Answer;
}
    
// Write scorers:
void AllScorers::WriteScorers()
{
    Bool_t ErrorCheck = kFALSE;
    
    for (Int_t k = 0; k<SignalScorers.size(); ++k)
    {
        SignalScorers.at(k)->WriteScorer();
        if (SignalScorers.at(k)->ContainsNoErrors()==kFALSE) {ErrorCheck = kTRUE;}
    }
    
    for (Int_t k = 0; k<ClusterScorers.size(); ++k)
    {
        ClusterScorers.at(k)->WriteScorer();
        if (ClusterScorers.at(k)->ContainsNoErrors()==kFALSE) {ErrorCheck = kTRUE;}
    }
    
    if (ErrorCheck==kTRUE) {ErrorMessage("Something went wrong with writing the scorers!");}
}

// Read scorers:
void AllScorers::ReadScorers()
{
    Bool_t ErrorCheck = kFALSE;
    
    for (Int_t k = 0; k<SignalScorers.size(); ++k)
    {
        SignalScorers.at(k)->ReadScorer();
        if (SignalScorers.at(k)->ContainsNoErrors()==kFALSE) {ErrorCheck = kTRUE;}
    }
    
    for (Int_t k = 0; k<ClusterScorers.size(); ++k)
    {
        ClusterScorers.at(k)->ReadScorer();
        if (ClusterScorers.at(k)->ContainsNoErrors()==kFALSE) {ErrorCheck = kTRUE;}
    }
    
    if (ErrorCheck==kTRUE) {ErrorMessage("Something went wrong with reading the scorers!");}
}

// Full plotting function:
void AllScorers::MakePlots()
{
    TString WriteName = "";
    TCanvas* ThisPlot = 0;
    
    for (Int_t k = 0; k<SignalScorers.size(); ++k)
    {
        WriteName = OutputPath + "/" + Key + "SignalScorer_" + SignalScorers.at(k)->GetName() + ".png";
        ThisPlot = SignalScorers.at(k)->PlotScorer();
        ThisPlot->SaveAs(WriteName.Data());
        ThisPlot->Close();
    }
    
    for (Int_t k = 0; k<ClusterScorers.size(); ++k)
    {
        WriteName = OutputPath + "/" + Key + "ClusterScorer_" + ClusterScorers.at(k)->GetName() + ".png";
        ThisPlot = ClusterScorers.at(k)->PlotScorer();
        ThisPlot->SaveAs(WriteName.Data());
        ThisPlot->Close();
    }
}

// Function to obtain number of included scores:
Int_t AllScorers::GetNumberOfClusterScores()
{
    Int_t nScores = 0;
    if (UseClusterScore_DepositedEnergy==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_Size==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_KineticEnergy==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_RelativisticBeta==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_Rvalue==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_LogR==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_TOF==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_SpaceLength==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_TimeLength==kTRUE) {nScores = nScores + 1;}
    if (UseClusterScore_SpaceRadius==kTRUE) {nScores = nScores + 1;}
    return nScores;
}

Int_t AllScorers::GetNumberOfSignalScores()
{
    Int_t nScores = 0;
    if (UseSignalScore_DepositedEnergy==kTRUE) {nScores = nScores + 1;}
    if (UseSignalScore_KineticEnergy==kTRUE) {nScores = nScores + 1;}
    if (UseSignalScore_RelativisticBeta==kTRUE) {nScores = nScores + 1;}
    if (UseSignalScore_Rvalue==kTRUE) {nScores = nScores + 1;}
    if (UseSignalScore_TOF==kTRUE) {nScores = nScores + 1;}
    return nScores;
}
