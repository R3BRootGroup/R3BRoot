#include "R3BGladMagnet.h"
#include "FairLogger.h"

// NOTE: as for now, these values are the same used
//       for the geometry creation (v17).
//       These will move also old files.
const Double_t __GLAD_POS_DX = -42.0; // offset on the Z axis
const Double_t __GLAD_POS_DY = 0.0;   // offset on the Y axis (not present)
const Double_t __GLAD_POS_DZ = 308.8; // offset on the Z axis (distance from target)
const Double_t __GLAD_ROT = 14;       // rotation on the -Y axis
const TString __GLAD_NAME = "Glad Magnet";

R3BGladMagnet::R3BGladMagnet()
    // NOTE: Delegate Constructor, such that geometry operations are set up.
    : R3BGladMagnet("")
{
}

R3BGladMagnet::R3BGladMagnet(const TString& geoFile)
    : R3BModule(__GLAD_NAME, __GLAD_NAME, kFALSE, geoFile, { __GLAD_POS_DX, __GLAD_POS_DY, __GLAD_POS_DZ })
{
    // TODO: There is probably a better way to solve this
    TGeoRotation rot;
    rot.RotateY(90.0);
    rot.RotateZ(-90.0);
    rot.RotateY(__GLAD_ROT);
    rot.RotateZ(180.0);

    R3BModule::SetRotation(rot);
}

void R3BGladMagnet::SetPosition(const TGeoTranslation&)
{
    LOG(FATAL) << "Moving " << GetName() << " (which is a " << ClassName() << ") is not allowed!" << FairLogger::endl;
}

void R3BGladMagnet::SetRotation(const TGeoRotation&)
{
    LOG(FATAL) << "Rotating " << GetName() << " (which is a " << ClassName() << ") is not allowed!" << FairLogger::endl;
}

ClassImp(R3BGladMagnet)
