// Class that handles input parameter control.
// Written by C. A. Douma.

// General directives:
#ifndef R3BASCIIFILEGENERATOR_H
#define R3BASCIIFILEGENERATOR_H

// Include C++ headers:
#include <iostream>
#include <fstream>

// Include ROOT headers:
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TF1.h"
#include "TH1I.h"

// Include R3B headers:
#include "R3BInputClass.h"
#include "Nuclei.h"

// use standard namespace:
using namespace std;

// Define the class:
class R3BASCIIFileGenerator
{
public:
    // Constructor (Default):
    R3BASCIIFileGenerator();
    
    // Destructor (Default):
    virtual ~R3BASCIIFileGenerator();
    
    // Member function declarations:
    void ErrorMessage(TString const Message);   // Error-handling.
    void PrintAllErrors();                      // Error-handling.
    void DisableErrorPrinting();                // Error-handling.
    void EnableErrorPrinting();                 // Error-handling.
    
    void LinkInputClass(R3BInputClass* inp);    // Link this class to an inputs-class so that we know how to generate the file.
    void ObtainParticleGun();                   // Loads the proper inputs into this class. LinkInputClass() should have been called before using this function.
    void Generate4Vector();                     // Fills ParticleGun_OneVector with a newly-generated particle.
    void CreateFile();                          // Generates the new ASCII-file. ObtainParticleGun() should have been called before using this function.
    void SetFileName(TString const Name);       // Changes the FileName of the output ASCII-file to a user-specified file.
    void SetnEvents(Int_t const nn);            // Changes the number of events to a user-specified value.
    void SetProfile(TString const Option);      // Sets the profile in polar angle Theta for random number generation.
    void SetNThreads(Int_t const nT_tot, Int_t const nT_cur); // specifies number of threads for multithreading.
    std::vector<Double_t> DefineMultBoundaries(); // Defines the probabilities for the different multiplicities.
    
    void InitializeSnFiles();                   // Initializes the parameters that we need for the Sn-files from the Inputs-class.
    void ReadandWriteOneSnEvent(Int_t const ThisEvent, Int_t const CurrentMultiplicity); // Reads and writes one Sn-event from an Sn-file to the output-file.
    void WriteOneExEvent(Int_t const ThisEvent); // Creates and writes a single event from the Ex-generator.
    Double_t Bisection_Function(Double_t const En, Int_t const TheMultiplicity); // Used in WriteOneExEvent() for computation of the correct neutron energy.
    
    Bool_t ContainsNoErrors();                  // return whether the class-Errors-vector contains error messages yes/no.
    void SetCutsomBoundaries(std::vector<Double_t> MultProbabilities); // Specifies multiplicity distribution manually.
    
private:
    // Class content:
    Bool_t PrintErrors;                     // Decides whether errors are printed on-the-fly yes/no.
    Bool_t ParticleGun_Loaded;              // Keeps track whether a proper particle gun was loaded yes/no.
    std::vector<TString> Errors;            // Error log.
    TString FileName;                       // Name of the output ASCII-file, including its absolute path.
    R3BInputClass* Inputs;                  // Pointer to an inputs class, to base the construction of the ASCII-file on.
    Nuclei* TheNuclei;                      // Pointer to the Nuclei-class, where we get our masses from.
    TRandom3* Generator;                    // Random number generation engine. Is constructed once in Constructor.
    TString RandomizeOption;                // Decides according to what pattern (in polar angle Theta) we generate particles.
    Int_t TotalNumberOfThreads;             // Total number of threads. We generate a file for each thread, with a different seed & a differnt name.
    Int_t CurrentThread;                    // Total number of threads. We generate a file for each thread, with a different seed & a differnt name.
    
    // Content for reading Sn DataFiles:
    Bool_t SnFiles_Initialized;             // Test if the Sn-files are indeed there.
    std::ofstream ASCII_File;               // The file that we produce for the ASCII particle gun in the MC transport.
    std::ifstream* SnDataFiles;             // The array with all the Sn files for different multiplicities.
    std::vector<TString> SnDataFileName;    // Contains the FileNames of the Sn-files, including their absolute paths.
    std::vector<Int_t> FinalCounters;       // We start reading from StartCounters and we finish reading at FinalCounters.
    std::vector<Int_t> StartCounters;       // We start reading from StartCounters and we finish reading at FinalCounters.
    std::vector<Int_t> CurrentCounters;     // Counts how many events we have read from the Sn-files.
    Bool_t Include_SnNucleus;               // Decides whether or not we include the Sn nucleus from the Sn-files in our simulations.
    Int_t nReadedFiles;                     // Contains the array size of SnDataFiles and the size of the other vectors in this section.
    Int_t StartReading;                     // Inputs-parameter to initialize StartCounters.
    Int_t EndReading;                       // Inputs-parameter to initialize EndCounters.
    Bool_t RandomRotateNeutrons;            // If this is kTRUE, the p-vectors of the neutrons in the SnDataFiles will be randomly rotated along the z-axis (same angle for all neutrons per event).
    
    // Particle Gun Inputs:
    TString TheVMCFolder;
    Int_t R3BRoot_nEvents;
    TString ParticleGun_ParticleType;
    TString ParticleCun_GunType;
    Int_t ParticleGun_Multiplicity;
    Bool_t ParticleGun_Randomize_Multiplicity;
    Bool_t ParticleGun_Uniformize_Detected_Multiplicity;
    TString ParticleGun_Multiplicity_Distribution;
    Double_t ParticleGun_SingleDetectionRate;
    Double_t ParticleGun_InfiniteDetectionRate;
    Double_t ParticleGun_x_position;
    Double_t ParticleGun_y_position;
    Double_t ParticleGun_z_position;
    Double_t ParticleGun_x_direction;
    Double_t ParticleGun_y_direction;
    Double_t ParticleGun_z_direction;
    Double_t ParticleGun_AngularSpread;
    Double_t ParticleGun_MinimalKineticEnergy;
    Double_t ParticleGun_MaximalKineticEnergy;
    
    // Particle Gun variables to compute:
    Int_t ParticleGun_PDGCode;
    Double_t ParticleGun_TotalMass;
    Int_t ParticleGun_MassNumber;
    Int_t ParticleGun_ChargeNumber;
    TVector3* ParticleGun_Direction;
    
    // Ex-generator variables:
    Bool_t ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks;
    TString ParticleGun_ExGenerator_BeamIsotope;
    TString ParticleGun_ExGenerator_TargetIsotope;
    Double_t ParticleGun_ExGenerator_SingleTheta; // [rad]
    std::vector<Double_t> ParticleGun_ExGenerator_PhiVector; // [rad]
    std::vector<TLorentzVector*> ParticleGun_ExGenerator_NeutronTracks;
    Int_t ParticleGun_ExGenerator_NeutronMultiplicity;
    Double_t ParticleGun_ExGenerator_BeamEnergy;
    Double_t ParticleGun_ExGenerator_Recoil_ExcitationEnergy;
    std::vector<Double_t> CustomBoundaries;
    Bool_t CustomBoundaries_AreSpecified;
    
    // Multiplicity randomization distribution:
    Double_t* Mult_Random_Boundaries;
    
    // Random number generation gun-variables:
    TLorentzVector* ParticleGun_OneVector;
    
    // Generate ROOT dictionary:
    ClassDef(R3BASCIIFileGenerator, 1);
};

#endif
