//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2016 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file Garfield/geant4/src/RunConfiguration.cxx
/// \brief Implementation of the Garfield::RunConfiguration class 
///
/// Garfield garfieldpp example adapted to Virtual Monte Carlo.
///
/// \date 28/10/2015
/// \author I. Hrivnacova; IPN, Orsay

#include "RunConfiguration.h"
#include "FastSimulation.h"

namespace VMC
{
namespace Garfield
{

//_____________________________________________________________________________
RunConfiguration::RunConfiguration(const TString& geometry,
    	                           const TString& physicsList,
                                   const TString& specialProcess)
  : TG4RunConfiguration(geometry, physicsList, specialProcess, false, false)
{
/// Standard constructor
/// \param geometry        Selection of geometry option
/// \param physicsList     Selection of physics
/// \param specialProcess  Selection of the special processes
}

//_____________________________________________________________________________
RunConfiguration::~RunConfiguration()
{
/// Destructor
}

//
// protected methods
//

//_____________________________________________________________________________
TG4VUserFastSimulation*  RunConfiguration::CreateUserFastSimulation()
{
/// Create Garfield fast simulation model

  G4cout << "Going to create FastSimulation" << G4endl;

  return new FastSimulation();
}

}
}
