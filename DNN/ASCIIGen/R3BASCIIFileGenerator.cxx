// Include this class header:
#include "R3BASCIIFileGenerator.h"

// Default Constructor:
R3BASCIIFileGenerator::R3BASCIIFileGenerator()
{
    Generator = new TRandom3(0); // 0 --> Initializes with time.
    PrintErrors = kTRUE;
    ParticleGun_Loaded = kFALSE;
    Errors.clear();
    Inputs = 0;
    FileName = "./ASCII.txt";
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    RandomizeOption = "Uniform";
    TotalNumberOfThreads = 1;
    CurrentThread = 0;
    
    TheVMCFolder = "./";
    R3BRoot_nEvents = 2;
    ParticleCun_GunType = "NewlyGenerated";
    ParticleGun_ParticleType = "neutron";
    ParticleGun_Multiplicity = 1;
    ParticleGun_Randomize_Multiplicity = kFALSE;
    ParticleGun_Uniformize_Detected_Multiplicity = kFALSE;
    ParticleGun_Multiplicity_Distribution = "Default";
    ParticleGun_SingleDetectionRate = 0.922;
    ParticleGun_InfiniteDetectionRate = 0.32;
    ParticleGun_x_position = 0.0;
    ParticleGun_y_position = 0.0;
    ParticleGun_z_position = 0.0;
    ParticleGun_x_direction = 0.0;
    ParticleGun_y_direction = 0.0;
    ParticleGun_z_direction = 1.0;
    ParticleGun_AngularSpread = 4.0;
    ParticleGun_MinimalKineticEnergy = 599.0;
    ParticleGun_MaximalKineticEnergy = 601.0;
    
    SnFiles_Initialized = kFALSE;
    SnDataFiles = 0;
    SnDataFileName.clear();
    FinalCounters.clear();
    StartCounters.clear();
    CustomBoundaries.clear();
    CurrentCounters.clear();
    Include_SnNucleus = kTRUE;
    nReadedFiles = 1;
    StartReading = 0;
    EndReading = 9999;
    RandomRotateNeutrons = kFALSE;
    CustomBoundaries_AreSpecified = kFALSE;
    
    ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks = kFALSE;
    ParticleGun_ExGenerator_BeamIsotope = "proton";
    ParticleGun_ExGenerator_TargetIsotope = "Sn132";
    ParticleGun_ExGenerator_SingleTheta = 0.0;
    ParticleGun_ExGenerator_PhiVector.clear();
    ParticleGun_ExGenerator_NeutronTracks.clear();
    ParticleGun_ExGenerator_NeutronMultiplicity = 1;
    ParticleGun_ExGenerator_BeamEnergy = 600.0;
    ParticleGun_ExGenerator_Recoil_ExcitationEnergy = 0.0;
    
    ParticleGun_PDGCode = 2112;
    ParticleGun_TotalMass = 939.565413358;
    ParticleGun_MassNumber = 1;
    ParticleGun_ChargeNumber = 1;
    ParticleGun_Direction = new TVector3();
    ParticleGun_OneVector = new TLorentzVector();
    
    Mult_Random_Boundaries = 0;
}
    
// Destructor:
R3BASCIIFileGenerator::~R3BASCIIFileGenerator() 
{
    delete TheNuclei;
    delete ParticleGun_OneVector;
    delete ParticleGun_Direction;
    Errors.clear();
    
    ASCII_File.close();
    SnDataFileName.clear();
    FinalCounters.clear();
    StartCounters.clear();
    CustomBoundaries.clear();
    CurrentCounters.clear();
    ParticleGun_ExGenerator_PhiVector.clear();
    ParticleGun_ExGenerator_NeutronTracks.clear();
}

// Member function definitions:
#include "LinkInputClass.h"
#include "ErrorMessage.h"
#include "SetCutsomBoundaries.h"
#include "ObtainParticleGun.h"
#include "Generate4Vector.h"
#include "CreateFile.h"
#include "SetFunctions.h"
#include "SetNThreads.h"
#include "ReadandWriteOneSnEvent.h"
#include "InitializeSnFiles.h"
#include "DefineMultBoundaries.h"
#include "Bisection_Function.h"
#include "WriteOneExEvent.h"

// Generate ROOT dictionary:
ClassImp(R3BASCIIFileGenerator);
