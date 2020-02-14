void R3BNeuLANDTranslator::RetrieveInputs()
{
    // Obtain all required inputs:
    ceff = Inputs->GetInputDouble("NeuLAND_Digitizer_Effective_LightSpeed","cm/ns");
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    UseSimulation = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    nPaddlesPerPlane = Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane");
    nDoublePlanes = Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
    ParticleType_ToBeDetected = Inputs->GetInputString("NeuLAND_ParticleType_ToBeDetected");
    ParticleMass = TheNuclei->GetMass(ParticleType_ToBeDetected,"MeV");
    ParticlePDG = TheNuclei->GetPDGCode(ParticleType_ToBeDetected);
    MarkSignals_Method = Inputs->GetInputString("NeuLAND_Translator_MarkPrimSignals");
    MarkPrimPoints_Method = Inputs->GetInputString("NeuLAND_Translator_ObtainPrimPoints");
    MinimizationMarking_IncludeTime = Inputs->GetInputBoolian("NeuLAND_Translator_MinimizationMarking_IncludeTime");
    MinimizationMarking_AllowMaxDist = Inputs->GetInputBoolian("NeuLAND_Translator_MinimizationMarking_AllowMaxDist");
    MinimizationMarking_MaxDistance = Inputs->GetInputDouble("NeuLAND_Translator_MinimizationMarking_MaxDistance","cm");
    Target_PrimVolume_Thickness = Inputs->GetInputDouble("TARGET_PrimaryVolume_Thickness","cm");
    TargetX = Inputs->GetInputDouble("TARGET_center_x_position","cm");
    TargetY = Inputs->GetInputDouble("TARGET_center_y_position","cm");
    TargetZ = Inputs->GetInputDouble("TARGET_center_z_position","cm");
    TargetT = Inputs->GetTargetTime();
    NeuLAND_Center_X = Inputs->GetInputDouble("NeuLAND_center_x_position","cm");
    NeuLAND_Center_Y = Inputs->GetInputDouble("NeuLAND_center_y_position","cm");
    NeuLAND_Front_Z = Inputs->GetInputDouble("NeuLAND_front_z_position","cm");
    NeuLAND_Rot_X = Inputs->GetInputDouble("NeuLAND_x_rotation_angle","degree");
    NeuLAND_Rot_Y = Inputs->GetInputDouble("NeuLAND_y_rotation_angle","degree");
    NeuLAND_Rot_Z = Inputs->GetInputDouble("NeuLAND_z_rotation_angle","degree");
    NeuLAND_Active_Bar_Thickness = Inputs->GetInputDouble("NeuLAND_BC408_BarThickness","cm");
    NeuLAND_Total_Bar_Length = Inputs->GetInputDouble("NeuLAND_Total_BarLength","cm");
    NeuLAND_TotalBarThicknessXY = Inputs->GetTotalPaddleThicknessXY();
    NeuLAND_TotalBarThicknessZ = Inputs->GetTotalPaddleThicknessZ();
    TheOutputPath = Inputs->GetInputString("TheOutputPath");
    TheVMCFolder = Inputs->GetInputString("TheVMCFolder");
    MaxClusterMult = Inputs->GetInputInteger("NeuLAND_Translator_MaxMultiplicityPerCluster_JanClusters");
    UseVETO = Inputs->GetInputBoolian("VETO_Include_in_SETUP");
    UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    UseNEBVETO = Inputs->GetInputBoolian("NEBULA_VET_Include_in_SETUP");
    NumberOfDoublePlanes_PerModule = Inputs->GetInputInteger("NeuLAND_DoublePlanes_PerModule");
    DoModularSplit = Inputs->GetInputBoolian("NeuLAND_UseModularSplit");
    ModularDistance = Inputs->GetInputDouble("NeuLAND_Distance_BetweenModules","cm");
    BeamBeta = Inputs->GetAvgBeamBeta();
    ParticleGun_x_position = Inputs->GetInputDouble("ParticleGun_x_position","cm");
    ParticleGun_y_position = Inputs->GetInputDouble("ParticleGun_y_position","cm");
    ParticleGun_z_position = Inputs->GetInputDouble("ParticleGun_z_position","cm");
}
