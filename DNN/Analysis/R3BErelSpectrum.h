// FairTask that obtains the relative energy spectrum of 
// the neutrons from the data. This specific task uses the
// simulation data to obtain the perfect spectrum; the
// theoretical maximum of what one should be able to
// obtain in case of perfect reconstruction. 
#ifndef R3BERELSPECTRUM_H
#define R3BERELSPECTRUM_H

// Inclusion of C++ headers:
#include <iostream>
#include <vector>

// Inclusion of ROOT headers:
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH1D.h"
#include "TLorentzVector.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "R3BSignal.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

using namespace std;

class R3BErelSpectrum : public FairTask 
{
    public:
        // Default constructor:
        R3BErelSpectrum();

        // Destructor:
        virtual ~R3BErelSpectrum();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);

        // Finishing the task:
        virtual void Finish();
        
        // Member functions to pass parameters:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        
        // Other member functions:
        void SetBranchName(TString const str) {BranchName = str;}

    private:
        // Class content:
        TClonesArray* fArrayLorentzTracks;          // Contains the neutron tracks for which we generate a spectrum.
        TClonesArray* fArrayMCNeutronTracks;        // Contains the Monte Carlo primary neutron tracks.
        TClonesArray* fArrayMCRecoilTracks;         // Contains the Monte Carlo primary non-neutron tracks.
        TClonesArray* fArraySignals;                // Contains the Detected Signals.
        
        // Control variables:
        Int_t EventCounter;                      
        Int_t nEvents;     
        Int_t NbinsSpectrum;
        Double_t EmaxSpectrum;
        TString BranchName;
        
        // Input parameters:
        TString GunType;
        Int_t MaxMultiplicity;                      
        TString FileName;                         
        TString ParticleType_ToBeDetected;          
        Double_t ParticleMass;                      
        TString BeamType;
        Double_t BeamEnergy;    
        Double_t AngularSpread;
        Double_t BeamEnergyDiff;
        TString OutputPath;
        Bool_t IncludeRecoil_ErelSpectrum;
        
        // The spectra:
        TH1D** ErelSpectrum_GunMultiplicity;
        TH1D** ErelSpectrum_DetectedMultiplicity;
        TH1D** ErelSpectrum_ExpMultiplicity;
        Int_t* GunCounters;
        Int_t* GunCounters_Limited;
        Int_t* DetCounters;
        Int_t* DetCounters_Limited;
        Int_t* ExpCounters;
        Int_t* ExpCounters_Limited;
        
        // Auxillary classes:
        Nuclei* TheNuclei;
        R3BInputClass* Inputs;
        TFile* TheOutputFile;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BErelSpectrum, 1);
};

#endif
