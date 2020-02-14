// General signal class. Each R3B digi or other detector signal 
// may be translated to this class so that it can then be
// processed in the same way as NeuLAND digis.
#ifndef R3BSIGNAL_H
#define R3BSIGNAL_H

// Include C++ headers:
#include <iostream>

// Include ROOT headers:
#include "TObject.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"

// Include R3B headers:

class R3BSignal : public TObject
{
    public:
        // Default Constructor:   
        R3BSignal();
        
        // Copy constructor:
        R3BSignal(R3BSignal* Other);
        
        // Full content constructor:
        R3BSignal(Double_t const xx, Double_t const yy, Double_t const zz, Double_t const tt, Double_t const EE, Bool_t const SimPrim, Bool_t const ExpPrim, Int_t const Cryst);
        
        // Constructor for the creation of experimental primaries:
        R3BSignal(Double_t const xx, Double_t const yy, Double_t const zz, Double_t const tt);
  
        // Destructor:
        virtual ~R3BSignal();

        // Set-functions:
        void SetPositionX(Double_t const pos) {Position_X = pos;}
        void SetPositionY(Double_t const pos) {Position_Y = pos;}
        void SetPositionZ(Double_t const pos) {Position_Z = pos;}
        void SetTime(Double_t const t) {Time = t;}
        void SetEnergy(Double_t const E) {Energy = E;}
        void SetCrystalIndex(Int_t const n) {CrystalIndex = n;}
        void SetPrimarySim() {IsPrimary_Simulation = kTRUE;}
        void SetNonPrimarySim() {IsPrimary_Simulation = kFALSE;}
        void SetPrimaryExp() {IsPrimary_Experimental = kTRUE;}
        void SetNonPrimaryExp() {IsPrimary_Experimental = kFALSE;}
  
        // Get-functions. Must be func() const to perserve compatibility to clusterengine.
        Double_t GetPositionX() const {return Position_X;}
        Double_t GetPositionY() const {return Position_Y;}
        Double_t GetPositionZ() const {return Position_Z;}
        Double_t GetTime() const {return Time;}
        Double_t GetEnergy() const {return Energy;}
        Bool_t IsPrimarySim() const {return IsPrimary_Simulation;}
        Bool_t IsPrimaryExp() const {return IsPrimary_Experimental;}
        Int_t GetCrystalIndex() const {return CrystalIndex;}
        
        // Other member functions:
        Bool_t IsHorPaddle(TString const TheCase, Int_t const nPaddlesPerPlane);
        Int_t GetCurrentPlane(TString const TheCase, Int_t const nPaddlesPerPlane);
        Double_t GetBeta(Double_t const Target_T, Double_t const Target_X, Double_t const Target_Y, Double_t const Target_Z);
        void Duplicate(R3BSignal* Other);
  
        private:
            // Class content:
            Double_t Position_X;
            Double_t Position_Y;
            Double_t Position_Z;
            Double_t Time;
            Double_t Energy;
            Bool_t   IsPrimary_Simulation;
            Bool_t   IsPrimary_Experimental;
            Int_t    CrystalIndex;

        public:
            // generate ROOT dictionary:
            ClassDef(R3BSignal, 1)
};

#endif
