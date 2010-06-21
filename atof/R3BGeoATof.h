#ifndef R3BGEOATOF_H
#define R3BGEOATOF_H

#include "FairGeoSet.h"

class  R3BGeoATof : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  R3BGeoATof();
  ~R3BGeoATof() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(R3BGeoATof,0) // Class for Aladin TOF
};

inline Int_t R3BGeoATof::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !R3BGEOATOF_H */
