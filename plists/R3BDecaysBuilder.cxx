/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol     hapol@fpddux.usc.es
//*-- Date: 03/2006
//*-- Last Update: 06/03/06 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   Physics List -- Decays builder
//
// --------------------------------------------------------------
// Comments:
//
//  06/03/06 Included after full physics revision. Migrated to geant4.8
//           Based on examples/extended/medical/GammaTherapy
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "R3BDecaysBuilder.h"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Decay.hh"


R3BDecaysBuilder::R3BDecaysBuilder(const G4String& name)
   :  G4VPhysicsConstructor(name){}


R3BDecaysBuilder::~R3BDecaysBuilder(){}


void R3BDecaysBuilder::ConstructParticle(){}


void R3BDecaysBuilder::ConstructProcess() {
  //
  // Add the decay proccess. More to come with ions?
  //

  // Add Decay Process

  G4Decay* fDecayProcess = new G4Decay();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if (fDecayProcess->IsApplicable(*particle)) {

      //      pmanager ->AddProcess(fDecayProcess);

      // set ordering for PostStepDoIt and AtRestDoIt
      //pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
      //pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);
      pmanager->AddProcess(fDecayProcess, 1,-1, 5);

    }
  }
}

