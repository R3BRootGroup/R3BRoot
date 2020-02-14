// FairTask that Just duplicates one branch from an 
// input-file into a new output-file. Used for merging MT files.
#ifndef R3BSCOREAPPLIER_H
#define R3BSCOREAPPLIER_H

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

using namespace std;

class R3BScoreApplier : public FairTask 
{
    public:
        // Constructor:
        R3BScoreApplier();

        // Destructor:
        virtual ~R3BScoreApplier();

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
        Int_t GetPerfectMultiplicity();
        Int_t GetDNNMultiplicity();
        Int_t ApplyCalibrationCuts();        

    private:
        // Class content:
        TClonesArray* fArraySignals;
        TClonesArray* fArrayClusters;
        TClonesArray* fArrayMult;
        
        // Output arrays:
        TClonesArray* fArrayPrims_Signals_CutsMult;
        TClonesArray* fArrayPrims_Signals_DNNMult;
        TClonesArray* fArrayPrims_Signals_PerfectMult;
        TClonesArray* fArrayPrims_Clusters_CutsMult;
        TClonesArray* fArrayPrims_Clusters_DNNMult;
        TClonesArray* fArrayPrims_Clusters_PerfectMult;
        
        // Input parameters:
        Int_t MaxMultiplicity;
        
        // Other quantities:
        Int_t nEvents;
        Int_t EventCounter;
        TFile* TheOutputFile;
        TH1D** SignalTotalScoringResults;
        TH1D** ClusterTotalScoringResults;
        TH1D** SignalAvgScoringResults;
        TH1D** ClusterAvgScoringResults;
        TH1I* PerfectMultHist;
        TH1I* CutsMultHist;
        TH1I* DNNMultHist;
        
        // TDR calibration cuts:
        Bool_t UseCalibrationCuts;
        Double_t* fCuts;
        Double_t fKappa;
        
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        AllScorers* TheScorers;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BScoreApplier, 1);
};

#endif
