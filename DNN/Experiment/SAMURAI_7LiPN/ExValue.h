// Stores the individual excitation energy per event (& other required things)
#ifndef EXVALUE_H
#define EXVALUE_H

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

class ExValue : public TObject
{
    public:
        // Default Constructor:   
        ExValue();
        
        // Copy constructor:
        ExValue(ExValue* Other);
        
        // Full content constructor:
        ExValue(Double_t const En, Double_t const E, Double_t const A, Double_t const Al, Double_t const f, Double_t const Eprime, Double_t const Edep, Int_t const nClus);
  
        // Destructor:
        virtual ~ExValue();

        // Set-functions:
        void SetENeutron(Double_t const En) {ENeutron = En;}
        void SetEx(Double_t const E) {Ex = E;}
        void SetAplha(Double_t const A) {Alpha = A;}
        void SetAplhaLAB(Double_t const A) {Alpha_LAB = A;}
        void SetPhi(Double_t const f) {Phi = f;}
        void SetEPrime(Double_t const Eprime) {Eder = Eprime;}
        void SetEdepTotal(Double_t const Edep) {EdepTotal = Edep;}
        void SetnClusters(Int_t const nClus) {nClusters = nClus;}
  
        // Get-functions.
        Double_t GetEx() {return Ex;}
        Double_t GetAlpha() {return Alpha;}
        Double_t GetAlphaLAB() {return Alpha_LAB;}
        Double_t GetPhi() {return Phi;}
        Double_t GetENeutron() {return ENeutron;}
        Double_t GetEPrime() {return Eder;}
        Double_t GetEdepTotal() {return EdepTotal;}
        Int_t GetnClusters() {return nClusters;}
        
        // Other member functions:
       
        private:
            // Class content:
            Double_t ENeutron;
            Double_t Ex;
            Double_t Alpha;
            Double_t Alpha_LAB;
            Double_t Phi;
            Double_t Eder;
            Double_t EdepTotal;
            Int_t nClusters;

        public:
            // generate ROOT dictionary:
            ClassDef(ExValue, 2)
};

#endif
