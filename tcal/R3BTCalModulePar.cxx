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
    list->add("time", fTime, NCHMAX);
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
    if (!list->fill("time", fTime, NCHMAX))
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
        fTime[i] = 0.;
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
        if ((fBinLow[i] != 0) && (fBinUp[i] != 0) && (fTime[i] != 0))
            LOG(INFO) << "   BinLow: " << fBinLow[i] << " BinUp " << fBinUp[i] << " Time:" << fTime[i]
                      << FairLogger::endl;
    }
}

Bool_t R3BTCalModulePar::Compare(const R3BTCalModulePar& that) const
{
    //
    Bool_t test_h = (fModuleId == that.fModuleId) && (fSide == that.fSide) && (fNofChannels == that.fNofChannels);

    Bool_t test_d = kTRUE;
    for (Int_t i = 0; i < fNofChannels; i++)
    {

        Int_t a = GetBinLowAt(i);
        Int_t b = that.GetBinLowAt(i);
        Int_t c = GetBinUpAt(i);
        Int_t d = that.GetBinUpAt(i);

        Double_t t1 = GetTimeAt(i);
        Double_t t2 = that.GetTimeAt(i);
        if ((a != b) || (c != d) || (t1 != t2))
        {
            test_d = kFALSE;
            break;
        }
    }

    return (test_h && test_d);
}
