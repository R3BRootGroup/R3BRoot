#include "R3BGeoGDML.h"

R3BGeoGDML::R3BGeoGDML(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BGeoGDML(geoFile, { trans, rot })
{
}

R3BGeoGDML::R3BGeoGDML(const TString& geoFile, const TGeoCombiTrans& combi)
    : fCombiTrans(combi)
{
    SetGeometryFileName(geoFile);
}

void R3BGeoGDML::ConstructGeometry() { ConstructGDMLGeometry(dynamic_cast<TGeoMatrix*>(fCombiTrans.Clone())); }
