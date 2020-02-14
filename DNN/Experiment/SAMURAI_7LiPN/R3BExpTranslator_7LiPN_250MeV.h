// FairTask that translates the outcome of an experiment
// to R3BRoot format (using R3BSignals).
#ifndef R3BEXPTRANSLATOR_7LIPN_250MEV_H
#define R3BEXPTRANSLATOR_7LIPN_250MEV_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TVector3.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "R3BSignal.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

using namespace std;

class R3BExpTranslator_7LiPN_250MeV : public FairTask 
{
    public:
        // Default constructor:
        R3BExpTranslator_7LiPN_250MeV();

        // Destructor:
        virtual ~R3BExpTranslator_7LiPN_250MeV();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Functions to pass input parameters:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        
        // control functions:
        void ConnectToInputFiles(TString const Folder);
        Int_t GetNumberOfEvents();
        void UpdateInputs();
        Int_t ReadCurrentEvent();
        Bool_t ConnectLeafs();
        void SetTestMode() {TestMode = kTRUE;}
        void SetRealMode() {TestMode = kFALSE;}
        void SetBackgroundMode() {BackGroundMode = kTRUE;}
        void SetSignalMode()     {BackGroundMode = kFALSE;}
        

    private:
        
        // Output File information:
        TFile* TheOutputFile;
        TClonesArray* fArraySignals;
        TClonesArray* fArrayVETOSignals;
        TClonesArray* fArrayNEBULASignals;
        TClonesArray* fArrayNEBVETOSignals;
        
        // Input File information:
        Bool_t ConnectionTest;
        std::vector<TString> InputFileNames;
        std::vector<TFile*>  InputFiles;
        std::vector<TTree*>  InputTrees;
        
        // Input tree-leafs (add what you need):
        TLeaf** NLNeutron_fTime;
        TLeaf** NLNeutron_fBar;
        TLeaf** NLNeutron_fMeVee;
        TLeaf** NLNeutron_fPosVec_fX;
        TLeaf** NLNeutron_fPosVec_fY;
        TLeaf** NLNeutron_fPosVec_fZ;
        TLeaf** nlv_id;
        TLeaf** nlv_tof;
        TLeaf** nlv_eloss;
        TLeaf** NEBULAPla_fQAveCal;
        TLeaf** NEBULAPla_fTAveCal;
        TLeaf** NEBULAPla_fLayer;
        TLeaf** NEBULAPla_fSubLayer;
        TLeaf** NEBULAPla_fPosCal;
        TLeaf** NEBULAPla_id;
        
        // Input parameters:
        Bool_t TestMode;
        Bool_t BackGroundMode;
        Bool_t OutputMessages;
        TString OutputPath;
        Int_t EventCounter;                 
        Int_t nEvents;
        Int_t nPaddlesPerPlane; 
        Int_t nDoublePlanes; 
        Double_t NeuLAND_Center_X;
        Double_t NeuLAND_Center_Y; 
        Double_t NeuLAND_Front_Z; 
        Double_t NeuLAND_Rot_X; 
        Double_t NeuLAND_Rot_Y; 
        Double_t NeuLAND_Rot_Z;
        Double_t NeuLAND_TotalBarThicknessXY;
        Double_t NeuLAND_TotalBarThicknessZ;
        Int_t VETO_nBars;
        Double_t VETO_Center_X;
        Double_t VETO_Center_Y; 
        Double_t VETO_Front_Z; 
        Double_t VETO_Rot_X; 
        Double_t VETO_Rot_Y; 
        Double_t VETO_Rot_Z;
        Double_t VETO_TotalBarThicknessXY;
        Double_t VETO_TotalBarThicknessZ;
        Int_t NEBULA_NBars_OnePlane;
        Int_t NEBULA_NModules;
        Int_t NEBULA_DistModules;
        Double_t NEBULA_Center_X;
        Double_t NEBULA_Center_Y; 
        Double_t NEBULA_Front_Z; 
        Double_t NEBULA_Rot_X; 
        Double_t NEBULA_Rot_Y; 
        Double_t NEBULA_Rot_Z;
        Double_t NEBULA_TotalBarThickness;
        Int_t NEBULA_VETO_nBars;
        Double_t NEBULA_VETO_Center_X;
        Double_t NEBULA_VETO_Center_Y; 
        Double_t NEBULA_VETO_Front_Z; 
        Double_t NEBULA_VETO_Rot_X; 
        Double_t NEBULA_VETO_Rot_Y; 
        Double_t NEBULA_VETO_Rot_Z;
        Double_t NEBULA_VETO_TotalBarThicknessXY;
        Double_t NEBULA_VETO_TotalBarThicknessZ;
        Double_t TargetT;
        Double_t TargetX;
        Double_t TargetY;
        Double_t TargetZ;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BExpTranslator_7LiPN_250MeV, 1);
};

#endif
