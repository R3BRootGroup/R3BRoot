void R3BInputClass::CreateFile(TString const Name)
{
    // Creates a new InputFile. But only if it does not yet exist!
    
    // Set the default:
    Linked = kFALSE;
    NLines = 0;
    
    // Pass the input to the class:
    FileName = Name;
    
    // Next, check if this file exists, by opening it:
    ifstream CheckFile;
    CheckFile.open(FileName.Data());
    
    // Test if it worked:
    if (CheckFile.is_open()==kTRUE)
    {
        ErrorMessage("You tried to overwrite an existing input-file!");
    }
    else
    {
        // In this case, we can proceed.
        
        // Create the new file from scratch:
        std::ofstream TheFile (FileName.Data(), std::ofstream::out);
        Linked = kTRUE;
        NLines = 0;
        TheFile.close();
        
        // Get the string specifying the R3BRoot source directory:
        TString TheVMC = TString(gSystem->Getenv("VMCWORKDIR"));
        TheVMC = TheVMC + "/";
        
        // Next, we can add all of our input values:
        
        // General:
        ++NLines; AddInputBoolian("MT_Merge_MCFile",kFALSE);
        ++NLines;  AddInputString("TheOutputPath",TheVMC + "/DNN/OutputFiles/");
        ++NLines;  AddInputString("TheVMCFolder",TheVMC);
        ++NLines;  AddInputString("R3BRoot_MonteCarlo_OutputFile","MC_File.root");
        ++NLines;  AddInputString("R3BRoot_MonteCarlo_ParFile","Par_File.root");
        ++NLines;  AddInputString("R3BRoot_MonteCarlo_PlotFile","Plot_File.root");
        ++NLines;  AddInputString("R3BRoot_MonteCarlo_Engine","TGeant4");
        ++NLines;  AddInputString("R3BRoot_MonteCarlo_PhysicsList","QGSP_INCLXX_HP");
        ++NLines;  AddInputString("R3BRoot_TGeant4_ControlMacro","g4config.in");
        ++NLines; AddInputBoolian("R3BRoot_StoreTrajectories",kTRUE);
        ++NLines; AddInputBoolian("R3BRoot_RandomSeed_IsTimeDependent",kTRUE);
        ++NLines; AddInputInteger("R3BRoot_nEvents",1000000);
        ++NLines; AddInputInteger("R3BRoot_MonteCarloTransport_nMaxSteps",15000);
        ++NLines; AddInputInteger("R3BRoot_R3BStack_nMaxSteps",1000);
        
        // Cave:
        ++NLines; AddInputBoolian("R3BRoot_Cave_IsVacuum",kFALSE);
        
        // Particle Gun:
        ++NLines;  AddInputString("ParticleGun_ASCII_GunChoice","SnData_500keV");
        ++NLines;  AddInputString("ParticleGun_ASCII_FileName","Current_ASCII_File.dat");
        ++NLines;  AddInputString("ParticleGun_ParticleType","neutron");
        ++NLines; AddInputInteger("ParticleGun_Multiplicity",5);
        ++NLines; AddInputBoolian("ParticleGun_Randomize_Multiplicity",kTRUE);
        ++NLines; AddInputBoolian("ParticleGun_Uniformize_Detected_Multiplicity",kFALSE);
        ++NLines;  AddInputString("ParticleGun_MultDistribution","Default");
        ++NLines;  AddInputDouble("ParticleGun_SingleDetectionRate","dimless",0.999);
        ++NLines;  AddInputDouble("ParticleGun_InfiniteDetectionRate","dimless",0.001);
        ++NLines;  AddInputDouble("ParticleGun_x_position","cm",0.0);
        ++NLines;  AddInputDouble("ParticleGun_y_position","cm",0.0);
        ++NLines;  AddInputDouble("ParticleGun_z_position","cm",0.0);
        ++NLines;  AddInputDouble("ParticleGun_x_direction","cm",0.0);
        ++NLines;  AddInputDouble("ParticleGun_y_direction","cm",0.0);
        ++NLines;  AddInputDouble("ParticleGun_z_direction","cm",1.0);
        ++NLines;  AddInputDouble("ParticleGun_AngularSpread","degree",4.0);
        ++NLines;  AddInputString("ParticleGun_AngularSpread_Pattern","Uniform");
        ++NLines;  AddInputDouble("ParticleGun_MinimalKineticEnergy","MeV/u",599.0);
        ++NLines;  AddInputDouble("ParticleGun_MaximalKineticEnergy","MeV/u",601.0);
        ++NLines; AddInputInteger("ParticleGun_StartEventReading_SnDataFiles",0);
        ++NLines; AddInputInteger("ParticleGun_EndEventReading_SnDataFiles",9999);
        ++NLines; AddInputBoolian("ParticleGun_IncludeSnNucleus_SnDataFiles",kTRUE);
        ++NLines; AddInputBoolian("ParticleGun_RandomRotateNeutrons_SnDataFiles",kTRUE);
        ++NLines; AddInputBoolian("ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks",kFALSE);
        ++NLines;  AddInputString("ParticleGun_ExGenerator_BeamIsotope","proton");
        ++NLines;  AddInputString("ParticleGun_ExGenerator_TargetIsotope","In132");
        ++NLines;  AddInputDouble("ParticleGun_ExGenerator_BeamEnergy","MeV/u",600.0);
        ++NLines;  AddInputDouble("ParticleGun_ExGenerator_Recoil_ExcitationEnergy","MeV",0.0);
        
        // NeuLAND:
        ++NLines; AddInputBoolian("NeuLAND_Use_Original",kTRUE);
        ++NLines;  AddInputDouble("NeuLAND_Total_BarLength","cm",250.0);
        ++NLines;  AddInputDouble("NeuLAND_BC408_BarThickness","cm",4.8);
        ++NLines;  AddInputDouble("NeuLAND_ReflectionFoil_Thickness","cm",0.02);
        ++NLines;  AddInputDouble("NeuLAND_WrappingTape_Thickness","cm",0.05);
        ++NLines;  AddInputDouble("NeuLAND_Bars_AirGab","cm",0.03);
        ++NLines;  AddInputDouble("NeuLAND_EndPoint_ConeLength","cm",5.0);
        ++NLines;  AddInputDouble("NeuLAND_EndPoint_ConeRadius","cm",1.2);
        ++NLines;  AddInputDouble("NeuLAND_AluminiumFrame_Width","cm",5.0);
        ++NLines;  AddInputDouble("NeuLAND_AluminiumFrame_Thickness","cm",0.05);
        ++NLines; AddInputInteger("NeuLAND_Number_of_Bars_per_plane",50);
        ++NLines; AddInputInteger("NeuLAND_Number_of_DoublePlanes",30);
        ++NLines;  AddInputDouble("NeuLAND_center_x_position","cm",0.0);
        ++NLines;  AddInputDouble("NeuLAND_center_y_position","cm",0.0);
        ++NLines;  AddInputDouble("NeuLAND_front_z_position","cm",1400.0);
        ++NLines;  AddInputDouble("NeuLAND_x_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("NeuLAND_y_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("NeuLAND_z_rotation_angle","degree",0.0);
        ++NLines; AddInputBoolian("NeuLAND_Visualize_Geometry_DuringBuild",kFALSE);
        ++NLines; AddInputInteger("NeuLAND_Detector_Color",kCyan);
        ++NLines; AddInputInteger("NeuLAND_Frame_Color",kRed);
        ++NLines;  AddInputString("NeuLAND_Geometry_FileName","NeuLAND_Geometry.geo.root");
        ++NLines;  AddInputString("NeuLAND_ParticleType_ToBeDetected","neutron");
        ++NLines; AddInputBoolian("NeuLAND_UseModularSplit",kFALSE);
        ++NLines; AddInputInteger("NeuLAND_DoublePlanes_PerModule",30);
        ++NLines;  AddInputDouble("NeuLAND_Distance_BetweenModules","cm",0.0);
        
        // VETO:
        ++NLines; AddInputBoolian("VETO_Include_in_SETUP",kFALSE);
        ++NLines;  AddInputDouble("VETO_Total_BarLength","cm",190.0);
        ++NLines;  AddInputDouble("VETO_BC408_BarThickness_XY","cm",31.0);
        ++NLines;  AddInputDouble("VETO_BC408_BarThickness_Z","cm",1.0);
        ++NLines;  AddInputDouble("VETO_ReflectionFoil_Thickness","cm",0.02);
        ++NLines;  AddInputDouble("VETO_WrappingTape_Thickness","cm",0.05);
        ++NLines;  AddInputDouble("VETO_Bars_AirGab","cm",0.03);
        ++NLines;  AddInputDouble("VETO_EndPoint_ConeLength","cm",12.0);
        ++NLines;  AddInputDouble("VETO_EndPoint_ConeRadius","cm",3.0);
        ++NLines;  AddInputDouble("VETO_AluminiumFrame_Width","cm",-1.0);
        ++NLines;  AddInputDouble("VETO_AluminiumFrame_Thickness","cm",-1.0);
        ++NLines; AddInputInteger("VETO_Number_of_Bars_in_plane",8);
        ++NLines;  AddInputDouble("VETO_center_x_position","cm",0.0);
        ++NLines;  AddInputDouble("VETO_center_y_position","cm",0.0);
        ++NLines;  AddInputDouble("VETO_front_z_position","cm",1370.0);
        ++NLines;  AddInputDouble("VETO_x_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("VETO_y_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("VETO_z_rotation_angle","degree",0.0);
        ++NLines; AddInputBoolian("VETO_Visualize_Geometry_DuringBuild",kFALSE);
        ++NLines; AddInputInteger("VETO_Detector_Color",kYellow);
        ++NLines; AddInputInteger("VETO_Frame_Color",kYellow+2);
        ++NLines;  AddInputString("VETO_Geometry_FileName","VETO_Geometry.geo.root");
        
        // NEBULA:
        ++NLines; AddInputBoolian("NEBULA_Include_in_SETUP",kFALSE);
        ++NLines;  AddInputDouble("NEBULA_Total_BarLength","cm",180.0);
        ++NLines;  AddInputDouble("NEBULA_BC408_BarThickness","cm",11.8);
        ++NLines;  AddInputDouble("NEBULA_ReflectionFoil_Thickness","cm",0.02);
        ++NLines;  AddInputDouble("NEBULA_WrappingTape_Thickness","cm",0.05);
        ++NLines;  AddInputDouble("NEBULA_Bars_AirGab","cm",0.03);
        ++NLines;  AddInputDouble("NEBULA_EndPoint_ConeLength","cm",8.0);
        ++NLines;  AddInputDouble("NEBULA_EndPoint_ConeRadius","cm",3.0);
        ++NLines;  AddInputDouble("NEBULA_AluminiumFrame_Width","cm",-1.0);
        ++NLines;  AddInputDouble("NEBULA_AluminiumFrame_Thickness","cm",-1.0);
        ++NLines; AddInputInteger("NEBULA_Number_of_Bars_in_OnePlane",30);
        ++NLines; AddInputInteger("NEBULA_Number_of_DoublePlaneModules",2);
        ++NLines;  AddInputDouble("NEBULA_Distance_BetweenModules","cm",85.0);
        ++NLines;  AddInputDouble("NEBULA_center_x_position","cm",0.0);
        ++NLines;  AddInputDouble("NEBULA_center_y_position","cm",0.0);
        ++NLines;  AddInputDouble("NEBULA_front_z_position","cm",1800.0); 
        ++NLines;  AddInputDouble("NEBULA_x_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("NEBULA_y_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("NEBULA_z_rotation_angle","degree",0.0);
        ++NLines; AddInputBoolian("NEBULA_Visualize_Geometry_DuringBuild",kFALSE);
        ++NLines; AddInputInteger("NEBULA_Detector_Color",kAzure-2);
        ++NLines; AddInputInteger("NEBULA_Frame_Color",kRed+2);
        ++NLines;  AddInputString("NEBULA_Geometry_FileName","NEBULA_Geometry.geo.root");
        ++NLines;  AddInputString("NEBULA_ParticleType_ToBeDetected","neutron");
        
        // NEBULA VETO:
        ++NLines; AddInputBoolian("NEBULA_VET_Include_in_SETUP",kFALSE);
        ++NLines;  AddInputDouble("NEBULA_VET_Total_BarLength","cm",190.0);
        ++NLines;  AddInputDouble("NEBULA_VET_BC408_BarThickness_XY","cm",32.0);
        ++NLines;  AddInputDouble("NEBULA_VET_BC408_BarThickness_Z","cm",1.0);
        ++NLines;  AddInputDouble("NEBULA_VET_ReflectionFoil_Thickness","cm",0.02);
        ++NLines;  AddInputDouble("NEBULA_VET_WrappingTape_Thickness","cm",0.05);
        ++NLines;  AddInputDouble("NEBULA_VET_Bars_AirGab","cm",0.03);
        ++NLines;  AddInputDouble("NEBULA_VET_EndPoint_ConeLength","cm",12.0);
        ++NLines;  AddInputDouble("NEBULA_VET_EndPoint_ConeRadius","cm",3.0);
        ++NLines;  AddInputDouble("NEBULA_VET_AluminiumFrame_Width","cm",-1.0);
        ++NLines;  AddInputDouble("NEBULA_VET_AluminiumFrame_Thickness","cm",-1.0);
        ++NLines; AddInputInteger("NEBULA_VET_Number_of_Bars_in_plane",12);
        ++NLines;  AddInputDouble("NEBULA_VET_center_x_position","cm",0.0);
        ++NLines;  AddInputDouble("NEBULA_VET_center_y_position","cm",0.0);
        ++NLines;  AddInputDouble("NEBULA_VET_front_z_position","cm",1770.0);
        ++NLines;  AddInputDouble("NEBULA_VET_x_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("NEBULA_VET_y_rotation_angle","degree",0.0);
        ++NLines;  AddInputDouble("NEBULA_VET_z_rotation_angle","degree",0.0);
        ++NLines; AddInputBoolian("NEBULA_VET_Visualize_Geometry_DuringBuild",kFALSE);
        ++NLines; AddInputInteger("NEBULA_VET_Detector_Color",kOrange-3);
        ++NLines; AddInputInteger("NEBULA_VET_Frame_Color",kOrange+8);
        ++NLines;  AddInputString("NEBULA_VET_Geometry_FileName","NEBULA_VET_Geometry.geo.root");
        
        // SAMURAI Setup:
        ++NLines; AddInputBoolian("SAMURAI_SETUP_IncludeSetup",kFALSE);
        ++NLines;  AddInputDouble("SAMURAI_SETUP_center_x_position","cm",0.0);
        ++NLines;  AddInputDouble("SAMURAI_SETUP_center_y_position","cm",0.0);
        ++NLines;  AddInputDouble("SAMURAI_SETUP_center_z_position","cm",436.7);
        
        // GLAD magnet. This one should not be rotated or shifted,
        // since the magnetic field map then has to be transformed along!
        ++NLines; AddInputBoolian("GLAD_Magnet_IncludeInSetup",kTRUE);
        ++NLines;  AddInputString("GLAD_Geometry_FileName","GLAD_Geometry.geo.root");
        ++NLines; AddInputBoolian("GLAD_MagneticField_IncludeInSetup",kTRUE);
        ++NLines;  AddInputDouble("GLAD_MagneticFieldScale","percent",100.0);
        
        // Target:
        ++NLines;  AddInputDouble("TARGET_center_x_position","cm",0.0);
        ++NLines;  AddInputDouble("TARGET_center_y_position","cm",0.0);
        ++NLines;  AddInputDouble("TARGET_center_z_position","cm",0.0);
        ++NLines;  AddInputDouble("TARGET_PrimaryVolume_Thickness","cm",1.0);
        
        // NeuLAND Digitizer:
        ++NLines; AddInputInteger("NeuLAND_Digitizer_nDifferentRuns",1);
        ++NLines;  AddInputString("NeuLAND_Digitizer_OutputFile","NeuLAND_Digitizer_File.root");
        ++NLines; AddInputBoolian("NeuLAND_Digitizer_PrintParameters",kFALSE);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_PMT_Threshold","MeV",1.0);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_SaturationEnergy","MeV",83.33);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_Time_Resolution","ns",0.15);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_Energy_Resolution","MeV",0.05);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_IntegrationTime","ns",400.0);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_Effective_LightSpeed","cm/ns",14.0);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_AttenuationLength","cm",125.0);
        ++NLines;  AddInputDouble("NeuLAND_Digitizer_PMTPulse_DecayTime","ns",2.1);
        
        // NeuLAND Clustering:
        ++NLines;  AddInputString("NeuLAND_ClusterCondition","Block");
        ++NLines;  AddInputDouble("NeuLAND_ClusterRadius_X","cm",7.5);
        ++NLines;  AddInputDouble("NeuLAND_ClusterRadius_Y","cm",7.5);
        ++NLines;  AddInputDouble("NeuLAND_ClusterRadius_Z","cm",7.5);
        ++NLines;  AddInputDouble("NeuLAND_ClusterRadius_Time","ns",1.0);
        
        // VETO Digitizer:
        ++NLines; AddInputBoolian("VETO_Digitizer_PrintParameters",kFALSE);
        ++NLines;  AddInputDouble("VETO_Digitizer_PMT_Threshold","MeV",1.0);
        ++NLines;  AddInputDouble("VETO_Digitizer_SaturationEnergy","MeV",83.33);
        ++NLines;  AddInputDouble("VETO_Digitizer_Time_Resolution","ns",0.300);
        ++NLines;  AddInputDouble("VETO_Digitizer_Energy_Resolution","MeV",0.05);
        ++NLines;  AddInputDouble("VETO_Digitizer_IntegrationTime","ns",400.0);
        ++NLines;  AddInputDouble("VETO_Digitizer_Effective_LightSpeed","cm/ns",14.0);
        ++NLines;  AddInputDouble("VETO_Digitizer_AttenuationLength","cm",125.0);
        ++NLines;  AddInputDouble("VETO_Digitizer_PMTPulse_DecayTime","ns",2.1);
        
        // NEBULA Digitizer:
        ++NLines; AddInputBoolian("NEBULA_Digitizer_PrintParameters",kFALSE);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_PMT_Threshold","MeV",1.0);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_SaturationEnergy","MeV",83.33);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_Time_Resolution","ns",0.160);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_Energy_Resolution","MeV",0.05);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_IntegrationTime","ns",400.0);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_Effective_LightSpeed","cm/ns",11.5);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_AttenuationLength","cm",125.0);
        ++NLines;  AddInputDouble("NEBULA_Digitizer_PMTPulse_DecayTime","ns",2.1);
        
        // NEBULA Clustering:
        ++NLines;  AddInputString("NEBULA_ClusterCondition","Block");
        ++NLines;  AddInputDouble("NEBULA_ClusterRadius_X","cm",18.0);
        ++NLines;  AddInputDouble("NEBULA_ClusterRadius_Y","cm",18.0);
        ++NLines;  AddInputDouble("NEBULA_ClusterRadius_Z","cm",18.0);
        ++NLines;  AddInputDouble("NEBULA_ClusterRadius_Time","ns",1.0);
        
        // NEBULA VETO Digitizer:
        ++NLines; AddInputBoolian("NEBULA_VET_Digitizer_PrintParameters",kFALSE);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_PMT_Threshold","MeV",1.0);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_SaturationEnergy","MeV",83.33);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_Time_Resolution","ns",0.300);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_Energy_Resolution","MeV",0.05);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_IntegrationTime","ns",400.0);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_Effective_LightSpeed","cm/ns",11.5);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_AttenuationLength","cm",125.0);
        ++NLines;  AddInputDouble("NEBULA_VET_Digitizer_PMTPulse_DecayTime","ns",2.1);
        
        // DNN processing chain:
        ++NLines;  AddInputString("NeuLAND_Digitizer_JanMayer_Processing","NeuLAND_Clusters_JanMayer.root");
        ++NLines;  AddInputString("NeuLAND_SignalFile","NeuLAND_Signal_File.root");
        ++NLines; AddInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals",kTRUE);
        ++NLines;  AddInputString("NeuLAND_Translator_MarkPrimSignals","BackTracing");
        ++NLines;  AddInputString("NeuLAND_Translator_ObtainPrimPoints","LandPoint");
        ++NLines; AddInputBoolian("NeuLAND_Translator_MinimizationMarking_IncludeTime",kTRUE); 
        ++NLines; AddInputBoolian("NeuLAND_Translator_MinimizationMarking_AllowMaxDist",kFALSE); 
        ++NLines;  AddInputDouble("NeuLAND_Translator_MinimizationMarking_MaxDistance","cm",5.0e10);
        ++NLines; AddInputInteger("NeuLAND_Translator_MaxMultiplicityPerCluster_JanClusters",1000);
        ++NLines;  AddInputString("NeuLAND_ClusterFile","NeuLAND_Cluster_File.root");
        ++NLines;  AddInputString("NeuLAND_FilteredClusterFile","NeuLAND_Filtered_Cluster_File.root");
        
        // DNN IO-files generation:
        ++NLines;  AddInputString("NeuLAND_DNNTextFile","DNN_InputFile.txt");
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_SoftwareStructure","PythonText");
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_BoolianInputStructure","StandardSigmoid");
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_BoolianOutputStructure","StandardSigmoid");
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_InfoStructure","IO_Signals_Elena_12004_5");
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_Step2Structure","ScoringPlus_OneCluster");
        ++NLines; AddInputInteger("NeuLAND_DNNTextFile_Significance",4);
        ++NLines; AddInputInteger("NeuLAND_DNNTextFile_MaxNumberOfSignals",100);
        ++NLines; AddInputInteger("NeuLAND_DNNTextFile_MaxNumberOfClusters",30);
        ++NLines; AddInputInteger("NeuLAND_DNNTextFile_Step2_nOutputFlags_PerCluster",2);
        ++NLines; AddInputInteger("NeuLAND_DNNTextFile_EventBatchSize",1000);
        ++NLines; AddInputInteger("NeuLAND_DNNTextFile_nEpochs",2);
        ++NLines; AddInputInteger("NeuLAND_DNNTextFile_nStep2Epochs",5);
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_SaveNetwork_BetweenEpochs",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_IncludeTargetInputs",kFALSE);
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_SkipBuilding",kFALSE);
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_NormalizeInputs",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_ValidationMode",kTRUE);
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_PreselectionMethod","Christiaan");
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_ForceBatchesEqualLength",kTRUE);
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_NormalizationMethod","Default");
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_MarkusFormat_AddRestNeutronsToOutput",kFALSE);
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_UseTextFileBias",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_BiasSingleClusterEvents",kFALSE);
        ++NLines;  AddInputString("NeuLAND_DNNTextFile_TimeNormaliation_Method","Data");
        ++NLines; AddInputBoolian("NeuLAND_DNNTextFile_DataTimeNormaliation_UsePrimTimes",kFALSE);
        ++NLines;  AddInputDouble("NeuLAND_DNNTextFile_TimeNormaliation_LeftBoundary","ns",59.0);
        ++NLines;  AddInputDouble("NeuLAND_DNNTextFile_TimeNormaliation_RightBoundary","ns",70.0);
        ++NLines;  AddInputString("NeuLAND_DNN_OutputFile","DNN_Multiplicities.root");
        
        // Traditional Method:
        ++NLines;  AddInputString("NeuLAND_TraditionalMethod_ReconstructionFile","NeuLAND_TradMed_ReconstructionFile.root");
        
        // Beta-reconstruction:
        ++NLines;  AddInputString("BetaReconstruction_NeuLAND_Select_VETOCondition","Advanced");
        ++NLines;  AddInputString("BetaReconstruction_NEBULA_Select_VETOCondition","Advanced");
        ++NLines;  AddInputDouble("BetaReconstruction_Beta_LowerCut","dimless",0.780);
        ++NLines;  AddInputDouble("BetaReconstruction_Beta_UpperCut","dimless",0.810);
        ++NLines;  AddInputString("BetaReconstruction_OutputFile","BetaReconstructedNeutrons.root");
        ++NLines;  AddInputString("SingleReconstruction_OutputFile","SingleReconstructedNeutrons.root");
        
        // ScoringPlus method:
        ++NLines; AddInputInteger("NeuLAND_ScoringHistogramResolution",400);
        ++NLines;  AddInputString("NeuLAND_ScoringOutputFile","NeuLAND_ScoringPlusMethod.root");
        ++NLines;  AddInputString("NeuLAND_DNNScoringOutputFile","NeuLAND_ScoringDNNPlus.root");
        ++NLines;  AddInputString("NeuLAND_Scoring_Combination_of_Individual_Scores","Addition");
        ++NLines;  AddInputString("NeuLAND_Scoring_HowToFind_PrimSignalWithinCluster","TOF");
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_DepositedEnergy",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_KineticEnergy",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_RelativisticBeta",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_TOF",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_Size",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_Rvalue",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_LogR",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_SpaceLength",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_SpaceRadius",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseClusterScore_TimeLength",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseSignalScore_DepositedEnergy",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseSignalScore_KineticEnergy",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseSignalScore_RelativisticBeta",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseSignalScore_TOF",kTRUE);
        ++NLines; AddInputBoolian("NeuLAND_Scoring_UseSignalScore_Rvalue",kTRUE);
        ++NLines;  AddInputString("NeuLAND_Scoring_DNNcalculation_SelectMultiplicity","DNN");
        
        // Final Analysis:
        ++NLines;  AddInputString("NeuLAND_Distributions","NeuLAND_Histograms.root");
        ++NLines; AddInputInteger("NeuLAND_JanGraphs_PhysList_nPoints",30);
        ++NLines;  AddInputDouble("NeuLAND_JanGraphs_PhysList_Estep","MeV",1.0);
        ++NLines; AddInputInteger("NeuLAND_EnergySpreadPlot_NBins",500);
        ++NLines; AddInputBoolian("NeuLAND_EnergySpreadPlot_RandomizeBarPositions",kTRUE);
        ++NLines;  AddInputString("NeuLAND_Reconstruction_FinalFile","FinalReconstruction.root");
        ++NLines;  AddInputString("NeuLAND_Reconstruction_MethodChoice","DNNScoringPlus_SingleTOF");
        ++NLines; AddInputBoolian("NeuLAND_Reconstruction_UseClusters",kTRUE);
        ++NLines;  AddInputString("NeuLAND_Multiplicity_MethodChoice","DNN");
        ++NLines; AddInputInteger("NeuLAND_Reconstruction_Efficiency_Matrix_Size",1);
        ++NLines; AddInputBoolian("IncludeRecoil_ErelSpectrum",kTRUE);
        ++NLines;  AddInputDouble("NeuLAND_Lower_EdepCut_Ex_Spectra","MeV",10.0);
    }
    
    // done.
}
    
