#ifndef R3BGladMAGNET_H
#define R3BGladMAGNET_H

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "FairDetector.h"
#include "FairModule.h"
#include "R3BModule.h"
#include "TGeoMatrix.h"

/******************************************/
//Edited on: 22.06.2017
//by: Lorenzo Zanetti -- lzanetti@ikp.tu-darmstadt.de
//Description:
// Adding back compatibility for "just-by-name" constructor.
// The macros __GLAD_POS_D{X,Y,Z} and __GLAD_ROT are being defined
// in order to fix the placement from here
// NOTE: overrides the placement in the geometry file!
/*****************************************/

//NOTE: as for now, these valuse are the same used
//      for the geometry creation (v17).
//      These will move also old files.
#define __GLAD_POS_DX -42.0 //offset on the Z axis
#define __GLAD_POS_DY 0.0 //offset on the Y axis (not present)
#define __GLAD_POS_DZ 308.8 //offset on the Z axis (distance from target)
#define __GLAD_ROT 14 //rotatio on the -Y axis
                      //NOTE: there are a few rotation involved
                      //      look at the R3BGladMagnet.cxx for details.

class R3BGladMagnet : public R3BModule {
  
public:
  R3BGladMagnet(const char * name, const char *Title="R3BGlad Magnet");
  R3BGladMagnet(const char * name, TString geoFile, const char *Title="R3BGlad Magnet");
  R3BGladMagnet();
  virtual ~R3BGladMagnet();

  void ConstructGeometry();
  
  Bool_t CheckIfSensitive(std::string name);

private:
  ClassDef(R3BGladMagnet,2) //R3BGladMagnet
};

#endif //R3BGladMAGNET_H

