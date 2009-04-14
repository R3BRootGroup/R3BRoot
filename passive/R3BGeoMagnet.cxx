//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// R3BGeoMagnet
//
// Class for the geometry of detector part Magnet
//
/////////////////////////////////////////////////////////////

#include "R3BGeoMagnet.h"

ClassImp(R3BGeoMagnet)

R3BGeoMagnet::R3BGeoMagnet() {
  // Constructor
  fName="magnet";
  maxSectors=0;
  maxModules=1;
  strcpy(modName,"m");
  strcpy(eleName,"m");
}
