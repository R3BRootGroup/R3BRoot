// FairTask that will do neutron reconstruction by cutting
// on relativistic beta alone. Can apply VETO conditions and can
// combine NeuLAND and NEBULA results.
#ifndef R3BBETARECONSTRUCTOR_H
#define R3BBETARECONSTRUCTOR_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TVector3.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "R3BSignal.h"
#include "R3BNeulandDigi.h"
#include "R3BSignalCluster.h"
#include "R3BInputClass.h"
#include "Nuclei.h"
#include "AllScorers.h"

using namespace std;

class R3BBetaReconstructor : public FairTask 
{
    public:
        // Default constructor:
        R3BBetaReconstructor();

        // Destructor:
        virtual ~R3BBetaReconstructor();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Parameter Member functions:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        
        // Auxillary functions:
        Int_t GetNEBULAModule(R3BSignalCluster* ThisCluster);
        Int_t GetNEBVETOModule(R3BSignal* ThisSignal);
        
    private:
        // Class content:
        TClonesArray* fArrayNeuLANDClusters;
        TClonesArray* fArrayNEBULAClusters;
        TClonesArray* fArrayNeuLANDVETOSignals;
        TClonesArray* fArrayNEBULAVETOSignals;
        
        TClonesArray* fArrayPrimHits_NeuLAND;
        TClonesArray* fArrayPrimHits_NEBULA;
        TClonesArray* fArrayPrimHits_Combined;
        
        // Input parameters:
        Int_t MaxMultiplicity;
        Bool_t UseNEBULA;
        Bool_t UseVETO;
        Bool_t UseNEBULAVETO;
        TString NeuLAND_VETOCondition;
        TString NEBULA_VETOCondition;
        Double_t Beta_LowerCut;
        Double_t Beta_UpperCut;
        Double_t Target_T;
        Double_t Target_X;
        Double_t Target_Y;
        Double_t Target_Z;
        Double_t ceff;
        Int_t NEBULA_nModules;
        Int_t NEBULA_nBarsPerModules;
        Int_t NEBULA_nVETOBarsPerModule;
        
        // Other parameters:
        Int_t EventCounter;
        Int_t nEvents;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
        AllScorers* TheScorers;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BBetaReconstructor, 1);
};

#endif
