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

#ifndef R3BDecaysBuilder_h
#define R3BDecaysBuilder_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"


class R3BDecaysBuilder : public G4VPhysicsConstructor {
private:

   // hide assignment operator
  R3BDecaysBuilder & operator=(const R3BDecaysBuilder &right);
  R3BDecaysBuilder(const R3BDecaysBuilder&);

public:

  R3BDecaysBuilder(const G4String& name = "decays");
  virtual ~R3BDecaysBuilder();

  // This method is dummy for physics
  virtual void ConstructParticle();

  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type
  virtual void ConstructProcess();

};
#endif








