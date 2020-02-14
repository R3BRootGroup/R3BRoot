// FairTask that Just duplicates one branch from an 
// input-file into a new output-file. Used for merging MT files.
#ifndef R3BMERGER_H
#define R3BMERGER_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"

// Add all branches:
#include "FairMCPoint.h"
#include "../../r3bdata/R3BMCTrack.h"
#include "../../r3bdata/neulandData/R3BNeulandPoint.h"
#include "R3BNeulandPixel.h"
#include "R3BNeulandDigi.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"
#include "R3BSimpleCluster.h"

using namespace std;

class R3BMerger : public FairTask 
{
    public:
        // Constructor:
        R3BMerger(Int_t const nBran, TString* const Name, TString* const Title);

        // Destructor:
        virtual ~R3BMerger();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Multiple digitizer runs:
        void SetTag(TString const Tag) {OutputNameTag = Tag;}
        
        // Other member-functions:

    private:
        // Class content:
        TClonesArray** fArrays;
        TString* BranchName;
        TString* BranchTitle;
        Int_t nBranches;
        Int_t EventCounter;
        TFile* TheOutputFile;
        TString OutputNameTag;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BMerger, 1);
};

#endif
