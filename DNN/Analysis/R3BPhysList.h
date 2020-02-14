// FairTask that translates the NeuLAND digis into general signals.
// Each detector should have its own translator, so that the DNN 
// software can be applied to general signals.
// Written by C. A. Douma.
#ifndef R3BPHYSLIST_H
#define R3BPHYSLIST_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH1D.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "FairMCPoint.h"
#include "../../r3bdata/R3BMCTrack.h"
#include "../../r3bdata/neulandData/R3BNeulandPoint.h"
#include "R3BNeulandDigi.h"
#include "R3BInputClass.h"
#include "Nuclei.h"
#include "ObjDouble.h"

using namespace std;

class R3BPhysList : public FairTask 
{
    public:
        // Default constructor:
        R3BPhysList();

        // Destructor:
        virtual ~R3BPhysList();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Apply parameters:
        void LinkInputClass(R3BInputClass* TheseInputs) {Inputs = TheseInputs;}
        void SetNevents(Int_t const nn) {nEvents = nn;}

    private:
        // Class content:
        Int_t nEvents;
        Int_t EventCounter;
        Int_t InteractionCounter;
        Int_t ProtonCounter;
        Int_t DetectionCounter;
        
        // Input/output arrays:
        TClonesArray* fArrayTracks;
        TClonesArray* fArrayPoints;
        TClonesArray* fArrayDigis;
        
        // Histograms:
        TH1D** Hist_Edep;
        TH1D** Hist_AlphaScatter;
        TH1D* Hist_ParticleID;
        TString* ParticleID;
        Int_t Nparticles;
        
        // Inputs:
        Nuclei* TheNuclei;
        R3BInputClass* Inputs;
        TString FileName;
        TFile* TheOutputFile;
        
        // Other:
        ObjDouble* InteractionRate;
        ObjDouble* ProtonRate;
        ObjDouble* DetectionRate;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BPhysList, 1);
};

#endif
