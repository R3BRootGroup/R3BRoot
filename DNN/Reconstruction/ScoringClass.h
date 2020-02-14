// General Scoring Class for the neutron reconstruction.
#ifndef SCORINGCLASS_H
#define SCORINGCLASS_H

// Include C++ headers:
#include <iostream>

// Include ROOT headers:
#include "TObject.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TH1D.h"

// Include R3B headers:

using namespace std;

class ScoringClass : public TObject
{
    public:
        // Default Constructor:   
        ScoringClass();
  
        // Destructor:
        virtual ~ScoringClass();

        // Set-functions:
        void SetName(TString const Name);
        void SetKey(TString const ThisKey);
        void AssignOutputFile(TFile* f);
        void UpdateScorer(Double_t const Value, Bool_t const IsPrimary);
        void MarkScorerForClusters();
        void MarkScorerForSignals();
        void SetPlotLogScale();
        void SetPlotLinearScale();
        
        // Get-functions:
        TString GetName();
        TFile* GetOutputFile();
        Double_t GetHistScore(Double_t const Value, Bool_t const IsPrimary);
        Double_t GetScoreDivision(Double_t const Value);
        Bool_t IsMarkedAsCluster();
        Bool_t IsPlotLogScale();
        
        // Control-functions:
        void GenerateSpectrum(Int_t const Nbins, Double_t const Xmin, Double_t const Xmax);
        void WriteScorer();
        void ReadScorer();
        
        // Error handling:
        void ErrorMessage(TString const Message);
        void PrintAllErrors();
        void DisableErrorPrinting();
        void EnableErrorPrinting();
        Bool_t ContainsNoErrors();
        
        // Plotting functions:
        TCanvas* DrawHistDouma(TH1D* const Raw, TString const Title, TString const xlabel, TString const ylabel, Int_t const Color, Bool_t const UseLogScale);
        TCanvas* PlotScorer();
        void SetScaleFactor(Double_t const scale);
        
        // Normalization:
        Double_t GetMeanShift();
        Double_t GetScaleFactor();
  
        private:
            // Class content:
            TString Key;
            TString ScorerName;
            TH1D* FullSpectrum;
            TH1D* PrimarySpectrum;
            TFile* OutputFile;
            Bool_t IsCluster;
            Bool_t Plot_UseLogScale;
            
            // Error handling:
            std::vector<TString> Errors;
            Bool_t PrintErrors;
            
            // Plotting:
            Double_t ScaleFactor;

        public:
            // generate ROOT dictionary:
            ClassDef(ScoringClass, 1)
};

#endif
