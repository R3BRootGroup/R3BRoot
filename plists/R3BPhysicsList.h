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

#ifndef R3BPhysicsList_h
#define R3BPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class R3BPhysicsListMessenger;
class G4StepLimiterBuilder;

class R3BPhysicsList: public G4VModularPhysicsList {
private:
  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;
  G4int    verbose;
  G4bool   emBuilderIsRegisted;
  G4bool   decayIsRegisted;
  G4bool   stepLimiterIsRegisted;
  G4bool   helIsRegisted;
  G4bool   bicIsRegisted;
  G4bool   ionIsRegisted;
  G4bool   gnucIsRegisted;
  
  R3BPhysicsListMessenger* pMessenger;
  G4StepLimiterBuilder* steplimiter;
  
public:
  R3BPhysicsList();
  ~R3BPhysicsList();
  
  // SetCuts()   
  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();

  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);

  void AddPhysicsList(const G4String&);
  void SetVerbose(G4int val);
};
#endif



