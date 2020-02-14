// Written by C. A. Douma.

// General directives:
#ifndef DNNDATAPOINT_H
#define DNNDATAPOINT_H

// Include C++ headers:
#include <iostream>
#include <fstream>

// Include ROOT headers:
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"

// Include R3B headers:

// Include other headers:

// use standard namespace:
using namespace std;

// Define the class:
class DNNDataPoint
{
public:
    // Constructor (Default):
    DNNDataPoint();
    
    // Destructor (Default):
    virtual ~DNNDataPoint();
    
    // Set-functions:
    void Set_INCLXX_INCLXX(Double_t const num, Double_t const err) {INCLXX_INCLXX_Point = num; INCLXX_INCLXX_Error = err;}
    void Set_INCLXX_INCLXX_2nd(Double_t const num, Double_t const err) {INCLXX_INCLXX_2ndPoint = num; INCLXX_INCLXX_2ndError = err;}
    void Set_INCLXX_BERT(Double_t const num, Double_t const err) {INCLXX_BERT_Point = num; INCLXX_BERT_Error = err;}
    void Set_BERT_INCLXX(Double_t const num, Double_t const err) {BERT_INCLXX_Point = num; BERT_INCLXX_Error = err;}
    void Set_BERT_BERT(Double_t const num, Double_t const err) {BERT_BERT_Point = num; BERT_BERT_Error = err;}
    
    // Get-functions:
    Double_t GetAvgPoint() {return Avg_Point;}
    Double_t GetStatError() {return Stat_Error;}
    Double_t GetPhysError() {return Phys_Error;}
    
    Double_t Get_INCLXX_INCLXX_Value() {return INCLXX_INCLXX_Point;}
    Double_t Get_INCLXX_INCLXX_2ndValue() {return INCLXX_INCLXX_2ndPoint;}
    Double_t Get_INCLXX_BERT_Value() {return INCLXX_BERT_Point;}
    Double_t Get_BERT_INCLXX_Value() {return BERT_INCLXX_Point;}
    Double_t Get_BERT_BERT_Value() {return BERT_BERT_Point;}
    
    Double_t Get_INCLXX_INCLXX_Error() {return INCLXX_INCLXX_Error;}
    Double_t Get_INCLXX_INCLXX_2ndError() {return INCLXX_INCLXX_2ndError;}
    Double_t Get_INCLXX_BERT_Error() {return INCLXX_BERT_Error;}
    Double_t Get_BERT_INCLXX_Error() {return BERT_INCLXX_Error;}
    Double_t Get_BERT_BERT_Error() {return BERT_BERT_Error;}
    
    // Member-functions:
    void ComputeAvgPoint();
                                                   
private:
    // Inputs:
    Double_t INCLXX_INCLXX_Point;
    Double_t INCLXX_INCLXX_2ndPoint;
    Double_t INCLXX_BERT_Point;
    Double_t BERT_INCLXX_Point;
    Double_t BERT_BERT_Point;
    
    Double_t INCLXX_INCLXX_Error;
    Double_t INCLXX_INCLXX_2ndError;
    Double_t INCLXX_BERT_Error;
    Double_t BERT_INCLXX_Error;
    Double_t BERT_BERT_Error;
    
    // Outputs:
    Double_t Avg_Point;
    Double_t Stat_Error;
    Double_t Phys_Error;
    
    // Generate ROOT dictionary:
    ClassDef(DNNDataPoint, 1);
};

#endif
