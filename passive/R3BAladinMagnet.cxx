#include "R3BAladinMagnet.h"
#include "FairLogger.h"

const Double_t __ALADIN_POS_DX = 0.;
const Double_t __ALADIN_POS_DY = 0.;
const Double_t __ALADIN_POS_DZ = 350. - 95.;
const Double_t __ALADIN_ROT = +7.;
const TString __ALADIN_NAME = "Aladin Magnet";

R3BAladinMagnet::R3BAladinMagnet()
    // NOTE: Delegate Constructor, such that geometry operations are set up.
    : R3BAladinMagnet("")
{
}

R3BAladinMagnet::R3BAladinMagnet(const TString& geoFile)
    : R3BModule(__ALADIN_NAME,
                __ALADIN_NAME,
                kFALSE,
                geoFile,
                { __ALADIN_POS_DX, __ALADIN_POS_DY, __ALADIN_POS_DZ },
                { "", -90., __ALADIN_ROT, 90. })
{
}

void R3BAladinMagnet::SetPosition(const TGeoTranslation&)
{
    LOG(FATAL) << "Moving " << GetName() << " (which is a " << ClassName() << ") is not allowed!";
}

void R3BAladinMagnet::SetRotation(const TGeoRotation&)
{
    LOG(FATAL) << "Rotating " << GetName() << " (which is a " << ClassName() << ") is not allowed!";
}

ClassImp(R3BAladinMagnet)
