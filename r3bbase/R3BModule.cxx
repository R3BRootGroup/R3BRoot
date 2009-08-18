#include "R3BModule.h"

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
R3BModule::R3BModule() : FairModule() { }
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BModule::R3BModule( const Char_t* Name, const Char_t* title, Bool_t Active)
  : FairModule(Name,title,Active) { }
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BModule::~R3BModule() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------

ClassImp(R3BModule)
  
