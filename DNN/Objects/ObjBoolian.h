// Define this header only if it does not yet exist:
#ifndef OBJBOOLIAN_H
#define OBJBOOLIAN_H

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
class ObjBoolian : public TObject
{
  // Declaration of member functions:  
  public:
      
    // Default constructor:  
    ObjBoolian();

    // Name constructor:
    ObjBoolian(TString const Key);
    
    // Content constructor:
    ObjBoolian(TString const Key, Bool_t const num);
    
    // Destructor:
    virtual ~ObjBoolian();

    // Set-functions:
    void SetContent(Bool_t const num);
    void SetTheName(TString const Key);
    
    // Get-functions:
    Bool_t GetContent();
    TString GetTheName();
    
    // Printing function:
    void Print();
    
    // Writing function:
    void WriteToFile(TFile* TheFile);
    
    // Reading function:
    void ReadFromFile(TFile* TheFile);
    
  // Definition of the class content:  
  protected:
    Bool_t TheContent;
    TString ThisName;

  // Calling of the ClassDef-macro to complete the ROOT-inheritance:
  public:
    ClassDef(ObjBoolian, 2)
};

// Ending header definition:
#endif
