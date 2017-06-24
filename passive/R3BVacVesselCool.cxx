#include "R3BVacVesselCool.h"
#include "FairLogger.h"

R3BVacVesselCool::R3BVacVesselCool()
    : R3BModule()
{
}

R3BVacVesselCool::R3BVacVesselCool(const TString& name)
    : R3BModule(name, "VacVesselCool", kFALSE)
{
}

R3BVacVesselCool::R3BVacVesselCool(const TString& name,
                                   const TString& geoFile,
                                   const TGeoTranslation& trans,
                                   const TGeoRotation& rot)
    : R3BVacVesselCool(name, geoFile, { trans, rot })
{
}

R3BVacVesselCool::R3BVacVesselCool(const TString& name, const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BModule(name, "VacVesselCool", kFALSE, geoFile, combi)
{
}

void R3BVacVesselCool::ConstructGeometry()
{

    if (TString(GetName()) != TString("LiH"))
    {
        LOG(FATAL) << "VacVesselCool is only supposed to work with LiH Target" << FairLogger::endl;
        // At least if the previous code was interpreted correctly
    }
    R3BModule::ConstructGeometry();
}

ClassImp(R3BVacVesselCool)
