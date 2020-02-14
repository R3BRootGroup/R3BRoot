// Every CXX-file includes its own header file:
#include "R3BRecoTranslator.h"
#include "RoundOff.h"

// Default constructor definition:
R3BRecoTranslator::R3BRecoTranslator() : FairTask("R3BRecoTranslator")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Inputs:
    fArraySignals = new TClonesArray("R3BSignal");       
    fArrayNEBULASignals = new TClonesArray("R3BSignal");       
    fArrayClusters = new TClonesArray("R3BSignalCluster");
    fArrayNEBULAClusters = new TClonesArray("R3BSignalCluster");      
    
    // Primary hit classes:
    fArrayPrimSignals_TradMed_Clusters_CutsMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_TradMed_Clusters_DNNMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_TradMed_Clusters_PerfectMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_ScoringPlus_Signals_CutsMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_ScoringPlus_Signals_DNNMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_ScoringPlus_Signals_PerfectMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_ScoringPlus_Clusters_CutsMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_ScoringPlus_Clusters_DNNMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult = new TClonesArray("R3BSignal");
    fArrayPrimSignals_DNNScoringPlus = new TClonesArray("R3BSignal");
    fArrayPrimSignals_DNNScoringPlus_SingleTOF = new TClonesArray("R3BSignal");
    fArrayPrimSignals_DNNScoringPlus_Max = new TClonesArray("R3BSignal");
    fArrayPrimSignals_BetaReconstruction_NeuLAND = new TClonesArray("R3BSignal");
    fArrayPrimSignals_BetaReconstruction_NEBULA = new TClonesArray("R3BSignal");
    fArrayPrimSignals_BetaReconstruction_Combined = new TClonesArray("R3BSignal");
    fArrayPrimSignals_SingleReconstruction_NeuLAND = new TClonesArray("R3BSignal");
    fArrayPrimSignals_SingleReconstruction_NEBULA = new TClonesArray("R3BSignal");
    fArrayPrimSignals_SingleReconstruction_Combined = new TClonesArray("R3BSignal");
    
    // Neutron track classes:
    fRecoNeutronTracks_TradMed_Clusters_CutsMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_TradMed_Clusters_DNNMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_TradMed_Clusters_PerfectMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_ScoringPlus_Signals_CutsMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_ScoringPlus_Signals_DNNMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_DNNScoringPlus = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_DNNScoringPlus_SingleTOF = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_DNNScoringPlus_Max = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_BetaReconstruction_NeuLAND = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_BetaReconstruction_NEBULA = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_BetaReconstruction_Combined = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_SingleReconstruction_NeuLAND = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_SingleReconstruction_NEBULA = new TClonesArray("TLorentzVector");
    fRecoNeutronTracks_SingleReconstruction_Combined = new TClonesArray("TLorentzVector");
    
    // Other outputs:
    fPerfectNeutronTracks_Signals = new TClonesArray("TLorentzVector");
    fPerfectNeutronTracks_Clusters = new TClonesArray("TLorentzVector");
    AllSignals = new TClonesArray("R3BSignal");
    ClusterHeads = new TClonesArray("R3BSignal");
    TheNeutronHits = new TClonesArray("R3BSignal");
    TheNeutronTracks = new TClonesArray("TLorentzVector");
    
    // Control variables:
    EventCounter = 0;        
    nEvents = 0;             
    
    // Input parameters:
    UseNEBULA = kFALSE;
    ceff = 14.0;             
    EffMatrix_nMaxHits = 1;
    Target_T = 0.0;
    Target_X = 0.0;
    Target_Y = 0.0;
    Target_Z = 0.0;
    ParticleType = "neutron";
    ParticleMass = 1.0;
    Multiplicity_Method = "Cuts";
    Reconstruction_Method = "Traditional";
    OutputPath = "./";
    SimulationData_IsAvailable = kFALSE;
    Step2_MultiplicityChoice = "DNN";
    Include_SingleClusterEvents = kTRUE;
    
    // Efficiency matrices:
    EffMatrix_TradMed_Clusters_CutsMult = 0;
    EffMatrix_TradMed_Clusters_DNNMult = 0;
    EffMatrix_TradMed_Clusters_PerfectMult = 0;
    EffMatrix_ScoringPlus_Signals_CutsMult = 0;
    EffMatrix_ScoringPlus_Signals_DNNMult = 0;
    EffMatrix_ScoringPlus_Signals_PerfectMult = 0;
    EffMatrix_ScoringPlus_Clusters_CutsMult = 0;
    EffMatrix_ScoringPlus_Clusters_DNNMult = 0;
    EffMatrix_ScoringPlus_Clusters_PerfectMult = 0;
    EffMatrix_DNNScoringPlus = 0;
    EffMatrix_DNNScoringPlus_SingleTOF = 0;
    EffMatrix_DNNScoringPlus_Max = 0;
    EffMatrix_BetaReconstruction_NeuLAND = 0;
    EffMatrix_BetaReconstruction_NEBULA = 0;
    EffMatrix_BetaReconstruction_Combined = 0;
    EffMatrix_SingleReconstruction_NeuLAND = 0;
    EffMatrix_SingleReconstruction_NEBULA = 0;
    EffMatrix_SingleReconstruction_Combined = 0;
         
    // Auxillary classes:
    Inputs = 0;
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
    TheScorers = new AllScorers();
}

// Destructor definition:
R3BRecoTranslator::~R3BRecoTranslator()
{
    // Delete the arrays:
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if(fArrayClusters) {fArrayClusters->Clear(); delete fArrayClusters;}
    if(fArrayNEBULAClusters) {fArrayNEBULAClusters->Clear(); delete fArrayNEBULAClusters;}
    if(fArrayNEBULASignals) {fArrayNEBULASignals->Clear(); delete fArrayNEBULASignals;}
    
    // Primary hit classes:
    if(fArrayPrimSignals_TradMed_Clusters_CutsMult) {fArrayPrimSignals_TradMed_Clusters_CutsMult->Clear(); delete fArrayPrimSignals_TradMed_Clusters_CutsMult;}
    if(fArrayPrimSignals_TradMed_Clusters_DNNMult) {fArrayPrimSignals_TradMed_Clusters_DNNMult->Clear(); delete fArrayPrimSignals_TradMed_Clusters_DNNMult;}
    if(fArrayPrimSignals_TradMed_Clusters_PerfectMult) {fArrayPrimSignals_TradMed_Clusters_PerfectMult->Clear(); delete fArrayPrimSignals_TradMed_Clusters_PerfectMult;}
    if(fArrayPrimSignals_ScoringPlus_Signals_CutsMult) {fArrayPrimSignals_ScoringPlus_Signals_CutsMult->Clear(); delete fArrayPrimSignals_ScoringPlus_Signals_CutsMult;}
    if(fArrayPrimSignals_ScoringPlus_Signals_DNNMult) {fArrayPrimSignals_ScoringPlus_Signals_DNNMult->Clear(); delete fArrayPrimSignals_ScoringPlus_Signals_DNNMult;}
    if(fArrayPrimSignals_ScoringPlus_Signals_PerfectMult) {fArrayPrimSignals_ScoringPlus_Signals_PerfectMult->Clear(); delete fArrayPrimSignals_ScoringPlus_Signals_PerfectMult;}
    if(fArrayPrimSignals_ScoringPlus_Clusters_CutsMult) {fArrayPrimSignals_ScoringPlus_Clusters_CutsMult->Clear(); delete fArrayPrimSignals_ScoringPlus_Clusters_CutsMult;}
    if(fArrayPrimSignals_ScoringPlus_Clusters_DNNMult) {fArrayPrimSignals_ScoringPlus_Clusters_DNNMult->Clear(); delete fArrayPrimSignals_ScoringPlus_Clusters_DNNMult;}
    if(fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult) {fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult->Clear(); delete fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult;}
    if(fArrayPrimSignals_DNNScoringPlus) {fArrayPrimSignals_DNNScoringPlus->Clear(); delete fArrayPrimSignals_DNNScoringPlus;}
    if(fArrayPrimSignals_DNNScoringPlus_SingleTOF) {fArrayPrimSignals_DNNScoringPlus_SingleTOF->Clear(); delete fArrayPrimSignals_DNNScoringPlus_SingleTOF;}
    if(fArrayPrimSignals_DNNScoringPlus_Max) {fArrayPrimSignals_DNNScoringPlus_Max->Clear(); delete fArrayPrimSignals_DNNScoringPlus_Max;}
    if(fArrayPrimSignals_BetaReconstruction_NeuLAND) {fArrayPrimSignals_BetaReconstruction_NeuLAND->Clear(); delete fArrayPrimSignals_BetaReconstruction_NeuLAND;}
    if(fArrayPrimSignals_BetaReconstruction_NEBULA) {fArrayPrimSignals_BetaReconstruction_NEBULA->Clear(); delete fArrayPrimSignals_BetaReconstruction_NEBULA;}
    if(fArrayPrimSignals_BetaReconstruction_Combined) {fArrayPrimSignals_BetaReconstruction_Combined->Clear(); delete fArrayPrimSignals_BetaReconstruction_Combined;}
    if(fArrayPrimSignals_SingleReconstruction_NeuLAND) {fArrayPrimSignals_SingleReconstruction_NeuLAND->Clear(); delete fArrayPrimSignals_SingleReconstruction_NeuLAND;}
    if(fArrayPrimSignals_SingleReconstruction_NEBULA) {fArrayPrimSignals_SingleReconstruction_NEBULA->Clear(); delete fArrayPrimSignals_SingleReconstruction_NEBULA;}
    if(fArrayPrimSignals_SingleReconstruction_Combined) {fArrayPrimSignals_SingleReconstruction_Combined->Clear(); delete fArrayPrimSignals_SingleReconstruction_Combined;}
    
    // Neutron track classes:
    if(fRecoNeutronTracks_TradMed_Clusters_CutsMult) {fRecoNeutronTracks_TradMed_Clusters_CutsMult->Clear(); delete fRecoNeutronTracks_TradMed_Clusters_CutsMult;}
    if(fRecoNeutronTracks_TradMed_Clusters_DNNMult) {fRecoNeutronTracks_TradMed_Clusters_DNNMult->Clear(); delete fRecoNeutronTracks_TradMed_Clusters_DNNMult;}
    if(fRecoNeutronTracks_TradMed_Clusters_PerfectMult) {fRecoNeutronTracks_TradMed_Clusters_PerfectMult->Clear(); delete fRecoNeutronTracks_TradMed_Clusters_PerfectMult;}
    if(fRecoNeutronTracks_ScoringPlus_Signals_CutsMult) {fRecoNeutronTracks_ScoringPlus_Signals_CutsMult->Clear(); delete fRecoNeutronTracks_ScoringPlus_Signals_CutsMult;}
    if(fRecoNeutronTracks_ScoringPlus_Signals_DNNMult) {fRecoNeutronTracks_ScoringPlus_Signals_DNNMult->Clear(); delete fRecoNeutronTracks_ScoringPlus_Signals_DNNMult;}
    if(fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult) {fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult->Clear(); delete fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult;}
    if(fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult) {fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult->Clear(); delete fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult;}
    if(fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult) {fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult->Clear(); delete fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult;}
    if(fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult) {fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult->Clear(); delete fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult;}
    if(fRecoNeutronTracks_DNNScoringPlus) {fRecoNeutronTracks_DNNScoringPlus->Clear(); delete fRecoNeutronTracks_DNNScoringPlus;}
    if(fRecoNeutronTracks_DNNScoringPlus_SingleTOF) {fRecoNeutronTracks_DNNScoringPlus_SingleTOF->Clear(); delete fRecoNeutronTracks_DNNScoringPlus_SingleTOF;}
    if(fRecoNeutronTracks_DNNScoringPlus_Max) {fRecoNeutronTracks_DNNScoringPlus_Max->Clear(); delete fRecoNeutronTracks_DNNScoringPlus_Max;}
    if(fRecoNeutronTracks_BetaReconstruction_NeuLAND) {fRecoNeutronTracks_BetaReconstruction_NeuLAND->Clear(); delete fRecoNeutronTracks_BetaReconstruction_NeuLAND;}
    if(fRecoNeutronTracks_BetaReconstruction_NEBULA) {fRecoNeutronTracks_BetaReconstruction_NEBULA->Clear(); delete fRecoNeutronTracks_BetaReconstruction_NEBULA;}
    if(fRecoNeutronTracks_BetaReconstruction_Combined) {fRecoNeutronTracks_BetaReconstruction_Combined->Clear(); delete fRecoNeutronTracks_BetaReconstruction_Combined;}
    if(fRecoNeutronTracks_SingleReconstruction_NeuLAND) {fRecoNeutronTracks_SingleReconstruction_NeuLAND->Clear(); delete fRecoNeutronTracks_SingleReconstruction_NeuLAND;}
    if(fRecoNeutronTracks_SingleReconstruction_NEBULA) {fRecoNeutronTracks_SingleReconstruction_NEBULA->Clear(); delete fRecoNeutronTracks_SingleReconstruction_NEBULA;}
    if(fRecoNeutronTracks_SingleReconstruction_Combined) {fRecoNeutronTracks_SingleReconstruction_Combined->Clear(); delete fRecoNeutronTracks_SingleReconstruction_Combined;}
    
    // Other outputs:
    if(fPerfectNeutronTracks_Signals) {fPerfectNeutronTracks_Signals->Clear(); delete fPerfectNeutronTracks_Signals;}
    if(fPerfectNeutronTracks_Clusters) {fPerfectNeutronTracks_Clusters->Clear(); delete fPerfectNeutronTracks_Clusters;}
    if(AllSignals) {AllSignals->Clear(); delete AllSignals;}
    if (ClusterHeads) {ClusterHeads->Clear(); delete ClusterHeads;}
    if(TheNeutronHits) {TheNeutronHits->Clear(); delete TheNeutronHits;}
    if(TheNeutronTracks) {TheNeutronTracks->Clear(); delete TheNeutronTracks;}
    
    // Delete matrices:
    delete EffMatrix_TradMed_Clusters_CutsMult;
    delete EffMatrix_TradMed_Clusters_DNNMult;
    delete EffMatrix_TradMed_Clusters_PerfectMult;
    delete EffMatrix_ScoringPlus_Signals_CutsMult;
    delete EffMatrix_ScoringPlus_Signals_DNNMult;
    delete EffMatrix_ScoringPlus_Signals_PerfectMult;
    delete EffMatrix_ScoringPlus_Clusters_CutsMult;
    delete EffMatrix_ScoringPlus_Clusters_DNNMult;
    delete EffMatrix_ScoringPlus_Clusters_PerfectMult;
    delete EffMatrix_DNNScoringPlus;
    delete EffMatrix_DNNScoringPlus_SingleTOF;
    delete EffMatrix_DNNScoringPlus_Max;
    delete EffMatrix_BetaReconstruction_NeuLAND;
    delete EffMatrix_BetaReconstruction_NEBULA;
    delete EffMatrix_BetaReconstruction_Combined;
    delete EffMatrix_SingleReconstruction_NeuLAND;
    delete EffMatrix_SingleReconstruction_NEBULA;
    delete EffMatrix_SingleReconstruction_Combined;
    
    // Delete other objects:
    delete TheNuclei;
    delete TheScorers;
}

// FairTask initialization function:
InitStatus R3BRecoTranslator::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BRecoTranslator::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BRecoTranslator: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs:
    UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    ceff = Inputs->GetInputDouble("NeuLAND_Digitizer_Effective_LightSpeed","cm/ns");
    EffMatrix_nMaxHits = Inputs->GetInputInteger("NeuLAND_Reconstruction_Efficiency_Matrix_Size");
    Target_T = Inputs->GetTargetTime();
    Target_X = Inputs->GetInputDouble("TARGET_center_x_position","cm");
    Target_Y = Inputs->GetInputDouble("TARGET_center_y_position","cm");
    Target_Z = Inputs->GetInputDouble("TARGET_center_z_position","cm");
    ParticleType = Inputs->GetInputString("NeuLAND_ParticleType_ToBeDetected");
    ParticleMass = TheNuclei->GetMass(ParticleType,"MeV");
    SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    Multiplicity_Method = Inputs->GetInputString("NeuLAND_Multiplicity_MethodChoice");
    Step2_MultiplicityChoice = Inputs->GetInputString("NeuLAND_Scoring_DNNcalculation_SelectMultiplicity");
    Reconstruction_Method = Inputs->GetInputString("NeuLAND_Reconstruction_MethodChoice");
    OutputPath = Inputs->GetInputString("TheOutputPath");
    Bool_t UseClusters = Inputs->GetInputBoolian("NeuLAND_Reconstruction_UseClusters");
    if (Reconstruction_Method=="ScoringPlus")
    {
        if (UseClusters==kTRUE) {Reconstruction_Method = Reconstruction_Method + "_Clusters";}
        else {Reconstruction_Method = Reconstruction_Method + "_Signals";}
    }
    
    // Fill all input arrays:
    Bool_t IniTest = kTRUE;
    IniTest = InitializeInputClonesArray(ioman,fArraySignals,"Signals"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayClusters,"Clusters"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_TradMed_Clusters_CutsMult,"PrimaryHits_TradMed_Clusters_CutsMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_TradMed_Clusters_DNNMult,"PrimaryHits_TradMed_Clusters_DNNMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_TradMed_Clusters_PerfectMult,"PrimaryHits_TradMed_Clusters_PerfectMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_ScoringPlus_Signals_CutsMult,"PrimaryHits_ScoringPlus_Signals_CutsMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_ScoringPlus_Signals_DNNMult,"PrimaryHits_ScoringPlus_Signals_DNNMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_ScoringPlus_Signals_PerfectMult,"PrimaryHits_ScoringPlus_Signals_PerfectMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_ScoringPlus_Clusters_CutsMult,"PrimaryHits_ScoringPlus_Clusters_CutsMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_ScoringPlus_Clusters_DNNMult,"PrimaryHits_ScoringPlus_Clusters_DNNMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult,"PrimaryHits_ScoringPlus_Clusters_PerfectMult"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_DNNScoringPlus,"PrimaryHits_DNNScoringPlus"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_DNNScoringPlus_SingleTOF,"PrimaryHits_DNNScoringPlus_SingleTOF"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_DNNScoringPlus_Max,"PrimaryHits_DNNScoringPlus_Max"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_BetaReconstruction_NeuLAND,"BetaReconstructed_PrimHits_NeuLAND"); if (IniTest==kFALSE) {return kFATAL;}
    IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_SingleReconstruction_NeuLAND,"SingleReconstructed_PrimHits_NeuLAND"); if (IniTest==kFALSE) {return kFATAL;}
    
    if (UseNEBULA==kTRUE)
    {
        IniTest = InitializeInputClonesArray(ioman,fArrayNEBULASignals,"NEBULASignals"); if (IniTest==kFALSE) {return kFATAL;}
        IniTest = InitializeInputClonesArray(ioman,fArrayNEBULAClusters,"NEBULAClusters"); if (IniTest==kFALSE) {return kFATAL;}
        IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_BetaReconstruction_NEBULA,"BetaReconstructed_PrimHits_NEBULA"); if (IniTest==kFALSE) {return kFATAL;}
        IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_BetaReconstruction_Combined,"BetaReconstructed_PrimHits_Combined"); if (IniTest==kFALSE) {return kFATAL;}
        IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_SingleReconstruction_NEBULA,"SingleReconstructed_PrimHits_NEBULA"); if (IniTest==kFALSE) {return kFATAL;}
        IniTest = InitializeInputClonesArray(ioman,fArrayPrimSignals_SingleReconstruction_Combined,"SingleReconstructed_PrimHits_Combined"); if (IniTest==kFALSE) {return kFATAL;}
    }
    
    // Register all output arrays:
    ioman->Register("NeutronTracks_TradMed_Clusters_CutsMult","TLorenzVector",fRecoNeutronTracks_TradMed_Clusters_CutsMult,kTRUE);
    ioman->Register("NeutronTracks_TradMed_Clusters_DNNMult","TLorenzVector",fRecoNeutronTracks_TradMed_Clusters_DNNMult,kTRUE);
    ioman->Register("NeutronTracks_TradMed_Clusters_PerfectMult","TLorenzVector",fRecoNeutronTracks_TradMed_Clusters_PerfectMult,kTRUE);
    ioman->Register("NeutronTracks_ScoringPlus_Signals_CutsMult","TLorenzVector",fRecoNeutronTracks_ScoringPlus_Signals_CutsMult,kTRUE);
    ioman->Register("NeutronTracks_ScoringPlus_Signals_DNNMult","TLorenzVector",fRecoNeutronTracks_ScoringPlus_Signals_DNNMult,kTRUE);
    ioman->Register("NeutronTracks_ScoringPlus_Signals_PerfectMult","TLorenzVector",fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult,kTRUE);
    ioman->Register("NeutronTracks_ScoringPlus_Clusters_CutsMult","TLorenzVector",fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult,kTRUE);
    ioman->Register("NeutronTracks_ScoringPlus_Clusters_DNNMult","TLorenzVector",fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult,kTRUE);
    ioman->Register("NeutronTracks_ScoringPlus_Clusters_PerfectMult","TLorenzVector",fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult,kTRUE);
    ioman->Register("NeutronTracks_DNNScoringPlus","TLorenzVector",fRecoNeutronTracks_DNNScoringPlus,kTRUE);
    ioman->Register("NeutronTracks_DNNScoringPlus_SingleTOF","TLorenzVector",fRecoNeutronTracks_DNNScoringPlus_SingleTOF,kTRUE);
    ioman->Register("NeutronTracks_DNNScoringPlus_Max","TLorenzVector",fRecoNeutronTracks_DNNScoringPlus_Max,kTRUE);
    ioman->Register("NeutronTracks_BetaReconstruction_NeuLAND","TLorenzVector",fRecoNeutronTracks_BetaReconstruction_NeuLAND,kTRUE);
    ioman->Register("NeutronTracks_SingleReconstruction_NeuLAND","TLorenzVector",fRecoNeutronTracks_SingleReconstruction_NeuLAND,kTRUE);
    
    if (UseNEBULA==kTRUE)
    {
        ioman->Register("NeutronTracks_BetaReconstruction_NEBULA","TLorenzVector",fRecoNeutronTracks_BetaReconstruction_NEBULA,kTRUE);
        ioman->Register("NeutronTracks_BetaReconstruction_Combined","TLorenzVector",fRecoNeutronTracks_BetaReconstruction_Combined,kTRUE);
        ioman->Register("NeutronTracks_SingleReconstruction_NEBULA","TLorenzVector",fRecoNeutronTracks_SingleReconstruction_NEBULA,kTRUE);
        ioman->Register("NeutronTracks_SingleReconstruction_Combined","TLorenzVector",fRecoNeutronTracks_SingleReconstruction_Combined,kTRUE);
    }
    
    ioman->Register("NeutronTracks_PerfectMethod_Signals","TLorenzVector",fPerfectNeutronTracks_Signals,kTRUE);
    ioman->Register("NeutronTracks_PerfectMethod_Clusters","TLorenzVector",fPerfectNeutronTracks_Clusters,kTRUE);
    ioman->Register("AllSignals","R3BSignal",AllSignals,kTRUE);
    ioman->Register("ClusterHeads","R3BSignal",ClusterHeads,kTRUE);
    ioman->Register("TheNeutronHits","R3BSignal",TheNeutronHits,kTRUE);
    ioman->Register("TheNeutronTracks","TLorenzVector",TheNeutronTracks,kTRUE);
    
    // Initialize efficiency matrices:
    InitializeEffMatrix(EffMatrix_TradMed_Clusters_CutsMult);
    InitializeEffMatrix(EffMatrix_TradMed_Clusters_DNNMult);
    InitializeEffMatrix(EffMatrix_TradMed_Clusters_PerfectMult);
    InitializeEffMatrix(EffMatrix_ScoringPlus_Signals_CutsMult);
    InitializeEffMatrix(EffMatrix_ScoringPlus_Signals_DNNMult);
    InitializeEffMatrix(EffMatrix_ScoringPlus_Signals_PerfectMult);
    InitializeEffMatrix(EffMatrix_ScoringPlus_Clusters_CutsMult);
    InitializeEffMatrix(EffMatrix_ScoringPlus_Clusters_DNNMult);
    InitializeEffMatrix(EffMatrix_ScoringPlus_Clusters_PerfectMult);
    InitializeEffMatrix(EffMatrix_DNNScoringPlus);
    InitializeEffMatrix(EffMatrix_DNNScoringPlus_SingleTOF);
    InitializeEffMatrix(EffMatrix_DNNScoringPlus_Max);
    InitializeEffMatrix(EffMatrix_BetaReconstruction_NeuLAND);
    InitializeEffMatrix(EffMatrix_BetaReconstruction_NEBULA);
    InitializeEffMatrix(EffMatrix_BetaReconstruction_Combined);
    InitializeEffMatrix(EffMatrix_SingleReconstruction_NeuLAND);
    InitializeEffMatrix(EffMatrix_SingleReconstruction_NEBULA);
    InitializeEffMatrix(EffMatrix_SingleReconstruction_Combined);
    
    // Next, initialize the scoring class:
    TheScorers->LinkInputsClass(Inputs);
    Bool_t ScoreTest = TheScorers->Initialize();

    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    if (ScoreTest==kFALSE) {std::cout << "\n\nAllScorers Initialization Failed!\n\n"; return kFATAL;}
    
    // Load the previously determined scorers:
    TheScorers->ReadScorers();
    
    EventCounter = 0;
    return kSUCCESS;
}

Bool_t R3BRecoTranslator::InitializeInputClonesArray(FairRootManager* &ioman, TClonesArray* &TheArray, TString const ArrayName)
{
    if ((TClonesArray*)ioman->GetObject(ArrayName.Data()) == nullptr)
    {
            cout << "I/O-manager FATAL: R3BRecoTranslator::Init() ==> Could not locate <" + ArrayName + ">!\n\n";
            return kFALSE;
    }
    TheArray = (TClonesArray*) ioman->GetObject(ArrayName.Data());
    return kTRUE;
}

void R3BRecoTranslator::InitializeEffMatrix(Double_t** &EffMatrix)
{
    EffMatrix = new Double_t*[EffMatrix_nMaxHits+2];
    
    for (Int_t krow = 0; krow<(EffMatrix_nMaxHits+2); ++krow)
    {
        EffMatrix[krow] = new Double_t[EffMatrix_nMaxHits+1];
        
        for (Int_t kcol = 0; kcol<(EffMatrix_nMaxHits+1); ++kcol)
        {
            EffMatrix[krow][kcol] = 0.0;
        }
    }
}

// FairTask Execute function:
void R3BRecoTranslator::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the inputs into the TClonesArrays.
    Int_t nClusters_NeuLAND = fArrayClusters->GetEntries();
    Int_t nClusters_NEBULA = fArrayNEBULAClusters->GetEntries();
    
    // Reset all output Arrays:
    fRecoNeutronTracks_TradMed_Clusters_CutsMult->Clear();
    fRecoNeutronTracks_TradMed_Clusters_DNNMult->Clear();
    fRecoNeutronTracks_TradMed_Clusters_PerfectMult->Clear();
    fRecoNeutronTracks_ScoringPlus_Signals_CutsMult->Clear();
    fRecoNeutronTracks_ScoringPlus_Signals_DNNMult->Clear();
    fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult->Clear();
    fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult->Clear();
    fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult->Clear();
    fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult->Clear();
    fRecoNeutronTracks_DNNScoringPlus->Clear();
    fRecoNeutronTracks_DNNScoringPlus_SingleTOF->Clear();
    fRecoNeutronTracks_DNNScoringPlus_Max->Clear();
    fRecoNeutronTracks_BetaReconstruction_NeuLAND->Clear();
    fRecoNeutronTracks_BetaReconstruction_NEBULA->Clear();
    fRecoNeutronTracks_BetaReconstruction_Combined->Clear();
    fRecoNeutronTracks_SingleReconstruction_NeuLAND->Clear();
    fRecoNeutronTracks_SingleReconstruction_NEBULA->Clear();
    fRecoNeutronTracks_SingleReconstruction_Combined->Clear();
    
    fPerfectNeutronTracks_Signals->Clear();
    fPerfectNeutronTracks_Clusters->Clear();
    AllSignals->Clear();
    ClusterHeads->Clear();
    TheNeutronHits->Clear();
    TheNeutronTracks->Clear();
    
    // Begin by converting the R3BSignals from the different hits to TLorentVectors:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        ConvertSignalToTrack(fArrayPrimSignals_TradMed_Clusters_CutsMult,fRecoNeutronTracks_TradMed_Clusters_CutsMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_TradMed_Clusters_DNNMult,fRecoNeutronTracks_TradMed_Clusters_DNNMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_TradMed_Clusters_PerfectMult,fRecoNeutronTracks_TradMed_Clusters_PerfectMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_ScoringPlus_Signals_CutsMult,fRecoNeutronTracks_ScoringPlus_Signals_CutsMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_ScoringPlus_Signals_DNNMult,fRecoNeutronTracks_ScoringPlus_Signals_DNNMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_ScoringPlus_Signals_PerfectMult,fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_ScoringPlus_Clusters_CutsMult,fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_ScoringPlus_Clusters_DNNMult,fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult,fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_DNNScoringPlus,fRecoNeutronTracks_DNNScoringPlus,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_DNNScoringPlus_SingleTOF,fRecoNeutronTracks_DNNScoringPlus_SingleTOF,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_DNNScoringPlus_Max,fRecoNeutronTracks_DNNScoringPlus_Max,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_BetaReconstruction_NeuLAND,fRecoNeutronTracks_BetaReconstruction_NeuLAND,kFALSE);
        ConvertSignalToTrack(fArrayPrimSignals_SingleReconstruction_NeuLAND,fRecoNeutronTracks_SingleReconstruction_NeuLAND,kFALSE);
    }
    
    if (UseNEBULA==kTRUE)
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NEBULA>1)))
        {
            ConvertSignalToTrack(fArrayPrimSignals_BetaReconstruction_NEBULA,fRecoNeutronTracks_BetaReconstruction_NEBULA,kFALSE);
            ConvertSignalToTrack(fArrayPrimSignals_BetaReconstruction_Combined,fRecoNeutronTracks_BetaReconstruction_Combined,kFALSE);
        }
        
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
        {
            ConvertSignalToTrack(fArrayPrimSignals_SingleReconstruction_NEBULA,fRecoNeutronTracks_SingleReconstruction_NEBULA,kFALSE);
            ConvertSignalToTrack(fArrayPrimSignals_SingleReconstruction_Combined,fRecoNeutronTracks_SingleReconstruction_Combined,kFALSE);
        }
    }
    
    // Next, we wish to update the efficiency matrices:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_TradMed_Clusters_CutsMult,EffMatrix_TradMed_Clusters_CutsMult,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_TradMed_Clusters_DNNMult,EffMatrix_TradMed_Clusters_DNNMult,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_TradMed_Clusters_PerfectMult,EffMatrix_TradMed_Clusters_PerfectMult,"NeuLAND");
        UpdateEfficiencyMatrixSignals(fArrayPrimSignals_ScoringPlus_Signals_CutsMult,EffMatrix_ScoringPlus_Signals_CutsMult);
        UpdateEfficiencyMatrixSignals(fArrayPrimSignals_ScoringPlus_Signals_DNNMult,EffMatrix_ScoringPlus_Signals_DNNMult);
        UpdateEfficiencyMatrixSignals(fArrayPrimSignals_ScoringPlus_Signals_PerfectMult,EffMatrix_ScoringPlus_Signals_PerfectMult);
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_ScoringPlus_Clusters_CutsMult,EffMatrix_ScoringPlus_Clusters_CutsMult,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_ScoringPlus_Clusters_DNNMult,EffMatrix_ScoringPlus_Clusters_DNNMult,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult,EffMatrix_ScoringPlus_Clusters_PerfectMult,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_DNNScoringPlus,EffMatrix_DNNScoringPlus,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_DNNScoringPlus_SingleTOF,EffMatrix_DNNScoringPlus_SingleTOF,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_DNNScoringPlus_Max,EffMatrix_DNNScoringPlus_Max,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_BetaReconstruction_NeuLAND,EffMatrix_BetaReconstruction_NeuLAND,"NeuLAND");
        UpdateEfficiencyMatrixClusters(fArrayPrimSignals_SingleReconstruction_NeuLAND,EffMatrix_SingleReconstruction_NeuLAND,"NeuLAND");
    }
    
    if (UseNEBULA==kTRUE)
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NEBULA>1)))
        {
            UpdateEfficiencyMatrixClusters(fArrayPrimSignals_BetaReconstruction_NEBULA,EffMatrix_BetaReconstruction_NEBULA,"NEBULA");
            UpdateEfficiencyMatrixClusters(fArrayPrimSignals_BetaReconstruction_Combined,EffMatrix_BetaReconstruction_Combined,"Combined");
        }
        
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
        {
            UpdateEfficiencyMatrixClusters(fArrayPrimSignals_SingleReconstruction_NEBULA,EffMatrix_SingleReconstruction_NEBULA,"NEBULA");
            UpdateEfficiencyMatrixClusters(fArrayPrimSignals_SingleReconstruction_Combined,EffMatrix_SingleReconstruction_Combined,"Combined");
        }
    }
    
    // Then, build the neutron tracks according to a perfect reconstruction:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        ConvertSignalToTrack(fArraySignals,fPerfectNeutronTracks_Signals,kTRUE);
    }
    
    // Then, find the cluster heads among all clusters in NeuLAND:
    Int_t nClusters = fArrayClusters->GetEntries();
    Int_t nPrims = 0;
    Int_t ClusterHead_Index = 0;
    R3BSignalCluster* ThisCluster;
    R3BSignal* ClusterHead;
    R3BSignal* ClusterHead_Duplicate;
    
    for (Int_t kclus = 0; kclus<nClusters; ++kclus)
    {
        // Retrieve the current cluster:
        ThisCluster = (R3BSignalCluster*) fArrayClusters->At(kclus);
        
        // Retrieve the number of primary signals inside the cluster:
        nPrims = ThisCluster->GetPrimaryMultSim();
        
        // Retrieve the cluster head:
        ClusterHead = TheScorers->GetPrimaryClusterHit(ThisCluster);
        
        // Fill this in into the new array:
        if (nPrims>0)
        {
            for (Int_t n = 0; n<nPrims; ++n)
            {
                // Need to fill it in multiple times to accomodate
                // for multiple hits per cluster:
                new ((*ClusterHeads)[ClusterHead_Index]) R3BSignal(ClusterHead);
        
                // Retrieve the clusterhead now from the new array, so
                // that we avoid scrambling the labels of the original array:
                ClusterHead_Duplicate = (R3BSignal*) ClusterHeads->At(ClusterHead_Index);
        
                // Set proper labels:
                ClusterHead_Duplicate->SetNonPrimaryExp();
                ClusterHead_Duplicate->SetPrimarySim();
                
                // Update index:
                ClusterHead_Index = ClusterHead_Index + 1;
            }
        }
        
        // Done.
    }
    
    // Then, build the neutron tracks from the cluster heads, according to perfect reconstruction:
    if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
    {
        ConvertSignalToTrack(ClusterHeads,fPerfectNeutronTracks_Clusters,kTRUE);
    }
    
    // Next, we have to decide which output is the 'official' output.
    if ((Multiplicity_Method=="Cuts")&&(Reconstruction_Method=="Traditional"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_TradMed_Clusters_CutsMult);
            DuplicatePrimaryHits(fArrayPrimSignals_TradMed_Clusters_CutsMult);
        }
    }
    else if ((Multiplicity_Method=="DNN")&&(Reconstruction_Method=="Traditional"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_TradMed_Clusters_DNNMult);
            DuplicatePrimaryHits(fArrayPrimSignals_TradMed_Clusters_DNNMult);
        }
    }    
    else if ((Multiplicity_Method=="Perfect")&&(Reconstruction_Method=="Traditional"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_TradMed_Clusters_PerfectMult);
            DuplicatePrimaryHits(fArrayPrimSignals_TradMed_Clusters_PerfectMult);
        }
    }
    else if ((Multiplicity_Method=="Cuts")&&(Reconstruction_Method=="ScoringPlus_Signals"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_ScoringPlus_Signals_CutsMult);
            DuplicatePrimaryHits(fArrayPrimSignals_ScoringPlus_Signals_CutsMult);
        }
    }
    else if ((Multiplicity_Method=="DNN")&&(Reconstruction_Method=="ScoringPlus_Signals"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_ScoringPlus_Signals_DNNMult);
            DuplicatePrimaryHits(fArrayPrimSignals_ScoringPlus_Signals_DNNMult);
        }
    }
    else if ((Multiplicity_Method=="Perfect")&&(Reconstruction_Method=="ScoringPlus_Signals"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_ScoringPlus_Signals_PerfectMult);
            DuplicatePrimaryHits(fArrayPrimSignals_ScoringPlus_Signals_PerfectMult);
        }
    }
    else if ((Multiplicity_Method=="Cuts")&&(Reconstruction_Method=="ScoringPlus_Clusters"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_ScoringPlus_Clusters_CutsMult);
            DuplicatePrimaryHits(fArrayPrimSignals_ScoringPlus_Clusters_CutsMult);
        }
    }
    else if ((Multiplicity_Method=="DNN")&&(Reconstruction_Method=="ScoringPlus_Clusters"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_ScoringPlus_Clusters_DNNMult);
            DuplicatePrimaryHits(fArrayPrimSignals_ScoringPlus_Clusters_DNNMult);
        }
    }
    else if ((Multiplicity_Method=="Perfect")&&(Reconstruction_Method=="ScoringPlus_Clusters"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult);
            DuplicatePrimaryHits(fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult);
        }
    }
    else if ((((Multiplicity_Method=="DNN")&&(Step2_MultiplicityChoice=="DNN"))||
              ((Multiplicity_Method=="Perfect")&&(Step2_MultiplicityChoice=="Perfect")))&&
              ((Reconstruction_Method=="DNNScoringPlus")||(Reconstruction_Method=="DNNScoringPlus_SingleTOF")))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            if (Reconstruction_Method=="DNNScoringPlus")
            {
                SetExpMarksOnSignals(fArrayPrimSignals_DNNScoringPlus);
                DuplicatePrimaryHits(fArrayPrimSignals_DNNScoringPlus);
            }
            else
            {
                SetExpMarksOnSignals(fArrayPrimSignals_DNNScoringPlus_SingleTOF);
                DuplicatePrimaryHits(fArrayPrimSignals_DNNScoringPlus_SingleTOF);
            }
        }
    }
    else if ((((Multiplicity_Method=="DNN")&&(Step2_MultiplicityChoice=="Perfect"))||
              ((Multiplicity_Method=="Perfect")&&(Step2_MultiplicityChoice=="DNN")))&&
              ((Reconstruction_Method=="DNNScoringPlus")||(Reconstruction_Method=="DNNScoringPlus_SingleTOF")))
    {
        cout << "R3BRecoTranslator ERROR: For the Reco-method <"+Reconstruction_Method+">, The Multiplicity Methods of steps 1 & 2 have to match!\n";
    }
    else if ((((Multiplicity_Method=="DNN")&&(Step2_MultiplicityChoice=="DNN"))||
              ((Multiplicity_Method=="Perfect")&&(Step2_MultiplicityChoice=="Perfect")))&&
              (Reconstruction_Method=="DNNScoringPlus_Maximum"))
    {
        if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
        {
            SetExpMarksOnSignals(fArrayPrimSignals_DNNScoringPlus_Max);
            DuplicatePrimaryHits(fArrayPrimSignals_DNNScoringPlus_Max);
        }
    }
    else if ((((Multiplicity_Method=="DNN")&&(Step2_MultiplicityChoice=="Perfect"))||
              ((Multiplicity_Method=="Perfect")&&(Step2_MultiplicityChoice=="DNN")))&&
              (Reconstruction_Method=="DNNScoringPlus_Maximum"))
    {
        cout << "R3BRecoTranslator ERROR: For the Reco-method <"+Reconstruction_Method+">, The Multiplicity Methods of steps 1 & 2 have to match!\n";
    }
    else if ((Multiplicity_Method=="Beta")&&(Step2_MultiplicityChoice=="Beta"))
    {
        if (UseNEBULA==kFALSE)
        {
            if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
            {
                SetExpMarksOnSignals(fArrayPrimSignals_BetaReconstruction_NeuLAND);
                DuplicatePrimaryHits(fArrayPrimSignals_BetaReconstruction_NeuLAND);
            }
        }
        else
        {
            if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
            {
                SetExpMarksOnSignals(fArrayPrimSignals_BetaReconstruction_Combined);
                DuplicatePrimaryHits(fArrayPrimSignals_BetaReconstruction_Combined);
            }
        }
    }
    else if ((Multiplicity_Method=="Single")&&(Step2_MultiplicityChoice=="Single"))
    {
        if (UseNEBULA==kFALSE)
        {
            if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&(nClusters_NeuLAND>1)))
            {
                SetExpMarksOnSignals(fArrayPrimSignals_SingleReconstruction_NeuLAND);
                DuplicatePrimaryHits(fArrayPrimSignals_SingleReconstruction_NeuLAND);
            }
        }
        else
        {
            if ((Include_SingleClusterEvents==kTRUE)||((Include_SingleClusterEvents==kFALSE)&&((nClusters_NeuLAND+nClusters_NEBULA)>1)))
            {
                SetExpMarksOnSignals(fArrayPrimSignals_SingleReconstruction_Combined);
                DuplicatePrimaryHits(fArrayPrimSignals_SingleReconstruction_Combined);
            }
        }
    }
    else
    {
        cout << "R3BRecoTranslator ERROR: The Multiplicity Method <" << Multiplicity_Method << "> was not programmed!\n";
        cout << "R3BRecoTranslator ERROR: The Reconstruction Method <" << Reconstruction_Method << "> was not programmed!\n";
    }
    
    // Then, convert the output to tracks:
    ConvertSignalToTrack(TheNeutronHits,TheNeutronTracks,kFALSE);
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BRecoTranslator Log: We processed " << EventCounter << " events.\n";}
    
    // Done.
}

void R3BRecoTranslator::ConvertSignalToTrack(TClonesArray* &Input, TClonesArray* &Output, Bool_t const TestForSimMark)
{
    // Define parameters:
    Int_t nTracks = Input->GetEntries();
    Double_t c_Light = 29.9792458;
    Int_t TrackIndex = 0;
    
    // Declare what we need:
    R3BSignal* ThisSignal;
    TLorentzVector* Momentum;
    TVector3 Distance;
    Double_t Dist;
    Double_t TOF;
    Double_t beta;
    Double_t gamma;
    Double_t beta_x;
    Double_t beta_y;
    Double_t beta_z;
    
    // Create the tracks:
    for (Int_t k = 0; k<nTracks; ++k)
    {
        // Retrieve the current signal:
        ThisSignal = (R3BSignal*) Input->At(k);
        
        // Test for simulation mark:
        if ((TestForSimMark==kFALSE)||((TestForSimMark==kTRUE)&&(ThisSignal->IsPrimarySim()==kTRUE)))
        {
            // Fill the distance-vector:
            Distance.SetX(ThisSignal->GetPositionX() - Target_X);
            Distance.SetY(ThisSignal->GetPositionY() - Target_Y);
            Distance.SetZ(ThisSignal->GetPositionZ() - Target_Z);
    
            // Compute the length of the distance-vector:
            Dist = TMath::Sqrt(Distance.X()*Distance.X() + Distance.Y()*Distance.Y() + Distance.Z()*Distance.Z());
    
            // Then, compute beta:
            TOF = TMath::Abs(ThisSignal->GetTime() - Target_T);
            beta = Dist/(c_Light*TOF);
    
            // Next, construct gamma and beta_x, beta_y, beta_z:
            gamma = 1.0/TMath::Sqrt(1.0 - beta*beta);
            beta_x = beta*TMath::Sin(Distance.Theta())*TMath::Cos(Distance.Phi());
            beta_y = beta*TMath::Sin(Distance.Theta())*TMath::Sin(Distance.Phi());
            beta_z = beta*TMath::Cos(Distance.Theta());
        
            // Then, create the output-track:
            new ((*Output)[TrackIndex]) TLorentzVector();
            Momentum = (TLorentzVector*) Output->At(TrackIndex);
    
            // And build the TLorentzVector:
            Momentum->SetT(ParticleMass*gamma);
            Momentum->SetX(ParticleMass*gamma*beta_x);
            Momentum->SetY(ParticleMass*gamma*beta_y);
            Momentum->SetZ(ParticleMass*gamma*beta_z);
            
            // Update the track index:
            TrackIndex = TrackIndex + 1;
            
            // Done.
        }
    }
}

void R3BRecoTranslator::UpdateEfficiencyMatrixSignals(TClonesArray* &PrimSignals, Double_t** &EffMatrix)
{
    // Updates the efficiency matrix for clusters. Define parameters:
    Int_t nSignals = fArraySignals->GetEntries();
    Int_t nPrims = PrimSignals->GetEntries();
    
    // Declare what we need:
    R3BSignal* ThisSignal;
    R3BSignal* PrimSignal;
    Int_t nPrimHits_InCluster_Sim;
    Int_t nPrimHits_InCluster_Exp;
    Double_t Distance;
    Int_t Distance_Counter = 0;
    
    // Loop over the clusters:
    for (Int_t ksig = 0; ksig<nSignals; ++ksig)
    {
        // Retrieve the current cluster:
        ThisSignal = (R3BSignal*) fArraySignals->At(ksig);
        
        // Set the simulation numbers:
        nPrimHits_InCluster_Sim = 0;
        if (ThisSignal->IsPrimarySim()==kTRUE) {nPrimHits_InCluster_Sim = 1;}
            
        // Next, loop over the primary signals and compare them:
        Distance_Counter = 0;
        nPrimHits_InCluster_Exp = 0;
            
        for (Int_t kprim = 0; kprim<nPrims; ++kprim)
        {
            // Retrieve current primary signal:
            PrimSignal = (R3BSignal*) PrimSignals->At(kprim);
                
            // Compute distance:
            Distance = 0.0;
            Distance = Distance + (PrimSignal->GetPositionX() - ThisSignal->GetPositionX())*(PrimSignal->GetPositionX() - ThisSignal->GetPositionX());
            Distance = Distance + (PrimSignal->GetPositionY() - ThisSignal->GetPositionY())*(PrimSignal->GetPositionY() - ThisSignal->GetPositionY());
            Distance = Distance + (PrimSignal->GetPositionZ() - ThisSignal->GetPositionZ())*(PrimSignal->GetPositionZ() - ThisSignal->GetPositionZ());
            Distance = Distance + ceff*ceff*(PrimSignal->GetTime() - ThisSignal->GetTime())*(PrimSignal->GetTime() - ThisSignal->GetTime());
            Distance = TMath::Sqrt(Distance);
                
            // Count:
            if (Distance<0.1) {Distance_Counter = Distance_Counter + 1;}
        }
            
        // Now, if Distance_Counter==0, then the current signal was not assigned as a primary
        // signal. If it is one, it is a primary signal and if it is larger, we made an error:
        if (Distance_Counter==0)
        {
            // Do nothing.
        }
        else if (Distance_Counter==1)
        {
            // Then, count this signal:
            nPrimHits_InCluster_Exp = nPrimHits_InCluster_Exp + 1;
        }
        else
        {
            cout << "R3BRecoTranslator FATAL ERROR: We linked a signal to more then 1 reconstructed primary hit!\n";
        }
        
        // Now that we have the number of both Sim & Exp primary hits in the cluster, we can update our matrix.
        // First, prevent that we run out of the index space:
        if (nPrimHits_InCluster_Exp>EffMatrix_nMaxHits) {nPrimHits_InCluster_Exp = EffMatrix_nMaxHits;}
        if (nPrimHits_InCluster_Sim>EffMatrix_nMaxHits) {nPrimHits_InCluster_Sim = EffMatrix_nMaxHits;}
        
        // Then, update:
        if ((nPrimHits_InCluster_Exp>=0)&&(nPrimHits_InCluster_Exp<=EffMatrix_nMaxHits)&&(nPrimHits_InCluster_Sim>=0)&&(nPrimHits_InCluster_Sim<=EffMatrix_nMaxHits))
        {
            EffMatrix[nPrimHits_InCluster_Exp][nPrimHits_InCluster_Sim] = EffMatrix[nPrimHits_InCluster_Exp][nPrimHits_InCluster_Sim] + 1.0;
            EffMatrix[EffMatrix_nMaxHits+1][nPrimHits_InCluster_Sim] = EffMatrix[EffMatrix_nMaxHits+1][nPrimHits_InCluster_Sim] + 1.0;
        }
        
        // Done.
    }
}    

void R3BRecoTranslator::UpdateEfficiencyMatrixClusters(TClonesArray* &PrimSignals, Double_t** &EffMatrix, TString const Detector)
{
    // Updates the efficiency matrix for clusters. Define parameters:
    Int_t nClusters = 0;
    Int_t nPrims = PrimSignals->GetEntries();
    if (Detector=="NeuLAND") {nClusters = fArrayClusters->GetEntries();}
    if (Detector=="NEBULA") {nClusters = fArrayNEBULAClusters->GetEntries();}
    if (Detector=="Combined") {nClusters = fArrayClusters->GetEntries() + fArrayNEBULAClusters->GetEntries();}
    
    // Declare what we need:
    R3BSignalCluster* ThisCluster;
    R3BSignal* ThisSignal;
    R3BSignal* PrimSignal;
    Int_t ClusterSize;
    Int_t nPrimHits_InCluster_Sim;
    Int_t nPrimHits_InCluster_Exp;
    Double_t Distance;
    Int_t Distance_Counter = 0;
    
    // Loop over the clusters:
    for (Int_t kclus = 0; kclus<nClusters; ++kclus)
    {
        // Retrieve the current cluster:
        if (Detector=="Combined")
        {
            if (kclus<fArrayClusters->GetEntries()) {ThisCluster = (R3BSignalCluster*) fArrayClusters->At(kclus);}
            else {ThisCluster = (R3BSignalCluster*) fArrayNEBULAClusters->At(kclus - fArrayClusters->GetEntries());}
        }
        if (Detector=="NeuLAND") {ThisCluster = (R3BSignalCluster*) fArrayClusters->At(kclus);}
        if (Detector=="NEBULA") {ThisCluster = (R3BSignalCluster*) fArrayNEBULAClusters->At(kclus);}
        
        // Retrieve the cluster size:
        ClusterSize = ThisCluster->GetSize();
        
        // Retrieve the number of primary hits in the cluster
        // according to the MC data:
        nPrimHits_InCluster_Sim = ThisCluster->GetPrimaryMultSim();
        
        // Then, build the number of primary hits according to 
        // the reconstruction:
        nPrimHits_InCluster_Exp = 0;
        
        // Next, loop over the signals in the cluster:
        for (Int_t ksig = 0; ksig<ClusterSize; ++ksig)
        {
            // Retrieve the current signal:
            ThisSignal = (R3BSignal*) ThisCluster->Atp(ksig);
            
            // Reset the distance counter:
            Distance_Counter = 0;
            
            // Next, loop over the primary signals and compare them:
            for (Int_t kprim = 0; kprim<nPrims; ++kprim)
            {
                // Retrieve current primary signal:
                PrimSignal = (R3BSignal*) PrimSignals->At(kprim);
                
                // Compute distance:
                Distance = 0.0;
                Distance = Distance + (PrimSignal->GetPositionX() - ThisSignal->GetPositionX())*(PrimSignal->GetPositionX() - ThisSignal->GetPositionX());
                Distance = Distance + (PrimSignal->GetPositionY() - ThisSignal->GetPositionY())*(PrimSignal->GetPositionY() - ThisSignal->GetPositionY());
                Distance = Distance + (PrimSignal->GetPositionZ() - ThisSignal->GetPositionZ())*(PrimSignal->GetPositionZ() - ThisSignal->GetPositionZ());
                Distance = Distance + ceff*ceff*(PrimSignal->GetTime() - ThisSignal->GetTime())*(PrimSignal->GetTime() - ThisSignal->GetTime());
                Distance = TMath::Sqrt(Distance);
                
                // Count:
                if (Distance<0.1) {Distance_Counter = Distance_Counter + 1;}
            }
            
            // Now, if Distance_Counter==0, then the current signal was not assigned as a primary
            // signal. If it is one, it is a primary signal and if it is larger, we made an error:
            if (Distance_Counter==0)
            {
                // Do nothing.
            }
            else if (Distance_Counter==1)
            {
                // Then, count this signal:
                nPrimHits_InCluster_Exp = nPrimHits_InCluster_Exp + 1;
            }
            else
            {
                cout << "R3BRecoTranslator FATAL ERROR: We linked a cluster signal to more then 1 reconstructed primary hit!\n";
            }
            
            // Close the loop over the cluster:
        }
        
        // Now that we have the number of both Sim & Exp primary hits in the cluster, we can update our matrix.
        // First, prevent that we run out of the index space:
        if (nPrimHits_InCluster_Exp>EffMatrix_nMaxHits) {nPrimHits_InCluster_Exp = EffMatrix_nMaxHits;}
        if (nPrimHits_InCluster_Sim>EffMatrix_nMaxHits) {nPrimHits_InCluster_Sim = EffMatrix_nMaxHits;}
        
        // Then, update:
        if ((nPrimHits_InCluster_Exp>=0)&&(nPrimHits_InCluster_Exp<=EffMatrix_nMaxHits)&&(nPrimHits_InCluster_Sim>=0)&&(nPrimHits_InCluster_Sim<=EffMatrix_nMaxHits))
        {
            EffMatrix[nPrimHits_InCluster_Exp][nPrimHits_InCluster_Sim] = EffMatrix[nPrimHits_InCluster_Exp][nPrimHits_InCluster_Sim] + 1.0;
            EffMatrix[EffMatrix_nMaxHits+1][nPrimHits_InCluster_Sim] = EffMatrix[EffMatrix_nMaxHits+1][nPrimHits_InCluster_Sim] + 1.0;
        }
        
        // Done.
    }
}    

void R3BRecoTranslator::SetExpMarksOnSignals(TClonesArray* &PrimSignals)
{
    // This function duplicates the array of all signals and provides Exp marks on them.
    // Define parameters:
    Int_t nSignals = fArraySignals->GetEntries();
    Int_t nPrims = PrimSignals->GetEntries();
    
    // Declare what we need:
    R3BSignal* ThisSignal;
    R3BSignal* PrimSignal;
    Double_t Distance;
    Int_t Distance_Counter = 0;
    
    // Then, loop over the signals:
    for (Int_t ksig = 0; ksig<nSignals; ++ksig)
    {
        // Retrieve the current signal:
        ThisSignal = (R3BSignal*) fArraySignals->At(ksig);
        
        // Duplicate it into the new array:
        new ((*AllSignals)[ksig]) R3BSignal(ThisSignal);
        
        // Now, retrieve it from the new array:
        ThisSignal = (R3BSignal*) AllSignals->At(ksig);
        
        // Reset the experimental mark (sim-mark is already OK).
        ThisSignal->SetNonPrimaryExp();
        
        // Reset the distance counter:
        Distance_Counter = 0;
        
        // Then, loop over the primary signals & compare:
        for (Int_t kprim = 0; kprim<nPrims; ++kprim)
        {
            // Retrieve current primary signal:
            PrimSignal = (R3BSignal*) PrimSignals->At(kprim);
            
            // Compute distance:
            Distance = 0.0;
            Distance = Distance + (PrimSignal->GetPositionX() - ThisSignal->GetPositionX())*(PrimSignal->GetPositionX() - ThisSignal->GetPositionX());
            Distance = Distance + (PrimSignal->GetPositionY() - ThisSignal->GetPositionY())*(PrimSignal->GetPositionY() - ThisSignal->GetPositionY());
            Distance = Distance + (PrimSignal->GetPositionZ() - ThisSignal->GetPositionZ())*(PrimSignal->GetPositionZ() - ThisSignal->GetPositionZ());
            Distance = Distance + ceff*ceff*(PrimSignal->GetTime() - ThisSignal->GetTime())*(PrimSignal->GetTime() - ThisSignal->GetTime());
            Distance = TMath::Sqrt(Distance);
            
            // Count:
            if (Distance<0.1) {Distance_Counter = Distance_Counter + 1;}
        }
        
        // Now, if Distance_Counter==0, then the current signal was not a primary
        // signal. If it is 1, it is a primary signal and if it is larger, we made an error:
        if (Distance_Counter==0)
        {
            // Do nothing.
        }
        else if (Distance_Counter==1)
        {
            // Then, set the label:
            ThisSignal->SetPrimaryExp();
        }
        else
        {
            cout << "R3BRecoTranslator FATAL ERROR: We linked a signal to more then 1 reconstructed primary hit!\n";
        }
        
        // Done.
    }
}   

void R3BRecoTranslator::DuplicatePrimaryHits(TClonesArray* &PrimSignals)
{
    // Duplicates PrimSignals into TheNeutronHits:
    Int_t nPrims = PrimSignals->GetEntries();
    Int_t nSignals = fArraySignals->GetEntries();
    Int_t nClusters = fArrayClusters->GetEntries();
    R3BSignal* ThisSignal;
    R3BSignal* OneSignal;
    Double_t Distance;
    Int_t Distance_Counter = 0;
    
    // Loop over the primary signals:
    for (Int_t k = 0; k<nPrims; ++k)
    {
        // Retrieve current signal:
        ThisSignal = (R3BSignal*) PrimSignals->At(k);
        
        // Generate the new one:
        new ((*TheNeutronHits)[k]) R3BSignal(ThisSignal);
        
        // Obtain the new signal:
        ThisSignal = (R3BSignal*) TheNeutronHits->At(k);
        
        // Set the proper labels:
        ThisSignal->SetPrimaryExp();
        ThisSignal->SetNonPrimarySim();
        
        // Reset the distance counter:
        Distance_Counter = 0;
        
        // Compare for the simulation label:
        for (Int_t ksig = 0; ksig<nSignals; ++ksig)
        {
            // Retrieve current signal:
            OneSignal = (R3BSignal*) fArraySignals->At(ksig);
            
            // Compute distance:
            Distance = 0.0;
            Distance = Distance + (OneSignal->GetPositionX() - ThisSignal->GetPositionX())*(OneSignal->GetPositionX() - ThisSignal->GetPositionX());
            Distance = Distance + (OneSignal->GetPositionY() - ThisSignal->GetPositionY())*(OneSignal->GetPositionY() - ThisSignal->GetPositionY());
            Distance = Distance + (OneSignal->GetPositionZ() - ThisSignal->GetPositionZ())*(OneSignal->GetPositionZ() - ThisSignal->GetPositionZ());
            Distance = Distance + ceff*ceff*(OneSignal->GetTime() - ThisSignal->GetTime())*(OneSignal->GetTime() - ThisSignal->GetTime());
            Distance = TMath::Sqrt(Distance);
            
            // Count:
            if (Distance<0.1) {Distance_Counter = Distance_Counter + 1;}
        }
        
        if (Distance_Counter==0)
        {
            // Do nothing.
        }
        else if (Distance_Counter==1)
        {
            // Then, set the label:
            ThisSignal->SetPrimarySim();
        }
        else
        {
            cout << "R3BRecoTranslator FATAL ERROR: We linked a prim. signal to more then 1 other signal!\n";
        }
    }
        
    // Next, set the proper experimental labels for the cluster heads:
    Int_t nClusHeads = ClusterHeads->GetEntries();
    
    for (Int_t k = 0; k<nClusHeads; ++k)
    {
        // Retrieve current signal:
        ThisSignal = (R3BSignal*) ClusterHeads->At(k);
        
        // Compare for the exp. label:
        ThisSignal->SetNonPrimaryExp();
        
        // Reset the distance counter:
        Distance_Counter = 0;
        
        for (Int_t kprim = 0; kprim<nPrims; ++kprim)
        {
            // Retrieve current signal:
            OneSignal = (R3BSignal*) PrimSignals->At(kprim);
            
            // Compute distance:
            Distance = 0.0;
            Distance = Distance + (OneSignal->GetPositionX() - ThisSignal->GetPositionX())*(OneSignal->GetPositionX() - ThisSignal->GetPositionX());
            Distance = Distance + (OneSignal->GetPositionY() - ThisSignal->GetPositionY())*(OneSignal->GetPositionY() - ThisSignal->GetPositionY());
            Distance = Distance + (OneSignal->GetPositionZ() - ThisSignal->GetPositionZ())*(OneSignal->GetPositionZ() - ThisSignal->GetPositionZ());
            Distance = Distance + ceff*ceff*(OneSignal->GetTime() - ThisSignal->GetTime())*(OneSignal->GetTime() - ThisSignal->GetTime());
            Distance = TMath::Sqrt(Distance);
            
            // Count:
            if (Distance<0.1) {Distance_Counter = Distance_Counter + 1;}
        }
        
        if (Distance_Counter==0)
        {
            // Do nothing.
        }
        else if (Distance_Counter==1)
        {
            // Then, set the label:
            ThisSignal->SetPrimaryExp();
        }
        else
        {
            cout << "R3BRecoTranslator FATAL ERROR: We linked a clusterhead to more then 1 prim. signal!\n";
        }
    }
    
    // Done.
}     
        
// Definition of Finish-function:
void R3BRecoTranslator::Finish()
{
    // Select the OutputFile:
    TheOutputFile->cd();
    
    // Then, write the efficiency matrices to a .txt-file:
    if (SimulationData_IsAvailable==kTRUE)
    {
        TString FileName = OutputPath + "/Reconstruction_Efficiency_Matrices.txt";
        WriteTextFile = std::ofstream(FileName.Data(), std::ofstream::out);
    
        // Export matrices:
        WriteOneMatrix(EffMatrix_TradMed_Clusters_CutsMult,"Traditional Reconstruction with Multiplicity from TDR Cuts");
        WriteOneMatrix(EffMatrix_TradMed_Clusters_DNNMult,"Traditional Reconstruction with Multiplicity from DNN");
        WriteOneMatrix(EffMatrix_TradMed_Clusters_PerfectMult,"Traditional Reconstruction with MC Detected Multiplicity");
        WriteOneMatrix(EffMatrix_ScoringPlus_Signals_CutsMult,"Scoring+ Signal Reconstruction with Multiplicity from TDR Cuts");
        WriteOneMatrix(EffMatrix_ScoringPlus_Signals_DNNMult,"Scoring+ Signal Reconstruction with Multiplicity from DNN");
        WriteOneMatrix(EffMatrix_ScoringPlus_Signals_PerfectMult,"Scoring+ Signal Reconstruction with MC Detected Multiplicity");
        WriteOneMatrix(EffMatrix_ScoringPlus_Clusters_CutsMult,"Scoring+ Cluster Reconstruction with Multiplicity from TDR Cuts");
        WriteOneMatrix(EffMatrix_ScoringPlus_Clusters_DNNMult,"Scoring+ Cluster Reconstruction with Multiplicity from DNN");
        WriteOneMatrix(EffMatrix_ScoringPlus_Clusters_PerfectMult,"Scoring+ Cluster Reconstruction with MC Detected Multiplicity");
        WriteOneMatrix(EffMatrix_DNNScoringPlus,"Scoring++ (DNN computation) Cluster Reconstruction with Multiplicity from DNN");
        WriteOneMatrix(EffMatrix_DNNScoringPlus_SingleTOF,"Scoring++ (DNN computation) Cluster Rec. with Mult. from DNN & if Mult==1, Take simply smallest TOF cluster.");
        WriteOneMatrix(EffMatrix_DNNScoringPlus_Max,"Scoring++ (Max. possible computation) Cluster Reconstruction with Multiplicity from DNN");
        WriteOneMatrix(EffMatrix_BetaReconstruction_NeuLAND,"NeuLAND Beta Reconstruction with Multiplicity from the same procedure:");
        
        if (UseNEBULA==kTRUE)
        {
            WriteOneMatrix(EffMatrix_BetaReconstruction_NEBULA,"NEBULA Beta Reconstruction with Multiplicity from the same procedure:");
            WriteOneMatrix(EffMatrix_BetaReconstruction_Combined,"COMBINED Beta Reconstruction with Multiplicity from the same procedure:");
        }
        
        WriteOneMatrix(EffMatrix_SingleReconstruction_NeuLAND,"NeuLAND Single Reconstruction with Multiplicity from the same procedure:");
        
        if (UseNEBULA==kTRUE)
        {
            WriteOneMatrix(EffMatrix_SingleReconstruction_NEBULA,"NEBULA Single Reconstruction with Multiplicity from the same procedure:");
            WriteOneMatrix(EffMatrix_SingleReconstruction_Combined,"COMBINED Single Reconstruction with Multiplicity from the same procedure:");
        }
    
        // Close the .txt-file:
        WriteTextFile.close();
    }
}

void R3BRecoTranslator::WriteOneMatrix(Double_t** &EffMatrix, TString const Method_Title)
{
    TString st = "";
    TString kstr = "";
    
    WriteTextFile << "Reconstruction Efficiency Matrix for [" << Method_Title << "]\n";
    WriteTextFile << "        Number of MC simulation marks:\n";
    WriteTextFile << "                ";
    for (Int_t k = 0; k<(EffMatrix_nMaxHits+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << " marks: ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(EffMatrix_nMaxHits+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(EffMatrix_nMaxHits+1); ++kcol)
        {
            if (EffMatrix[EffMatrix_nMaxHits+1][kcol]>0.0)
            {
                EffMatrix[krow][kcol] = EffMatrix[krow][kcol]/EffMatrix[EffMatrix_nMaxHits+1][kcol];
            }
            else
            {
                EffMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Exp. marks: " << kstr << ": ";
            }
            
            WriteTextFile << RoundOff(100.0*EffMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    /*
    for (Int_t kcol = 0; kcol<(EffMatrix_nMaxHits+1); ++kcol)
    {
        if (kcol==0) {WriteTextFile << "Counts:      " << ": ";}
        WriteTextFile << EffMatrix[EffMatrix_nMaxHits+1][kcol] << " ";
    }
    WriteTextFile << "\n";
    */
    
    Double_t Avg = 0.0;
    for (Int_t k = 0; k<(EffMatrix_nMaxHits+1); ++k)
    {
        Avg = Avg + EffMatrix[k][k];
    }
    Avg = Avg/((Int_t) (EffMatrix_nMaxHits+1));
    WriteTextFile << "Average Performance = " << RoundOff(100.0*Avg,4) << "%";
    
    WriteTextFile << "\n\n";
}    

// Generate ROOT dictionary:
ClassImp(R3BRecoTranslator);
