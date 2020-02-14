// FairTask that Just duplicates one branch from an 
// input-file into a new output-file. Used for merging MT files.
#ifndef R3BNEUTRONCONVERTOR_H
#define R3BNEUTRONCONVERTOR_H

// Inclusion of C++ headers:
#include <iostream>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TVector3.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"

// Add all branches:
#include "R3BNeulandCADCluster.h"
#include "R3BNeulandCADNeutron.h"
#include "R3BNeulandCADHit.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

using namespace std;

class R3BNeutronConvertor : public FairTask 
{
    public:
        // Constructor:
        R3BNeutronConvertor();

        // Destructor:
        virtual ~R3BNeutronConvertor();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Other member-functions:
        void SetNevents(Int_t const nn) {nEvents = nn;}
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        TVector3 ObtainPixel(Double_t const xpoint_c, Double_t const ypoint_c, Double_t const zpoint_c);
        
        // Multiple digitizer runs:
        void SetTag(TString const Tag) {OutputNameTag = Tag;}

    private:
        // Class content:
        TClonesArray* fArrayClusters;
        TClonesArray* fArrayNeutrons;
        TFile* TheOutputFile;
        Int_t nEvents;
        Int_t EventCounter;
        
        // Input parameters:
        Int_t nPaddlesPerPlane;
        Int_t nDoublePlanes;
        Double_t NeuLAND_Center_X;
        Double_t NeuLAND_Center_Y;
        Double_t NeuLAND_Front_Z;
        Double_t NeuLAND_Rot_X;
        Double_t NeuLAND_Rot_Y;
        Double_t NeuLAND_Rot_Z;
        Double_t NeuLAND_Active_Bar_Thickness;
        Double_t NeuLAND_Total_Bar_Length;
        Double_t NeuLAND_TotalBarThicknessXY;
        Double_t NeuLAND_TotalBarThicknessZ;
        
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TString OutputNameTag;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BNeutronConvertor, 1);
};

#endif
