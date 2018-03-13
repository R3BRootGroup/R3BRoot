#ifndef R3BROOT_R3BGEOGDML_H
#define R3BROOT_R3BGEOGDML_H

#include "FairModule.h"
#include "TGeoMatrix.h"

class R3BGeoGDML : public FairModule
{
  public:
    R3BGeoGDML() = default;
    R3BGeoGDML(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());
    explicit R3BGeoGDML(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    void ConstructGeometry() override;

  protected:
    TGeoCombiTrans fCombiTrans;
    ClassDefOverride(R3BGeoGDML, 1)
};

#endif // R3BROOT_R3BGEOGDML_H
