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

private:
   TGeoCombiTrans *gLobalPos;  //!

public:
    R3BGladMagnet(const char * name, const char *Title="R3BGlad Magnet");
    R3BGladMagnet();
    virtual ~R3BGladMagnet();
    void ConstructGeometry();

    void ConstructSubPartEcrans(TGeoVolume* vol);
    void ConstructDemiEcransTh(TGeoVolume* vol);
    void ConstructEnceinteI(TGeoVolume* pWorld);
    void ConstructEnceinteE(TGeoVolume* pWorld);
    void ConstructFonfE(TGeoVolume* pWorld);
    void ConstructFondS(TGeoVolume* pWorld);
    void ConstructGToles(TGeoVolume* pWorld);

    Bool_t CheckIfSensitive(std::string name);
    ClassDef(R3BGladMagnet,1) //R3BGladMagnet




};

#endif //R3BGladMAGNET_H

