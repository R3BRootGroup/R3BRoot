// Class that generates the TextFiles that contain the DNN 
// input and output files.

// General directives:
#ifndef NORMALIZATIONCLASS_H
#define NORMALIZATIONCLASS_H

// Include C++ headers:
#include <iostream>
#include <vector>
#include <algorithm>

// Include ROOT headers:
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1D.h"

// Include R3B headers:
#include "R3BInputClass.h"
#include "AllScorers.h"

// use standard namespace:
using namespace std;

// Structure definition for parameter list:
struct NormUnit
{
    TString Name;
    TString Normalization_Option;
    Double_t Old_MeanShift;
    Double_t Old_ScaleFactor;
    Double_t New_MeanShift;
    Double_t New_ScaleFactor;
    TH1D* ControlHistogram;
};

// Define the class:
class NormalizationClass
{
public:
    // Constructor (Default):
    NormalizationClass();
    
    // Destructor (Default):
    virtual ~NormalizationClass();
    
    // Control functions:
    void AddParameter(TString const Name, TString const Opt, Double_t const OldMean, Double_t const OldScale, Double_t const newMean, Double_t const NewScale);
    void AddBlankPar(TString const Name);
    void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
    void LinkScorers(AllScorers* TheseScorers) {TheScorers = TheseScorers;}
    void AdaptOldScale(TString const Name, Double_t const Value);
    void AdaptNewScale(TString const Name, Double_t const Value);
    void SetOldScale(TString const Name, Double_t const Left, Double_t const Right);
    void SetNewScale(TString const Name, Double_t const Left, Double_t const Right);
    void DefaultNorms(Double_t const NewLeft, Double_t const NewRight);
    void ElenaNorms(Double_t const NewLeft, Double_t const NewRight);
    void BlankDefaultNorms(Double_t const NewLeft, Double_t const NewRight);
    void BlankElenaNorms(Double_t const NewLeft, Double_t const NewRight);
    void WriteNormFile(TString const TheFileName);
    void PerformNormalization() {ExecuteNormalization = kTRUE;}
    void SkipNormalization() {ExecuteNormalization = kFALSE;}
    void ClearParameters() {TheNormParameters.clear();}
    
    Bool_t ReadNormFile(TString const TheFileName);
    
    // User functions:
    Double_t Normalize(TString const Name, Double_t const Value);
    
    // Get-functions:
    NormUnit* GetNormPar(Int_t const Index);
    Int_t GetNparameters() {return TheNormParameters.size();}
    
    // Data driven times:
    void SetDataDrivenTimes(Double_t const mean, Double_t const width) {DataDriven_MeanTime = mean; DataDriven_ScaleTime = TMath::Abs(width);}
    
private:
    // Class content:
    R3BInputClass* Inputs;
    Bool_t ExecuteNormalization;
    std::vector<NormUnit> TheNormParameters; // list of normalization parameters.
    AllScorers* TheScorers;
    Double_t DataDriven_MeanTime;
    Double_t DataDriven_ScaleTime;
    
    // Generate ROOT dictionary:
    ClassDef(NormalizationClass, 1);
};

#endif
