// General directives:
#ifndef NUCLEUS_H
#define NUCLEUS_H

// Include C++ headers
#include <iostream>
#include <algorithm>

// include ROOT-headers:
#include "TMath.h"
#include "TString.h"

// Use standard namespaces:
using namespace std;

// Include own headers:
#include "Constants.h"
#include "EnergyLevel.h"

class Nucleus
{
public:
    // Constructor:
    Nucleus();
    
    // Destructor:
    virtual ~Nucleus();
    
    // ===============================================================================================================
    
    // Get-functions:
    TString GetName();
    Int_t GetA();
    Int_t GetZ(); 
    Double_t GetLifeTime(TString const Unit);
    Double_t GetAbundance(TString const mode);
    Double_t GetBindingEnergy(TString const mode, TString const Unit);
    Double_t GetSp(TString const Unit);
    Double_t GetSn(TString const Unit);
    Double_t GetMass(TString const mode, TString const Unit);
    Int_t GetPDGCode();
        
    // ===============================================================================================================
    
    // Set-functions:
    void SetName(TString const str);
    void SetA(Int_t const n);
    void SetZ(Int_t const n);
    void SetLifeTime(TString const Unit, Double_t const t);
    void SetAbundance(TString const mode, Double_t const a);
    void SetMass(TString const mode, TString const Unit, Double_t const m);
    void SetBindingEnergy(TString const mode, TString const Unit, Double_t const E);
    void SetSp(TString const Unit, Double_t const E);
    void SetSn(TString const Unit, Double_t const E);
    void ComputePDGCode();
    void SetPDGCode(Int_t const PDG);
   
    // =======================================================================================================
    
    // Print-function:
    void Print(Bool_t const PrintGammas, Int_t const JBoundary);

    // =======================================================================================================
    
    // Function declarations for adding energy levels to a nucleus:
    void AddEnergyLevel(Int_t const Key, Double_t const Energy, TString const Unit);
    void SetLevelLifeTime(Int_t const Key, Double_t const Time, TString const Unit);
    void SetLevelBGT(Int_t const Key, Double_t const BGT);
    void SetLevelBF(Int_t const Key, Double_t const BF);
    void SetLevelReference(Int_t const Key, TString const Refs);
    void ClearLevelReference(Int_t const Key);
    void AddLevelAngMom(Int_t const Key, Int_t const JJ, TString const Par, Bool_t const Acc);
    void AddLevelAngMom(Int_t const Key, Double_t const JJ, TString const Par, Bool_t const Acc); 
    void AddLevelAngMomBetween(Int_t const Key, Double_t const JJ, TString const Par, Bool_t const Acc); // To accomodate half-integers.
    void ClearLevelAngMom(Int_t const Key);
    void AddLevelGamma(Int_t const Key, Double_t const EE, Double_t const II, Double_t const Ef, TString const GamMult, TString const Unit);
    void ClearLevelGamma(Int_t const Key);
    void SortEnergyLevels();
    
    // Direct methods for obtaining the database:
    Int_t GetNElevels();
    EnergyLevel GetEnergyLevel(Int_t const Index);
    
    // More elegant method of obtaining informatuion from the level scheme:
    Int_t GetNE();
    Int_t GetNAngMoms();
    Int_t GetNGammas();
    
    void SelectGroundState();
    void SelectElevel(Int_t const Index);
    void SelectElevel_WithKey(Int_t const ThisKey);
    void SelectFinalLevel();
    void SelectAngMom(Int_t const Index);
    void SelectGamma(Int_t const Index);
    
    Int_t GetKey();
    
    Double_t GetLevelE(TString const Unit);
    Double_t GetLevelLifeTime(TString const Unit);
    Double_t GetBGT();
    Double_t GetBF(); 
    Double_t GetJ();
    Double_t GetGammaE();
    Double_t GetGammaI();
    Double_t GetGammaEf();
    
    TString GetRefs();
    TString GetParity();
    TString GetGammaTransitionType();
    
    Bool_t GetJacc();
    
    EnergyLevel GetFinalLevel();
    
    // =======================================================================================================    
        
// Define class content:
private:
    TString Name;                    // Nucleus name, like "Sn122".
    Int_t A;                         // Number of nucleons.
    Int_t Z;                         // Number of protons.
    Int_t PDGCode;                   // Geant4 particle ID code.
    Double_t E_Bind;                 // Binding energy per nucleon [MeV].
    Double_t Mass;                   // Mass in [MeV].
    Double_t LifeTime;               // Lifetime in [sec].
    Double_t Abundance;              // natural abundance of this nucleus [dim. less]. Between 0 & 1.
    Double_t Sp;                     // 1p separation energy [MeV]
    Double_t Sn;                     // 1n separation energy [MeV]
    PhysicsConstants PhysConst;      // Contains the constants of physics necessary to do the computations.
    
    std::vector<EnergyLevel> Levels; // Contains all energy levels from the NNDC database (See structure-file for the list of the units).
    Int_t LevelIndex;                // selection index for obtaining information.
    Int_t AngIndex;                  // selection index for obtaining information.
    Int_t GammaIndex;                // selection index for obtaining information.
    
    // Generate ROOT dictionary:
    ClassDef(Nucleus, 1);
};

#endif
