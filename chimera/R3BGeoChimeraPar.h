#ifndef R3BGEOCHIMERAPAR_H
#define R3BGEOCHIMERAPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoChimeraPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoChimeraPar(const char* name="R3BGeoChimeraPar",
             const char* title="Chimera Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoChimeraPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoChimeraPar,1)
};

#endif /* !R3BGEOCHIMERAPAR_H */
