// Define this header only if it does not yet exist:
#ifndef OBJDOUBLE_H
#define OBJDOUBLE_H

// Include required C++ headers:
#include <iostream>

// Include required ROOT headers:
#include "TObject.h"
#include "TMath.h"
#include "TFile.h"
#include "TString.h"

// Use standard namespace:
using namespace std;

// Class definition:
class ObjDouble : public TObject
{
  // Declaration of member functions:  
  public:
      
    // Default constructor:  
    ObjDouble();

    // Name constructor:
    ObjDouble(TString const Key);
    
    // Content constructor:
    ObjDouble(TString const Key, Double_t const num);
    
    // Destructor:
    virtual ~ObjDouble();

    // Set-functions:
    void SetNumber(Double_t const num);
    void SetTheName(TString const Key);
    
    // Get-functions:
    Double_t GetNumber();
    TString GetTheName();
    
    // Printing function:
    void Print();
    
    // Writing function:
    void WriteToFile(TFile* TheFile);
    
    // Reading function:
    void ReadFromFile(TFile* TheFile);
    
  // Definition of the class content:  
  protected:
    Double_t TheNumber;
    TString ThisName;

  // Calling of the ClassDef-macro to complete the ROOT-inheritance:
  public:
    ClassDef(ObjDouble, 2)
};

// Ending header definition:
#endif
