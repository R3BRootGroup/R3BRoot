#ifndef R3BGEOLANDPAR_H
#define R3BGEOLANDPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoLandPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoLandPar(const char* name="R3BGeoLandPar",
             const char* title="Land Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoLandPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoLandPar,1)
};

#endif /* !R3BGEOLANDPAR_H */
