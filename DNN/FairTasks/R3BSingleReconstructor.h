// FairTask that will do neutron reconstruction by cutting
// on relativistic beta alone. Can apply VETO conditions and can
// combine NeuLAND and NEBULA results.
#ifndef R3BSINGLERECONSTRUCTOR_H
#define R3BSINGLERECONSTRUCTOR_H

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
#include "R3BInputClass.h"
#include "Nuclei.h"

using namespace std;

class R3BSingleReconstructor : public FairTask 
{
    public:
        // Default constructor:
        R3BSingleReconstructor();

        // Destructor:
        virtual ~R3BSingleReconstructor();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Parameter Member functions:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        
    private:
        // Class content:
        TClonesArray* fArrayNeuLANDSignals;
        TClonesArray* fArrayNEBULASignals;
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
        
        // Other parameters:
        Int_t EventCounter;
        Int_t nEvents;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BSingleReconstructor, 1);
};

#endif
