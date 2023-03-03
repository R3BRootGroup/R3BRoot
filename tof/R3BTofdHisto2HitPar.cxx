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
// -----------------------------------------------------
// -----            R3BTofdHisto2HitPar            -----
// -----        Created Jul 2019 by L.Bott         -----
// -----------------------------------------------------

/* Some notes:
 * In order to generate input for this run:
 * TofdCal2Histo
 */

#include "R3BTofdHisto2HitPar.h"
#include "R3BEventHeader.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
#include "R3BTofdCalData.h"
#include "R3BTofdHitPar.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"
#include "R3BLogger.h"

#include "TCanvas.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TImage.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMath.h"
#include "TProfile.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"

#include <iostream>
#include <limits>
#include <stdlib.h>

#include "TSystem.h"

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 20; // nanoseconds.
} // namespace

TFile* histofilename;

R3BTofdHisto2HitPar::R3BTofdHisto2HitPar()
    : FairTask("R3BTofdHisto2HitPar", 1)
    , fCalItemsLos(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fParameter(1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTofdY(0.)
    , fTofdQ(0.)
    , fTofdCutLow(0.)
    , fTofdCutHigh(0.)
    , fTofdSmiley(true)
    , fParaFile("")
    , fHistoFile("")
{
}

R3BTofdHisto2HitPar::R3BTofdHisto2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fParameter(1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTofdY(0.)
    , fTofdQ(0.)
    , fTofdCutLow(0.)
    , fTofdCutHigh(0.)
    , fTofdSmiley(true)
    , fParaFile("")
    , fHistoFile("")
{
}

R3BTofdHisto2HitPar::~R3BTofdHisto2HitPar()
{
    if (fCal_Par)
    {
        delete fCal_Par;
    }
}

InitStatus R3BTofdHisto2HitPar::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    header = dynamic_cast<R3BEventHeader*>(rm->GetObject("EventHeader."));
    if (header)
        R3BLOG(info, "EventHeader. was found");
    else
        R3BLOG(info, "EventHeader. was not found");

    // may be = NULL!
    fCalData = (TClonesArray*)rm->GetObject("TofdCal");
    if (!fCalData)
    {
        return kFATAL;
    }
    if (!fNofModules)
    {
        LOG(error) << "R3BTofdHisto2HitPar::Init() Number of modules not set. ";
        return kFATAL;
    }
    // fCalItemsLos = (TClonesArray*)rm->GetObject("LosCal");
    // if (NULL == fCalItemsLos)
    //    LOG(fatal) << "Branch LosCal not found";
    LOG(info) << "Histo filename: " << fHistoFile;
    histofilename = TFile::Open(fHistoFile);
    if (histofilename == 0)
    {
        LOG(error) << "Cannot open Histo file!";
        return kFATAL;
    }
    return kSUCCESS;
}

void R3BTofdHisto2HitPar::SetParContainers()
{
    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCal_Par = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fCal_Par)
    {
        LOG(error) << "R3BTofdHisto2HitPar::Init() Couldn't get handle on TofdHitPar. ";
    }
    //  fCal_Par->setChanged();
}

void R3BTofdHisto2HitPar::Exec(Option_t* option) {}

void R3BTofdHisto2HitPar::FinishEvent() {}

void R3BTofdHisto2HitPar::FinishTask()
{
    if (fParameter == 1)
    {
        // Determine time offset of the 2 PMTs of one paddle. This procedure
        // assumes a sweep run in the middle of the ToF wall horizontally.
        // Since all paddles are mounted vertically one can determine the offset.
        // Half of the offset is added to PM1 and half to PM2.
        LOG(warning) << "Calling function calcOffset";
        calcOffset();
        // Determine ToT offset between top and bottom PMT
        LOG(warning) << "Calling function calcToTOffset";
        calcToTOffset(fTofdCutLow, fTofdCutHigh);
        // Determine sync offset between paddles
        LOG(warning) << "Calling function calcSync";
        calcSync();
        LOG(info) << "Call walk correction before next step!";
    }

    if (fParameter == 2)
    {
        // Determine effective speed of light in [cm/s] for each paddle
        LOG(warning) << "Calling function calcVeff";
        calcVeff();
        // Determine light attenuation lambda for each paddle
        LOG(warning) << "Calling function calcLambda";
        calcLambda(fTofdCutLow, fTofdCutHigh);
    }

    if (fParameter == 3)
    {
        // calculation of position dependend charge
        if (fTofdSmiley)
        {
            LOG(warning) << "Calling function smiley";
            Double_t para2[4];
            Double_t min2 = -50.; // -40 effective bar length
            Double_t max2 = 50.;  // 40 effective bar length = 80 cm
                                  // we will use 50 here for some fit safety margin
            for (Int_t i = 0; i < fNofPlanes; i++)
            {
                for (Int_t j = 0; j < fPaddlesPerPlane; j++) //
                {
                    if (histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)))
                    {
                        LOG(warning) << "Calling Plane " << i + 1 << " Bar " << j + 1;
                        R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                        smiley((TH2F*)histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)),
                               min2,
                               max2,
                               para2,
                               i,
                               j);
                        Double_t offset1 = par->GetOffset1();
                        Double_t offset2 = par->GetOffset2();
                        Double_t veff = par->GetVeff();
                        Double_t sync = par->GetSync();
                        par->SetPola(para2[0]);
                        par->SetPolb(para2[1]);
                        par->SetPolc(para2[2]);
                        par->SetPold(para2[3]);
                    }
                }
            }
            fCal_Par->setChanged();
        }
        else
        {
            LOG(warning) << "Calling function doubleExp";
            Double_t para[4];
            Double_t min = -50.; // effective bar length
            Double_t max = 50.;  // effective bar length = 80 cm

            for (Int_t i = 0; i < fNofPlanes; i++)
            {
                for (Int_t j = 0; j < fPaddlesPerPlane; j++)
                {
                    if (histofilename->Get(Form("Tot1_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
                    {
                        R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                        doubleExp((TH2F*)histofilename->Get(Form("Tot1_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)),
                                  min,
                                  max,
                                  para,
                                  i,
                                  j,
                                  1);
                        Double_t offset1 = par->GetOffset1();
                        Double_t offset2 = par->GetOffset2();
                        Double_t veff = par->GetVeff();
                        Double_t sync = par->GetSync();
                        par->SetPar1a(para[0]);
                        par->SetPar1b(para[1]);
                        par->SetPar1c(para[2]);
                        par->SetPar1d(para[3]);
                    }
                    if (histofilename->Get(Form("Tot2_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
                    {
                        R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                        doubleExp((TH2F*)histofilename->Get(Form("Tot2_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)),
                                  min,
                                  max,
                                  para,
                                  i,
                                  j,
                                  2);
                        Double_t offset1 = par->GetOffset1();
                        Double_t offset2 = par->GetOffset2();
                        Double_t veff = par->GetVeff();
                        Double_t sync = par->GetSync();
                        par->SetPar2a(para[0]);
                        par->SetPar2b(para[1]);
                        par->SetPar2c(para[2]);
                        par->SetPar2d(para[3]);
                    }
                }
            }
            fCal_Par->setChanged();
        }
    }

    if (fParameter == 4)
    {
        // Z correction for each plane
        LOG(warning) << "Calling function zcorr";
        TCanvas* czcorr = new TCanvas("czcorr", "czcorr", 100, 100, 800, 800);
        Double_t para[8];
        Double_t pars[3];
        Int_t min = fTofdCutLow, max = fTofdCutHigh; // select range for peak search
        // for (Int_t i = 0; i < fNofPlanes; i++)
        for (Int_t i = 1; i < 2; i++)
        {
            // for (Int_t j = 0; j < fPaddlesPerPlane; j++) //
            for (Int_t j = 37; j < 41; j++) //
            {
                if (histofilename->Get(Form("Q_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
                {
                    R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                    std::cout << "Calling Plane: " << i + 1 << " Bar " << j + 1 << "\n";
                    zcorr((TH2F*)histofilename->Get(Form("Q_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)),
                          min,
                          max,
                          pars,
                          i,
                          j,
                          czcorr);
                    Double_t offset1 = par->GetOffset1();
                    Double_t offset2 = par->GetOffset2();
                    Double_t veff = par->GetVeff();
                    Double_t sync = par->GetSync();
                    para[0] = par->GetPar1a();
                    para[1] = par->GetPar1b();
                    para[2] = par->GetPar1c();
                    para[3] = par->GetPar1d();
                    para[4] = par->GetPar2a();
                    para[5] = par->GetPar2b();
                    para[6] = par->GetPar2c();
                    para[7] = par->GetPar2d();
                    std::cout << "Write parameter: " << pars[0] << " " << pars[1] << " " << pars[2] << "\n";
                    par->SetPar1za(pars[0]);
                    par->SetPar1zb(pars[1]);
                    par->SetPar1zc(pars[2]);
                    czcorr->Clear();
                }
            }
        }
        fCal_Par->setChanged();
    }
}

void R3BTofdHisto2HitPar::calcOffset()
{
    TCanvas* cOffset = new TCanvas("cOffset", "cOffset", 10, 10, 1000, 900);
    cOffset->Divide(2, 2);
    R3BTofdHitModulePar* mpar;
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        if (histofilename->Get(Form("Time_Diff_Plane_%i", i + 1)))
        {
            LOG(warning) << "Found histo Time_Diff_Plane_" << i + 1;
            auto* h = (TH2F*)histofilename->Get(Form("Time_Diff_Plane_%i", i + 1))->Clone();
            for (Int_t j = 0; j < fPaddlesPerPlane; j++)
            {
                mpar = new R3BTofdHitModulePar();
                Double_t offset = 0.;
                cOffset->cd(i + 1);
                h->Draw("colz");
                TH1F* histo_py = (TH1F*)h->ProjectionY("histo_py", j + 2, j + 2, "");
                histo_py->Rebin(4);
                Int_t binmax = histo_py->GetMaximumBin();
                Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
                TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.3, Max + 0.3);
                histo_py->Fit("fgaus", "QR0");
                offset = fgaus->GetParameter(1); // histo_py->GetXaxis()->GetBinCenter(binmax);
                LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " Offset  " << offset;
                mpar->SetPlane(i + 1);
                mpar->SetPaddle(j + 1);
                mpar->SetOffset1(-offset / 2.);
                mpar->SetOffset2(offset / 2.);
                fCal_Par->AddModulePar(mpar);
            }
        }
    }
    fCal_Par->setChanged();
}
void R3BTofdHisto2HitPar::calcToTOffset(Double_t totLow, Double_t totHigh)
{
    TCanvas* cToTOffset = new TCanvas("cToTOffset", "cToTOffset", 10, 10, 1000, 900);
    cToTOffset->Divide(1, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        for (Int_t j = 0; j < fPaddlesPerPlane; j++)
        {
            Double_t offset = 0.;
            R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
            if (histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)))
            {
                LOG(warning) << "Found histo SqrtQ_vs_PosToT_Plane_" << i + 1 << "_Bar_" << j + 1;
                auto* h = (TH2F*)histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1))->Clone();
                cToTOffset->cd(1);
                h->Draw("colz");
                auto* histo_py = (TH2F*)h->ProjectionX("histo_py", totLow, totHigh, "");
                cToTOffset->cd(2);
                histo_py->Rebin(2);
                histo_py->Draw();
                Int_t binmax = histo_py->GetMaximumBin();
                Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
                TF1* fgaus = new TF1(
                    "fgaus", "gaus(0)", Max - .7, Max + .7); // new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
                histo_py->Fit("fgaus", "QR0");
                offset = fgaus->GetParameter(1);
                fgaus->Draw("SAME");
                histo_py->SetAxisRange(Max - 1.4, Max + 1.4, "X");
                h->SetAxisRange(Max - 1.4, Max + 1.4, "X");
                h->SetAxisRange(totLow, totHigh, "Y");
                std::cout << "Mean: " << offset << "\n";
                cToTOffset->Update();
                gPad->WaitPrimitive();
                delete fgaus;
                delete h;
                delete histo_py;
            }
            LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " ToT Offset  " << offset << "\n";
            par->SetToTOffset1(sqrt(exp(offset)));
            par->SetToTOffset2(1. / sqrt(exp(offset)));
        }
    }
    fCal_Par->setChanged();
}

void R3BTofdHisto2HitPar::calcSync()
{
    TCanvas* cSync = new TCanvas("cSync", "cSync", 10, 10, 1000, 900);
    cSync->Divide(2, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        if (histofilename->Get(Form("Time_Sync_Plane_%i", i + 1)))
        {
            LOG(warning) << "Found histo Time_Sync_Plane_" << i + 1;
            auto* h = (TH2F*)histofilename->Get(Form("Time_Sync_Plane_%i", i + 1))->Clone();
            for (Int_t j = 0; j < fPaddlesPerPlane; j++)
            {
                cSync->cd(i + 1);
                h->Draw("colz");
                auto* histo_py = (TH1F*)h->ProjectionY("histo_py", j + 2, j + 2, "");
                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                Int_t binmax = histo_py->GetMaximumBin();
                Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
                Double_t MaxEntry = histo_py->GetBinContent(binmax);
                TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 10., Max + 10.);
                fgaus->SetParameters(MaxEntry, Max, 20);
                histo_py->Fit("fgaus", "QR0");
                Double_t sync = fgaus->GetParameter(1); // histo_py->GetXaxis()->GetBinCenter(binmax);
                par->SetSync(sync);
                LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " Sync  " << sync;
            }
        }
    }
    fCal_Par->setChanged();
}

void R3BTofdHisto2HitPar::calcVeff()
{
    TCanvas* cVeff = new TCanvas("cVeff", "cVeff", 10, 10, 1000, 900);
    cVeff->Divide(2, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        for (Int_t j = 0; j < fPaddlesPerPlane; j++)
        {
            Double_t max = 0.;
            Double_t veff = 7.;
            if (histofilename->Get(Form("Time_Diff_Plane_%i", i + 1)))
            {
                LOG(warning) << "Found histo Time_Diff_Plane_" << i + 1;
                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                if (!par)
                {
                    LOG(info) << "Hit par not found, Plane: " << i + 1 << ", Bar: " << j + 1;
                    continue;
                }
                auto* h = (TH2F*)histofilename->Get(Form("Time_Diff_Plane_%i", i + 1))->Clone();
                cVeff->cd(i + 1);
                h->Draw("colz");
                TH1F* histo_py = (TH1F*)h->ProjectionY("histo_py", j + 2, j + 2, "");
                Int_t binmax = histo_py->GetMaximumBin();
                max = histo_py->GetXaxis()->GetBinCenter(binmax);
                Double_t maxEntry = histo_py->GetBinContent(binmax);
                auto* fgaus = new TF1("fgaus", "gaus(0)", max - 0.3, max + 0.3); /// TODO: find best range
                fgaus->SetParameters(maxEntry, max, 20);
                histo_py->Fit("fgaus", "QR0");
                Double_t offset1 = par->GetOffset1();
                Double_t offset2 = par->GetOffset2();
                Double_t sync = par->GetSync();
                max = fgaus->GetParameter(1) + offset1 - offset2; /// TODO: needs to be tested
                // max = max+offset1-offset2;
                veff = fTofdY / max; // effective speed of light in [cm/s]
                LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " offset  " << par->GetOffset1();
                LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " max  " << max;
                LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " veff  " << veff;
                par->SetVeff(veff);
            }
        }
    }
    fCal_Par->setChanged();
}
void R3BTofdHisto2HitPar::calcLambda(Double_t totLow, Double_t totHigh)
{
    TCanvas* cToTOffset = new TCanvas("cLambda", "cLambda", 10, 10, 1000, 900);
    cToTOffset->Divide(1, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        for (Int_t j = 0; j < fPaddlesPerPlane; j++)
        {
            Double_t offset = 0.;
            R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
            if (histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)))
            {
                LOG(warning) << "Found histo SqrtQ_vs_PosToT_Plane_" << i + 1 << "_Bar_" << j + 1;
                auto* h = (TH2F*)histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1))->Clone();
                cToTOffset->cd(1);
                h->Draw("colz");
                auto* histo_py = (TH2F*)h->ProjectionX("histo_py", totLow, totHigh, "");
                cToTOffset->cd(2);
                histo_py->Draw();
                Int_t binmax = histo_py->GetMaximumBin();
                Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
                TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
                histo_py->Fit("fgaus", "QR0");
                offset = fgaus->GetParameter(1);
                fgaus->Draw("SAME");
                histo_py->SetAxisRange(Max - .5, Max + .5, "X");
                h->SetAxisRange(Max - .5, Max + .5, "X");
                h->SetAxisRange(totLow, totHigh, "Y");
                cToTOffset->Update();
                delete fgaus;
                delete h;
                delete histo_py;
            }
            else
                LOG(error) << "Missing histo plane " << i + 1 << " bar " << j + 1;
            Double_t lambda = fTofdY / offset;
            LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " ToT Offset  " << offset << " Lambda " << lambda
                         << "\n";
            par->SetLambda(lambda);
        }
    }
    fCal_Par->setChanged();
}
void R3BTofdHisto2HitPar::doubleExp(TH2F* histo, Double_t min, Double_t max, Double_t* para, Int_t p, Int_t b, Int_t s)
{
    // This fits the exponential decay of the light in a paddle. The 2 PMTs are fit with the same function but one
    // side will deliver negative attenuation parameters and the other positive.
    Double_t y[1000], x[1000];
    Int_t n = 0;
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = 0;
    }
    TGraph* gr1 = new TGraph();
    TGraph* gr2 = new TGraph();
    TCanvas* cfit_exp = new TCanvas("cfit_exp", "fit exponential", 100, 100, 800, 800);
    cfit_exp->Clear();
    cfit_exp->Divide(1, 3);
    cfit_exp->cd(1);
    TH2F* histo1 = (TH2F*)histo->Clone();
    TH2F* histo2 = (TH2F*)histo->Clone();
    histo1->Draw("colz");
    histo1->RebinY(2);
    histo1->SetAxisRange(fTofdCutLow, fTofdCutHigh, "Y");
    cfit_exp->cd(2);
    for (Int_t i = 1; i < histo1->GetNbinsX() - 1; i++)
    {
        TH1F* histo_py = (TH1F*)histo1->ProjectionY("histo_py", i, i, "");
        histo_py->Draw();
        histo_py->SetAxisRange(fTofdCutLow, fTofdCutHigh, "X");
        x[n] = histo1->GetXaxis()->GetBinCenter(i);
        Int_t binmax = histo_py->GetMaximumBin();
        y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
        if ((x[n] < -40. || x[n] > 40.)) // || y[n] < 40.)
        {
            delete histo_py;
            continue;
        }
        if (histo_py->GetMaximum() > 20)
            n++;
        delete histo_py;
    }
    gr1 = new TGraph(n, x, y);
    gr1->SetTitle("First guess;pos in cm;ToT in ns");
    gr1->Draw("A*");
    TF1* f1 = new TF1("f1", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
    f1->SetParameters(520., 0.001, 17234, -485.);
    f1->SetLineColor(kGreen);
    gr1->Fit("f1", "", "", min, max);
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = f1->GetParameter(j);
        std::cout << "Parameter: " << para[j] << "\n";
    }
    // fit again but with more information and better cuts
    n = 0;
    cfit_exp->cd(3);
    for (Int_t i = 1; i < histo2->GetNbinsX(); i++)
    {
        Double_t pos = histo2->GetXaxis()->GetBinCenter(i);
        Double_t ymean = para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3];
        histo2->SetAxisRange(ymean - 5., ymean + 5., "Y");
        histo2->Draw("colz");
        TH1F* histo_py = (TH1F*)histo2->ProjectionY("histo_py", i, i, "");
        histo_py->Draw();
        x[n] = histo2->GetXaxis()->GetBinCenter(i);
        Int_t binmax = histo_py->GetMaximumBin();
        y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
        if (histo_py->GetMaximum() > 2)
            n++;
        delete histo_py;
    }
    gr2 = new TGraph(n, x, y);
    gr2->SetTitle("More information and better cuts;pos in cm;ToT in ns");
    gr2->Draw("A*");
    TF1* f2 = new TF1("f2", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
    f2->SetParameters(para[0], para[1], para[2], para[3]);
    f2->SetLineColor(kRed);
    gr2->Fit("f2", "", "", min, max);
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = f2->GetParameter(j);
        std::cout << "Parameter: " << para[j] << "\n";
    }
    cfit_exp->cd(1);
    f1->Draw("SAME");
    f2->Draw("SAME");
    cfit_exp->Update();
    // TImage *img = TImage::Create();
    // img->FromPad(cfit_exp);
    // img->WriteImage(Form("./calib/dexp/dexp_%i_%i_%i.png",p,b,s));
    // gPad->WaitPrimitive();
    gSystem->Sleep(3000);
    delete gr1;
    delete gr2;
    delete f1;
    delete f2;
    delete cfit_exp;
}
void R3BTofdHisto2HitPar::smiley(TH2F* histo, Double_t min, Double_t max, Double_t* para, Int_t p, Int_t b)
{
    // This fits the smiley: Sqrt(q1*q2) returns position dependent charge, we fit that via pol3 and try to correct
    Double_t y[1000], x[1000];
    Int_t n = 0;
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = 0;
    }
    TGraph* gr1 = new TGraph();
    TGraph* gr2 = new TGraph();
    TCanvas* cfit_smiley = new TCanvas("cfit_smiley", "fit smiley", 100, 100, 800, 800);
    cfit_smiley->Clear();
    cfit_smiley->Divide(1, 4);
    cfit_smiley->cd(1);
    TH2F* histo1 = (TH2F*)histo->Clone();
    histo1->Draw("colz");
    TH2F* histo2 = (TH2F*)histo->Clone();
    histo2->RebinX(50);
    histo2->GetYaxis()->SetRangeUser(fTofdCutLow, fTofdCutHigh);
    // histo2->SetAxisRange(fTofdCutLow,fTofdCutHigh,"Y");
    cfit_smiley->cd(2);
    histo2->Draw("colz");
    std::cout << "Searching for points to fit...\n";
    for (Int_t i = 1; i < histo2->GetNbinsX(); i++)
    {
        // std::cout<<"Bin "<<i<<" of "<<histo2->GetNbinsX()<<" with cut: "<<fTofdCutLow<<" < sqrt(q1*q2) <
        // "<<fTofdCutHigh<<"\n";
        cfit_smiley->cd(2);
        TLine* l = new TLine(
            histo2->GetXaxis()->GetBinCenter(i), fTofdCutLow, histo2->GetXaxis()->GetBinCenter(i), fTofdCutHigh);
        l->SetLineColor(kRed);
        l->SetLineWidth(2.);
        l->Draw();
        cfit_smiley->cd(3);
        TH1F* histo_py = (TH1F*)histo2->ProjectionY("histo_py", i, i, "");
        histo_py->Draw();
        // cfit_smiley->Update();
        x[n] = histo2->GetXaxis()->GetBinCenter(i);
        Int_t binmax = histo_py->GetMaximumBin();
        y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);

        if ((x[n] < min || x[n] > max) || (y[n] < fTofdCutLow || y[n] > fTofdCutHigh))
        {
            delete histo_py;
            continue;
        }
        if (histo_py->GetMaximum() > 5)
        {
            n++;
            delete l;
        }
        delete histo_py;
    }
    gr1 = new TGraph(n, x, y);
    gr1->SetTitle("Points found for fitting;y position in cm; sqrt(tot1*tot2)");
    gr1->Draw("A*");
    std::cout << "Start fitting\n";
    TF1* f1 = new TF1("f1", "pol3", min, max);
    f1->SetLineColor(kGreen);
    gr1->Fit("f1", "Q", "", min, max);
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = f1->GetParameter(j);
        std::cout << "Parameter: " << para[j] << "\n";
    }
    // fit again but with more information and better cuts
    std::cout << "Fit again with more information\n";
    n = 0;
    cfit_smiley->cd(4);
    for (Int_t i = 1; i < histo2->GetNbinsX(); i++)
    {
        Double_t pos = histo2->GetXaxis()->GetBinCenter(i);
        Double_t ymean = f1->Eval(pos);
        histo2->SetAxisRange(ymean - 5., ymean + 5., "Y");
        histo2->Draw("colz");
        TH1F* histo_py = (TH1F*)histo2->ProjectionY("histo_py", i, i, "");
        histo_py->Draw();
        x[n] = histo2->GetXaxis()->GetBinCenter(i);
        Int_t binmax = histo_py->GetMaximumBin();
        y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
        if (histo_py->GetMaximum() > 2)
            n++;
        delete histo_py;
    }
    gr2 = new TGraph(n, x, y);
    gr2->SetTitle("More information;y position in cm;sqrt(q1*q2)");
    gr2->Draw("A*");
    f1->DrawCopy("SAME");
    TF1* f2 = new TF1("f2", "pol3", min, max);
    f2->SetParameters(para[0], para[1], para[2], para[3]);
    f2->SetLineColor(kRed);
    gr2->Fit("f2", "0Q", "", min, max);
    f2->Draw("SAME");
    std::cout << "Will write:\n";
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = f2->GetParameter(j);
        std::cout << "Parameter: " << para[j] << "\n";
    }
    histo2->GetYaxis()->SetRangeUser(fTofdCutLow, fTofdCutHigh);
    auto legend = new TLegend(.9, 0.7, .99, 0.9);
    legend->AddEntry("f1", "First Fit", "l");
    legend->AddEntry("f2", "Second Fit", "l");
    legend->Draw();
    cfit_smiley->cd(2);
    f1->Draw("SAME");
    f2->Draw("SAME");
    cfit_smiley->cd(1);
    f1->Draw("SAME");
    f2->Draw("SAME");
    cfit_smiley->Update();
    TImage* img = TImage::Create();
    img->FromPad(cfit_smiley);
    img->WriteImage(Form("./calib/pol3/pol3_%i_%i.png", p, b));
    // gPad->WaitPrimitive();
    gSystem->Sleep(3000);
    delete histo1;
    delete histo2;
    delete gr1;
    delete gr2;
    delete f1;
    delete f2;
    delete cfit_smiley;
}
void R3BTofdHisto2HitPar::zcorr(TH2F* histo, Int_t min, Int_t max, Double_t* pars, Int_t pl, Int_t b, TCanvas* czcorr)
{
    Double_t par[3000] = { 0 };
    Int_t maxplane = 1, maxbar = 44, nPeaks = 180, fNPeaks = 0;
    Double_t x[3000] = { 0 };
    // TCanvas* czcorr = new TCanvas("czcorr", "czcorr", 100, 100, 800, 800);
    czcorr->Divide(1, 3);
    czcorr->cd(1);
    auto* h = (TH2F*)histo->Clone();
    h->Draw("colz");
    h->SetAxisRange(-40., 40., "X");
    h->SetAxisRange(min, max, "Y");
    // Projection of charge axis
    auto* h1 = h->ProjectionY("p_y");
    czcorr->cd(2);
    h1->Draw();
    gPad->SetLogy();
    // Use TSpectrum to find the peak candidates
    TSpectrum* s = new TSpectrum(nPeaks);
    Int_t nfound = s->Search(h1, 1, "", 0.001); // lower threshold than default 0.05
    Double_t* xpeaks = new Double_t[nfound];
    std::cout << "Found " << nfound << " candidate peaks to fit\n";
    czcorr->Update();
    // Eliminate background peaks
    nPeaks = 0;
    xpeaks = s->GetPositionX();
    for (Int_t p = 0; p < nfound; p++)
    {
        Double_t xp = xpeaks[p];
        Int_t bin = h1->GetXaxis()->FindBin(xp);
        Double_t yp = h1->GetBinContent(bin);
        if (yp - TMath::Sqrt(yp) < 1.)
        {
            // std::cout<<"peak @ "<<xp<<" to small, continue\n";
            continue;
        }
        // std::cout<<"peak @ "<<xp<<"\n";
        par[2 * nPeaks] = yp;
        par[2 * nPeaks + 1] = xp;
        nPeaks++;
    }
    if (nPeaks < 2)
    {
        string doagain = "y";
        Double_t xus = par[2 * nPeaks + 1];
        do
        {
            for (Int_t i = 0; i < nPeaks; i++)
            {
                std::cout << "Not enough peaks\nGive charge of main peak:\n";
                std::cout << "Peak @ " << par[2 * nPeaks + 1];
                Double_t z = 0;
                while ((std::cout << " corresponds to Z=") && !(std::cin >> z))
                {
                    std::cout << "That's not a number;";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                if (z != 0)
                    xus = (Double_t)z;
            }
            std::cout << "Do again? (y/n) ";
            std::cin >> doagain;
        } while (doagain != "n");
        pars[0] = xus / par[2 * nPeaks + 1];
        pars[1] = 0.;
        pars[2] = 1;
        return;
    }

    Double_t peaks[nPeaks];
    for (Int_t i = 0; i < nPeaks; i++)
    {
        peaks[i] = 0;
    }
    // Double_t peaks[nPeaks] = {0};

    for (Int_t i = 0; i < nPeaks; i++)
    {
        printf("Found peak @ %f\n", xpeaks[i]);
        peaks[i] = par[2 * i + 1];
        TLine* l = new TLine(par[2 * i + 1], 0, par[2 * i + 1], par[2 * i]);
        l->SetLineColor(kRed);
        l->SetLineWidth(2.);
        l->Draw();
    }
    czcorr->Update();

    // select useful peaks
    sort(peaks, peaks + nPeaks);
    Double_t zpeaks[3000] = { 0 };
    string doagain = "y";
    Int_t nfp;
    do
    {
        nfp = 0;
        for (Int_t i = 0; i < nPeaks; i++)
        {
            std::cout << "Peak @ " << peaks[i];
            Double_t z = 0;
            while ((std::cout << " corresponds to Z=") && !(std::cin >> z))
            {
                std::cout << "That's not a number;";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            if (z == 0)
            {
                Float_t xl = peaks[i];
                Int_t bin = h1->GetXaxis()->FindBin(xl);
                Float_t yl = h1->GetBinContent(bin);
                TLine* l = new TLine(xl, 0, xl, yl);
                l->SetLineColor(kWhite);
                l->SetLineWidth(2.);
                l->Draw();
                czcorr->Update();
                continue;
            }
            x[nfp] = (Double_t)z;
            zpeaks[nfp] = peaks[i];
            Float_t xl = peaks[i];
            Int_t bin = h1->GetXaxis()->FindBin(xl);
            Float_t yl = h1->GetBinContent(bin);
            TLine* l = new TLine(xl, 0, xl, yl);
            l->SetLineColor(kBlue);
            l->SetLineWidth(2.);
            l->Draw();
            czcorr->Update();
            // std::cout<<"z real " << x[nfp] << " z found " << zpeaks[nfp] <<"\n";
            nfp++;
        }
        std::cout << "Do again? (y/n) ";
        std::cin >> doagain;
    } while (doagain != "n");
    if (nfp < 2)
    {
        delete s;
        // delete czcorr;
        std::cout << "Not enough Peaks to fit! Will set charge to main peak.\n";
        if (zpeaks[0] != 0)
            pars[0] = x[0] / zpeaks[0];
        else
            pars[0] = 1;
        pars[1] = 0.;
        pars[2] = 1.;
        // std::cout<<Form("par%i= ",j)<<pars[j]<<"\n";
        return;
    }
    // fit charge axis
    std::cout << "Selected " << nfp << " useful peaks to fit\nStart fitting...\n";
    auto* gr1 = new TGraph();
    TF1* fitz = new TF1("fitz", "[0]*TMath::Power(x,[2])+[1]", min, max);
    fitz->SetParameters(1.5, 2., .1);
    czcorr->cd(3);
    gr1 = new TGraph(nfp, zpeaks, x);
    gr1->Draw("A*");
    gr1->Fit("fitz", "Q", "", min, max);
    czcorr->Update();
    // write parameters
    std::cout << "Optimise the fit, double click to finish\n";
    // gPad->WaitPrimitive();
    auto* fitzr = gr1->GetFunction("fitz");
    for (Int_t j = 0; j < 3; j++)
    {
        pars[j] = fitzr->GetParameter(j);
        // std::cout<<Form("par%i= ",j)<<pars[j]<<"\n";
    }
    czcorr->Update();
    // TImage *img = TImage::Create();
    // img->FromPad(czcorr);
    // img->WriteImage(Form("./calib/zcorrpol3/zcorr_pol3_%i_%i.png",pl,b));
    gPad->WaitPrimitive();
    gSystem->Sleep(3000);
    delete s;
    delete gr1;
    // delete czcorr;
    delete fitz;
}

ClassImp(R3BTofdHisto2HitPar)
