// Class that generates the TextFiles that contain the DNN 
// input and output files.

// General directives:
#ifndef HISTMERGER_H
#define HISTMERGER_H

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

// Include R3B headers:

// use standard namespace:
using namespace std;

// Define the class:
class HistMerger
{
public:
    // Constructor (Default):
    HistMerger();
    
    // Destructor (Default):
    virtual ~HistMerger();
    
    // Set functions:
    void AddFile(TString const ThisFile) {InputFiles.push_back(ThisFile);}
    void SetOutputFile(TString const ThisFile) {OutputFile = ThisFile;}
    void AddHistogram(TString const ThisHist) {HistNames.push_back(ThisHist);}
    void ClearFiles() {InputFiles.clear();}
    void ClearHistograms() {HistNames.clear();}
    
    // Control Functions:
    void LoadHistograms();
    void AddHistograms();
    void WriteHistograms();
    void MergeHistograms();
    
private:
    // Class content:
    std::vector<TString> InputFiles;
    TFile** InFiles;
    TString OutputFile;
    
    std::vector<std::vector<TH1D*>> TheHistograms; // NOTE: first index is file, second one is histogram.
    std::vector<TString> HistNames;
    
    std::vector<TH1D*> OutputHistograms;
    
    // Generate ROOT dictionary:
    ClassDef(HistMerger, 1);
};

#endif
