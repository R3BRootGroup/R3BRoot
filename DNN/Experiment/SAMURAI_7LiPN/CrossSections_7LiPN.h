// Class that computes the 7Li(p,n)7Be cross sections from the output of the ExcitationEnergy-Class
// Written by C. A. Douma.

// General directives:
#ifndef CROSSSECTIONS_7LIPN_H
#define CROSSSECTIONS_7LIPN_H

// Include C++ headers:
#include <iostream>
#include <vector>

// Include ROOT headers:
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TMatrixDSym.h"
#include "TRandom3.h"
#include "TGraphErrors.h"

// Include R3B headers:
#include "Excitation_Energy_7LiPN.h"
#include "R3BInputClass.h"
#include "Nuclei.h"

// use standard namespace:
using namespace std;

// Define the class:
class CrossSections_7LiPN
{
public:
    // Constructor (Default):
    CrossSections_7LiPN();
    
    // Destructor (Default):
    virtual ~CrossSections_7LiPN();
    
    // SetFunctions:
    void Select250MeV()     {Experiment_Is250MeV = kTRUE; Experiment_Is110MeV = kFALSE;}
    void Select110MeV()     {Experiment_Is250MeV = kFALSE; Experiment_Is110MeV = kTRUE;}
    void SelectValidation() {Experiment_IsValidation = kTRUE;}
    void SelectExperiment() {Experiment_IsValidation = kFALSE;}
    void SetBranchTitle(TString const Branch)   {BranchTitle = Branch;}
    void SetMCDetectionEfficiency(Double_t const epsMC, Double_t const epsMCerr) {MC_Detection_Efficiency = epsMC; MC_Detection_Efficiency_Error = epsMCerr;}
    void UseComputedEfficiencies() {UseExternalEfficiencies = kFALSE;}
    void UseCustomEfficiencies(TString const ThisFile) {UseExternalEfficiencies = kTRUE; External_EffFile = ThisFile;}
    void SetBandPosition(Double_t const pos) {BandPosition = pos;}
    void SetLogScale() {UseCrossLogScale = kTRUE;}
    void SetLinearScale() {UseCrossLogScale = kFALSE;}
    
    // Get-functions:
    TString GetBranchTitle() {return BranchTitle;}
    
    // Error handling:
    void ErrorMessage(TString const Message);
    void PrintAllErrors();
    void DisableErrorPrinting();
    void EnableErrorPrinting();
    Bool_t ContainsNoErrors();    
    
    // Control functions:
    void LinkInputClass(R3BInputClass* inp);
    void LoadHistograms();
    void FitHistograms();
    void ComputeMCAreas();
    void ResetHistogramVectors();
    void ComputeEfficiencies();
    void ReadEfficiencies();
    void ComputeTheCrossSection();
    void AnalyzeValidation();
    void AnalyzeExperiment();
    
    // Other member functions:
    TH1D* SubtractHists(TString const NewHistName, TH1D* SignalHist, TH1D* BackHist, Double_t const BackScale, Double_t const LowerBoundary, Double_t const UpperBoundary, Bool_t const DoSubtraction);
    TCanvas** DrawHistDoumaArray(Int_t const Nhist, Int_t const Nhor, Int_t const Nvert, TH1D* const Raw[], TString const Title[], TString const xlabel, TString const ylabel, Int_t const color, Double_t const ScaleFactor, Bool_t const GlobalMax);
    TString RoundOff(Double_t const our_input, Int_t const significance);
    TCanvas* DrawGraphDouma(TGraphErrors* const Raw, TString const Title, TString const xlabel, TString const ylabel, Int_t const Color, Double_t const ScaleFactor, Bool_t const UseLogScale, TString const PlotStyle);

    
private:
    // Class content:
    Bool_t Experiment_Is110MeV;
    Bool_t Experiment_Is250MeV;
    Bool_t Experiment_IsValidation;

    // Histograms & Files:
    TString BranchTitle;
    TString SignalFileName;
    TString BackgroundFileName;
    TString OutputPath;
    
    // Computing peak areas:
    std::vector<TH1D*> SignalHists;
    std::vector<TH1D*> BackgroundHists;
    std::vector<TH1D*> SubtractedHists;
    std::vector<Double_t> PeakAreas;
    std::vector<Double_t> PeakAreaErrors;
    
    // Store the perfect peak areas:
    std::vector<Double_t> MonteCarlo_PeakAreas;
    std::vector<Double_t> MonteCarlo_PeakAreaErrors;
    
    // Detection efficiencies:
    Double_t MC_Detection_Efficiency;
    Double_t MC_Detection_Efficiency_Error;
    std::vector<Double_t> Epsilon;
    std::vector<Double_t> Epsilon_Error;
    std::vector<Double_t> Epsilon_Systematic;
    Bool_t UseExternalEfficiencies;
    TString External_EffFile;
    
    // Final cross section:
    TGraphErrors* TheCrossSection;
    TH1D* TheSystErrorBand;
    TH1D* TheSystErrorPlot;
    Double_t BandPosition;
    Bool_t UseCrossLogScale;
    
    // Randomization:
    TRandom3* Generator;
    
    // Error handling:
    Bool_t PrintErrors;    
    std::vector<TString> Errors;   
    
    // Input parameters:
    R3BInputClass* Inputs;
    Bool_t Inputs_IsLinked;
    
    // Nuclear Database:
    Nuclei* TheNuclei;
    
    // Generate ROOT dictionary:
    ClassDef(CrossSections_7LiPN, 1);
};

#endif
