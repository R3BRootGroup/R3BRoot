#ifndef R3BTARGET_H
#define R3BTARGET_H

#include "R3BModule.h"

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

    ClassDef(R3BTarget, 3)
};

#endif // R3BTARGET_H
