// Include this class header:
#include "R3BTextFileGenerator.h"

// Define global random number generator:
TRandom3* TheGenerator;

// Include also auxillary sorting functions:
#include "AuxillarySortFunctions.h"

// Default Constructor:
R3BTextFileGenerator::R3BTextFileGenerator()
{
    ioman = 0;                      
    fArraySignals = 0;              
    fArrayDigis = 0;                
    fArrayClusters = 0;             
    fArrayFilteredClusters = 0;     
    fArrayNeutronTracks = 0;        
    fArrayGunMult = 0;
    fArrayNeuMult = 0;
    fArrayDetMult = 0;
    fArrayDNNMult = 0;
    nDigiRuns = 1;
    CurrentDigiRun = 0;
    TheGenerator = 0;
    
    IO_Manager_IsLinked = kFALSE;
    NeutronTracks_IsLinked = kFALSE;
    Mults_AreLinked = 0;
    DNNMults_AreLinked = 0;
    Signals_IsLinked = 0;
    Digis_IsLinked = 0;
    Clusters_IsLinked = 0;
    Filtered_IsLinked = 0;
    
    N_inputs = 0;
    N_outputs = 0;
    Nevents = 0;
    InputNeurons.clear();
    OutputNeurons.clear();
    Tags.clear();
    Input_ISYES = 1.0;
    Input_ISNO = 0.0;
    Output_ISYES = 1.0;
    Output_ISNO = 0.0;
    
    InfoUse = "";
    InfoUse_Step2 = "";
    NmaxSignals = 100;
    
    SubFolder = "/DNN_DataBase/";
    FileName = "./Default.txt";
    FileOption = "";
    FileTest = kFALSE;
    EventCounter = 0;
    FillCounter = 0;
    BatchCounter = 0;
    IO_Significance = 4;
    EventBatch = 0;
    
    ValidationMode = kFALSE;
    IncludeTarget = kFALSE;
    Target_X = 0.0;
    Target_Y = 0.0;
    Target_Z = 0.0;
    Target_T = 0.0;
    NeuLAND_Center_X = 0.0;
    NeuLAND_Center_Y = 0.0;
    NeuLAND_Front_Z = 1400.0;
    NeuLAND_Rot_X = 0.0;
    NeuLAND_Rot_Y = 0.0;
    NeuLAND_Rot_Z = 0.0;
    NeuLAND_Active_Bar_Thickness = 4.8;
    NeuLAND_Total_Bar_Length = 250.0;
    NeuLAND_TotalBarThicknessXY = 5.0;
    NeuLAND_TotalBarThicknessZ = 5.0;
    CompressBatches = kFALSE;
    ParentOutputPath = "./";
    ComputeSingleClusterRatio = kFALSE;
    
    PrintErrors = kTRUE;
    Errors.clear();
    ConsistencyCheck = kFALSE;
    
    Inputs = 0;
    PreselectionMethod = "Christiaan";
    MaxMultiplicity = 0;
    NbarsPerPlane = 1;
    NDoublePlanes = 1;
    ParticleType = "neutron";
    MarkusFormat_AddRestNeutrons = kFALSE;
    SimulationData_IsAvailable = kFALSE;
    Step2_MultiplicityChoice = "DNN";
    TimeNormalization_LeftBoundary = 0.0;
    TimeNormalization_RightBoundary = 100.0;
    TimeNormalization_Option = "Default";
    TimeNormalization_NormName = "T_Default";
    
    // Elena Hoemann network options:
    Include_HorPaddleCoordinate = kFALSE;
    Include_ZeroMult_Output = kFALSE;
    Include_ZeroMultEvents = kFALSE;
    Include_Multiplicity_MisAligned_Events = kFALSE;
    Include_SigneHitClusters = kFALSE;
    Include_TotalScores = kFALSE;
    Include_SignalScores = kFALSE;
    Include_SignalBetas = kFALSE;
    Eliminate_Tails_InNormalization = kFALSE;
    ReShuffle_ClusterVector = kFALSE;
    CutOff_ClusterVector = kFALSE;
    Kickout_EventsWithLostPrimClusters = kFALSE;
    Step2_UseDetectedMultiplicity = kFALSE;
    nOutputFlags_PerCluster = 1;
    DataTimeNormaliation_UsePrimTimes = kFALSE;
    
    // Nuclear DataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    
    // Scoring+ method:
    TheScorers = new AllScorers();
    TotalClusterScore_All = 0;
    TotalClusterScore_All = 0;
    
    // Multithreading:
    NumberOfThreads = 1;
    CurrentThread = 0;
    
    // Second reconstruction step:
    Select_Determined_Multiplicity = 1;
    Step2_IsSelected = kFALSE;
    Step2_MaxNClusters.clear();
    
    // Normalization class:
    DefaultNorms = 0;
    LearnAdvancedNorms = 0;
    ApplyAdvancedNorms = 0;
    ElenaNorms = 0;
    TheNormalization = 0;
    ExecuteNormalization = kFALSE;
}
    
// Destructor:
R3BTextFileGenerator::~R3BTextFileGenerator() 
{   
    if(fArrayNeutronTracks) {fArrayNeutronTracks->Clear(); delete fArrayNeutronTracks;}
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        if(fArraySignals[k]) {fArraySignals[k]->Clear(); delete fArraySignals[k];}
        if(fArrayClusters[k]) {fArrayClusters[k]->Clear(); delete fArrayClusters[k];}
        if(fArrayDigis[k]) {fArrayDigis[k]->Clear(); delete fArrayDigis[k];}
        if(fArrayFilteredClusters[k]) {fArrayFilteredClusters[k]->Clear(); delete fArrayFilteredClusters[k];}
        if(fArrayGunMult[k]) {fArrayGunMult[k]->Clear(); delete fArrayGunMult[k];}
        if(fArrayNeuMult[k]) {fArrayNeuMult[k]->Clear(); delete fArrayNeuMult[k];}
        if(fArrayDetMult[k]) {fArrayDetMult[k]->Clear(); delete fArrayDetMult[k];}
        if(fArrayDNNMult[k]) {fArrayDNNMult[k]->Clear(); delete fArrayDNNMult[k];}
    }
    
    delete Signals_IsLinked;
    delete Digis_IsLinked;
    delete Clusters_IsLinked;
    delete Filtered_IsLinked;
    delete Mults_AreLinked;
    delete DNNMults_AreLinked;
    
    InputNeurons.clear();
    OutputNeurons.clear();
    Tags.clear();
    Errors.clear();
    
    TheTextFile.close();
    OutTextFile.close();
    
    delete TheScorers;
    delete TheNuclei;
    delete DefaultNorms;
    delete LearnAdvancedNorms;
    delete ApplyAdvancedNorms;
    delete ElenaNorms;
}

// Member function definitions:
#include "SetCurrentDigiRun.h"
#include "SetTheThreads.h"
#include "DoesFileExist.h"
#include "GetPMTPossition.h"
#include "ErrorMessage.h"
#include "LinkInputClass.h"
#include "RoundOff.h"
#include "DoubleIntTotString.h"
#include "SetYESNO.h"
#include "GenerateOneFile.h"
#include "CreateFile.h"
#include "WriteEvent.h"
#include "WriteEventNTimes.h"
#include "NormClassDefinition.h"
#include "NormClassSelection.h"
#include "NormClassFinish.h"
#include "FinishTextFile.h"
#include "Fill_Network_IO.h"
#include "IO_Default_Fillfunction.h"
#include "FillOneCluster.h"
#include "FillOneEmptyCluster.h"
#include "IO_FillFunction_2to6_TradMed_BabyNetwork.h"
#include "IO_FillFunction_3to6_TradMed_BabyNetwork.h"
#include "IO_FillFunction_Signals_StandardNetwork.h"
#include "IO_FillFunction_Signals_MultNetwork.h"
#include "IO_FillFunction_MarkusPolleryd.h"
#include "IO_FillFunction_ElenaHoemann.h"
#include "IO_FillFunction_ScoringPlus.h"
#include "ProcessOneEvent_SingleCluster.h"
#include "LinkIOManager.h"
#include "LinkSignals.h"
#include "LinkDigis.h"
#include "LinkClusters.h"
#include "LinkNeutronTracks.h"
#include "LinkFilteredClusters.h"
#include "LinkMultiplicities.h"
#include "LinkDNNMultiplicity.h"
#include "Initialize.h"

// Generate ROOT dictionary:
ClassImp(R3BTextFileGenerator);
