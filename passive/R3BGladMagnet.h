#ifndef R3BGladMAGNET_H
#define R3BGladMAGNET_H

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "FairDetector.h"
#include "FairModule.h"
#include "R3BModule.h"
#include "TGeoMatrix.h"

class R3BGladMagnet : public R3BModule {
  
public:
  R3BGladMagnet(const char * name, const char *Title="R3BGlad Magnet");
  R3BGladMagnet(const char * name, TString geoFile, const char *Title="R3BGlad Magnet");
  R3BGladMagnet();
  virtual ~R3BGladMagnet();

  void ConstructGeometry();
  
  Bool_t CheckIfSensitive(std::string name);

private:
  Float_t fGladAngle;

  ClassDef(R3BGladMagnet,2) //R3BGladMagnet
};

#endif //R3BGladMAGNET_H

