// FairTask that Just duplicates one branch from an 
// input-file into a new output-file. Used for merging MT files.
#ifndef R3BPHYSLISTMERGER_H
#define R3BPHYSLISTMERGER_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"

// Add all branches:
#include "FairMCPoint.h"
#include "../../r3bdata/R3BMCTrack.h"
#include "../../r3bdata/neulandData/R3BNeulandPoint.h"
#include "R3BCADNeulandPoint.h"
#include "R3BCADNEBULAPoint.h"
#include "R3BCADVETOPoint.h"
#include "R3BCADNEBULAVETOPoint.h"

using namespace std;

class R3BPhysListMerger : public FairTask 
{
    public:
        // Constructor:
        R3BPhysListMerger();

        // Destructor:
        virtual ~R3BPhysListMerger();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Other member-functions:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}

    private:
        // Class content:
        TClonesArray** fArray_MCTrack;
        TClonesArray** fArray_NeulandPoints;
        TClonesArray** fArray_PrimNeutronPoints;
        TClonesArray** fArray_NEBULAPoints;
        TClonesArray** fArray_VETOPoints;
        TClonesArray** fArray_NEBVETOPoints;
        
        Int_t EventCounter;
        Int_t nEvents;
        Int_t nPhysicsLists;
        
        Bool_t NeuLAND_IsOriginal;
        Bool_t UseVETO;
        Bool_t UseNEBULA;
        Bool_t UseNEBVETO;
        
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BPhysListMerger, 1);
};

#endif
