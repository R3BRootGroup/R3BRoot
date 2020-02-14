// Class that computes the Traditional-Method neutron reconstruction.
// Written by C. A. Douma.

// General directives:
#ifndef R3B_DNN_PLOTS_H
#define R3B_DNN_PLOTS_H

// Include C++ headers:
#include <iostream>
#include <fstream>

// Include ROOT headers:
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TPaveText.h"
#include "TGraphErrors.h"

// Include R3B headers:
#include "R3BInputClass.h"

// use standard namespace:
using namespace std;

// Define the class:
class R3B_DNN_Plots
{
public:
    // Constructor (Default):
    R3B_DNN_Plots();
    
    // Destructor (Default):
    virtual ~R3B_DNN_Plots();
    
    // Control functions:
    void LinkInputClass(R3BInputClass* inp);    // Links this class to the inputs class.
    void CheckFiles();                          // Checks for all files whether they exist and if so, links to them.
    
    // Plotting algorithms:
    void ErelSpect(Int_t const Multiplicity, TString const CompMethod);
    void EdepHists(TString const NewFileName, Bool_t const ThisLogScale);
    void ScatterHists(TString const NewFileName, Bool_t const ThisLogScale);
    TCanvas* OneScatterHist(TString const NewFileName, Bool_t const ThisLogScale, TString const particle, Int_t const Rebinning);
    TCanvas* OneEdepHist(TString const NewFileName, Bool_t const ThisLogScale, TString const particle, Int_t const Rebinning);
    
    // Error handling:
    void ErrorMessage(TString const Message);
    void PrintAllErrors();
    void DisableErrorPrinting();
    void EnableErrorPrinting();
    Bool_t ContainsNoErrors();
    
    // Parameters:
    void SetScaleFactor(Double_t const scale) {if (scale>0.0) {ScaleFactor = scale;}}
    void SetOutputPath(TString const out) {OutPutPath = out;}
    void SetLogScaleMinimum(Double_t const min) {if (min>0.0) {LogScaleMinimum = min;}}
    
    // Set Tags:
    void Include_MC_Spectrum() {Include_ErelSpectrum_MCData = kTRUE;}
    void Include_PerfectSignal_Spectrum() {Include_ErelSpectrum_Perfect_Signals = kTRUE;}
    void Include_PerfectCluster_Spectrum() {Include_ErelSpectrum_Perfect_Clusters = kTRUE;}
    void Include_TradMed_CutsMult_Spectrum() {Include_ErelSpectrum_TradMed_Clusters_CutsMult = kTRUE;}
    void Include_TradMed_DNNMult_Spectrum() {Include_ErelSpectrum_TradMed_Clusters_DNNMult = kTRUE;}
    void Include_TradMed_PerfectMult_Spectrum() {Include_ErelSpectrum_TradMed_Clusters_PerfectMult = kTRUE;}
    void Include_SignalScoringPlus_CutsMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Signals_CutsMult = kTRUE;}
    void Include_SignalScoringPlus_DNNMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Signals_DNNMult = kTRUE;}
    void Include_SignalScoringPlus_PerfectMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Signals_PerfectMult = kTRUE;}
    void Include_ScoringPlus_CutsMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Clusters_CutsMult = kTRUE;}
    void Include_ScoringPlus_DNNMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Clusters_DNNMult = kTRUE;}
    void Include_ScoringPlus_PerfectMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Clusters_PerfectMult = kTRUE;}
    void Include_DNNScoringPlus() {Include_ErelSpectrum_DNNScoringPlus = kTRUE;}
    void Include_DNNScoringPlus_Max() {Include_ErelSpectrum_DNNScoringPlus_Max = kTRUE;}
    void Include_BetaReconstruction_NeuLAND() {Include_ErelSpectrum_BetaReconstruction_NeuLAND = kTRUE;}
    void Include_BetaReconstruction_NEBULA() {Include_ErelSpectrum_BetaReconstruction_NEBULA = kTRUE;}
    void Include_BetaReconstruction_Combined() {Include_ErelSpectrum_BetaReconstruction_Combined = kTRUE;}
    
    void Exclude_MC_Spectrum() {Include_ErelSpectrum_MCData = kFALSE;}
    void Exclude_PerfectSignal_Spectrum() {Include_ErelSpectrum_Perfect_Signals = kFALSE;}
    void Exclude_PerfectCluster_Spectrum() {Include_ErelSpectrum_Perfect_Clusters = kFALSE;}
    void Exclude_TradMed_CutsMult_Spectrum() {Include_ErelSpectrum_TradMed_Clusters_CutsMult = kFALSE;}
    void Exclude_TradMed_DNNMult_Spectrum() {Include_ErelSpectrum_TradMed_Clusters_DNNMult = kFALSE;}
    void Exclude_TradMed_PerfectMult_Spectrum() {Include_ErelSpectrum_TradMed_Clusters_PerfectMult = kFALSE;}
    void Exclude_SignalScoringPlus_CutsMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Signals_CutsMult = kFALSE;}
    void Exclude_SignalScoringPlus_DNNMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Signals_DNNMult = kFALSE;}
    void Exclude_SignalScoringPlus_PerfectMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Signals_PerfectMult = kFALSE;}
    void Exclude_ScoringPlus_CutsMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Clusters_CutsMult = kFALSE;}
    void Exclude_ScoringPlus_DNNMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Clusters_DNNMult = kFALSE;}
    void Exclude_ScoringPlus_PerfectMult_Spectrum() {Include_ErelSpectrum_ScoringPlus_Clusters_PerfectMult = kFALSE;}
    void Exclude_DNNScoringPlus() {Include_ErelSpectrum_DNNScoringPlus = kFALSE;}
    void Exclude_DNNScoringPlus_Max() {Include_ErelSpectrum_DNNScoringPlus_Max = kFALSE;}
    void Exclude_BetaReconstruction_NeuLAND() {Include_ErelSpectrum_BetaReconstruction_NeuLAND = kFALSE;}
    void Exclude_BetaReconstruction_NEBULA() {Include_ErelSpectrum_BetaReconstruction_NEBULA = kFALSE;}
    void Exclude_BetaReconstruction_Combined() {Include_ErelSpectrum_BetaReconstruction_Combined = kFALSE;}
    
    // Other functions:
    void PutErrorsToZero(TH1D* hist);
    
    TCanvas* DrawHistDouma(TH1D* const Raw, TString const Title, TString const xlabel, TString const ylabel, Int_t const Color, Bool_t const UseLogScale);
    TCanvas* DrawGraphDouma(TGraph* const Raw, TString const Title, TString const xlabel, TString const ylabel, Int_t const Color, Bool_t const UseLogScale, Double_t const Ymini, Double_t const Ymaxi);
    
    void CompareHist(TString const File1, TString const File2, TString const HistName, TString const Xlabel, TString const Ylabel, Int_t const Binning, Bool_t const UseLogScale);
    void CompareGraph(TString const File1, TString const File2, TString const GraphName, TString const Xlabel, TString const Ylabel, Bool_t const UseLogScale, Double_t const Ymini, Double_t const Ymaxi);
    void Compare3Graphs(TString const File1, TString const File2, TString const File3, TString const GraphName, TString const Xlabel, TString const Ylabel, Bool_t const UseLogScale, Double_t const Ymini, Double_t const Ymaxi);
    
    void Plot2Hists(TString const File, TString const HistName1, TString const HistName2, TString const Xlabel, TString const Ylabel, Int_t const Binning, Bool_t const UseLogScale);
    void Plot2Graphs(TString const File, TString const GraphName1, TString const GraphName2, TString const Xlabel, TString const Ylabel, Bool_t const UseLogScale, Double_t const Ymini, Double_t const Ymaxi);
    
    void Plot3HistsSum(TString const File, TString const HistName1, TString const HistName2, TString const HistName3, TString const Xlabel, TString const Ylabel, Int_t const Binning, Bool_t const UseLogScale);
    
    void AddErelPlot(TCanvas* ThePlot, TString const HistName, TString const LegendaText, Int_t const TheColor, Double_t const Xmin, Double_t const Xmax, Double_t const Ymin, Double_t const Ymax, Double_t& LegendaHeight, Bool_t const UseLogScale);
    
    Bool_t FindFirstPlot(TH1D* &FirstPlot, TString const HistName);
    
private:
    // Class content:
    TString OutPutPath;
    Double_t ScaleFactor;
    Double_t LogScaleMinimum;
    Bool_t UseNEBULA;
    
    // Links to the relevant output files:
    TFile* MCFile;          Bool_t MCFile_Exists;
    TFile* ParFile;         Bool_t ParFile_Exists;
    TFile* VisFile;         Bool_t VisFile_Exists;
    TFile* DigiFile;        Bool_t DigiFile_Exists;
    TFile* JanFile;         Bool_t JanFile_Exists;
    TFile* SignalFile;      Bool_t SignalFile_Exists;
    TFile* ClusterFile;     Bool_t ClusterFile_Exists;
    TFile* FilteredFile;    Bool_t FilteredFile_Exists;
    TFile* TradMedFile;     Bool_t TradMedFile_Exists;
    TFile* ScoreFile;       Bool_t ScoreFile_Exists;
    TFile* DistFile;        Bool_t DistFile_Exists;
    TFile* DNNFile;         Bool_t DNNFile_Exists;
    TFile* BetaFile;        Bool_t BetaFile_Exists;
    TFile* FinalFile;       Bool_t FinalFile_Exists;
    
    ifstream DNNTextFile;   Bool_t DNNTextFile_Exists;
    ifstream InputsFile;    Bool_t InputsFile_Exists;
    ifstream CutsFile;      Bool_t CutsFile_Exists;
    
    // Tags for including a plot yes/no:
    Bool_t Include_ErelSpectrum_MCData;
    Bool_t Include_ErelSpectrum_Perfect_Signals;
    Bool_t Include_ErelSpectrum_Perfect_Clusters;
    Bool_t Include_ErelSpectrum_TradMed_Clusters_CutsMult;
    Bool_t Include_ErelSpectrum_TradMed_Clusters_DNNMult;
    Bool_t Include_ErelSpectrum_TradMed_Clusters_PerfectMult;
    Bool_t Include_ErelSpectrum_ScoringPlus_Signals_CutsMult;
    Bool_t Include_ErelSpectrum_ScoringPlus_Signals_DNNMult;
    Bool_t Include_ErelSpectrum_ScoringPlus_Signals_PerfectMult;
    Bool_t Include_ErelSpectrum_ScoringPlus_Clusters_CutsMult;
    Bool_t Include_ErelSpectrum_ScoringPlus_Clusters_DNNMult;
    Bool_t Include_ErelSpectrum_ScoringPlus_Clusters_PerfectMult;
    Bool_t Include_ErelSpectrum_DNNScoringPlus;
    Bool_t Include_ErelSpectrum_DNNScoringPlus_Max;
    Bool_t Include_ErelSpectrum_BetaReconstruction_NeuLAND;
    Bool_t Include_ErelSpectrum_BetaReconstruction_NEBULA;
    Bool_t Include_ErelSpectrum_BetaReconstruction_Combined;
    
    // Error handling:
    Bool_t PrintErrors;                     // Decides whether errors are printed on-the-fly yes/no.
    std::vector<TString> Errors;            // Error log.
    
    // Input parameters:
    R3BInputClass* Inputs;
    
    // Generate ROOT dictionary:
    ClassDef(R3B_DNN_Plots, 1);
};

#endif
