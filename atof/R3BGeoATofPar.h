#ifndef R3BGEOATOFPAR_H
#define R3BGEOATOFPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoATofPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoATofPar(const char* name="R3BGeoATofPar",
             const char* title="ATof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoATofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoATofPar,1)
};

#endif /* !R3BGEOATOFPAR_H */
