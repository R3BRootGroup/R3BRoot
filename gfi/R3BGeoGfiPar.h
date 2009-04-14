#ifndef R3BGEOGFIPAR_H
#define R3BGEOGFIPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoGfiPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoGfiPar(const char* name="R3BGeoGfiPar",
             const char* title="Gfi Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoGfiPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoGfiPar,1)
};

#endif /* !R3BGEOGFIPAR_H */
