// Every CXX-file includes its own header:
#include "AllScorers.h"

// Default Constructor definition:
AllScorers::AllScorers()
{
    // Create Empty class:
    SignalScorers.clear();
    ClusterScorers.clear();
    Scorers_Initialized = kFALSE;
    Key = "Standard";
    ThisDetector = "NeuLAND";
    
    OutputPath = "./";
    OutputFile = "Scorers.root";
    BeamEperN = 600.0;
    BeamBeta = 0.75;
    ParticleGun_StartX = 0.0;
    ParticleGun_StartY = 0.0;
    ParticleGun_StartZ = 0.0;
    TargetT = 0.0;
    TargetX = 0.0;
    TargetY = 0.0;
    TargetZ = 0.0;
    NeuLAND_Center_X = 0.0;
    NeuLAND_Center_Y = 0.0;
    NeuLAND_Front_Z = 1400.0;
    NeuLAND_RotX = 0.0;
    NeuLAND_RotY = 0.0;
    NeuLAND_RotZ = 0.0;
    NeuLAND_Start_Time = 52.0;
    NeuLAND_Scintillator_Totalthickness = 5.0;
    NeuLAND_nBarsPerPlane = 50;
    NeuLAND_nDoublePlanes = 30;
    NeuLAND_nBarsTotal = 3000;
    ScoringResolution = 100;
    ParticleType_ToBeDetected = "neutron";
    ParticleMass = 931.0;
    ScoreCombination = "Multiplication";
    SelectPrimHitInCluster = "TOF";
    
    UseClusterScore_DepositedEnergy = kTRUE;
    UseClusterScore_Size = kTRUE;
    UseClusterScore_KineticEnergy = kTRUE;
    UseClusterScore_RelativisticBeta = kTRUE;
    UseClusterScore_Rvalue = kTRUE;
    UseClusterScore_LogR = kTRUE;
    UseClusterScore_TOF = kTRUE;
    UseClusterScore_SpaceLength = kTRUE;
    UseClusterScore_TimeLength = kTRUE;
    UseClusterScore_SpaceRadius = kTRUE;
            
    UseSignalScore_DepositedEnergy = kTRUE;
    UseSignalScore_KineticEnergy = kTRUE;
    UseSignalScore_RelativisticBeta = kTRUE;
    UseSignalScore_Rvalue = kTRUE;
    UseSignalScore_TOF = kTRUE;
    
    Errors.clear();
    PrintErrors = kTRUE;
    
    Inputs = 0;
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    Generator = new TRandom3(0);
}

// Destructor definition:
AllScorers::~AllScorers() 
{
    for (Int_t k = 0; k<SignalScorers.size(); ++k)  {delete SignalScorers.at(k);}  SignalScorers.clear();
    for (Int_t k = 0; k<ClusterScorers.size(); ++k) {delete ClusterScorers.at(k);} ClusterScorers.clear();
    Errors.clear();
    delete TheNuclei;
}

// Include member functions:
#include "Initialize.h"
#include "ControlFunctions.h"
#include "ErrorHandling.h"
#include "ComputeSignalScores.h"
#include "ComputeClusterScores.h"

// Generation of a ROOT-dictionary:
ClassImp(AllScorers)


