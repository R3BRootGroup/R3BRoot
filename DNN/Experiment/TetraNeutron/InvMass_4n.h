// FairTask that brings the reconstruction back into the signals 
// or the clusters that were originally produced.
#ifndef INVMASS_4N_H
#define INVMASS_4N_H

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
#include "TH1D.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "R3BInputClass.h"
#include "Nuclei.h"
#include "MassValue.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"

using namespace std;

class InvMass_4n : public FairTask 
{
    public:
        // Default constructor:
        InvMass_4n();

        // Destructor:
        virtual ~InvMass_4n();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Define whether to use simulated data too:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        void SetBranchTitle(TString const title) {BranchTitle = title;}
        
        // Other member functions:
        void LimitToMCMultiplicity() {LimitToMC = kTRUE;}
        void ReleaseMCMultiplicity() {LimitToMC = kFALSE;}

    private:
        // Class content:
        TClonesArray* fArrayMCNeutronTracks; // MC neutron tracks (when available)
        TClonesArray* fArrayNeuLANDSignals;  // Raw NeuLAND data
        TClonesArray* fArrayNeuLANDClusters; // Raw NeuLAND clusters
        TClonesArray* fArrayNeutronTracks;   // The neutron tracks we use for our analysis.
        TClonesArray* fArrayMass;            // The Inv Mass spectrum we want.
        
        // Other variables:
        TString BranchTitle;
        TString OutputPath;
        Int_t EventCounter;                 
        Int_t nEvents;
        Bool_t SimulationData_IsAvailable;
        Bool_t LimitToMC;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
        
        // Histograms:
        TH1D* MassHistogram;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(InvMass_4n, 1);
};

#endif
