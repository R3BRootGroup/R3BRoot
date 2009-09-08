#ifndef ELIGEOLUMONPAR_H
#define ELIGEOLUMONPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class ELIGeoLuMonPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  ELIGeoLuMonPar(const char* name="ELIGeoLuMonPar",
             const char* title="LuMon Geometry Parameters",
             const char* context="TestDefaultContext");
  ~ELIGeoLuMonPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(ELIGeoLuMonPar,1)
};

#endif /* !ELIGEOLUMONPAR_H */
