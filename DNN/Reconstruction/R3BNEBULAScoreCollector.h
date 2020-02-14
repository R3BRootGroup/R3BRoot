// FairTask that Just duplicates one branch from an 
// input-file into a new output-file. Used for merging MT files.
#ifndef R3BNEBULASCORECOLLECTOR_H
#define R3BNEBULASCORECOLLECTOR_H

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
#include "AllScorers.h"
#include "Nuclei.h"
#include "R3BInputClass.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"
#include "ObjInteger.h"

using namespace std;

class R3BNEBULAScoreCollector : public FairTask 
{
    public:
        // Constructor:
        R3BNEBULAScoreCollector();

        // Destructor:
        virtual ~R3BNEBULAScoreCollector();

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
        TClonesArray* fArraySignals;
        TClonesArray* fArrayClusters;
        TClonesArray* fArrayNeuMult;
        TClonesArray* fArrayGunMult;
        TClonesArray* fArrayDetMult;
        
        Int_t nEvents;
        Int_t EventCounter;
        Int_t MaxMultiplicity;
        Bool_t UseNEBULA;
        Bool_t SimulationData_IsAvailable;
        
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        AllScorers* TheScorers;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BNEBULAScoreCollector, 1);
};

#endif
