// FairTask that brings the reconstruction back into the signals 
// or the clusters that were originally produced.
#ifndef R3BMULTMATRIX_H
#define R3BMULTMATRIX_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TH2D.h"
#include "TPaveText.h"
#include "TSystem.h"
#include "TROOT.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "ObjInteger.h"
#include "R3BInputClass.h"
#include "Nuclei.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"
#include "ObjInteger.h"

using namespace std;

class R3BMultMatrix : public FairTask 
{
    public:
        // Default constructor:
        R3BMultMatrix();

        // Destructor:
        virtual ~R3BMultMatrix();

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
        
        // Update functions:
        void UpdatePerfectMatrices();
        void UpdateFoldedMatrices();
        void UpdateUnFoldedMatrices();
        void UpdateAlignedMatrices();
        void UpdateElenaMatrices();
        
        // Other functions:
        void ReadCalibrationFile();
        Int_t ApplyCalibrationCuts();
        TString RoundOff(Double_t const our_input, Int_t const significance);
        void WriteMultMatrix(Double_t** ThisMatrix, TString const ThisTitle, TString const IntMult);
        void PlotMultMatrix(Double_t** ThisMatrix, TString const ThisTitle, TString const IntMult);
        TCanvas* DrawHistDouma_2D_flat(TH2D* const Raw, TString const Title, TString const xlabel, TString const ylabel, TString const zlabel);
        
    private:
        // TClonesArrays to obtain multiplicities:
        TClonesArray* fArrayMCNeutronTracks;
        TClonesArray* fArraySignals;
        TClonesArray* fArrayMult;
        TClonesArray* fArrayCluster;
        TClonesArray* fArrayNEBULACluster;
        TClonesArray* fArrayNeuMult;
        TClonesArray* fArrayNEBMult;
        TClonesArray* fArrayNEBDetMult;
        TClonesArray* fArrayBetaPrimHits_NeuLAND;
        TClonesArray* fArrayBetaPrimHits_NEBULA;
        TClonesArray* fArrayBetaPrimHits_Combined;
        TClonesArray* fArraySinglePrimHits_NeuLAND;
        TClonesArray* fArraySinglePrimHits_NEBULA;
        TClonesArray* fArraySinglePrimHits_Combined;
        
        // Separation matrices:
        Double_t** MultMatrix_Max;
        Double_t** MultMatrix_Max_NEBULA;
        Double_t** MultMatrix_Max_Combined;
        Double_t** MultMatrix_TDR_AlignedEvents;
        Double_t** MultMatrix_TDR_UnFolded;
        Double_t** MultMatrix_TDR_Folded;
        Double_t** MultMatrix_TDR_Folded_Restricted;
        Double_t** MultMatrix_DNN_AlignedEvents;
        Double_t** MultMatrix_DNN_ElenaEvents;
        Double_t** MultMatrix_DNN_UnFolded;
        Double_t** MultMatrix_DNN_Folded;
        Double_t** MultMatrix_DNN_Folded_Restricted;
        Double_t** MultMatrix_Beta_Aligned;
        Double_t** MultMatrix_Beta_UnFolded;
        Double_t** MultMatrix_Beta_Folded;
        Double_t** MultMatrix_Beta_Folded_Restricted;
        Double_t** MultMatrix_Beta_Aligned_NEBULA;
        Double_t** MultMatrix_Beta_UnFolded_NEBULA;
        Double_t** MultMatrix_Beta_Folded_NEBULA;
        Double_t** MultMatrix_Beta_Folded_Restricted_NEBULA;
        Double_t** MultMatrix_Beta_Aligned_Combined;
        Double_t** MultMatrix_Beta_UnFolded_Combined;
        Double_t** MultMatrix_Beta_Folded_Combined;
        Double_t** MultMatrix_Beta_Folded_Restricted_Combined;
        Double_t** MultMatrix_Single_Aligned;
        Double_t** MultMatrix_Single_UnFolded;
        Double_t** MultMatrix_Single_Folded;
        Double_t** MultMatrix_Single_Folded_Restricted;
        Double_t** MultMatrix_Single_Aligned_NEBULA;
        Double_t** MultMatrix_Single_UnFolded_NEBULA;
        Double_t** MultMatrix_Single_Folded_NEBULA;
        Double_t** MultMatrix_Single_Folded_Restricted_NEBULA;
        Double_t** MultMatrix_Single_Aligned_Combined;
        Double_t** MultMatrix_Single_UnFolded_Combined;
        Double_t** MultMatrix_Single_Folded_Combined;
        Double_t** MultMatrix_Single_Folded_Restricted_Combined;
        
        // calibration cuts:
        Double_t fKappa;
        Double_t* fCuts;
        Bool_t UseCalibrationCuts;
        
        // Different multiplicities:
        Int_t BetaNeuLANDMultiplicity;
        Int_t BetaNEBULAMultiplicity;
        Int_t BetaCombinedMultiplicity;
        Int_t SingleNeuLANDMultiplicity;
        Int_t SingleNEBULAMultiplicity;
        Int_t SingleCombinedMultiplicity;
        Int_t GunMultiplicity;
        Int_t TDRMultiplicity;
        Int_t DNNMultiplicity;
        Int_t DetectedMultiplicity;
        Int_t NeuLANDMultiplicity;
        Int_t NEBULAMultiplicity;
        Int_t NEBDetectedMultiplicity;
        
        // Other variables:
        TString TheOutputPath;
        Int_t MaxMultiplicity;
        Bool_t UseNEBULA;
        Int_t EventCounter;                 
        Int_t nEvents;                      
        std::ofstream WriteTextFile;
        Bool_t Include_SingleClusterEvents;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BMultMatrix, 1);
};

#endif
