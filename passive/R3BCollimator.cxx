#include "R3BCollimator.h"
#include "R3BTGeoPar.h"

#include "TGeoManager.h"
#include "TGeoTube.h"

R3BCollimator::R3BCollimator()
    : R3BModule()
{
}

R3BCollimator::R3BCollimator(const TString& name)
    : R3BModule(name, "Collimator", kFALSE)
{
}

R3BCollimator::R3BCollimator(const TString& name, const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BCollimator(name, geoFile, { trans, rot })
{
}

R3BCollimator::R3BCollimator(const TString& name, const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BModule(name, "Collimator", kFALSE, geoFile, combi)
{
}

void R3BCollimator::ConstructGeometry()
{
    R3BModule::ConstructGeometry();
}

void R3BCollimator::SetParContainers()
{
}

ClassImp(R3BCollimator)
