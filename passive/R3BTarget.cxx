#include "R3BTarget.h"

R3BTarget::R3BTarget()
    : R3BModule()
{
}

R3BTarget::R3BTarget(const TString& name)
    : R3BModule(name, "Target", kFALSE)
{
}

R3BTarget::R3BTarget(const TString& name, const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BTarget(name, geoFile, { trans, rot })
{
}

R3BTarget::R3BTarget(const TString& name, const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BModule(name, "Target", kFALSE, geoFile, combi)
{
}

ClassImp(R3BTarget)
