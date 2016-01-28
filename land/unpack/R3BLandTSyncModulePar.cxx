#include "R3BLandTSyncModulePar.h"

#include "FairParamList.h" // for FairParamList
#include "FairLogger.h"

using namespace std;

R3BLandTSyncModulePar::R3BLandTSyncModulePar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fModuleId(0)
    , fSide(0)
{
    // Reset all parameters
    clear();
}

R3BLandTSyncModulePar::~R3BLandTSyncModulePar()
{
}

void R3BLandTSyncModulePar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BLandTSyncModulePar::putParams() called" << FairLogger::endl;
    if (!list)
    {
        return;
    }
    list->add("module_id", fModuleId);
    list->add("side", fSide);
}

Bool_t R3BLandTSyncModulePar::getParams(FairParamList* list)
{
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fill("module_id", &fModuleId))
    {
        return kFALSE;
    }
    if (!list->fill("side", &fSide))
    {
        return kFALSE;
    }

    return kTRUE;
}

void R3BLandTSyncModulePar::clear()
{
}

void R3BLandTSyncModulePar::printParams()
{
    LOG(INFO) << "   R3BLandTSyncModulePar: Time Calibration Parameters: " << FairLogger::endl;
    LOG(INFO) << "   fBarId: " << fModuleId << FairLogger::endl;
    LOG(INFO) << "   fSide: " << fSide << FairLogger::endl;
//     LOG(INFO) << "   fNofChannels: " << fNofChannels << FairLogger::endl; //TODO
//     for (Int_t i = 0; i < fNofChannels; i++)
//     {
//         if ((fBinLow[i] != 0) && (fBinUp[i] != 0) && (fSlope[i] != 0))
//             LOG(INFO) << "   BinLow: " << fBinLow[i] << " BinUp " << fBinUp[i] << " Slope:" << fSlope[i]
//                       << " Offset:" << fOffset[i] << FairLogger::endl;
    
}

ClassImp(R3BLandTSyncModulePar);