// FairTask that Just duplicates one branch from an 
// input-file into a new output-file. Used for merging MT files.
#ifndef R3BDNNSCOREAPPLIER_H
#define R3BDNNSCOREAPPLIER_H

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
#include "TH1D.h"
#include "TH1I.h"

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
#include "R3BTextFileGenerator.h"

using namespace std;

class R3BDNNScoreApplier : public FairTask 
{
    public:
        // Constructor:
        R3BDNNScoreApplier();

        // Destructor:
        virtual ~R3BDNNScoreApplier();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Other member-functions:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        
        // Assisiant functions:
        void ReadCalibrationFile();      

    private:
        // Class content:
        TClonesArray* fArrayClusters;
        TClonesArray* fArrayDNNMult;
        
        // Output arrays:
        TClonesArray* fArrayPrims;
        TClonesArray* fArrayPrims_SingleTOF;
        TClonesArray* fArrayPrims_Max;
        
        // Input parameters:
        Int_t MaxMultiplicity;
        TString OutputPath;
        Int_t NmaxClusters;
        Int_t TotalNumberOfScorers;
        Bool_t SimulationData_IsAvailable;
        TString MultiplicityChoice;
        Int_t nOutputNeurons;
        
        // TextFiles:
        ifstream* DNN_Step2_TextFile;
        
        // Other quantities:
        Int_t nEvents;
        Int_t EventCounter;
        TFile* TheOutputFile;
        std::vector<Int_t> DNN_ClusterBoundaries;

        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        AllScorers* TheScorers;
        R3BTextFileGenerator* FileGen;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BDNNScoreApplier, 1);
};

#endif
