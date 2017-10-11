#include "R3BTarget.h"
#include "R3BTGeoPar.h"

#include "TGeoManager.h"
#include "TGeoTube.h"

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

void R3BTarget::ConstructGeometry()
{
    R3BModule::ConstructGeometry();

    // Position and rotation
    TGeoNode* main_vol = gGeoManager->GetTopVolume()->FindNode("TargetEnveloppe_0");
    TGeoMatrix* matr = main_vol->GetMatrix();
    fTGeoPar->SetPosXYZ(matr->GetTranslation()[0], matr->GetTranslation()[1], matr->GetTranslation()[2]);
    fTGeoPar->SetRotXYZ(0., -TMath::Abs(TMath::ASin(matr->GetRotationMatrix()[2]) * TMath::RadToDeg()), 0.);

    // Dimensions
    TGeoTube* tube = (TGeoTube*)gGeoManager->GetTopVolume()->FindNode("TargetEnveloppe_0")->GetVolume()->GetShape();
    fTGeoPar->SetDimXYZ(tube->GetRmax(), tube->GetRmax(), tube->GetDz());

    fTGeoPar->setChanged();
}

void R3BTarget::SetParContainers()
{
    fTGeoPar = (R3BTGeoPar*) FairRuntimeDb::instance()->getContainer("TargetGeoPar");
}

ClassImp(R3BTarget)
