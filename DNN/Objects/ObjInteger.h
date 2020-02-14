// Define this header only if it does not yet exist:
#ifndef OBJINTEGER_H
#define OBJINTEGER_H

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
class ObjInteger : public TObject
{
  // Declaration of member functions:  
  public:
      
    // Default constructor:  
    ObjInteger();

    // Name constructor:
    ObjInteger(TString const Key);
    
    // Content constructor:
    ObjInteger(TString const Key, Int_t const num);
    
    // Destructor:
    virtual ~ObjInteger();

    // Set-functions:
    void SetNumber(Int_t const num);
    void SetTheName(TString const Key);
    
    // Get-functions:
    Int_t GetNumber();
    TString GetTheName();
    
    // Printing function:
    void Print();
    
    // Writing function:
    void WriteToFile(TFile* TheFile);
    
    // Reading function:
    void ReadFromFile(TFile* TheFile);
    
  // Definition of the class content:  
  protected:
    Int_t TheNumber;
    TString ThisName;

  // Calling of the ClassDef-macro to complete the ROOT-inheritance:
  public:
    ClassDef(ObjInteger, 2)
};

// Ending header definition:
#endif
