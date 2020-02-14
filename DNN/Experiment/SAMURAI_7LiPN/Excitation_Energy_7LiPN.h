// FairTask that brings the reconstruction back into the signals 
// or the clusters that were originally produced.
#ifndef EXCITATION_ENERGY_7LIPN_H
#define EXCITATION_ENERGY_7LIPN_H

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
#include "ExValue.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"

using namespace std;

class Excitation_Energy_7LiPN : public FairTask 
{
    public:
        // Default constructor:
        Excitation_Energy_7LiPN();

        // Destructor:
        virtual ~Excitation_Energy_7LiPN();

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
        Double_t GetEx(TLorentzVector* const Track, Int_t const Option);
        void AdjustNeutronTrack(TLorentzVector* Track, Double_t NewEkin);
        void LimitToMCMultiplicity() {LimitToMC = kTRUE;}
        void ReleaseMCMultiplicity() {LimitToMC = kFALSE;}
        
        // Get-functions:
        Int_t GetNScatterBins() {return NScatterBins;}
        Double_t GetScatterStep() {return ScatterStep;}

    private:
        // Class content:
        TClonesArray* fArrayNeuLANDSignals;
        TClonesArray* fArrayNeuLANDClusters;
        TClonesArray* fArrayNeuLANDVETOSignals;
        TClonesArray* fArrayNeutronTracks;
        TClonesArray* fArrayEx;
        TClonesArray* fArrayEx_MultipleClusters;
        TClonesArray* fArrayMCNeutronTracks;
        
        // Other variables:
        TString NeuLAND_VETOCondition;
        TString BranchTitle;
        TString OutputPath;
        Bool_t UseVETO;
        Int_t EventCounter;                 
        Int_t nEvents;
        Double_t AvgBeamEnergy;
        Bool_t SimulationData_IsAvailable;
        Bool_t LimitToMC;
        Double_t Lower_EdepCut;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
        
        // Histograms:
        Int_t NScatterBins;
        Double_t ScatterStep;
        Int_t NExBins;
        TH1D** ExHistograms;
        TH1D** ExHistograms_MultClus;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(Excitation_Energy_7LiPN, 1);
};

#endif
