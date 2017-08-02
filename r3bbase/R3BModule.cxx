#include "R3BModule.h"
#include "FairLogger.h"
#include "TGeoManager.h"

R3BModule::R3BModule()
    : FairModule()
{
}

R3BModule::R3BModule(const TString& name, const TString& title, const Bool_t active)
    : FairModule(name, title, active)
{
}

R3BModule::R3BModule(const TString& name,
                     const TString& title,
                     const Bool_t active,
                     const TString& geoFile,
                     const TGeoTranslation& trans,
                     const TGeoRotation& rot)
    : R3BModule(name, title, active, geoFile, { trans, rot })
{
}

R3BModule::R3BModule(const TString& name,
                     const TString& title,
                     const Bool_t active,
                     const TString& geoFile,
                     const TGeoCombiTrans& combi)
    : FairModule(name, title, active)
    , fCombiTrans(combi)
{
    SetGeometryFileName(geoFile);
}

void R3BModule::ConstructGeometry()
{
    if (!GetGeometryFileName().EndsWith(".root"))
    {
        LOG(FATAL) << GetName() << " (which is a " << ClassName() << ") geometry file is not specified!"
                   << FairLogger::endl;
    }
    ConstructRootGeometry();
}

void R3BModule::ConstructRootGeometry()
{
    LOG(INFO) << "R3BModule: Constructing " << GetName() << " (which is a " << ClassName()
              << ") geometry from ROOT file " << GetGeometryFileName() << " ..." << FairLogger::endl;
    FairModule::ConstructRootGeometry();

    if (!fCombiTrans.IsIdentity())
    {
        auto n = gGeoManager->GetTopNode()->GetDaughter(gGeoManager->GetTopNode()->GetNdaughters() - 1);
        ((TGeoNodeMatrix*)n)->SetMatrix(fCombiTrans.MakeClone());
    }
}

ClassImp(R3BModule)
