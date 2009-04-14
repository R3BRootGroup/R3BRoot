#ifndef R3BGEOTARGET_H
#define R3BGEOTARGET_H

#include "FairGeoSet.h"

class  R3BGeoTarget : public FairGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  R3BGeoTarget();
  ~R3BGeoTarget() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  ClassDef(R3BGeoTarget,0) // Class for geometry of Target
};

#endif  /* !R3BGEOTARGET_H */
