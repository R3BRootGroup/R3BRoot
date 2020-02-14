// FairTask that translates the R3BSignalClusters into a more suitable
// format. The R3BSignalCluster is and std::vector of R3BSignals. Hence,
// it contains ALL information. This step filteres out only the information
// that we need. Moreover, it also produces a .txt-file to serve as
// input for the DNN. This .txt-file will contain the same information
// as the filtered cluster information.
#ifndef R3BCLUSTERTRANSLATOR_H
#define R3BCLUSTERTRANSLATOR_H

// Inclusion of C++ headers:
#include <iostream>
#include <fstream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "R3BSignal.h"
#include "R3BNeulandDigi.h"
#include "R3BSignalCluster.h"
#include "R3BSimpleCluster.h"
#include "R3BInputClass.h"
#include "R3BTextFileGenerator.h"
#include "Nuclei.h"

using namespace std;

class R3BClusterTranslator : public FairTask 
{
    public:
        // Default constructor:
        R3BClusterTranslator();

        // Destructor:
        virtual ~R3BClusterTranslator();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Parameter Member functions:
        void LinkInputsClass(R3BInputClass* inp) {Inputs = inp;}
        void SetnEvents(Int_t const nn) {nEvents = nn;}
        
        // Multithreading:
        void SetTheThreads(Int_t const nT, Int_t const cT);
        
        // In validation mode, we must be able to select between steps 1 & 2:
        void SelectStep2() {ThisIsStep2 = kTRUE;}
        void SelectStep1() {ThisIsStep2 = kFALSE;}
        
    private:
        // Class content:
        TClonesArray** fArraySignals;
        TClonesArray** fArrayDigis;
        TClonesArray** fArraySignalClusters;
        TClonesArray** fArraySimpleClusters;
        Int_t nDigiRuns;
        
        // Control histograms:
        TH1I** Multiplicity;
        TH1I** NumberOfClustersHist;
        TH1D** EnergyDeposition;
        TH1D** AverageSize;
        TH2D*** Calibration;
        
        // Other parameters:
        Int_t MaxMultiplicity;
        Int_t EventCounter;
        Int_t nEvents; // Not by inputs, but by FairTask!
        Double_t Target_T;
        Double_t Target_X;
        Double_t Target_Y;
        Double_t Target_Z;
        Bool_t NoTextFile;
        TString NormalizationMethod;
        Bool_t ValidationMode;
        Bool_t ThisIsStep2;
        TString Step2_Structure;
        Double_t* Detection_Efficiencies;
        Int_t* BiasFrequencies;
        Bool_t UseTextFileBias;
        TString TheOutputPath;
        Bool_t SimulationData_IsAvailable;
        Bool_t ComputeSingleClusterRatio;
        Double_t SingleFraction;
        
        // Multithreading:
        Int_t NumberOfThreads;
        Int_t CurrentThread;
        
        // Auxillary classes:
        TRandom3* Generator;
        R3BInputClass* Inputs;
        R3BTextFileGenerator* FileGen;
        R3BTextFileGenerator** FileGen_Step2;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BClusterTranslator, 1);
};

#endif
