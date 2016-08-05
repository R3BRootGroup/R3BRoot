#ifndef GFLASH_RUN_CONFIGURATION1_H
#define GFLASH_RUN_CONFIGURATION1_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2016 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file Garfield/geant4/include/RunConfiguration.h 
/// \brief Definition of the Garfield::RunConfiguration class 
///
/// Garfield garfieldpp example adapted to Virtual Monte Carlo.
///
/// \date 28/10/2015
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4RunConfiguration.h"

namespace VMC
{
namespace Garfield
{

/// \ingroup Garfield
/// \brief User Geant4 VMC run configuration
///
/// This class overrides the default Geant4 VMC dummy 
/// fast simulation construction with the user class.
///
/// \author I. Hrivnacova; IPN, Orsay

class RunConfiguration : public TG4RunConfiguration
{
  public:
    RunConfiguration(const TString& geometry,
    	             const TString& physicsList = "FTFP_BERT",
                     const TString& specialProcess = "stepLimiter");
    virtual ~RunConfiguration();

    // methods
    virtual TG4VUserFastSimulation*  CreateUserFastSimulation();
};

}
}

#endif //GFLASH_RUN_CONFIGURATION1_H

