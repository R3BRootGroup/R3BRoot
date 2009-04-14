#ifndef R3BGEOMAGNET_H
#define R3BGEOMAGNET_H

#include "FairGeoSet.h"
#include "TString.h"

class  R3BGeoMagnet : public FairGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  R3BGeoMagnet();
  ~R3BGeoMagnet() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  ClassDef(R3BGeoMagnet,0) // Class for the geometry of Magnet
};

#endif  /* !R3BGEOMAGNET_H */
