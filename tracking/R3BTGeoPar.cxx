#include "R3BTGeoPar.h"

#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList
#include "FairRtdbRun.h"

ClassImp(R3BTGeoPar);

R3BTGeoPar::R3BTGeoPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fPosX(0.)
    , fPosY(0.)
    , fPosZ(0.)
    , fRotX(0.)
    , fRotY(0.)
    , fRotZ(0.)
    , fDimX(0.)
    , fDimY(0.)
    , fDimZ(0.)
{
}

R3BTGeoPar::~R3BTGeoPar() {}

void R3BTGeoPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BTGeoPar::putParams() called" << FairLogger::endl;
    if (!list)
    {
        return;
    }
    list->add("PositionX", fPosX);
    list->add("PositionY", fPosY);
    list->add("PositionZ", fPosZ);
    list->add("RotationX", fRotX);
    list->add("RotationY", fRotY);
    list->add("RotationZ", fRotZ);
    list->add("DimensionX", fDimX);
    list->add("DimensionY", fDimY);
    list->add("DimensionZ", fDimZ);
}

Bool_t R3BTGeoPar::getParams(FairParamList* list)
{
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fill("PositionX", &fPosX))
    {
        return kFALSE;
    }
    if (!list->fill("PositionY", &fPosY))
    {
        return kFALSE;
    }
    if (!list->fill("PositionZ", &fPosZ))
    {
        return kFALSE;
    }
    if (!list->fill("RotationX", &fRotX))
    {
        return kFALSE;
    }
    if (!list->fill("RotationY", &fRotY))
    {
        return kFALSE;
    }
    if (!list->fill("RotationZ", &fRotZ))
    {
        return kFALSE;
    }
    if (!list->fill("DimensionX", &fDimX))
    {
        return kFALSE;
    }
    if (!list->fill("DimensionY", &fDimY))
    {
        return kFALSE;
    }
    if (!list->fill("DimensionZ", &fDimZ))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BTGeoPar::clear() {}

void R3BTGeoPar::printParams()
{
    LOG(INFO) << " -----------  " << GetName() << " Geometry Parameters -------------  " << FairLogger::endl;
    LOG(INFO) << " Position in cave: " << fPosX << " " << fPosY << " " << fPosZ << FairLogger::endl;
    LOG(INFO) << " Rotation in deg: " << fRotX << " " << fRotY << " " << fRotZ << FairLogger::endl;
    LOG(INFO) << " Dimensions: " << fDimX << " " << fDimY << " " << fDimZ << FairLogger::endl;
    LOG(INFO) << " ------------------------------------------------------------------  " << FairLogger::endl;
}
