// $Id$

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007, Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: vmc@pcroot.cern.ch
//-------------------------------------------------

/// \file fllibs.C
/// \brief Macro for loading FLUKA libraries


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <iostream>

#include <TSystem.h>
#include <TString.h>

#endif


void fllibs()
{
/// Macro function for loading FLUKA libraries

    cout << endl;
    cout << "-I- TFluka: Loading Fluka libraries ..." << endl;

    gSystem->Load("libfluka.so");

    cout << "-I- TFluka: Loading Fluka libraries ... finished" << endl;
    cout << endl;

}

