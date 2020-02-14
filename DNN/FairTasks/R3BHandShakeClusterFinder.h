// Dervied from Jan Mayers NeuLAND Clusterfinder. It uses the same
// Clustering engine and the same principles. However, this task
// Is written to be compatible with the R3BSignal-Class.
#ifndef R3BHANDSHAKECLUSTERFINDER_H
#define R3BHANDSHAKECLUSTERFINDER_H

// Inclusion of C++ headers:
#include <iostream>
#include <vector>
#include <algorithm>

// Inclusion of ROOT-headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH1D.h"

// Inclusion of R3BRoot headers:
#include "FairLogger.h"
#include "ClusteringEngine.h"
#include "FairTask.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

// Class definition:
class R3BHandShakeClusterFinder : public FairTask
{
    public:
        // Default constructor:
        R3BHandShakeClusterFinder();

        // Destructor:
        virtual ~R3BHandShakeClusterFinder();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);
        
        // Tasks to be done AFTER writing the event:
        virtual void FinishEvent();

        // Finishing the task:
        virtual void Finish();
        
        // Other member functions:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        void SetMT(Int_t const nT, Int_t const cT);
        
        // Multiple digitizer runs:
        void SetTag(TString const Tag) {OutputNameTag = Tag;}
        
        // Set detector:
        void SetDetector(TString const det) {ThisDetector = det;}

    private:
        // Class content.
        Neuland::ClusteringEngine<R3BSignal*> fClusteringEngine;     
        TClonesArray* fArraySignals;                   
        TClonesArray* fArrayClusters;
        Double_t TimeRadius;
        Double_t SpaceRadius;
        Double_t XRadius;
        Double_t YRadius;
        Double_t ZRadius;
        Int_t ClusterCondition_Tag;
        Int_t EventCounter;
        Int_t nEvents;
        Bool_t ComputeSingleClusterRatio;
        Int_t SingleMultCounter;
        Int_t SingleClusterCounter;
        TString TheOutputPath;
        Int_t TotalNumberOfThreads;
        Int_t CurrentThread;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
        TString OutputNameTag;
        TString ThisDetector;
        TString ClusterCondition;
        
        // Control histograms:
        TH1D* Signal_Time;
        TH1D* Signal_Energy;
        TH1D* Signal_X;
        TH1D* Signal_Y;
        TH1D* Signal_Z;
        
        TH1D* Cluster_Time;
        TH1D* Cluster_Energy;
        TH1D* Cluster_X;
        TH1D* Cluster_Y;
        TH1D* Cluster_Z;
        
        TH1D* Cluster_Numbers;
        TH1D* Cluster_Size;
        

    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BHandShakeClusterFinder, 1);
};

#endif
