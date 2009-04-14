#ifndef R3BGEOTOFPAR_H
#define R3BGEOTOFPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoTofPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoTofPar(const char* name="R3BGeoTofPar",
             const char* title="Tof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoTofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoTofPar,1)
};

#endif /* !R3BGEOTOFPAR_H */
