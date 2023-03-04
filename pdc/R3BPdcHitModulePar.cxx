/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BPdcHitModulePar.h"

#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList

#include "TF1.h"
#include "TH1F.h"
#include "TPad.h"

using namespace std;

ClassImp(R3BPdcHitModulePar);

R3BPdcHitModulePar::R3BPdcHitModulePar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fWire(0)
    , fdTmin(0)
    , fdTmax(0)
    , fTmin(0)
    , fTmax(0)
    , fnpoints(0)
    , fXT_xArray()
    , fXT_tArray()
{
    // Reset all parameters
    clear();
}

R3BPdcHitModulePar::~R3BPdcHitModulePar() {}

void R3BPdcHitModulePar::putParams(FairParamList* list)
{
    LOG(info) << "R3BPdcHitModulePar::putParams() called";
    if (!list)
    {
        return;
    }
    list->add("wire", fWire);
    /*
        list->add("OffsetUp", fOffsetUp);
        list->add("OffsetDown", fOffsetDown);
        list->add("Veff", fOffset2);
    */
}

Bool_t R3BPdcHitModulePar::getParams(FairParamList* list)
{
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fill("wire", &fWire))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BPdcHitModulePar::clear() { fWire = 0; }

void R3BPdcHitModulePar::SetXT_xArray(Double_t* XT_xArray, Int_t isize)
{
    for (Int_t i = 0; i < isize; i++)
    {
        fXT_xArray[i] = XT_xArray[i];
    }
}
void R3BPdcHitModulePar::SetXT_tArray(Double_t* XT_tArray, Int_t isize)
{
    for (Int_t i = 0; i < isize; i++)
    {
        fXT_tArray[i] = XT_tArray[i];
    }
}
const Double_t* R3BPdcHitModulePar::GetXT_xArray() const { return fXT_xArray; }
const Double_t* R3BPdcHitModulePar::GetXT_tArray() const { return fXT_tArray; }
void R3BPdcHitModulePar::printParams()
{
    LOG(info) << "   R3BPdcHitModulePar: FIBER HIT Calibration Parameters: ";
    LOG(info) << "   fWire: " << fWire;
    LOG(info) << "   fdTmin: " << fdTmin;
    LOG(info) << "   fdTmax: " << fdTmax;
    LOG(info) << "   fTmin: " << fTmin;
    LOG(info) << "   fTmax: " << fTmax;
    LOG(info) << "   fSync: " << fSync;
    LOG(info) << " fnpoints: " << fnpoints;
    for (Int_t i = 0; i < fnpoints; i++)
    {
        LOG(info) << " t_ns & x_mm: " << i << ", " << fXT_tArray[i] << ", " << fXT_xArray[i];
    }
}

void R3BPdcHitModulePar::DrawParams()
{
    /*    Int_t type = 2; // VFTX
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
                h1->SetBinContent(fBinLow[i], fOffset[i]);
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
        gPad->Update();*/
}
