#ifndef R3BGEODTOFPAR_H
#define R3BGEODTOFPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeodTofPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeodTofPar(const char* name="R3BGeodTofPar",
             const char* title="mTof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeodTofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeodTofPar,1)
};

#endif /* !R3BGEODTOFPAR_H */
