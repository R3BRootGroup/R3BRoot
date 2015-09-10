#include "R3BTCalModulePar.h"

#include "FairParamList.h" // for FairParamList
#include "FairLogger.h"

using namespace std;

ClassImp(R3BTCalModulePar);

R3BTCalModulePar::R3BTCalModulePar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fModuleId(0)
    , fSide(0)
    , fNofChannels(0)
{
    // Reset all parameters
    clear();
}

R3BTCalModulePar::~R3BTCalModulePar()
{
}

void R3BTCalModulePar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BTCalModulePar::putParams() called" << FairLogger::endl;
    if (!list)
    {
        return;
    }
    list->add("module_id", fModuleId);
    list->add("side", fSide);
    list->add("nofchannels", fNofChannels);
    list->add("bin_low", fBinLow, NCHMAX);
    list->add("bin_up", fBinUp, NCHMAX);
    list->add("slope", fSlope, NCHMAX);
    list->add("offset", fOffset, NCHMAX);
}

Bool_t R3BTCalModulePar::getParams(FairParamList* list)
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
    if (!list->fill("nofchannels", &fNofChannels))
    {
        return kFALSE;
    }
    if (!list->fill("bin_low", fBinLow, NCHMAX))
    {
        return kFALSE;
    }
    if (!list->fill("bin_up", fBinUp, NCHMAX))
    {
        return kFALSE;
    }
    if (!list->fill("slope", fSlope, NCHMAX))
    {
        return kFALSE;
    }
    if (!list->fill("offset", fOffset, NCHMAX))
    {
        return kFALSE;
    }

    return kTRUE;
}

void R3BTCalModulePar::clear()
{
    fModuleId = fSide = fNofChannels = 0;
    // <DB> Not so much overhead here.
    for (Int_t i = 0; i < NCHMAX; i++)
    {
        fBinLow[i] = fBinUp[i] = 0;
        fSlope[i] = 0.;
        fOffset[i] = 0.;
    }
}

void R3BTCalModulePar::printParams()
{
    LOG(INFO) << "   R3BTCalModulePar: Time Calibration Parameters: " << FairLogger::endl;
    LOG(INFO) << "   fBarId: " << fModuleId << FairLogger::endl;
    LOG(INFO) << "   fSide: " << fSide << FairLogger::endl;
    LOG(INFO) << "   fNofChannels: " << fNofChannels << FairLogger::endl;
    for (Int_t i = 0; i < fNofChannels; i++)
    {
        if ((fBinLow[i] != 0) && (fBinUp[i] != 0) && (fSlope[i] != 0))
            LOG(INFO) << "   BinLow: " << fBinLow[i] << " BinUp " << fBinUp[i] << " Slope:" << fSlope[i]
                      << " Offset:" << fOffset[i] << FairLogger::endl;
    }
}

Double_t R3BTCalModulePar::GetTimeTacquila(Int_t tdc)
{
    for (Int_t i = 0; i < fNofChannels; i++)
    {
        if (tdc >= fBinLow[i] && tdc <= fBinUp[i])
        {
            Double_t time = fOffset[i] + fSlope[i] * (Double_t)(tdc - fBinLow[i]);
            return time;
        }
    }
    return -10000.;
}

Double_t R3BTCalModulePar::GetTimeVFTX(Int_t tdc)
{
    for (Int_t i = 0; i < fNofChannels; i++)
    {
        if (tdc == fBinLow[i])
        {
            Double_t time = fOffset[i];
            return time;
        }
    }
    return -10000.;
}
