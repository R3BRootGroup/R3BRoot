#ifndef R3BGEODCHPAR_H
#define R3BGEODCHPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoDchPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoDchPar(const char* name="R3BGeoDchPar",
             const char* title="Dch Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoDchPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoDchPar,1)
};

#endif /* !R3BGEODCHPAR_H */
