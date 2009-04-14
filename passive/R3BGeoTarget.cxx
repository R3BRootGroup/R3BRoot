//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// R3BGeoTarget
//
// Class for geometry of Target
//
/////////////////////////////////////////////////////////////

#include "R3BGeoTarget.h"

ClassImp(R3BGeoTarget)

R3BGeoTarget::R3BGeoTarget() {
  // Constructor
  fName="target";
  strcpy(modName,"t");
  strcpy(eleName,"t");
  maxSectors=0;
  maxModules=1;
}
