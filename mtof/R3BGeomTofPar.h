#ifndef R3BGEOMTOFPAR_H
#define R3BGEOMTOFPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeomTofPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeomTofPar(const char* name="R3BGeomTofPar",
             const char* title="mTof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeomTofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeomTofPar,1)
};

#endif /* !R3BGEOMTOFPAR_H */
