#ifndef R3BGEOTRAPAR_H
#define R3BGEOTRAPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoTraPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoTraPar(const char* name="R3BGeoTraPar",
             const char* title="Tra Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoTraPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoTraPar,1)
};

#endif /* !R3BGEOTRAPAR_H */
