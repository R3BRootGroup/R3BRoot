void NormalizationClass::ElenaNorms(Double_t const NewLeft, Double_t const NewRight)
{
    // Generates default values for certain normalization processes.
    // values are scaled to [NewLeft,NewRight]. We use the exact same
    // parameters as Elena did here.
    TheNormParameters.clear();
    
    // Position coordinate:
    NormUnit X_Norm;
    X_Norm.Name = "X";
    X_Norm.Normalization_Option = "DoubleCutoff";
    X_Norm.Old_MeanShift = 0.0;
    X_Norm.Old_ScaleFactor = 125.0;
    X_Norm.New_MeanShift = 0.5*(NewLeft+NewRight);
    X_Norm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    X_Norm.ControlHistogram = 0;
    TheNormParameters.push_back(X_Norm);
    
    // Time-coordinte:
    NormUnit T_Norm;
    T_Norm.Name = "T_Default";
    T_Norm.Normalization_Option = "DoubleCutoff";
    T_Norm.Old_MeanShift = 75.0;
    T_Norm.Old_ScaleFactor = 25.0;
    T_Norm.New_MeanShift = 0.5*(NewLeft+NewRight);
    T_Norm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    T_Norm.ControlHistogram = 0;
    TheNormParameters.push_back(T_Norm);
    
    NormUnit T_Norm_Old;
    T_Norm_Old.Name = "T_Old";
    T_Norm_Old.Normalization_Option = "DoubleCutoff";
    T_Norm_Old.Old_MeanShift = 75.0;
    T_Norm_Old.Old_ScaleFactor = 25.0;
    T_Norm_Old.New_MeanShift = 0.5*(NewLeft+NewRight);
    T_Norm_Old.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    T_Norm_Old.ControlHistogram = 0;
    TheNormParameters.push_back(T_Norm_Old);
    
    // Total Energy Deposition:
    NormUnit TotalEnergyNorm;
    TotalEnergyNorm.Name = "TotalEnergy";
    TotalEnergyNorm.Normalization_Option = "DoubleCutoff";
    TotalEnergyNorm.Old_MeanShift = 1100.0;
    TotalEnergyNorm.Old_ScaleFactor = 1100.0;
    TotalEnergyNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    TotalEnergyNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    TotalEnergyNorm.ControlHistogram = 0;
    TheNormParameters.push_back(TotalEnergyNorm);
    
    // Signal Energy Deposition:
    NormUnit SignalEnergyNorm;
    SignalEnergyNorm.Name = "SignalEnergy";
    SignalEnergyNorm.Normalization_Option = "DoubleCutoff";
    SignalEnergyNorm.Old_MeanShift = 50.0;
    SignalEnergyNorm.Old_ScaleFactor = 50.0;
    SignalEnergyNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    SignalEnergyNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    SignalEnergyNorm.ControlHistogram = 0;
    TheNormParameters.push_back(SignalEnergyNorm);
    
    // Number of Clusters:
    NormUnit nClustersNorm;
    nClustersNorm.Name = "nClusters";
    nClustersNorm.Normalization_Option = "DoubleCutoff";
    nClustersNorm.Old_MeanShift = 28.0;
    nClustersNorm.Old_ScaleFactor = 28.0;
    nClustersNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    nClustersNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    nClustersNorm.ControlHistogram = 0;
    TheNormParameters.push_back(nClustersNorm);
    
    // Relativistic Beta:
    NormUnit BetaNorm;
    BetaNorm.Name = "RelativisticBeta";
    BetaNorm.Normalization_Option = "DoubleCutoff";
    BetaNorm.Old_MeanShift = 0.5;
    BetaNorm.Old_ScaleFactor = 0.5;
    BetaNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    BetaNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    BetaNorm.ControlHistogram = 0;
    TheNormParameters.push_back(BetaNorm);

    // TotalSignalScore:
    NormUnit TotalSignalScore;
    TotalSignalScore.Name = "TotalSignalScore";
    TotalSignalScore.Normalization_Option = "DoubleCutoff";
    TotalSignalScore.Old_MeanShift = 0.75*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"));
    TotalSignalScore.Old_ScaleFactor = 0.75*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"));
    TotalSignalScore.New_MeanShift = 0.5*(NewLeft+NewRight);
    TotalSignalScore.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    TotalSignalScore.ControlHistogram = 0;
    TheNormParameters.push_back(TotalSignalScore);
    
    // TotalClusterScore:
    NormUnit TotalClusterScore;
    TotalClusterScore.Name = "TotalClusterScore";
    TotalClusterScore.Normalization_Option = "DoubleCutoff";
    TotalClusterScore.Old_MeanShift = 0.5*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"));
    TotalClusterScore.Old_ScaleFactor = 0.5*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"));
    TotalClusterScore.New_MeanShift = 0.5*(NewLeft+NewRight);
    TotalClusterScore.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    TotalClusterScore.ControlHistogram = 0;
    TheNormParameters.push_back(TotalClusterScore);
    
    // SingleSignalScore:
    NormUnit SingleSignalScore;
    SingleSignalScore.Name = "SingleSignalScore";
    SingleSignalScore.Normalization_Option = "DoubleCutoff";
    SingleSignalScore.Old_MeanShift = 0.5;
    SingleSignalScore.Old_ScaleFactor = 0.5;
    SingleSignalScore.New_MeanShift = 0.5*(NewLeft+NewRight);
    SingleSignalScore.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    SingleSignalScore.ControlHistogram = 0;
    TheNormParameters.push_back(SingleSignalScore);
}

void NormalizationClass::BlankElenaNorms(Double_t const NewLeft, Double_t const NewRight)
{
    TheNormParameters.clear();
    
    AddBlankPar("X");                 SetNewScale("X",NewLeft,NewRight);
    AddBlankPar("T_Default");         SetNewScale("T_Default",NewLeft,NewRight);
    AddBlankPar("T_Old");         SetNewScale("T_Old",NewLeft,NewRight);
    AddBlankPar("TotalEnergy");       SetNewScale("TotalEnergy",NewLeft,NewRight);
    AddBlankPar("SignalEnergy");      SetNewScale("SignalEnergy",NewLeft,NewRight);
    AddBlankPar("nClusters");         SetNewScale("nClusters",NewLeft,NewRight);
    AddBlankPar("RelativisticBeta");  SetNewScale("RelativisticBeta",NewLeft,NewRight);
    AddBlankPar("TotalSignalScore");  SetNewScale("TotalSignalScore",NewLeft,NewRight);
    AddBlankPar("TotalClusterScore"); SetNewScale("TotalClusterScore",NewLeft,NewRight);
    AddBlankPar("SingleSignalScore"); SetNewScale("SingleSignalScore",NewLeft,NewRight);
}
