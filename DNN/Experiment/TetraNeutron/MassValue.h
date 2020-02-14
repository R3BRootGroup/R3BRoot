// Stores the individual excitation energy per event (& other required things)
#ifndef MASSVALUE_H
#define MASSVALUE_H

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

class MassValue : public TObject
{
    public:
        // Default Constructor:   
        MassValue();
        
        // Copy constructor:
        MassValue(MassValue* Other);
        
        // Full content constructor:
        MassValue(Double_t const m);
  
        // Destructor:
        virtual ~MassValue();

        // Set-functions:
        void SetMass(Double_t const m) {TheMass = m;}
        
        // Get-functions.
        Double_t GetMass() {return TheMass;}
        
        // Other member functions:
       
        private:
            // Class content:
            Double_t TheMass;

        public:
            // generate ROOT dictionary:
            ClassDef(MassValue, 1)
};

#endif
