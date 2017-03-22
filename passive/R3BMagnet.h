#ifndef R3BMAGNET_H
#define R3BMAGNET_H

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "FairDetector.h"
#include "FairModule.h"
#include "R3BModule.h"
#include "TGeoMatrix.h"

class R3BMagnet : public R3BModule {
  
public:
  R3BMagnet(const char * name, const char *Title="R3B Magnet");
  R3BMagnet(const char * name, TString geoFile, const char *Title="R3B Magnet");
  R3BMagnet();
  virtual ~R3BMagnet();
  
  void ConstructGeometry();
  
  Bool_t CheckIfSensitive(std::string name);
  
  ClassDef(R3BMagnet,2) //R3BMagnet
};

#endif //R3BMAGNET_H

