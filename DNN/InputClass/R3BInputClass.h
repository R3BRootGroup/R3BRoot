// Class that handles input parameter control.
// Written by C. A. Douma.

// General directives:
#ifndef R3BINPUTCLASS_H
#define R3BINPUTCLASS_H

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
#include "../Masses/Nuclei.h"

// use standard namespace:
using namespace std;

// Define the class:
class R3BInputClass
{
public:
    // Constructor (Default):
    R3BInputClass();
    
    // Destructor (Default):
    virtual ~R3BInputClass();
    
    // Member function declarations:
    TString RoundOff(Double_t const our_input, Int_t const significance);                                     // Converts Doubles to Strings.
    
    // Control functions:
    void LinkFile(TString const Name);                                                                        // Linkes the class to a .txt-file so that inputs can be collected.
    void Print(Bool_t const PrintContent);                                                                    // Default print function of this class. kTRUE prints content inputs too.
    void ErrorMessage(TString const Message);                                                                 // Handles error messages:
    void PrintAllErrors();                                                                                    // Prints the entire log of errors:
    void DisableErrorPrinting();                                                                              // After calling this, errors are NOT shown when they are encountered.
    void EnableErrorPrinting();                                                                               // After calling this, errors ARE shown when encoutered.
    void CreateFile(TString const Name);                                                                      // Creates a new input file from scratch with default input values.
    void CreateElenaFile(TString const Name);                                                                 // Creates a new input file from scratch with input values as in Elena's thesis.
    void CreateSAMURAIFile(TString const Name);                                                               // Creates a new input file from scratch with input values corresponding to SAMURAI.
    void ReadFile();                                                                                          // Adds the content to the linked file to the vectors in the class.
    void WriteFile();                                                                                         // Overwrites the linked file with the content of the class.
    void CheckPhysicsList();                                                                                  // Checks whether the Physics List input matches an allowed option.
    void AllowCompositePhysicsList();                                                                         // In physics list check, this allows composite lists to also pass the check.
    void ForbidCompositePhysicsList();                                                                        // In physics list check, this prevents composite lists to pass the check.
    
    // Retrieve inputs:
    Double_t GetInputDouble(TString const Description, TString const Unit);                                   // When ReadFile() has been called externally, this retrieves a Double_t input.
    Int_t GetInputInteger(TString const Description);                                                         // When ReadFile() has been called externally, this retrieves a Int_t input.
    Bool_t GetInputBoolian(TString const Description);                                                        // When ReadFile() has been called externally, this retrieves a Bool_t input.
    TString GetInputString(TString const Description);                                                        // When ReadFile() has been called externally, this retrieves a TString input.
    
    // Generate inputs:
    void AddInputDouble(TString const Description, TString const Unit, Double_t const Value);                 // Creates a new input in the file (if it does not exist) of type Double_t.
    void AddInputString(TString const Description, TString const Value);                                      // Creates a new input in the file (if it does not exist) of type TString.
    void AddInputInteger(TString const Description, Int_t const Value);                                       // Creates a new input in the file (if it does not exist) of type Int_t.
    void AddInputBoolian(TString const Description, Bool_t const Value);                                      // Creates a new input in the file (if it does not exist) of type Bool_t.
    
    // Modify inputs:
    void ModifyDouble(TString const Description, TString const Unit, Double_t const Value);                   // Modifies an existing Double_t input in the linked file.
    void ModifyInteger(TString const Description, Int_t const Value);                                         // Modifies an existing Int_t input in the linked file.
    void ModifyBoolian(TString const Description, Bool_t const Value);                                        // Modifies an existing Bool_t input in the linked file.
    void ModifyString(TString const Description, TString const Value);                                        // Modifies an existing TString input in the linked file.
    
    // Remove inputs:
    void EraseInput(TString const Description);                                                               // Erases a specific input from the linked .txt-file.
    
    // Get-functions:
    Bool_t IsLinked() {return Linked;}                                                                        // returns whether the class is linked to an inputs file.
    Bool_t ContainsNoErrors();                                                                                // return whether the class-Errors-vector contains error messages yes/no.
    Bool_t PhysicsList_IsChecked();                                                                           // Shows whether the physics list input has been checked for validity.
    Double_t GetAvgBeamEnergy();                                                                              // direct function to access average beam energy in MeV (NB: not MeV/u!)
    Double_t GetAvgBeamEnergyPerNucleon();                                                                    // direct function to access average beam energy in MeV/u.
    Double_t GetBeamEnergyDiff();                                                                             // direct function to access beam energy difference in MeV (NB: not MeV/u!)
    Double_t GetAvgBeamBeta();                                                                                // direct function to access the relativistic beta of the beam.
    Double_t GetTargetTime();                                                                                 // direct function for the time [ns] that the beam needs to travel to the target.
    Double_t GetTotalPaddleThickness();                                                                       // direct function access the total paddle thickness (max of xy & z):
    Double_t GetTotalPaddleThicknessXY();                                                                     // direct function access the total paddle thickness in xy-direction.
    Double_t GetTotalPaddleThicknessZ();                                                                      // direct function access the total paddle thickness in z-direction.
    Double_t GetNEBULATotalPaddleThickness();                                                                 // direct function access the total paddle thickness (max of xy & z) for NEBULA.
    Double_t GetNeuLANDStartTime();                                                                           // direct function to compute the time from the particle gun to NeuLAND front.
    Double_t GetNEBULAStartTime();                                                                            // direct function to compute the time from the particle gun to NEBULA front.
    Double_t GetVETOTotalPaddleThicknessZ();                                                                  // Direct function to obtain the total thickness of the NeuLAND VETO.
    Double_t GetNEBULAVETOTotalPaddleThicknessZ();                                                            // Direct function to obtain the total thickness of the NEBULA VETO.
    Double_t GetVETOTotalPaddleThicknessXY();                                                                 // Direct function to obtain the total paddle width of the NeuLAND VETO.
    Double_t GetNEBULAVETOTotalPaddleThicknessXY();                                                           // Direct function to obtain the total paddle width of the NEBULA VETO.
    
private:
    // General Class content:
    TString FileName;               // Absolute path to the .txt-file holding the parameters:
    Int_t NLines;                   // Number of lines in the .txt-file
    Bool_t Linked;                  // Tells you whether this class is linked to a .txt-file or not.
    Bool_t PrintErrors;             // decides whether errors are printed as they are encountered or not.
    Bool_t PhysicsList_Checked;     // Keeps track on whether the physics list is check or not.
    Bool_t Allow_Composite_PlysicsList; // Decided whether the physics list should exactly equal a know list, or a composite is also allowed.
    std::vector<TString> Errors;    // Contains all error messages that have occurred in this class.
    
    // Input parameters:
    std::vector<TString>  Double_Descriptions;
    std::vector<TString>  Double_Units;
    std::vector<Double_t> Double_Values;
    std::vector<TString>  Integer_Descriptions;
    std::vector<Int_t>    Integer_Values;
    std::vector<TString>  String_Descriptions;
    std::vector<TString>  String_Values;
    std::vector<TString>  Boolian_Descriptions;
    std::vector<Bool_t>   Boolian_Values;
    
    // Inlcude the nuclear database:
    Nuclei* TheNuclei;
    
    // Generate ROOT dictionary:
    ClassDef(R3BInputClass, 1);
};

#endif
