void NormalizationClass::DefaultNorms(Double_t const NewLeft, Double_t const NewRight)
{
    // Generates default values for certain normalization processes.
    // values are scaled to [NewLeft,NewRight].
    TheNormParameters.clear();
    
    // First, we will do the time normalization. This is most tricky, so we will do it separately.
    
    // Start with the computation of the time it takes to reach the front of NeuLAND:
    Double_t Distance = TMath::Sqrt(TMath::Power(TMath::Abs(Inputs->GetInputDouble("ParticleGun_x_position","cm") - Inputs->GetInputDouble("TARGET_center_x_position","cm")),2.0) + 
                                    TMath::Power(TMath::Abs(Inputs->GetInputDouble("ParticleGun_y_position","cm") - Inputs->GetInputDouble("TARGET_center_y_position","cm")),2.0) + 
                                    TMath::Power(TMath::Abs(Inputs->GetInputDouble("ParticleGun_z_position","cm") - Inputs->GetInputDouble("TARGET_center_z_position","cm")),2.0));
    
    Distance = Distance + TMath::Sqrt(TMath::Power(TMath::Abs(Inputs->GetInputDouble("NeuLAND_center_x_position","cm") - Inputs->GetInputDouble("TARGET_center_x_position","cm")),2.0) + 
                                      TMath::Power(TMath::Abs(Inputs->GetInputDouble("NeuLAND_center_y_position","cm") - Inputs->GetInputDouble("TARGET_center_y_position","cm")),2.0) + 
                                      TMath::Power(TMath::Abs(Inputs->GetInputDouble("NeuLAND_front_z_position","cm")  - Inputs->GetInputDouble("TARGET_center_z_position","cm")),2.0));
    
    Double_t BeamBeta = Inputs->GetAvgBeamBeta();
    Double_t cLight = 29.9792458;
    Double_t TravelTime = Distance/(cLight*BeamBeta);
    Double_t TimeNormalization_LeftBoundary = Inputs->GetInputDouble("NeuLAND_DNNTextFile_TimeNormaliation_LeftBoundary","ns");
    Double_t TimeNormalization_RightBoundary = Inputs->GetInputDouble("NeuLAND_DNNTextFile_TimeNormaliation_RightBoundary","ns");
    
    // Next, we will do the default time normalization that provides the user with a sensible option:
    Double_t ceff = Inputs->GetInputDouble("NeuLAND_Digitizer_Effective_LightSpeed","cm/ns");
    Double_t TotalBarThickNess = Inputs->GetTotalPaddleThicknessZ();
    Double_t nDoublePlanes_d = (Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
    Double_t Realistic_TimeInterval = (30.0 + 1.0*nDoublePlanes_d)*TotalBarThickNess/(ceff*BeamBeta);
    
    NormUnit T_Norm_Default;
    T_Norm_Default.Name = "T_Default";
    T_Norm_Default.Normalization_Option = "LowerCutoff_UpperReset";
    T_Norm_Default.Old_MeanShift = TravelTime + 0.4*Realistic_TimeInterval;
    T_Norm_Default.Old_ScaleFactor = 0.6*Realistic_TimeInterval;
    T_Norm_Default.New_MeanShift = 0.5*(NewLeft+NewRight);
    T_Norm_Default.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    T_Norm_Default.ControlHistogram = 0;
    TheNormParameters.push_back(T_Norm_Default);
    
    // Then, do the custom time normalization:
    NormUnit T_Norm_Custom;
    T_Norm_Custom.Name = "T_Custom";
    T_Norm_Custom.Normalization_Option = "LowerCutoff_UpperReset";
    T_Norm_Custom.Old_MeanShift = 0.5*(TimeNormalization_LeftBoundary+TimeNormalization_RightBoundary);
    T_Norm_Custom.Old_ScaleFactor = 0.5*TMath::Abs(TimeNormalization_RightBoundary-TimeNormalization_LeftBoundary);
    T_Norm_Custom.New_MeanShift = 0.5*(NewLeft+NewRight);
    T_Norm_Custom.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    T_Norm_Custom.ControlHistogram = 0;
    TheNormParameters.push_back(T_Norm_Custom);
    
    // And, finally, do the data-driven normalization:
    NormUnit T_Norm_Data;
    T_Norm_Data.Name = "T_DataDriven";
    T_Norm_Data.Normalization_Option = "LowerCutoff_UpperReset";
    T_Norm_Data.Old_MeanShift = DataDriven_MeanTime;
    T_Norm_Data.Old_ScaleFactor = DataDriven_ScaleTime;
    T_Norm_Data.New_MeanShift = 0.5*(NewLeft+NewRight);
    T_Norm_Data.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    T_Norm_Data.ControlHistogram = 0;
    TheNormParameters.push_back(T_Norm_Data);
    
     NormUnit T_Norm_Old;
    T_Norm_Old.Name = "T_Old";
    T_Norm_Old.Normalization_Option = "UpperCutoff";
    Double_t Old_Realistic_TimeInterval = (30.0 + 1.0*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes")))*TotalBarThickNess/(ceff*BeamBeta);
    T_Norm_Old.Old_MeanShift = TravelTime + 0.5*Old_Realistic_TimeInterval;
    T_Norm_Old.Old_ScaleFactor = 0.5*Old_Realistic_TimeInterval;
    T_Norm_Old.New_MeanShift = 0.5*(NewLeft+NewRight);
    T_Norm_Old.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    T_Norm_Old.ControlHistogram = 0;
    TheNormParameters.push_back(T_Norm_Old);
     
    // x-coordinte:
    NormUnit X_Norm;
    X_Norm.Name = "X";
    X_Norm.Normalization_Option = "DoubleCutoff";
    X_Norm.Old_MeanShift = Inputs->GetInputDouble("NeuLAND_center_x_position","cm");
    Double_t Xlength = 0.5*Inputs->GetInputDouble("NeuLAND_Total_BarLength","cm");
    Double_t Xwidth = Inputs->GetTotalPaddleThicknessXY()*0.5;
    Xwidth = Xwidth*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane"));
    if (Xlength<Xwidth) {Xlength = Xwidth;}
    X_Norm.Old_ScaleFactor = Xlength;
    X_Norm.New_MeanShift = 0.5*(NewLeft+NewRight);
    X_Norm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    X_Norm.ControlHistogram = 0;
    TheNormParameters.push_back(X_Norm);
    
    // y-coordinte:
    NormUnit Y_Norm;
    Y_Norm.Name = "Y";
    Y_Norm.Normalization_Option = "DoubleCutoff";
    Y_Norm.Old_MeanShift = Inputs->GetInputDouble("NeuLAND_center_y_position","cm");
    Y_Norm.Old_ScaleFactor = Xlength;
    Y_Norm.New_MeanShift = 0.5*(NewLeft+NewRight);
    Y_Norm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    Y_Norm.ControlHistogram = 0;
    TheNormParameters.push_back(Y_Norm);
    
    // z-coordinte:
    NormUnit Z_Norm;
    Z_Norm.Name = "Z";
    Z_Norm.Normalization_Option = "DoubleCutoff";
    Double_t TotalDetectorThickness = ((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"))*2.0*TotalBarThickNess;
    Z_Norm.Old_MeanShift = Inputs->GetInputDouble("NeuLAND_front_z_position","cm") + 0.5*TotalDetectorThickness;
    Z_Norm.Old_ScaleFactor = 0.5*TotalDetectorThickness;
    Z_Norm.New_MeanShift = 0.5*(NewLeft+NewRight);
    Z_Norm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    Z_Norm.ControlHistogram = 0;
    TheNormParameters.push_back(Z_Norm);
       
    // Total Energy Deposition:
    NormUnit TotalEnergyNorm;
    TotalEnergyNorm.Name = "TotalEnergy";
    TotalEnergyNorm.Normalization_Option = "Linear";
    TotalEnergyNorm.Old_MeanShift = 0.35*(Inputs->GetAvgBeamEnergyPerNucleon())*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"))*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"))/30.0;
    TotalEnergyNorm.Old_ScaleFactor = 0.35*(Inputs->GetAvgBeamEnergyPerNucleon())*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"))*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"))/30.0;
    TotalEnergyNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    TotalEnergyNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    TotalEnergyNorm.ControlHistogram = 0;
    TheNormParameters.push_back(TotalEnergyNorm);
    
    // Total Number of Clusters:
    NormUnit nClustersNorm;
    nClustersNorm.Name = "nClusters";
    nClustersNorm.Normalization_Option = "Linear";
    nClustersNorm.Old_MeanShift = (3.5/600.0)*(Inputs->GetAvgBeamEnergyPerNucleon())*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"))*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"))/30.0;
    nClustersNorm.Old_ScaleFactor = (3.5/600.0)*(Inputs->GetAvgBeamEnergyPerNucleon())*((Int_t) Inputs->GetInputInteger("ParticleGun_Multiplicity"))*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"))/30.0;
    nClustersNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    nClustersNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    nClustersNorm.ControlHistogram = 0;
    TheNormParameters.push_back(nClustersNorm);
    
    // Signal Energy Deposition:
    NormUnit SignalEnergyNorm;
    SignalEnergyNorm.Name = "SignalEnergy";
    SignalEnergyNorm.Normalization_Option = "Linear";
    SignalEnergyNorm.Old_MeanShift = 50.0;
    SignalEnergyNorm.Old_ScaleFactor = 50.0;
    SignalEnergyNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    SignalEnergyNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    SignalEnergyNorm.ControlHistogram = 0;
    TheNormParameters.push_back(SignalEnergyNorm);
    
    // ClusterSize:
    NormUnit ClusterSizeNorm;
    ClusterSizeNorm.Name = "ClusterSize";
    ClusterSizeNorm.Normalization_Option = "Linear";
    ClusterSizeNorm.Old_MeanShift = 4.0;
    ClusterSizeNorm.Old_ScaleFactor = 4.0;
    ClusterSizeNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterSizeNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterSizeNorm.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterSizeNorm);
    
    // Relativistic Beta:
    NormUnit BetaNorm;
    BetaNorm.Name = "RelativisticBeta";
    BetaNorm.Normalization_Option = "Linear";
    BetaNorm.Old_MeanShift = 0.5;
    BetaNorm.Old_ScaleFactor = 0.5;
    BetaNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    BetaNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    BetaNorm.ControlHistogram = 0;
    TheNormParameters.push_back(BetaNorm);
    
    // R-value:
    Double_t Rval_Left = 0.0;
    Double_t ClusterEmin = 10.0; // [MeV].
    Double_t Rval_Right = BetaNorm.Old_ScaleFactor/ClusterEmin;

    NormUnit R_Norm;
    R_Norm.Name = "Rvalue";
    R_Norm.Normalization_Option = "Linear";
    R_Norm.Old_MeanShift = 0.5*(Rval_Left+Rval_Right);
    R_Norm.Old_ScaleFactor = 0.5*TMath::Abs(Rval_Right-Rval_Left);
    R_Norm.New_MeanShift = 0.5*(NewLeft+NewRight);
    R_Norm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    R_Norm.ControlHistogram = 0;
    TheNormParameters.push_back(R_Norm);
    
    // PlaneNumber per bar:
    NormUnit BarPerPlane_Norm;
    BarPerPlane_Norm.Name = "BarPerPlane";
    BarPerPlane_Norm.Normalization_Option = "Linear";
    BarPerPlane_Norm.Old_MeanShift = 0.5*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane"));
    BarPerPlane_Norm.Old_ScaleFactor = 0.5*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane"));
    BarPerPlane_Norm.New_MeanShift = 0.5*(NewLeft+NewRight);
    BarPerPlane_Norm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    BarPerPlane_Norm.ControlHistogram = 0;
    TheNormParameters.push_back(BarPerPlane_Norm);
    
    // Total Bar Number:
    NormUnit TotalBarNorm;
    TotalBarNorm.Name = "TotalBarNumber";
    TotalBarNorm.Normalization_Option = "Linear";
    TotalBarNorm.Old_MeanShift = ((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane"))*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"));
    TotalBarNorm.Old_ScaleFactor = ((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane"))*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"));
    TotalBarNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    TotalBarNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    TotalBarNorm.ControlHistogram = 0;
    TheNormParameters.push_back(TotalBarNorm);
    
    // PlaneNumber (counting single planes):
    NormUnit PlaneNorm;
    PlaneNorm.Name = "SinglePlaneNumber";
    PlaneNorm.Normalization_Option = "Linear";
    PlaneNorm.Old_MeanShift = ((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"));
    PlaneNorm.Old_ScaleFactor = ((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"));
    PlaneNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    PlaneNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    PlaneNorm.ControlHistogram = 0;
    TheNormParameters.push_back(PlaneNorm);
    
    // PlaneNumber (counting double planes):
    NormUnit dPlaneNorm;
    dPlaneNorm.Name = "DoublePlaneNumber";
    dPlaneNorm.Normalization_Option = "Linear";
    dPlaneNorm.Old_MeanShift = 0.5*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"));
    dPlaneNorm.Old_ScaleFactor = 0.5*((Int_t) Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes"));
    dPlaneNorm.New_MeanShift = 0.5*(NewLeft+NewRight);
    dPlaneNorm.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    dPlaneNorm.ControlHistogram = 0;
    TheNormParameters.push_back(dPlaneNorm);
    
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
    
    // -----------------------------------------------------------------------
    
    // Also define the normalizations for the cluster scoring properties:
    
    NormUnit ClusterScorer_Energy;
    ClusterScorer_Energy.Name = "ClusterScorer_Energy";
    ClusterScorer_Energy.Normalization_Option = "DoubleCutoff";
    ClusterScorer_Energy.Old_MeanShift = TheScorers->RetrieveClusterScorer("Energy")->GetMeanShift();
    ClusterScorer_Energy.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("Energy")->GetScaleFactor();
    ClusterScorer_Energy.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_Energy.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_Energy.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_Energy);
    
    NormUnit ClusterScorer_Size;
    ClusterScorer_Size.Name = "ClusterScorer_Size";
    ClusterScorer_Size.Normalization_Option = "DoubleCutoff";
    ClusterScorer_Size.Old_MeanShift = TheScorers->RetrieveClusterScorer("Size")->GetMeanShift();
    ClusterScorer_Size.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("Size")->GetScaleFactor();
    ClusterScorer_Size.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_Size.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_Size.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_Size);
    
    NormUnit ClusterScorer_TOF;
    ClusterScorer_TOF.Name = "ClusterScorer_TOF";
    ClusterScorer_TOF.Normalization_Option = "DoubleCutoff";
    ClusterScorer_TOF.Old_MeanShift = TheScorers->RetrieveClusterScorer("TOF")->GetMeanShift();
    ClusterScorer_TOF.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("TOF")->GetScaleFactor();
    ClusterScorer_TOF.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_TOF.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_TOF.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_TOF);
    
    NormUnit ClusterScorer_Beta;
    ClusterScorer_Beta.Name = "ClusterScorer_Beta";
    ClusterScorer_Beta.Normalization_Option = "DoubleCutoff";
    ClusterScorer_Beta.Old_MeanShift = TheScorers->RetrieveClusterScorer("Beta")->GetMeanShift();
    ClusterScorer_Beta.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("Beta")->GetScaleFactor();
    ClusterScorer_Beta.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_Beta.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_Beta.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_Beta);
    
    NormUnit ClusterScorer_Ekin;
    ClusterScorer_Ekin.Name = "ClusterScorer_Ekin";
    ClusterScorer_Ekin.Normalization_Option = "DoubleCutoff";
    ClusterScorer_Ekin.Old_MeanShift = TheScorers->RetrieveClusterScorer("Ekin")->GetMeanShift();
    ClusterScorer_Ekin.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("Ekin")->GetScaleFactor();
    ClusterScorer_Ekin.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_Ekin.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_Ekin.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_Ekin);
    
    NormUnit ClusterScorer_Rvalue;
    ClusterScorer_Rvalue.Name = "ClusterScorer_Rvalue";
    ClusterScorer_Rvalue.Normalization_Option = "DoubleCutoff";
    ClusterScorer_Rvalue.Old_MeanShift = TheScorers->RetrieveClusterScorer("Rvalue")->GetMeanShift();
    ClusterScorer_Rvalue.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("Rvalue")->GetScaleFactor();
    ClusterScorer_Rvalue.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_Rvalue.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_Rvalue.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_Rvalue);
    
    NormUnit ClusterScorer_LogR;
    ClusterScorer_LogR.Name = "ClusterScorer_LogR";
    ClusterScorer_LogR.Normalization_Option = "DoubleCutoff";
    ClusterScorer_LogR.Old_MeanShift = TheScorers->RetrieveClusterScorer("LogR")->GetMeanShift();
    ClusterScorer_LogR.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("LogR")->GetScaleFactor();
    ClusterScorer_LogR.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_LogR.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_LogR.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_LogR);
    
    NormUnit ClusterScorer_SpaceLength;
    ClusterScorer_SpaceLength.Name = "ClusterScorer_SpaceLength";
    ClusterScorer_SpaceLength.Normalization_Option = "DoubleCutoff";
    ClusterScorer_SpaceLength.Old_MeanShift = TheScorers->RetrieveClusterScorer("SpaceLength")->GetMeanShift();
    ClusterScorer_SpaceLength.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("SpaceLength")->GetScaleFactor();
    ClusterScorer_SpaceLength.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_SpaceLength.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_SpaceLength.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_SpaceLength);
    
    NormUnit ClusterScorer_SpaceRadius;
    ClusterScorer_SpaceRadius.Name = "ClusterScorer_SpaceRadius";
    ClusterScorer_SpaceRadius.Normalization_Option = "DoubleCutoff";
    ClusterScorer_SpaceRadius.Old_MeanShift = TheScorers->RetrieveClusterScorer("SpaceRadius")->GetMeanShift();
    ClusterScorer_SpaceRadius.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("SpaceRadius")->GetScaleFactor();
    ClusterScorer_SpaceRadius.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_SpaceRadius.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_SpaceRadius.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_SpaceRadius);
    
    NormUnit ClusterScorer_TimeLength;
    ClusterScorer_TimeLength.Name = "ClusterScorer_TimeLength";
    ClusterScorer_TimeLength.Normalization_Option = "DoubleCutoff";
    ClusterScorer_TimeLength.Old_MeanShift = TheScorers->RetrieveClusterScorer("TimeLength")->GetMeanShift();
    ClusterScorer_TimeLength.Old_ScaleFactor = TheScorers->RetrieveClusterScorer("TimeLength")->GetScaleFactor();
    ClusterScorer_TimeLength.New_MeanShift = 0.5*(NewLeft+NewRight);
    ClusterScorer_TimeLength.New_ScaleFactor = 0.5*TMath::Abs(NewRight-NewLeft);
    ClusterScorer_TimeLength.ControlHistogram = 0;
    TheNormParameters.push_back(ClusterScorer_TimeLength);
}

void NormalizationClass::BlankDefaultNorms(Double_t const NewLeft, Double_t const NewRight)
{
    TheNormParameters.clear();
    
    AddBlankPar("T_Custom");          SetNewScale("T_Custom",NewLeft,NewRight);
    AddBlankPar("T_DataDriven");      SetNewScale("T_DataDriven",NewLeft,NewRight);
    AddBlankPar("T_Default");         SetNewScale("T_Default",NewLeft,NewRight);
    AddBlankPar("T_Old");             SetNewScale("T_Old",NewLeft,NewRight);
    
    AddBlankPar("X");                 SetNewScale("X",NewLeft,NewRight);
    AddBlankPar("Y");                 SetNewScale("Y",NewLeft,NewRight);
    AddBlankPar("Z");                 SetNewScale("Z",NewLeft,NewRight);
    AddBlankPar("TotalEnergy");       SetNewScale("TotalEnergy",NewLeft,NewRight);
    AddBlankPar("SignalEnergy");      SetNewScale("SignalEnergy",NewLeft,NewRight);
    AddBlankPar("ClusterSize");       SetNewScale("ClusterSize",NewLeft,NewRight);
    AddBlankPar("nClusters");         SetNewScale("nClusters",NewLeft,NewRight);
    AddBlankPar("RelativisticBeta");  SetNewScale("RelativisticBeta",NewLeft,NewRight);
    AddBlankPar("Rvalue");            SetNewScale("Rvalue",NewLeft,NewRight);
    AddBlankPar("BarPerPlane");       SetNewScale("BarPerPlane",NewLeft,NewRight);
    AddBlankPar("TotalBarNumber");    SetNewScale("TotalBarNumber",NewLeft,NewRight);
    AddBlankPar("SinglePlaneNumber"); SetNewScale("SinglePlaneNumber",NewLeft,NewRight);
    AddBlankPar("DoublePlaneNumber"); SetNewScale("DoublePlaneNumber",NewLeft,NewRight);
    AddBlankPar("TotalSignalScore");  SetNewScale("TotalSignalScore",NewLeft,NewRight);
    AddBlankPar("TotalClusterScore"); SetNewScale("TotalClusterScore",NewLeft,NewRight);
    AddBlankPar("SingleSignalScore"); SetNewScale("SingleSignalScore",NewLeft,NewRight);
    
    AddBlankPar("ClusterScorer_Energy"); SetNewScale("ClusterScorer_Energy",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_Beta"); SetNewScale("ClusterScorer_Beta",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_TOF"); SetNewScale("ClusterScorer_TOF",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_Size"); SetNewScale("ClusterScorer_Size",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_Ekin"); SetNewScale("ClusterScorer_Ekin",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_Rvalue"); SetNewScale("ClusterScorer_Rvalue",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_LogR"); SetNewScale("ClusterScorer_LogR",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_SpaceLength"); SetNewScale("ClusterScorer_SpaceLength",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_SpaceRadius"); SetNewScale("ClusterScorer_SpaceRadius",NewLeft,NewRight);
    AddBlankPar("ClusterScorer_TimeLength"); SetNewScale("ClusterScorer_TimeLength",NewLeft,NewRight);
}
