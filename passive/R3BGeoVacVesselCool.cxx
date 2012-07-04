//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012

/////////////////////////////////////////////////////////////
// R3BGeoVacVesselCool
//
// Class for geometry of Cooling Si Tracker
//
/////////////////////////////////////////////////////////////

#include "R3BGeoVacVesselCool.h"

ClassImp(R3BGeoVacVesselCool)

R3BGeoVacVesselCool::R3BGeoVacVesselCool() {
  // Constructor
  fName="VacVesselcool";
  strcpy(modName,"v");
  strcpy(eleName,"v");
  maxSectors=0;
  maxModules=1;
}
