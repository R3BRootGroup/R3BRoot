// Written by C. A. Douma.

// General directives:
#ifndef DNNDATAGRAPH_H
#define DNNDATAGRAPH_H

// Include C++ headers:
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

// Include ROOT headers:
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"

// Include R3B headers:

// Include other headers:
#include "DNNDataPoint.h"

// use standard namespace:
using namespace std;

// Define the class:
class DNNDataGraph
{
public:
    // Constructor (Default):
    DNNDataGraph();
    
    // Destructor (Default):
    virtual ~DNNDataGraph();
    
    // Set-functions:
    void AddXpoint(Int_t const Index, Double_t const xx);
    void AddYpoint(Int_t const Index, Double_t const INCL_INCL, Double_t const INCL_INCL_2nd, Double_t const INCL_BERT, Double_t const BERT_INCL, Double_t const BERT_BERT);
    void AddYerror(Int_t const Index, Double_t const INCL_INCL, Double_t const INCL_INCL_2nd, Double_t const INCL_BERT, Double_t const BERT_INCL, Double_t const BERT_BERT);
    void SetErrorHeight(Double_t const hh) {ErrorHeight = hh;}
    
    // Member-functions:
    void ComputeGraph();
    void PrintGraph();
    
    // Get-functions:
    TGraphErrors* GetDataGraph() {return TheData;}
    TGraphErrors* GetPhysGraph() {return PhysError;}
    
private:
    // Points in the Graphs:
    std::vector<Double_t> Xpoints;
    std::vector<DNNDataPoint> Ypoints;
    Double_t ErrorHeight;
    
    // Graphs:
    TGraphErrors* TheData;
    TGraphErrors* PhysError;
    
    // Generate ROOT dictionary:
    ClassDef(DNNDataGraph, 1);
};

#endif
