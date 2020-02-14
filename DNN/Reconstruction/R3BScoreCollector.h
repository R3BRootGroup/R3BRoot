// FairTask that Just duplicates one branch from an 
// input-file into a new output-file. Used for merging MT files.
#ifndef R3BSCORECOLLECTOR_H
#define R3BSCORECOLLECTOR_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH2D.h"

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

class R3BScoreCollector : public FairTask 
{
    public:
        // Constructor:
        R3BScoreCollector();

        // Destructor:
        virtual ~R3BScoreCollector();

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
        TH2D** Calibration;
        TH2D* Calibration_Total;
        Int_t MaxMultiplicity;
        Bool_t SimulationData_IsAvailable;
        
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        AllScorers* TheScorers;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BScoreCollector, 1);
};

#endif
