#ifndef R3BTARGET_H
#define R3BTARGET_H

#include "R3BModule.h"

class R3BTGeoPar;

class R3BTarget : public R3BModule
{
  public:
    R3BTarget();

    R3BTarget(const TString& name);

    R3BTarget(const TString& name,
              const TString& geoFile,
              const TGeoTranslation& trans,
              const TGeoRotation& rot = TGeoRotation());

    R3BTarget(const TString& name, const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    virtual void ConstructGeometry();

    ClassDef(R3BTarget, 3)
    void SetParContainers();
  
protected:
    R3BTGeoPar *fTGeoPar; //!
};

#endif // R3BTARGET_H
