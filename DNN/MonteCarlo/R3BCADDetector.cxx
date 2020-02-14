#include "R3BCADDetector.h"
#include "FairLogger.h"
#include "TGeoManager.h"

R3BCADDetector::R3BCADDetector()
    : FairDetector()
    , fCutE(1e-3)
{
}

R3BCADDetector::R3BCADDetector(const TString& name, const Int_t detId)
    : FairDetector(name, kTRUE, detId)
    , fCutE(1e-3)
{
    SetVerboseLevel(1);
}

R3BCADDetector::R3BCADDetector(const TString& name,
                         const Int_t detId,
                         const TString& geoFile,
                         const TGeoTranslation& trans,
                         const TGeoRotation& rot)
    : R3BCADDetector(name, detId, geoFile, { trans, rot })
{
}

R3BCADDetector::R3BCADDetector(const TString& name, const Int_t detId, const TString& geoFile, const TGeoCombiTrans& combi)
    : FairDetector(name, kTRUE, detId)
    , fCutE(1e-3)
    , fCombiTrans(combi)
{
    SetVerboseLevel(1);
    SetGeometryFileName(geoFile);
}

void R3BCADDetector::ConstructGeometry()
{
    if (!GetGeometryFileName().EndsWith(".root"))
    {
        LOG(FATAL) << GetName() << " (which is a " << ClassName() << ") geometry file is not specified"
                   << FairLogger::endl;
    }
    ConstructRootGeometry();
}

void R3BCADDetector::ConstructRootGeometry()
{
    LOG(INFO) << "R3BCADDetector: Constructing " << GetName() << " (which is a " << ClassName()
              << ") geometry from ROOT file " << GetGeometryFileName() << " ..." << FairLogger::endl;
    FairModule::ConstructRootGeometry();

    if (!fCombiTrans.IsIdentity())
    {
        auto n = gGeoManager->GetTopNode()->GetDaughter(gGeoManager->GetTopNode()->GetNdaughters() - 1);
        ((TGeoNodeMatrix*)n)->SetMatrix(fCombiTrans.MakeClone());
    }
}

ClassImp(R3BCADDetector)
