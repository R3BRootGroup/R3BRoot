// FairTask that brings the reconstruction back into the signals 
// or the clusters that were originally produced.
#ifndef R3BRECOTRANSLATOR_H
#define R3BRECOTRANSLATOR_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TVector3.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"
#include "R3BInputClass.h"
#include "Nuclei.h"
#include "AllScorers.h"

using namespace std;

class R3BRecoTranslator : public FairTask 
{
    public:
        // Default constructor:
        R3BRecoTranslator();

        // Destructor:
        virtual ~R3BRecoTranslator();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Define whether to use simulated data too:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        void IncludeSingleClusterEvents() {Include_SingleClusterEvents = kTRUE;}
        void ExcludeSingleClusterEvents() {Include_SingleClusterEvents = kFALSE;}
        
        // NOTE: The input-file is the reconstruction file
        // and the output file is either the signals or the 
        // clusters.
        
        // Other member functions:
        Bool_t InitializeInputClonesArray(FairRootManager* &ioman, TClonesArray* &TheArray, TString const ArrayName);
        void InitializeEffMatrix(Double_t** &EffMatrix);
        
        void ConvertSignalToTrack(TClonesArray* &Input, TClonesArray* &Output, Bool_t const TestForSimMark);
        void UpdateEfficiencyMatrixClusters(TClonesArray* &PrimSignals, Double_t** &EffMatrix, TString const Detector);
        void UpdateEfficiencyMatrixSignals(TClonesArray* &PrimSignals, Double_t** &EffMatrix);
        void SetExpMarksOnSignals(TClonesArray* &PrimSignals);
        void DuplicatePrimaryHits(TClonesArray* &PrimSignals);
        
        void WriteOneMatrix(Double_t** &EffMatrix, TString const Method_Title);
        TString RoundOff(Double_t const our_input, Int_t const significance);

    private:
        // Class content:
        TClonesArray* fArraySignals;       
        TClonesArray* fArrayClusters;      
        TClonesArray* fArrayNEBULASignals;       
        TClonesArray* fArrayNEBULAClusters;      
        
        // Primary hit classes:
        TClonesArray* fArrayPrimSignals_TradMed_Clusters_CutsMult;
        TClonesArray* fArrayPrimSignals_TradMed_Clusters_DNNMult;
        TClonesArray* fArrayPrimSignals_TradMed_Clusters_PerfectMult;
        TClonesArray* fArrayPrimSignals_ScoringPlus_Signals_CutsMult;
        TClonesArray* fArrayPrimSignals_ScoringPlus_Signals_DNNMult;
        TClonesArray* fArrayPrimSignals_ScoringPlus_Signals_PerfectMult;
        TClonesArray* fArrayPrimSignals_ScoringPlus_Clusters_CutsMult;
        TClonesArray* fArrayPrimSignals_ScoringPlus_Clusters_DNNMult;
        TClonesArray* fArrayPrimSignals_ScoringPlus_Clusters_PerfectMult;
        TClonesArray* fArrayPrimSignals_DNNScoringPlus;
        TClonesArray* fArrayPrimSignals_DNNScoringPlus_SingleTOF;
        TClonesArray* fArrayPrimSignals_DNNScoringPlus_Max;
        TClonesArray* fArrayPrimSignals_BetaReconstruction_NeuLAND;
        TClonesArray* fArrayPrimSignals_BetaReconstruction_NEBULA;
        TClonesArray* fArrayPrimSignals_BetaReconstruction_Combined;
        TClonesArray* fArrayPrimSignals_SingleReconstruction_NeuLAND;
        TClonesArray* fArrayPrimSignals_SingleReconstruction_NEBULA;
        TClonesArray* fArrayPrimSignals_SingleReconstruction_Combined;
        
        // Neutron track classes:
        TClonesArray* fRecoNeutronTracks_TradMed_Clusters_CutsMult;
        TClonesArray* fRecoNeutronTracks_TradMed_Clusters_DNNMult;
        TClonesArray* fRecoNeutronTracks_TradMed_Clusters_PerfectMult;
        TClonesArray* fRecoNeutronTracks_ScoringPlus_Signals_CutsMult;
        TClonesArray* fRecoNeutronTracks_ScoringPlus_Signals_DNNMult;
        TClonesArray* fRecoNeutronTracks_ScoringPlus_Signals_PerfectMult;
        TClonesArray* fRecoNeutronTracks_ScoringPlus_Clusters_CutsMult;
        TClonesArray* fRecoNeutronTracks_ScoringPlus_Clusters_DNNMult;
        TClonesArray* fRecoNeutronTracks_ScoringPlus_Clusters_PerfectMult;
        TClonesArray* fRecoNeutronTracks_DNNScoringPlus;
        TClonesArray* fRecoNeutronTracks_DNNScoringPlus_SingleTOF;
        TClonesArray* fRecoNeutronTracks_DNNScoringPlus_Max;
        TClonesArray* fRecoNeutronTracks_BetaReconstruction_NeuLAND;
        TClonesArray* fRecoNeutronTracks_BetaReconstruction_NEBULA;
        TClonesArray* fRecoNeutronTracks_BetaReconstruction_Combined;
        TClonesArray* fRecoNeutronTracks_SingleReconstruction_NeuLAND;
        TClonesArray* fRecoNeutronTracks_SingleReconstruction_NEBULA;
        TClonesArray* fRecoNeutronTracks_SingleReconstruction_Combined;
        
        // Other outputs:
        TClonesArray* fPerfectNeutronTracks_Signals;
        TClonesArray* fPerfectNeutronTracks_Clusters;
        TClonesArray* AllSignals;
        TClonesArray* ClusterHeads;
        TClonesArray* TheNeutronHits;
        TClonesArray* TheNeutronTracks;
        
        // Control variables:
        Int_t EventCounter;                
        Int_t nEvents;                     
        std::ofstream WriteTextFile;
        
        // Input parameters:
        Bool_t UseNEBULA;
        Bool_t SimulationData_IsAvailable;
        Double_t ceff;                     
        Int_t EffMatrix_nMaxHits;
        Double_t Target_T;
        Double_t Target_X;
        Double_t Target_Y;
        Double_t Target_Z;
        TString ParticleType;
        Double_t ParticleMass;
        TString Multiplicity_Method;
        TString Reconstruction_Method;
        TString OutputPath;
        TString Step2_MultiplicityChoice;
        Bool_t Include_SingleClusterEvents;
        
        // Efficiency matrices:
        Double_t** EffMatrix_TradMed_Clusters_CutsMult;
        Double_t** EffMatrix_TradMed_Clusters_DNNMult;
        Double_t** EffMatrix_TradMed_Clusters_PerfectMult;
        Double_t** EffMatrix_ScoringPlus_Signals_CutsMult;
        Double_t** EffMatrix_ScoringPlus_Signals_DNNMult;
        Double_t** EffMatrix_ScoringPlus_Signals_PerfectMult;
        Double_t** EffMatrix_ScoringPlus_Clusters_CutsMult;
        Double_t** EffMatrix_ScoringPlus_Clusters_DNNMult;
        Double_t** EffMatrix_ScoringPlus_Clusters_PerfectMult;
        Double_t** EffMatrix_DNNScoringPlus;
        Double_t** EffMatrix_DNNScoringPlus_SingleTOF;
        Double_t** EffMatrix_DNNScoringPlus_Max;
        Double_t** EffMatrix_BetaReconstruction_NeuLAND;
        Double_t** EffMatrix_BetaReconstruction_NEBULA;
        Double_t** EffMatrix_BetaReconstruction_Combined;
        Double_t** EffMatrix_SingleReconstruction_NeuLAND;
        Double_t** EffMatrix_SingleReconstruction_NEBULA;
        Double_t** EffMatrix_SingleReconstruction_Combined;
             
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
        AllScorers* TheScorers;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BRecoTranslator, 1);
};

#endif
