#ifndef R3BVACVESSELCOOL_H
#define R3BVACVESSELCOOL_H

#include "R3BModule.h"

class R3BVacVesselCool : public R3BModule
{
  public:
    R3BVacVesselCool();

    R3BVacVesselCool(const TString& name);

    R3BVacVesselCool(const TString& name,
                     const TString& geoFile,
                     const TGeoTranslation& trans,
                     const TGeoRotation& rot = TGeoRotation());

    R3BVacVesselCool(const TString& name, const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    void ConstructGeometry(); // override;

    ClassDef(R3BVacVesselCool, 3)
    // ClassDefOverride(R3BVacVesselCool, 3)
};

#endif // R3BVACVESSELCOOL_H
