// General directives:
#ifndef NUCLEI_H
#define NUCLEI_H

// Include C++ headers
#include <iostream>
#include <algorithm>

// include ROOT-headers:
#include "TMath.h"
#include "TString.h"

// Use standard namespaces:
using namespace std;

// Include own headers:
#include "Nucleus.h"

class Nuclei
{
public:
    // Constructor:
    Nuclei();
    
    // Destructor:
    virtual ~Nuclei();
    
    // ============================================================================================================================================
    // Standard Get-functions:

    Int_t GetA(TString const Name);
    Int_t GetZ(TString const Name);
    Int_t GetPDGCode(TString const Name); 
    Double_t GetLifeTime(TString const Name, TString const Unit);
    Double_t GetSp(TString const Name, TString const Unit);
    Double_t GetSn(TString const Name, TString const Unit);
    Double_t GetAbundance(TString const Name, TString const mode);
    Double_t GetBindingEnergy(TString const Name, TString const Unit, TString const mode);
    Double_t GetMass(TString const Name, TString const Unit);
    Double_t GetAtomicMass(TString const Name, TString const Unit);
    TString GetRecoil_DT(TString const input_Name, Int_t const DT);
    Double_t GetIAS(TString const Name_Initial,TString const Name_Final, TString const Unit);
    TString FindNucleusFromPDG(Int_t const PDG);
    TString FindNucleus(Int_t const Z,Int_t const A);
    
    // ============================================================================================================================================
    // Get-functions for obtaining information from the energy levels:
    
    Int_t GetNElevels(TString const Name);
    EnergyLevel GetEnergyLevel(TString const Name, Int_t const Index);
    Int_t GetNE(TString const Name);
    Int_t GetNAngMoms(TString const Name);
    Int_t GetNGammas(TString const Name);
    void SelectElevel(TString const Name, Int_t const Index);
    void SelectGroundState(TString const Name);
    void SelectElevel_WithKey(TString const Name, Int_t const Key);
    void SelectFinalLevel(TString const Name);
    void SelectAngMom(TString const Name, Int_t const Index);
    void SelectGamma(TString const Name, Int_t const Index);
    Int_t GetKey(TString const Name);
    Double_t GetLevelE(TString const Name, TString const Unit);
    Double_t GetLevelLifeTime(TString const Name, TString const Unit);
    Double_t GetBGT(TString const Name);
    Double_t GetBF(TString const Name);
    Double_t GetJ(TString const Name);
    Double_t GetGammaE(TString const Name);
    Double_t GetGammaI(TString const Name);
    Double_t GetGammaEf(TString const Name);
    TString GetRefs(TString const Name);
    TString GetParity(TString const Name);
    TString GetGammaTransitionType(TString const Name);
    Bool_t GetJacc(TString const Name);
    EnergyLevel GetFinalLevel(TString const Name);
    
    // ============================================================================================================================================
    
    // Printing function:
    void Print(Bool_t const PrintGammas, TString const NucName, Int_t const JBoundary);
    
    // ============================================================================================================================================
    
    // Define more complicated Get-functions:
    Double_t GetNaturalMass(Int_t const Z, Int_t const Zab, TString const Unit);
    
    // ============================================================================================================================================
    
    // Define the Database:
    void BuildDataBase();
      
    // ============================================================================================================================================
    
// class content:
private:
    std::vector<Nucleus> TheNuclei;
    
    // Generate ROOT dictionary:
    ClassDef(Nuclei, 1);
};

#endif
