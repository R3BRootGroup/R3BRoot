// FairTask that translates the outcome of an experiment
// to R3BRoot format (using R3BSignals).
#ifndef R3BEXPTRANSLATOR_TEMPLATE_H
#define R3BEXPTRANSLATOR_TEMPLATE_H

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

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "R3BSignal.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

using namespace std;

class R3BExpTranslator_Template : public FairTask 
{
    public:
        // Default constructor:
        R3BExpTranslator_Template();

        // Destructor:
        virtual ~R3BExpTranslator_Template();

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
        void ConnectToInputFiles();
        Int_t GetNumberOfEvents();
        void UpdateInputs();
        Int_t ReadCurrentEvent();
        Bool_t ConnectLeafs();
        void SetTestMode() {TestMode = kTRUE;}
        void SetRealMode() {TestMode = kFALSE;}

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
        
        // Input tree-leafs (ATTENTION add what you need):
        TLeaf** NL_Hits_nl_bar;
        
        // Input parameters (ATTENTION add what you need):
        Bool_t TestMode;
        TString OutputPath;
        Int_t EventCounter;                 
        Int_t nEvents;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BExpTranslator_Template, 1);
};

#endif
