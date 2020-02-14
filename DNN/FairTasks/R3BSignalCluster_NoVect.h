// Vector of R3B-signals. To be used with the handshake
// clsuetrfinder on general signals.
#ifndef R3BSIGNALCLUSTER_NOVECT_H
#define R3BSIGNALCLUSTER_NOVECT_H

// Inclusion of C++ headers:
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <stdexcept>

// Inclusion of ROOT-headers:
#include "TVector3.h"
#include "TObject.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"

// Inclusion of R3BRoot headers:
#include "R3BSignal.h"

class R3BSignalCluster_NoVect : public TObject
{
    public:
        // Default constructor:
        R3BSignalCluster_NoVect();
        
        // Constructor to reserve memory from the beginning:
        R3BSignalCluster_NoVect(Int_t const MaxSize);
        
        // Destructor:
        virtual ~R3BSignalCluster_NoVect();
        
        // Memory Management:
        void AllocateMemory(Int_t const Size);
        void FreeMemory();
        
        // Get-functions:
        Int_t GetPrimaryMultSim();
        Int_t GetPrimaryMultExp();
        Int_t GetSize();
        Double_t GetEnergy();
        R3BSignal At(Int_t const k);
        R3BSignal* Atp(Int_t const k);
        R3BSignal* ObtainPointer(R3BSignal const sig);
        
        R3BSignal FindFirstSignal();
        void StartPosition(TVector3 &pos);
        Double_t GetStartT();
        Double_t GetStartX();
        Double_t GetStartY();
        Double_t GetStartZ();
        
        R3BSignal FindLastSignal();
        void StopPosition(TVector3 &pos);
        Double_t GetStopT();
        Double_t GetStopX();
        Double_t GetStopY();
        Double_t GetStopZ();
        
        R3BSignal FindClosestSignal(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetClosestT(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetClosestX(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetClosestY(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetClosestZ(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        
        R3BSignal FindFarestSignal(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetFarestT(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetFarestX(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetFarestY(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        Double_t GetFarestZ(Double_t const RotX, Double_t const RotY, Double_t const RotZ);
        
        // Control functions:
        void SetSignal(Int_t const k, R3BSignal const sig);
        void SetSignalp(Int_t const k, R3BSignal* const sig);
        void SortClusterTOF();
        
        // Marking-functions:
        void Mark() {TradMed_Mark = kTRUE;}
        void UnMark() {TradMed_Mark = kFALSE;}
        Bool_t IsMarked() {return TradMed_Mark;}
        
    private:
        // Class content:
        R3BSignal* TheArray;
        Int_t ArraySize;
        Bool_t TradMed_Mark;
    
    public:
        // Generation of the ROOT-dictionary:
        ClassDef(R3BSignalCluster_NoVect, 1)
};

#endif
