#ifndef R3BGEOSTARTRAPAR_H
#define R3BGEOSTARTRAPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoSTaRTraPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoSTaRTraPar(const char* name="R3BGeoSTaRTraPar",
             const char* title="STaRTrack Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoSTaRTraPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoSTaRTraPar,1)
};

#endif /* !R3BGEOSTARTRAPAR_H */
