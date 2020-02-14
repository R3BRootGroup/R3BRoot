// Filtered cluster after the translation is done.
#ifndef R3BSIMPLECLUSTER_H
#define R3BSIMPLECLUSTER_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT-headers:
#include "TObject.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"

// Inclusion of R3BRoot headers:
#include "R3BSignal.h"

class R3BSimpleCluster : public TObject
{
    public:
        // Default constructor:
        R3BSimpleCluster();
        
        // Copy-constructor:
        R3BSimpleCluster(R3BSimpleCluster* Cluster);
        
        // Destructor:
        virtual ~R3BSimpleCluster();
        
        // Labelling functions:
        void SetnPrimsSim(Int_t const n) {nPrimaries_Sim = n;}
        void SetnPrimsExp(Int_t const n) {nPrimaries_Exp = n;}
        void AddSimulatedPrimary() {nPrimaries_Sim = nPrimaries_Sim + 1;}
        void AddExperimentalPrimary() {nPrimaries_Exp = nPrimaries_Exp + 1;}
        
        // Other Set-functions:
        void SetClusterEnergy(Double_t const EE) {Total_Energy = EE;}
        void SetClusterSize(Int_t const Size) {nSignals = Size;}
        
        // Function to add a signal to the cluster.
        // NOTE: These functions will remain under
        // developement depending on what the DNN needs!
        void AddSignal(R3BSignal const sig);
        void AddSignalp(R3BSignal* const sig);
        
        // Get-functions:
        Int_t GetPrimariesSim() {return nPrimaries_Sim;}
        Int_t GetPrimariesExp() {return nPrimaries_Exp;}
        Int_t GetClusterSize()  {return nSignals;}
        Double_t GetClusterEnergy() {return Total_Energy;}
        
        // Reset-function:
        void MakeEmpty();
        
    private:
        // Class content:
        
        // Lables for detecting which clusters are primaries:
        Int_t nPrimaries_Sim;
        Int_t nPrimaries_Exp;
        
        // Cluster Content:
        Int_t nSignals;
        Double_t Total_Energy;
    
    public:
        // Generation of the ROOT-dictionary:
        ClassDef(R3BSimpleCluster, 1)
};

#endif
