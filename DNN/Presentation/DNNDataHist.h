// Written by C. A. Douma.

// General directives:
#ifndef DNNDATAHIST_H
#define DNNDATAHIST_H

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
#include "TH1D.h"

// Include R3B headers:

// Include other headers:
#include "DNNDataPoint.h"

// use standard namespace:
using namespace std;

// Define the class:
class DNNDataHist
{
public:
    // Constructor (Default):
    DNNDataHist();
    
    // Destructor (Default):
    virtual ~DNNDataHist();
    
    // Set-functions:
    void SetPath(TString const str) {ThePath = str;}
    void SetFileName(TString const str) {FileName = str;}
    void SetHistName(TString const str) {HistName = str;}
    void SetPhysHeight(Double_t const pos);
    
    // Scale functions:
    void Scale_INCL_INCL(Double_t const Scale) {if (Scale>0.0) {INCL_INCL_Scale = Scale;}}
    void Scale_INCL_INCL_2nd(Double_t const Scale) {if (Scale>0.0) {INCL_INCL_2nd_Scale = Scale;}}
    void Scale_INCL_BERT(Double_t const Scale) {if (Scale>0.0) {INCL_BERT_Scale = Scale;}}
    void Scale_BERT_INCL(Double_t const Scale) {if (Scale>0.0) {BERT_INCL_Scale = Scale;}}
    void Scale_BERT_BERT(Double_t const Scale) {if (Scale>0.0) {BERT_BERT_Scale = Scale;}}
    
    // Member-functions:
    void CollectPrimeHists();
    void ComputeNewHists();
    void RemoveEndBins();
    
    // Get-functions:
    TH1D* GetDataHist() {return DataHist;}
    TH1D* GetStatHist() {return StatHist;}
    TH1D* GetPhysHist() {return PhysHist;}
    Double_t GetPrimeHistContent(TString const Type);
    Double_t GetDataHistMax();
    
    TH1D* Get_INCL_INCL_Hist() {return Hist_INCL_INCL;}
    TH1D* Get_INCL_INCL_2nd_Hist() {return Hist_INCL_INCL_2nd;}
    TH1D* Get_INCL_BERT_Hist() {return Hist_INCL_BERT;}
    TH1D* Get_BERT_INCL_Hist() {return Hist_BERT_INCL;}
    TH1D* Get_BERT_BERT_Hist() {return Hist_BERT_BERT;}
    
private:
    // Locations:
    TString ThePath;
    TString FileName;
    TString HistName;
    Double_t PhysHeight;
    
    // Prime hists:
    TH1D* Hist_INCL_INCL;
    TH1D* Hist_INCL_INCL_2nd;
    TH1D* Hist_INCL_BERT;
    TH1D* Hist_BERT_INCL;
    TH1D* Hist_BERT_BERT;
    
    // Scales:
    Double_t INCL_INCL_Scale;
    Double_t INCL_INCL_2nd_Scale;
    Double_t INCL_BERT_Scale;
    Double_t BERT_INCL_Scale;
    Double_t BERT_BERT_Scale;
    
    // Computed Hists:
    TH1D* DataHist;
    TH1D* StatHist;
    TH1D* PhysHist;
    
    // Generate ROOT dictionary:
    ClassDef(DNNDataHist, 1);
};

#endif
