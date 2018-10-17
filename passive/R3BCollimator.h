#ifndef R3BCOLLIMATOR_H
#define R3BCOLLIMATOR_H

#include "R3BModule.h"

class R3BTGeoPar;

class R3BCollimator : public R3BModule
{
  public:
    R3BCollimator();

    R3BCollimator(const TString& name);

    R3BCollimator(const TString& name,
              const TString& geoFile,
              const TGeoTranslation& trans,
              const TGeoRotation& rot = TGeoRotation());

    R3BCollimator(const TString& name, const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    virtual void ConstructGeometry();

    ClassDef(R3BCollimator, 1)
    void SetParContainers();
  
protected:
    R3BTGeoPar *fTGeoPar; //!
};

#endif // R3BCOLLIMATOR_H
