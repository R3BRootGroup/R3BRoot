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

// Inclusion of R3BRoot headers:
#include "FairLogger.h"
#include "ClusteringEngine.h"
#include "FairTask.h"
#include "TCAConnector.h"
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

        // Finishing the task:
        virtual void Finish();
        
        // Other member functions:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        
        // Multiple digitizer runs:
        void SetTag(TString const Tag) {OutputNameTag = Tag;}

    private:
        // Class content.
        Neuland::ClusteringEngine<R3BSignal> fClusteringEngine;      // Cluster-engine to perform the tas.
        TCAInputConnector<R3BSignal> fSignals;                       // Inputs:  the signals.
        TCAOutputConnector<R3BSignalCluster> fClusters;              // Outputs: the clusters.
        Double_t TimeRadius;
        Double_t SpaceRadius;
        Double_t XRadius;
        Double_t YRadius;
        Double_t ZRadius;
        Int_t ClusterCondition_Tag;
        Int_t EventCounter;
        Int_t nEvents;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
        TString OutputNameTag;

    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BHandShakeClusterFinder, 1);
};

#endif
