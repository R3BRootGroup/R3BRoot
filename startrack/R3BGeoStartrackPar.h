#ifndef R3BGEOSTARTRACKPAR_H
#define R3BGEOSTARTRACKPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoStartrackPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoStartrackPar(const char* name="R3BGeoStartrackPar",
             const char* title="Startrack Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoStartrackPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoStartrackPar,1)
};

#endif /* !R3BGEOSTARTRACKPAR_H */
