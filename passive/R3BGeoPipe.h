#ifndef R3BGEOPIPE_H
#define R3BGEOPIPE_H

#include "FairGeoSet.h"

class  R3BGeoPipe : public FairGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  R3BGeoPipe();
  virtual ~R3BGeoPipe();
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  Bool_t create(FairGeoBuilder*);
  ClassDef(R3BGeoPipe,0) // Class for geometry of beam pipe
};

#endif  /* !R3BGEOPIPE_H */
