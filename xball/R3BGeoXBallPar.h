#ifndef R3BGEOXBALLPAR_H
#define R3BGEOXBALLPAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoXBallPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoXBallPar(const char* name="R3BGeoXBallPar",
             const char* title="XBall Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoXBallPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoXBallPar,1)
};

#endif /* !R3BGEOXBALLPAR_H */
