#ifndef R3BGEOFI7PAR_H
#define R3BGEOFI7PAR_H
 
#include "FairParGenericSet.h"

#include "TH1F.h"

class R3BGeoFi7Par : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  R3BGeoFi7Par(const char* name="R3BGeoFi7Par",
             const char* title="Fi7 Geometry Parameters",
             const char* context="TestDefaultContext");
  ~R3BGeoFi7Par(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(R3BGeoFi7Par,1)
};

#endif /* !R3BGEOFI7PAR_H */
