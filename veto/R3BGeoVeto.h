#ifndef R3BGEOVETO_H
#define R3BGEOVETO_H

#include "FairGeoSet.h"

class  R3BGeoVeto : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  R3BGeoVeto();
  ~R3BGeoVeto() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(R3BGeoVeto,0) // Class for STS
};

inline Int_t R3BGeoVeto::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !R3BGEOTOF_H */
