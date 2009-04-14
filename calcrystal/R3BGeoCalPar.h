#ifndef R3BGEOCALPAR_H
#define R3BGEOCALPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoCalPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoCalPar(const char* name="R3BGeoCalPar",
             const char* title="Cal Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoCalPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoCalPar,1)
};

#endif /* !R3BGEOCALPAR_H */
