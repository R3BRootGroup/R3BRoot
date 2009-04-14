#ifndef R3BGEOCAVE_H
#define R3BGEOCAVE_H

#include "FairGeoSet.h"
#include "TString.h"
#include "FairGeoMedia.h"
#include <fstream>
class  R3BGeoCave : public FairGeoSet {
protected:
  TString name;
public:
  R3BGeoCave();
  ~R3BGeoCave() {}
  const char* getModuleName(Int_t) {return name.Data();}  
  Bool_t read(fstream&,FairGeoMedia*);
  void addRefNodes();
  void write(fstream&);
  void print();
  ClassDef(R3BGeoCave,0) // Class for the geometry of CAVE
};

#endif  /* !PNDGEOCAVE_H */
