// Mimics Jan Mayers Physics List plots.
#ifndef R3BDETECTIONRATE_H
#define R3BDETECTIONRATE_H

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
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TRandom3.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "FairMCPoint.h"
#include "R3BSignal.h"
#include "R3BInputClass.h"
#include "Nuclei.h"
#include "ObjDouble.h"

using namespace std;

class R3BDetectionRate : public FairTask 
{
    public:
        // Default constructor:
        R3BDetectionRate();

        // Destructor:
        virtual ~R3BDetectionRate();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Apply parameters:
        void LinkInputClass(R3BInputClass* TheseInputs) {Inputs = TheseInputs;}
        void SetNevents(Int_t const nn) {nEvents = nn;}

    private:
        // Class content:
        Int_t nEvents;
        Int_t EventCounter;
        
        TRandom3* Generator;
        TClonesArray* fArraySignals;
        Int_t nMeasurementPoints;
        Double_t Estep;
        TString FileName;
        Int_t* RateCounters;
        Int_t NbinsE;
        Double_t Range;
        Double_t NeuLAND_Total_BarWidth;
        Double_t NeuLAND_Total_BarLength;
        Double_t NeuLAND_Distance;
        Bool_t Randomize_BarPositions;
        TH2D* EnergySpread;
        TH1D* PolarSpread;
        Int_t nPaddlesPerPlane;
        
        // Inputs & Masses:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BDetectionRate, 1);
};

#endif
