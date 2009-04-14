#ifndef R3BGEOTOF_H
#define R3BGEOTOF_H

#include "FairGeoSet.h"

class  R3BGeoTof : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  R3BGeoTof();
  ~R3BGeoTof() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(R3BGeoTof,0) // Class for STS
};

inline Int_t R3BGeoTof::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !R3BGEOTOF_H */
