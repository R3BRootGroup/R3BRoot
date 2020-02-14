// -----------------------------------------------------------------------------
// -----                      R3B_TradMed_NeutronTracker                          -----
// -----                Created 29-02-2012 by D.Kresan                     -----
// -----            Based on algorithm developed by M.Heil                 -----
// -----------------------------------------------------------------------------

#ifndef R3B_TRADMED_NEUTRONTRACKER_H
#define R3B_TRADMED_NEUTRONTRACKER_H


#include <iostream>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include <map>

#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TVector3.h"
#include "TH1D.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
	
#include "R3BSignal.h"
#include "R3BSignalCluster.h"

#include "R3BInputClass.h"
#include "Nuclei.h"
#include "ObjInteger.h"

class TClonesArray;
class TH1F;
class TH2F;


class R3B_TradMed_NeutronTracker : public FairTask
{
  public:
    // Default constructor:
    R3B_TradMed_NeutronTracker();
  
    // Destructor:
    ~R3B_TradMed_NeutronTracker();

    // Init-function:
    virtual InitStatus Init();
    
    // Exec-function:
    virtual void Exec(Option_t* opt);

    // Finish-function:
    virtual void Finish();
    
    // Reset-function:
    virtual void Reset();
  
    // Parameter inputs before running this code:
    void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
    void SetNevents(Int_t const nn) {nEvents = nn;}
    
    // Functions for the multiplicity determination based on the traditional cuts:
    void ReadCalibrationFile();
    Int_t ApplyCalibrationCuts();
    Int_t PerfectMultiplicity();
    Int_t GetDNNMultiplicity();
    
    // Functions to be used during the actual reconstruction:
    Int_t AdvancedMethod();
    void NextIteration(Int_t curIndex, R3BSignalCluster *curClus);
    void SortClustersBeta();
    Bool_t IsElastic(R3BSignalCluster *c1, R3BSignalCluster *c2);
    
    // Other functions:
    void SetMultiplicityDetermination(TString const Opt) {MultiplicityDetermination = Opt;}

private:
    
    // Core members:
    R3BInputClass* Inputs;
    Nuclei* TheNuclei;
    Int_t nEvents;
    TFile* TheOutputFile;
      
    // Arrays to hold the clusters & primary hits:
    TClonesArray *fArrayCluster;
    TClonesArray* fNeutHits;
    TClonesArray* fArrayMult;

    // Parameters:
    Double_t fTarget_Xpos;
    Double_t fTarget_Ypos;
    Double_t fTarget_Zpos;
    Double_t fBeam_Xpos;
    Double_t fBeam_Ypos;
    Double_t fBeam_Zpos;
    Int_t EventCounter;
    Bool_t UseCalibrationCuts;
    Int_t MaxMultiplicity;
    Double_t beamEnergy;
    Double_t beamBeta;
    TString ParticleType;
    Double_t ParticleMass; // MeV
    Double_t mNeutron; // particle mass in amu
    Double_t cMedia;
    Double_t dio;
    Int_t nBarsTotal;
    TString MultiplicityDetermination;
    
    // Constants of physics:
    Double_t amu; // atomic mass unit in MeV/c**2

    // Control histograms:
    TH1D* IntendedMultiplicity;
    TH1D* FoundMultiplicity;
    
    // Calibration cuts:
    Double_t fKappa;  // Slope of the cuts.
    Double_t* fCuts;  // 1D-array. Index is the position of the cuts.

    // Intermediate values;
    std::vector<R3BSignalCluster*> fVectorCluster;
    Int_t fNofClusters;
    Int_t fNofClusters1;
    Int_t fNofTracks;
    Int_t nNeut;
  
public: 
    ClassDef(R3B_TradMed_NeutronTracker,1)  
};

#endif
