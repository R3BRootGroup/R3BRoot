#ifndef R3B_NEULAND_DIGITIZER_H
#define R3B_NEULAND_DIGITIZER_H

#include <functional>
#include <map>
#include <vector>

#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

#include "FairTask.h"
#include "R3BNeulandDigi.h"
#include "../../neuland/shared/R3BNeulandGeoPar.h" // This has to do with the geometry parameter containers. We cannot change that to a local file!
#include "R3BNeulandPixel.h"
#include "../../r3bdata/neulandData/R3BNeulandPoint.h" // This has to do with the MC datapoints. we cannot change that to a local file!
#include "../../r3bdata/R3BMCTrack.h"
#include "R3BCADNeulandGeoPar.h"
#include "R3BCADNEBULAGeoPar.h"
#include "R3BCADNEBULAVETOGeoPar.h"
#include "R3BCADVETOGeoPar.h"

#include "DigitizingEngine.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

class TGeoNode;
class TH1F;
class TH2F;

/**
 * NeuLAND digitizing finder task
 * @author Jan Mayer
 *
 * For each event, get the R3BNeulandPoints and apply the detector response to create R3BNeulandDigis.
 *   Input:  Branch NeulandPoints = TClonesArray("R3BNeulandPoint")
 *           Stored Neuland Geometry Parameter NeulandGeoPar
 *   Output: Branch NeulandDigis  = TClonesArray("R3BNeulandDigi")
 *   Additional output: Some control histograms
 *
 */

class R3BNeulandCADDigitizer : public FairTask
{
  public:
    R3BNeulandCADDigitizer(const TString input = "NeulandPoints",
                        const TString output = "NeulandDigis",
                        const TString outputpx = "NeulandPixels");
    R3BNeulandCADDigitizer(DigitizingEngine* engine,
                        const TString input = "NeulandPoints",
                        const TString output = "NeulandDigis",
                        const TString outputpx = "NeulandPixels");

    ~R3BNeulandCADDigitizer() = default;

  private:
    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandCADDigitizer(const R3BNeulandCADDigitizer&);            // copy constructor
    R3BNeulandCADDigitizer(R3BNeulandCADDigitizer&&);                 // move constructor
    R3BNeulandCADDigitizer& operator=(const R3BNeulandCADDigitizer&); // copy assignment
    R3BNeulandCADDigitizer& operator=(R3BNeulandCADDigitizer&&);      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;
    void SetParContainers() override;

  public:
    void Exec(Option_t*) override;
    void AddFilter(const std::function<bool(const R3BNeulandDigi*)> f) { fDigiFilters.push_back(f); }
    
    // Member functions to change the parameters:
    void AllowManualParameters() {SetParametersManually = kTRUE;}
    void DisableManualParameters() {SetParametersManually = kFALSE;}
    void SetPaddleHalfLength(Double_t const pval) {fPaddleHalfLength = pval;}
    void SetPMTThreshold(Double_t const pval) {fPMTThresh = pval;};
    void SetSaturationCoefficient(Double_t const pval) {fSaturationCoefficient = pval;};
    void SetTimeRes(Double_t const pval) {fTimeRes = pval;};
    void SetERes(Double_t const pval) {fEResRel = pval;};
    void SetIntegrationTime(Double_t const pval) {fIntegrationTime = pval;};
    void SetcMedium(Double_t const pval) {fcMedium = pval;};
    void SetAttenuation(Double_t const pval) {fAttenuation = pval;};
    void SetLambda(Double_t const pval) {fLambda = pval;};
    void SetBatchMode() {PrintParameters = kFALSE;}
    void SetPrintMode() {PrintParameters = kTRUE;}
    
    // Member function to do everything at once:
    void SetParameters(R3BInputClass* inp);
    void SetNevents(Int_t const nn) {nEvents = nn;}
    
    // Physics list analysis:
    void EliminateParticle(TString const Particle);
    void AllowAllParticles() {ForBiddenTracks.clear(); ForBiddenParticles.clear();}
    void IncludeMotherTracks_InElimination() {IncludeMotherTracks = kTRUE;}
    void ExcludeMotherTracks_FromElimination() {IncludeMotherTracks = kFALSE;}
    
    // Multiple digitizer runs:
    void SetTag(TString const Tag) {OutputNameTag = Tag;}
    
    // Detector choice:
    void SetDetector(TString const Detector);
    
    // Link Inputs class:
    void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}

  private:
    DigitizingEngine* fDigitizingEngine; // owning (sink)

    TString fInput;
    TString fOutput;
    TString fOutputPx;

    TClonesArray* fNeulandPoints;                 // non-owning
    TClonesArray* fArrayTracks;                   // non-owning
    TClonesArray* fNeulandDigis;  // owning
    TClonesArray* fNeulandPixels; // owning

    TString Detector;
    R3BNeulandGeoPar* fNeulandGeoPar; // non-owning
    R3BCADNeulandGeoPar* fCADNeulandGeoPar; // non-owning
    R3BCADVETOGeoPar* fCADVETOGeoPar; // non-owning
    R3BCADNEBULAGeoPar* fCADNEBULAGeoPar; // non-owning
    R3BCADNEBULAVETOGeoPar* fCADNEBULAVETOGeoPar; // non-owning

    Bool_t IsValid(const R3BNeulandDigi*) const;
    std::vector<std::function<bool(const R3BNeulandDigi*)>> fDigiFilters;

    TH1F* hMultOne;
    TH1F* hMultTwo;
    TH1F* hRLTimeToTrig;
    TH2F* hElossVSQDC;
    
    // Digitizer parameters:
    Bool_t SetParametersManually;    // Decides whether we use this or not.
    Bool_t PrintParameters;          // Decides whether we print the parameters during events yes/no (very slow operation!)
    Double_t fPaddleHalfLength;      // [cm]
    Double_t fPMTThresh;             // [MeV]
    Double_t fSaturationCoefficient; //
    Double_t fTimeRes;               // time + Gaus(0., fTimeRes) [ns]
    Double_t fEResRel;               // Gaus(e, fEResRel * e) []
    Double_t fIntegrationTime;       // [ns]
    Double_t fcMedium;               // speed of light in material in [cm/ns]
    Double_t fAttenuation;           // light attenuation of plastic scintillator [1/cm]
    Double_t fLambda;                // decay constant of BC408 scintillator [1/ns]
    Bool_t Total_SetParameters_Worked; // Checks if SetParameters()-function worked yes/no.
    Bool_t IncludeMotherTracks;      // In case you eliminated particles, this decides whether their mother tracks are included yes/no.
    Bool_t EliminateAllFragments;    // Decides whether all fragments should be eliminated yes/no.
    
    // Branch names to decide which detector should be digitized:
    TString MCpoint_BranchName;
    TString ParContainer_BranchName;
    
    // Event counter:
    Int_t gEventCounter;
    Int_t nEvents;
    
    // Input Class:
    R3BInputClass* Inputs;
    Nuclei* TheNuclei;
    TFile* TheOutputFile;
    TString OutputNameTag;
    
    // Forbidded tracks:
    std::vector<Int_t> ForBiddenTracks;
    std::vector<TString> ForBiddenParticles;

  public:
    ClassDefOverride(R3BNeulandCADDigitizer, 0)
};

#endif // R3B_NEULAND_DIGITIZER_H
