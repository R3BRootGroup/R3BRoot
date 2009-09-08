#ifndef ELIGEOLUMON_H
#define ELIGEOLUMON_H

#include "FairGeoSet.h"

class  ELIGeoLuMon : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  ELIGeoLuMon();
  ~ELIGeoLuMon() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(ELIGeoLuMon,0) // Class for STS
};

inline Int_t ELIGeoLuMon::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !ELIGEOLUMON_H */
