// FairTask that brings the reconstruction back into the signals 
// or the clusters that were originally produced.
#ifndef R3BMULTREADER_H
#define R3BMULTREADER_H

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

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "ObjInteger.h"
#include "R3BSignal.h"
#include "R3BSignalCluster.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

using namespace std;

class R3BMultReader : public FairTask 
{
    public:
        // Default constructor:
        R3BMultReader();

        // Destructor:
        virtual ~R3BMultReader();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Define whether to use simulated data too:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        

    private:
        // Class content:
        TClonesArray* fArrayMults;
        TClonesArray* fArraySignals;
        TClonesArray* fArrayClusters;
        ifstream DNN_Output_TextFile;
        
        // Other variables:
        TString OutputPath;
        Int_t TextFileCounter;
        Int_t EventCounter;                 
        Int_t nEvents;
        Bool_t TextFile_IsThere;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BMultReader, 1);
};

#endif
