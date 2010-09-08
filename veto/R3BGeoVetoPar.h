#ifndef R3BGEOVETOPAR_H
#define R3BGEOVETOPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoVetoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoVetoPar(const char* name="R3BGeoVetoPar",
             const char* title="Veto Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoVetoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoVetoPar,1)
};

#endif /* !R3BGEOTOFPAR_H */
