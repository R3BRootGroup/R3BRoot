/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol     hapol@fpddux.usc.es
//*-- Date: 11/2004
//*-- Last Update: 04/03/06 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   Physics List 
//
// --------------------------------------------------------------
// Comments:
//
//  04/03/06 Full physics revision. Migrated to geant4.8
//           Based on examples/extended/medical/GammaTherapy
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "R3BPhysicsList.h"
#include "R3BPhysicsListMessenger.h"

#include "R3BParticlesBuilder.h"
#include "G4EmQEDBuilder.h"
#include "G4EmMuonBuilder.h"
#include "G4EmHadronBuilder.h"
#include "G4LowEnergyQEDBuilder.h"
#include "G4EmPenelopePhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "R3BDecaysBuilder.h"


//#include "EmHadronElasticBuilder.h"
//#include "EmBinaryCascadeBuilder.h"
//#include "EmIonBinaryCascadeBuilder.h"
//#include "EmGammaNucleusBuilder.h"
#include "G4HadronElasticPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4EmExtraPhysics.hh"

#include "G4IonINCLXXPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"

// std G4 headers
#include "G4UnitsTable.hh"
#include "G4LossTableManager.hh"
#include "G4EmProcessOptions.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

R3BPhysicsList::R3BPhysicsList():  G4VModularPhysicsList(){
  //
  // Constructor. Initializing values
  //
  
  emBuilderIsRegisted = false;
  decayIsRegisted = false;
  stepLimiterIsRegisted = false;
  helIsRegisted = false;
  bicIsRegisted = false;
  ionIsRegisted = false;
  gnucIsRegisted = false;
  verbose = 0;
  G4LossTableManager::Instance()->SetVerbose(0);
  defaultCutValue = 1.*CLHEP::mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;
  
  pMessenger = new R3BPhysicsListMessenger(this);
  
  // Add Physics builders
  RegisterPhysics(new R3BParticlesBuilder());
  steplimiter = new G4StepLimiterPhysics();
}


R3BPhysicsList::~R3BPhysicsList() {
  //
  // Destructor. Nothing to do
  //  
}


void R3BPhysicsList::ConstructParticle() {
  //
  // Construct Particles
  //

  if(verbose > 0)
    G4cout << "Construct Particles" << G4endl;

  G4VModularPhysicsList::ConstructParticle();
}



void R3BPhysicsList::ConstructProcess() {
  //
  // Construct Processes
  //

  if(verbose > 0)
    G4cout << "Construct Processes" << G4endl;

  if(!emBuilderIsRegisted) AddPhysicsList("standard");
  G4VModularPhysicsList::ConstructProcess();
  
  // Define energy interval for loss processes
  G4EmProcessOptions emOptions;
  emOptions.SetMinEnergy(0.1*CLHEP::keV);
  emOptions.SetMaxEnergy(100.*CLHEP::GeV);
  emOptions.SetDEDXBinning(90);
  emOptions.SetLambdaBinning(90);
  //emOptions.SetBuildPreciseRange(false);
  //  emOptions.SetApplyCuts(true);
  //emOptions.SetVerbose(0);
}

void R3BPhysicsList::AddPhysicsList(const G4String& name){
  //
  // Registering the physics processes
  //
  if(verbose > 0) {
    G4cout << "Add Physics <" << name 
           << "> emBuilderIsRegisted= " << emBuilderIsRegisted
           << G4endl;
  }
  if ((name == "standard") && !emBuilderIsRegisted) {
    RegisterPhysics(new G4EmQEDBuilder());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder());
    RegisterPhysics(new G4EmHadronBuilder());
    emBuilderIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    

    /*  } else if (name == "g4v52" && !emBuilderIsRegisted) {
    RegisterPhysics(new G4EmQEDBuilder52());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder52());
    RegisterPhysics(new G4EmHadronBuilder52());
    emBuilderIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "g4v71" && !emBuilderIsRegisted) {
    RegisterPhysics(new G4EmQEDBuilder71());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder71());
    RegisterPhysics(new G4EmHadronBuilder71());
    emBuilderIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
    */
  } else if (name == "lowenergy" && !emBuilderIsRegisted) {
    RegisterPhysics(new G4LowEnergyQEDBuilder());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder());
    RegisterPhysics(new G4EmHadronBuilder());
    emBuilderIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "penelope" && !emBuilderIsRegisted) {
    RegisterPhysics(new G4EmPenelopePhysics());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder());
    RegisterPhysics(new G4EmHadronBuilder());
    emBuilderIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "decay" && !decayIsRegisted) {
    RegisterPhysics(new R3BDecaysBuilder());
    decayIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "elastic" && !helIsRegisted) {
    RegisterPhysics(new G4HadronElasticPhysics());
    helIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
    
//  } else if (name == "binary" && !bicIsRegisted) {
//    RegisterPhysics(new EmBinaryCascadeBuilder());
//    bicIsRegisted = true;
//    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
//    
  } else if (name == "binary_ion" && !ionIsRegisted) {
    RegisterPhysics(new G4IonBinaryCascadePhysics());
    ionIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "gamma_nuc" && !gnucIsRegisted) {
    RegisterPhysics(new G4EmExtraPhysics());
    gnucIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "qgsp_bert") {
    RegisterPhysics(new G4HadronPhysicsQGSP_BERT());
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
    
  } else if (name == "ion_inclxx" && !ionIsRegisted) {
    RegisterPhysics(new G4IonINCLXXPhysics());
    ionIsRegisted = true;
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else {
    G4cout << "R3BPhysicsList::AddPhysicsList <" << name << ">" 
           << " fail - module is already regitered or is unknown " << G4endl;
  }
}





void R3BPhysicsList::SetCuts(){
  //
  //  Sets the cut on the physics interaction calculations.
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
  //
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");

  if (verbose>0) DumpCutValuesTable();
}


void R3BPhysicsList::SetVerbose(G4int val){
  //
  // Selecting verbosity
  //

  verbose = val;
}


void R3BPhysicsList::SetCutForGamma(G4double cut){
  //
  // Setting cut value for the gammas
  //

  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}


void R3BPhysicsList::SetCutForElectron(G4double cut){
  //
  // Setting cut value for the electron
  //

  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}


void R3BPhysicsList::SetCutForPositron(G4double cut) {
  //
  // Setting cut value for the positron
  //
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

