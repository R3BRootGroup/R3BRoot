//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012

#ifndef R3BGEOVACVESSELCOOL_H
#define R3BGEOVACVESSELCOOL_H

#include "FairGeoSet.h"

class  R3BGeoVacVesselCool : public FairGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  R3BGeoVacVesselCool();
  ~R3BGeoVacVesselCool() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  ClassDef(R3BGeoVacVesselCool,0) // Class for geometry of Cooling
};

#endif  /* !R3BGEOVACVESSELCOOL_H */
