#ifndef R3BGEOFI7_H
#define R3BGEOFI7_H

#include "FairGeoSet.h"

class  R3BGeoFi7 : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  R3BGeoFi7();
  ~R3BGeoFi7() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(R3BGeoFi7,0) // Class for STS
};

inline Int_t R3BGeoFi7::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !R3BGEOFI7_H */
