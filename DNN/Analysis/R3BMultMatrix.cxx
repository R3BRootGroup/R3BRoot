// Every CXX-file includes its own header file:
#include "R3BMultMatrix.h"

// Give it access to the traditional method multiplicities:
#include "TradMedMultiplicityTest.h"
#include "RoundOffMult.h"
#include "DrawHistDouma_2D_flat.h"
#include "PlotMultMatrix.h"

// Default constructor definition:
R3BMultMatrix::R3BMultMatrix() : FairTask("R3BMultMatrix")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayMCNeutronTracks = new TClonesArray("TLorentzVector");    
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayMult = new TClonesArray("ObjInteger");
    fArrayCluster = new TClonesArray("R3BSignalCluster");
    fArrayNEBULACluster = new TClonesArray("R3BSignalCluster");
    fArrayNeuMult = new TClonesArray("ObjInteger");
    fArrayNEBMult = new TClonesArray("ObjInteger");
    fArrayNEBDetMult = new TClonesArray("ObjInteger");
    fArrayBetaPrimHits_NeuLAND = new TClonesArray("R3BSignal");
    fArrayBetaPrimHits_NEBULA = new TClonesArray("R3BSignal");
    fArrayBetaPrimHits_Combined = new TClonesArray("R3BSignal");
    fArraySinglePrimHits_NeuLAND = new TClonesArray("R3BSignal");
    fArraySinglePrimHits_NEBULA = new TClonesArray("R3BSignal");
    fArraySinglePrimHits_Combined = new TClonesArray("R3BSignal");
    
    // Separation matrices:
    MultMatrix_Max = 0;
    MultMatrix_Max_NEBULA = 0;
    MultMatrix_Max_Combined = 0;
    MultMatrix_TDR_AlignedEvents = 0;
    MultMatrix_TDR_UnFolded = 0;
    MultMatrix_TDR_Folded = 0;
    MultMatrix_TDR_Folded_Restricted = 0;
    MultMatrix_DNN_AlignedEvents = 0;
    MultMatrix_DNN_ElenaEvents = 0;
    MultMatrix_DNN_UnFolded = 0;
    MultMatrix_DNN_Folded = 0;
    MultMatrix_DNN_Folded_Restricted = 0;
    MultMatrix_Beta_Aligned = 0;
    MultMatrix_Beta_UnFolded = 0;
    MultMatrix_Beta_Folded = 0;
    MultMatrix_Beta_Folded_Restricted = 0;
    MultMatrix_Beta_Aligned_NEBULA = 0;
    MultMatrix_Beta_UnFolded_NEBULA = 0;
    MultMatrix_Beta_Folded_NEBULA = 0;
    MultMatrix_Beta_Folded_Restricted_NEBULA = 0;
    MultMatrix_Beta_Aligned_Combined = 0;
    MultMatrix_Beta_UnFolded_Combined = 0;
    MultMatrix_Beta_Folded_Combined = 0;
    MultMatrix_Beta_Folded_Restricted_Combined = 0;
    MultMatrix_Single_Aligned = 0;
    MultMatrix_Single_UnFolded = 0;
    MultMatrix_Single_Folded = 0;
    MultMatrix_Single_Folded_Restricted = 0;
    MultMatrix_Single_Aligned_NEBULA = 0;
    MultMatrix_Single_UnFolded_NEBULA = 0;
    MultMatrix_Single_Folded_NEBULA = 0;
    MultMatrix_Single_Folded_Restricted_NEBULA = 0;
    MultMatrix_Single_Aligned_Combined = 0;
    MultMatrix_Single_UnFolded_Combined = 0;
    MultMatrix_Single_Folded_Combined = 0;
    MultMatrix_Single_Folded_Restricted_Combined = 0;
        
    // Other variables:
    TheOutputPath = "./";
    EventCounter = 0;         
    nEvents = 1;
    Inputs = 0;
    UseNEBULA = kFALSE;
    Include_SingleClusterEvents = kTRUE;
    
    // Different multiplicities:
    BetaNeuLANDMultiplicity = 0;
    BetaNEBULAMultiplicity = 0;
    BetaCombinedMultiplicity = 0;
    SingleNeuLANDMultiplicity = 0;
    SingleNEBULAMultiplicity = 0;
    SingleCombinedMultiplicity = 0;
    GunMultiplicity = 0;
    TDRMultiplicity = 0;
    DNNMultiplicity = 0;
    DetectedMultiplicity = 0;
    NeuLANDMultiplicity = 0;
    NEBULAMultiplicity = 0;
    NEBDetectedMultiplicity = 0;
    
    // TDR calibration cuts:
    fKappa = -0.1;
    fCuts = 0;
    UseCalibrationCuts = kTRUE;
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
}

// Destructor definition:
R3BMultMatrix::~R3BMultMatrix()
{
    // Delete the arrays:
    if(fArrayMult) {fArrayMult->Clear(); delete fArrayMult;}
    if(fArrayCluster) {fArrayCluster->Clear(); delete fArrayCluster;}
    if(fArrayNEBULACluster) {fArrayNEBULACluster->Clear(); delete fArrayNEBULACluster;}
    if(fArrayMCNeutronTracks) {fArrayMCNeutronTracks->Clear(); delete fArrayMCNeutronTracks;}
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if(fArrayNeuMult) {fArrayNeuMult->Clear(); delete fArrayNeuMult;}
    if(fArrayNEBDetMult) {fArrayNEBDetMult->Clear(); delete fArrayNEBDetMult;}
    if(fArrayNEBMult) {fArrayNEBMult->Clear(); delete fArrayNEBMult;}
    if(fArrayBetaPrimHits_NeuLAND) {fArrayBetaPrimHits_NeuLAND->Clear(); delete fArrayBetaPrimHits_NeuLAND;}
    if(fArrayBetaPrimHits_NEBULA) {fArrayBetaPrimHits_NEBULA->Clear(); delete fArrayBetaPrimHits_NEBULA;}
    if(fArrayBetaPrimHits_Combined) {fArrayBetaPrimHits_Combined->Clear(); delete fArrayBetaPrimHits_Combined;}
    if(fArraySinglePrimHits_NeuLAND) {fArraySinglePrimHits_NeuLAND->Clear(); delete fArraySinglePrimHits_NeuLAND;}
    if(fArraySinglePrimHits_NEBULA) {fArraySinglePrimHits_NEBULA->Clear(); delete fArraySinglePrimHits_NEBULA;}
    if(fArraySinglePrimHits_Combined) {fArraySinglePrimHits_Combined->Clear(); delete fArraySinglePrimHits_Combined;}
    
    // delete matrices:
    delete MultMatrix_Max;
    delete MultMatrix_Max_Combined;
    delete MultMatrix_Max_NEBULA;
    delete MultMatrix_TDR_AlignedEvents;
    delete MultMatrix_TDR_UnFolded;
    delete MultMatrix_TDR_Folded;
    delete MultMatrix_TDR_Folded_Restricted;
    delete MultMatrix_DNN_AlignedEvents;
    delete MultMatrix_DNN_ElenaEvents;
    delete MultMatrix_DNN_UnFolded;
    delete MultMatrix_DNN_Folded;
    delete MultMatrix_DNN_Folded_Restricted;
    delete MultMatrix_Beta_Aligned;
    delete MultMatrix_Beta_UnFolded;
    delete MultMatrix_Beta_Folded;
    delete MultMatrix_Beta_Folded_Restricted;
    delete MultMatrix_Beta_Aligned_NEBULA;
    delete MultMatrix_Beta_UnFolded_NEBULA;
    delete MultMatrix_Beta_Folded_NEBULA;
    delete MultMatrix_Beta_Folded_Restricted_NEBULA;
    delete MultMatrix_Beta_Aligned_Combined;
    delete MultMatrix_Beta_UnFolded_Combined;
    delete MultMatrix_Beta_Folded_Combined;
    delete MultMatrix_Beta_Folded_Restricted_Combined;
    delete MultMatrix_Single_Aligned;
    delete MultMatrix_Single_UnFolded;
    delete MultMatrix_Single_Folded;
    delete MultMatrix_Single_Folded_Restricted;
    delete MultMatrix_Single_Aligned_NEBULA;
    delete MultMatrix_Single_UnFolded_NEBULA;
    delete MultMatrix_Single_Folded_NEBULA;
    delete MultMatrix_Single_Folded_Restricted_NEBULA;
    delete MultMatrix_Single_Aligned_Combined;
    delete MultMatrix_Single_UnFolded_Combined;
    delete MultMatrix_Single_Folded_Combined;
    delete MultMatrix_Single_Folded_Restricted_Combined;
    
    // Delete other objects:
    delete TheNuclei;
}

// FairTask initialization function:
InitStatus R3BMultMatrix::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs:
    TheOutputPath = Inputs->GetInputString("TheOutputPath");
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    
    // Retrieve the MC neutron tracks:
    if ((TClonesArray*)ioman->GetObject("MCNeutronTracks") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init No MCNeutronTracks!\n\n";
        return kFATAL;
    }
    fArrayMCNeutronTracks = (TClonesArray*)ioman->GetObject("MCNeutronTracks");
    
    // Retrieve the Signals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init No Signals!\n\n";
        return kFATAL;
    }
    fArraySignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Retrieve the clusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init No Clusters!\n\n";
        return kFATAL;
    }
    fArrayCluster = (TClonesArray*)ioman->GetObject("Clusters");

    // Obtain the DNN multiplicity:
    if ((TClonesArray*)ioman->GetObject("DNN_Multiplicity") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init No DNN Multiplicity!\n\n";
        return kFATAL;
    }
    fArrayMult = (TClonesArray*)ioman->GetObject("DNN_Multiplicity");
    
    // Obtain the NeuLAND multiplicity:
    if ((TClonesArray*)ioman->GetObject("NeuLAND_Multiplicity") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init No NeuLAND_Multiplicity!\n\n";
        return kFATAL;
    }
    fArrayNeuMult = (TClonesArray*)ioman->GetObject("NeuLAND_Multiplicity");
    
    // Obtain NeuLAND Beta reconstruction:
    if ((TClonesArray*)ioman->GetObject("BetaReconstructed_PrimHits_NeuLAND") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init No BetaReconstructed_PrimHits_NeuLAND!\n\n";
        return kFATAL;
    }
    fArrayBetaPrimHits_NeuLAND = (TClonesArray*)ioman->GetObject("BetaReconstructed_PrimHits_NeuLAND");
    
    // Obtain NeuLAND Single reconstruction:
    if ((TClonesArray*)ioman->GetObject("SingleReconstructed_PrimHits_NeuLAND") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultMatrix::Init No SingleReconstructed_PrimHits_NeuLAND!\n\n";
        return kFATAL;
    }
    fArraySinglePrimHits_NeuLAND = (TClonesArray*)ioman->GetObject("SingleReconstructed_PrimHits_NeuLAND");
    
    // Same for NEBULA:
    if (UseNEBULA==kTRUE)
    {
        // Retrieve the clusters:
        if ((TClonesArray*)ioman->GetObject("NEBULAClusters") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No NEBULAClusters!\n\n";
            return kFATAL;
        }
        fArrayNEBULACluster = (TClonesArray*)ioman->GetObject("NEBULAClusters");
        
        if ((TClonesArray*)ioman->GetObject("BetaReconstructed_PrimHits_NEBULA") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No BetaReconstructed_PrimHits_NEBULA!\n\n";
            return kFATAL;
        }
        fArrayBetaPrimHits_NEBULA = (TClonesArray*)ioman->GetObject("BetaReconstructed_PrimHits_NEBULA");
        
        if ((TClonesArray*)ioman->GetObject("BetaReconstructed_PrimHits_Combined") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No BetaReconstructed_PrimHits_Combined!\n\n";
            return kFATAL;
        }
        fArrayBetaPrimHits_Combined = (TClonesArray*)ioman->GetObject("BetaReconstructed_PrimHits_Combined");
        
        if ((TClonesArray*)ioman->GetObject("SingleReconstructed_PrimHits_NEBULA") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No SingleReconstructed_PrimHits_NEBULA!\n\n";
            return kFATAL;
        }
        fArraySinglePrimHits_NEBULA = (TClonesArray*)ioman->GetObject("SingleReconstructed_PrimHits_NEBULA");
        
        if ((TClonesArray*)ioman->GetObject("SingleReconstructed_PrimHits_Combined") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No SingleReconstructed_PrimHits_Combined!\n\n";
            return kFATAL;
        }
        fArraySinglePrimHits_Combined = (TClonesArray*)ioman->GetObject("SingleReconstructed_PrimHits_Combined");
        
        if ((TClonesArray*)ioman->GetObject("NEBULA_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No NEBULA_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayNEBMult = (TClonesArray*)ioman->GetObject("NEBULA_Multiplicity");
        
        if ((TClonesArray*)ioman->GetObject("NEBULA_Detected_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No NEBULA_Detected_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayNEBDetMult = (TClonesArray*)ioman->GetObject("NEBULA_Detected_Multiplicity");
    }
    
    // Generate the neutron separation matrices:
    MultMatrix_Max = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Max_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Max_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_TDR_AlignedEvents = new Double_t*[MaxMultiplicity+2];
    MultMatrix_TDR_UnFolded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_TDR_Folded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_TDR_Folded_Restricted = new Double_t*[MaxMultiplicity+2];
    MultMatrix_DNN_AlignedEvents = new Double_t*[MaxMultiplicity+2];
    MultMatrix_DNN_ElenaEvents = new Double_t*[MaxMultiplicity+2];
    MultMatrix_DNN_UnFolded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_DNN_Folded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_DNN_Folded_Restricted = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Aligned = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_UnFolded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Folded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Folded_Restricted = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Aligned_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_UnFolded_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Folded_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Folded_Restricted_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Aligned_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_UnFolded_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Folded_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Beta_Folded_Restricted_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Aligned = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_UnFolded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Folded = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Folded_Restricted = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Aligned_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_UnFolded_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Folded_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Folded_Restricted_NEBULA = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Aligned_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_UnFolded_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Folded_Combined = new Double_t*[MaxMultiplicity+2];
    MultMatrix_Single_Folded_Restricted_Combined = new Double_t*[MaxMultiplicity+2];
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+2); ++krow)
    {
        MultMatrix_Max[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Max_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Max_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_TDR_AlignedEvents[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_TDR_UnFolded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_TDR_Folded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_TDR_Folded_Restricted[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_DNN_AlignedEvents[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_DNN_ElenaEvents[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_DNN_UnFolded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_DNN_Folded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_DNN_Folded_Restricted[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Aligned[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_UnFolded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Folded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Folded_Restricted[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Aligned_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_UnFolded_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Folded_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Folded_Restricted_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Aligned_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_UnFolded_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Folded_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Beta_Folded_Restricted_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Aligned[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_UnFolded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Folded[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Folded_Restricted[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Aligned_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_UnFolded_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Folded_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Folded_Restricted_NEBULA[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Aligned_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_UnFolded_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Folded_Combined[krow] = new Double_t[MaxMultiplicity+1];
        MultMatrix_Single_Folded_Restricted_Combined[krow] = new Double_t[MaxMultiplicity+1];
        
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            MultMatrix_Max[krow][kcol] = 0.0;
            MultMatrix_Max_Combined[krow][kcol] = 0.0;
            MultMatrix_Max_NEBULA[krow][kcol] = 0.0;
            MultMatrix_TDR_AlignedEvents[krow][kcol] = 0.0;
            MultMatrix_TDR_UnFolded[krow][kcol] = 0.0;
            MultMatrix_TDR_Folded[krow][kcol] = 0.0;
            MultMatrix_TDR_Folded_Restricted[krow][kcol] = 0.0;
            MultMatrix_DNN_AlignedEvents[krow][kcol] = 0.0;
            MultMatrix_DNN_ElenaEvents[krow][kcol] = 0.0;
            MultMatrix_DNN_UnFolded[krow][kcol] = 0.0;
            MultMatrix_DNN_Folded[krow][kcol] = 0.0;
            MultMatrix_DNN_Folded_Restricted[krow][kcol] = 0.0;
            MultMatrix_Beta_Aligned[krow][kcol] = 0.0;
            MultMatrix_Beta_UnFolded[krow][kcol] = 0.0;
            MultMatrix_Beta_Folded[krow][kcol] = 0.0;
            MultMatrix_Beta_Folded_Restricted[krow][kcol] = 0.0;
            MultMatrix_Beta_Aligned_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Beta_UnFolded_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Beta_Folded_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Beta_Folded_Restricted_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Beta_Aligned_Combined[krow][kcol] = 0.0;
            MultMatrix_Beta_UnFolded_Combined[krow][kcol] = 0.0;
            MultMatrix_Beta_Folded_Combined[krow][kcol] = 0.0;
            MultMatrix_Beta_Folded_Restricted_Combined[krow][kcol] = 0.0;
            MultMatrix_Single_Aligned[krow][kcol] = 0.0;
            MultMatrix_Single_UnFolded[krow][kcol] = 0.0;
            MultMatrix_Single_Folded[krow][kcol] = 0.0;
            MultMatrix_Single_Folded_Restricted[krow][kcol] = 0.0;
            MultMatrix_Single_Aligned_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Single_UnFolded_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Single_Folded_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Single_Folded_Restricted_NEBULA[krow][kcol] = 0.0;
            MultMatrix_Single_Aligned_Combined[krow][kcol] = 0.0;
            MultMatrix_Single_UnFolded_Combined[krow][kcol] = 0.0;
            MultMatrix_Single_Folded_Combined[krow][kcol] = 0.0;
            MultMatrix_Single_Folded_Restricted_Combined[krow][kcol] = 0.0;
        }
    }
    
    // The row index is the determined (possibly wrong) multiplicity.
    // the col index is the true mult. The extra row index, is for the totals.
    
    // Obtain the TDR calibration cuts:
    ReadCalibrationFile();

    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BMultMatrix::Exec(Option_t *option)
{
    // Extract all the different multiplicities:
    BetaNeuLANDMultiplicity = fArrayBetaPrimHits_NeuLAND->GetEntries();
    if (UseNEBULA==kTRUE) {BetaNEBULAMultiplicity = fArrayBetaPrimHits_NEBULA->GetEntries();} else {BetaNEBULAMultiplicity = 0;}
    if (UseNEBULA==kTRUE) {BetaCombinedMultiplicity = fArrayBetaPrimHits_Combined->GetEntries();} else {BetaCombinedMultiplicity = 0;}
    
    SingleNeuLANDMultiplicity = fArraySinglePrimHits_NeuLAND->GetEntries();
    if (UseNEBULA==kTRUE) {SingleNEBULAMultiplicity = fArraySinglePrimHits_NEBULA->GetEntries();} else {SingleNEBULAMultiplicity = 0;}
    if (UseNEBULA==kTRUE) {SingleCombinedMultiplicity = fArraySinglePrimHits_Combined->GetEntries();} else {SingleCombinedMultiplicity = 0;}
    
    GunMultiplicity = fArrayMCNeutronTracks->GetEntries();
    TDRMultiplicity = ApplyCalibrationCuts();
    DNNMultiplicity = 0;
    DetectedMultiplicity = 0;
    NeuLANDMultiplicity = 0;
    NEBULAMultiplicity = 0;
    NEBDetectedMultiplicity = 0;
    
    Int_t nSignals = fArraySignals->GetEntries();
    R3BSignal* ThisSignal;
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        if (ThisSignal->IsPrimarySim()==kTRUE) {DetectedMultiplicity = DetectedMultiplicity + 1;}
    }
    
    Int_t nClusters = fArrayCluster->GetEntries();
    Int_t ClusterDetMult = 0;
    R3BSignalCluster* ThisCluster;
    for (Int_t k = 0; k<nClusters; ++k)
    {
        ThisCluster = (R3BSignalCluster*) fArrayCluster->At(k);
        ClusterDetMult = ClusterDetMult + ThisCluster->GetPrimaryMultSim();
    }
    
    if (ClusterDetMult!=DetectedMultiplicity) {cout << "Event: " << EventCounter << " | Cluster & Signal Detected multiplicities were not equal! SERIOUS ERROR!!!\n";}
    
    ObjInteger* ThisObjInt = (ObjInteger*) fArrayMult->At(0);
    DNNMultiplicity = ThisObjInt->GetNumber();
    
    if (UseNEBULA==kTRUE)
    {
        ThisObjInt = (ObjInteger*) fArrayNeuMult->At(0);
        NeuLANDMultiplicity = ThisObjInt->GetNumber();
    
        ThisObjInt = (ObjInteger*) fArrayNEBMult->At(0);
        NEBULAMultiplicity = ThisObjInt->GetNumber();
    
        ThisObjInt = (ObjInteger*) fArrayNEBDetMult->At(0);
        NEBDetectedMultiplicity = ThisObjInt->GetNumber();
    }
    
    // Update all of our matrices:
    UpdatePerfectMatrices();
    UpdateFoldedMatrices();
    UpdateUnFoldedMatrices();
    UpdateAlignedMatrices();
    UpdateElenaMatrices();
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BMultMatrix Log: We processed " << EventCounter << " events.\n";}
    
}
        
// Definition of Finish-function:
void R3BMultMatrix::Finish()
{
    // Print the different neutron separation matrices:
    TString FileName = TheOutputPath + "/Folded_Neutron_Separation_Matrices.txt";
    WriteTextFile = std::ofstream(FileName.Data(), std::ofstream::out);
    
    // Write down the neutron separation matrices:
    WriteMultMatrix(MultMatrix_Max,"NeuLAND Perfect Reconstruction","Particle Gun Multiplicities");
    PlotMultMatrix(MultMatrix_Max,"NeuLAND Perfect Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    
    if (UseNEBULA==kTRUE)
    {
        WriteMultMatrix(MultMatrix_Max_NEBULA,"NEBULA Perfect Reconstruction","Particle Gun Multiplicities");
        WriteMultMatrix(MultMatrix_Max_Combined,"COMBINED Perfect Reconstruction","Particle Gun Multiplicities");
        PlotMultMatrix(MultMatrix_Max_Combined,"COMBINED Perfect Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    }
    
    WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";
    
    WriteMultMatrix(MultMatrix_TDR_AlignedEvents,"NeuLAND TDR Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_TDR_UnFolded,"NeuLAND Unfolded TDR Reconstruction","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_TDR_Folded,"NeuLAND Folded TDR Reconstruction","Particle Gun Multiplicities");
    WriteMultMatrix(MultMatrix_TDR_Folded_Restricted,"NeuLAND Folded & Restricted TDR Reconstruction","Particle Gun Multiplicities");
    
    PlotMultMatrix(MultMatrix_TDR_Folded,"NeuLAND Folded TDR Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    PlotMultMatrix(MultMatrix_TDR_Folded_Restricted,"NeuLAND Folded & Restricted TDR Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    
    WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";
    
    WriteMultMatrix(MultMatrix_DNN_AlignedEvents,"NeuLAND DNN Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_DNN_ElenaEvents,"NeuLAND DNN Rec. Int./Det. Equal Multiplicities","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_DNN_UnFolded,"NeuLAND Unfolded DNN Reconstruction","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_DNN_Folded,"NeuLAND Folded DNN Reconstruction","Particle Gun Multiplicities");
    WriteMultMatrix(MultMatrix_DNN_Folded_Restricted,"NeuLAND Folded & Restricted DNN Reconstruction","Particle Gun Multiplicities");
    PlotMultMatrix(MultMatrix_DNN_Folded,"NeuLAND Folded DNN Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    PlotMultMatrix(MultMatrix_DNN_Folded_Restricted,"NeuLAND Folded & Restricted DNN Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    
    WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";
    
    WriteMultMatrix(MultMatrix_Beta_Aligned,"NeuLAND Beta Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_Beta_UnFolded,"NeuLAND Unfolded Beta Reconstruction","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_Beta_Folded,"NeuLAND Folded Beta Reconstruction","Particle Gun Multiplicities");
    WriteMultMatrix(MultMatrix_Beta_Folded_Restricted,"NeuLAND Folded & Restricted Beta Reconstruction","Particle Gun Multiplicities");
    PlotMultMatrix(MultMatrix_Beta_Folded,"NeuLAND Folded Beta Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    PlotMultMatrix(MultMatrix_Beta_Folded_Restricted,"NeuLAND Folded & Restricted Beta Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    
    if (UseNEBULA==kTRUE)
    {
        WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";
        
        WriteMultMatrix(MultMatrix_Beta_Aligned_NEBULA,"NEBULA Beta Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Beta_UnFolded_NEBULA,"NEBULA Unfolded Beta Reconstruction","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Beta_Folded_NEBULA,"NEBULA Folded Beta Reconstruction","Particle Gun Multiplicities");
        WriteMultMatrix(MultMatrix_Beta_Folded_Restricted_NEBULA,"NEBULA Folded & Restricted Beta Reconstruction","Particle Gun Multiplicities");
        
        WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";

        WriteMultMatrix(MultMatrix_Beta_Aligned_Combined,"COMBINED Beta Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Beta_UnFolded_Combined,"COMBINED Unfolded Beta Reconstruction","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Beta_Folded_Combined,"COMBINED Folded Beta Reconstruction","Particle Gun Multiplicities");
        WriteMultMatrix(MultMatrix_Beta_Folded_Restricted_Combined,"COMBINED Folded & Restricted Beta Reconstruction","Particle Gun Multiplicities");
        PlotMultMatrix(MultMatrix_Beta_Folded_Combined,"COMBINED Folded Beta Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
        PlotMultMatrix(MultMatrix_Beta_Folded_Restricted_Combined,"COMBINED Folded & Restricted Beta Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    }
    
    WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";
    
    WriteMultMatrix(MultMatrix_Single_Aligned,"NeuLAND Single Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_Single_UnFolded,"NeuLAND Unfolded Single Reconstruction","Detected Multiplicities");
    WriteMultMatrix(MultMatrix_Single_Folded,"NeuLAND Folded Single Reconstruction","Particle Gun Multiplicities");
    WriteMultMatrix(MultMatrix_Single_Folded_Restricted,"NeuLAND Folded & Restricted Single Reconstruction","Particle Gun Multiplicities");
    PlotMultMatrix(MultMatrix_Single_Folded,"NeuLAND Folded Single Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    PlotMultMatrix(MultMatrix_Single_Folded_Restricted,"NeuLAND Folded & Restricted Single Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    
    if (UseNEBULA==kTRUE)
    {
        WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";
        
        WriteMultMatrix(MultMatrix_Single_Aligned_NEBULA,"NEBULA Single Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Single_UnFolded_NEBULA,"NEBULA Unfolded Single Reconstruction","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Single_Folded_NEBULA,"NEBULA Folded Single Reconstruction","Particle Gun Multiplicities");
        WriteMultMatrix(MultMatrix_Single_Folded_Restricted_NEBULA,"NEBULA Folded & Restricted Single Reconstruction","Particle Gun Multiplicities");
        
        WriteTextFile << "\n\n----------------------------------------------------------------------------\n\n";

        WriteMultMatrix(MultMatrix_Single_Aligned_Combined,"COMBINED Single Rec. Gun/Det. Equal Multiplicities","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Single_UnFolded_Combined,"COMBINED Unfolded Single Reconstruction","Detected Multiplicities");
        WriteMultMatrix(MultMatrix_Single_Folded_Combined,"COMBINED Folded Single Reconstruction","Particle Gun Multiplicities");
        WriteMultMatrix(MultMatrix_Single_Folded_Restricted_Combined,"COMBINED Folded & Restricted Single Reconstruction","Particle Gun Multiplicities");
        PlotMultMatrix(MultMatrix_Single_Folded_Combined,"COMBINED Folded Single Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
        PlotMultMatrix(MultMatrix_Single_Folded_Restricted_Combined,"COMBINED Folded & Restricted Single Reconstruction","Particle Gun Multiplicities"); // NOTE: Make a plot!
    }
   
    // Finish up:
    WriteTextFile << "\n\nNB: at 1n, the correct percentage in the Folded DNN matrix might be larger then the\n"
                  << "percentage in the max. matrix. This is, because the DNN does not have a 0n\n"
                  << "output neuron (although it is 0 for nSignals==0).\n"
                  << "Hence, Those events are mistaken for 1n events. It is not a bug!\n\n";
     
    WriteTextFile.close();
}

void R3BMultMatrix::WriteMultMatrix(Double_t** ThisMatrix, TString const ThisTitle, TString const IntMult)
{
    // Writes a single matrix:
    TString st = "";
    TString kstr = "";
    
    WriteTextFile << "\n\n";
    WriteTextFile << "Neutron Separation Matrix for " << ThisTitle.Data() << " :\n";
    WriteTextFile << "        " << IntMult.Data() << " :\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (ThisMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                ThisMatrix[krow][kcol] = ThisMatrix[krow][kcol]/ThisMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                ThisMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Det. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*ThisMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    Double_t ThisAvg = 0.0;
    for (Int_t k = 1; k<(MaxMultiplicity+1); ++k)
    {
        ThisAvg = ThisAvg + ThisMatrix[k][k];
    }
    ThisAvg = ThisAvg/((Int_t) MaxMultiplicity);
    WriteTextFile << "Average Performance = " << RoundOff(100.0*ThisAvg,4) << "%";
    
    WriteTextFile << "\n\n";
}

// Include Update member functions:
#include "UpdatePerfectMatrices.h"
#include "UpdateFoldedMatrices.h"
#include "UpdateUnFoldedMatrices.h"
#include "UpdateAlignedMatrices.h"
#include "UpdateElenaMatrices.h"

// Generate ROOT dictionary:
ClassImp(R3BMultMatrix);
