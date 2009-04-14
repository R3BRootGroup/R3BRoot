#ifndef R3BGEOPASSIVEPAR_H
#define R3BGEOPASSIVEPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"

class R3BGeoPassivePar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoPassivePar(const char* name="R3BGeoPassivePar",
             const char* title="Passive Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoPassivePar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoPassivePar,1)
};

#endif /* !R3BGEOPASSIVEPAR_H */
