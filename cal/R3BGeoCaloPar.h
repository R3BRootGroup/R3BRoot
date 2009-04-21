#ifndef R3BGEOCALOPAR_H
#define R3BGEOCALOPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoCaloPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoCaloPar(const char* name="R3BGeoCaloPar",
             const char* title="Calo Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoCaloPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoCaloPar,1)
};

#endif /* !R3BGEOCALPAR_H */
