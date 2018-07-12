#include "R3BTCalModulePar.h"

#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList

#include "TF1.h"
#include "TH1F.h"
#include "TPad.h"

using namespace std;

ClassImp(R3BTCalModulePar);

R3BTCalModulePar::R3BTCalModulePar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fPlane(0)
    , fPaddle(0)
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
    list->add("plane", fPlane);
    list->add("paddle", fPaddle);
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
    if (!list->fill("plane", &fPlane))
    {
        return kFALSE;
    }
    if (!list->fill("paddle", &fPaddle))
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
    fPlane = fPaddle = fSide = fNofChannels = 0;
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
    LOG(INFO) << "   fPlane: " << fPlane << FairLogger::endl;
    LOG(INFO) << "   fPaddle: " << fPaddle << FairLogger::endl;
    LOG(INFO) << "   fSide: " << fSide << FairLogger::endl;
    LOG(INFO) << "   fNofChannels: " << fNofChannels << FairLogger::endl;
    for (Int_t i = 0; i < fNofChannels; i++)
    {
        if ((fBinLow[i] != 0) && (fBinUp[i] != 0) && (fSlope[i] != 0))
            LOG(INFO) << "   BinLow: " << fBinLow[i] << " BinUp " << fBinUp[i] << " Slope:" << fSlope[i]
                      << " Offset:" << fOffset[i] << FairLogger::endl;
    }
}

Double_t R3BTCalModulePar::GetTimeClockTDC(Int_t tdc)
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

Double_t R3BTCalModulePar::GetTimeTacquila(Int_t tdc)
{
	tdc = tdc + 1;
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
        if ((tdc + 1) == fBinLow[i])
        {
            Double_t time = fOffset[i];
            return time;
        }
    }
    return -10000.;
}

void R3BTCalModulePar::DrawParams()
{
    Int_t type = 2; // VFTX
    if (fNofChannels > 0)
    {
        if (fSlope[0] > 0)
        {
            type = 1; // Tacquila
        }
    }
    else
    {
        return;
    }
    TH1F* h1 = new TH1F(Form("h1_%d_%d_%d", fPlane, fPaddle, fSide), "", 4096, -0.5, 4096.5);
    h1->Draw();
    for (Int_t i = 0; i < fNofChannels; i++)
    {
        if (1 == type)
        {
            TF1* f1 = new TF1(Form("f1_%d", i), "[0] + [1]*(x - [2])", fBinLow[i], fBinUp[i]);
            f1->SetParameter(0, fOffset[i]);
            f1->SetParameter(1, fSlope[i]);
            f1->SetParameter(2, fBinLow[i]);
            f1->Draw("same");
        }
        else if (2 == type)
        {
            h1->SetBinContent(1 + fBinLow[i], fOffset[i]);
        }
    }

    if (1 == type)
    {
        h1->GetYaxis()->SetRangeUser(0., 1.2 * fOffset[fNofChannels - 1]);
    }
    else if (2 == type)
    {
        h1->Draw();
    }
    gPad->Update();
}
