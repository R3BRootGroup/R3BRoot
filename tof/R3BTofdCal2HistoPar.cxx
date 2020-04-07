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
// -----            R3BTofdCal2HistoPar            -----
// -----        Created Jul 2019 by L.Bott         -----
// -----------------------------------------------------

/* Some notes:
 * In order to generate input for this run:
 * TofdCal2Histo
 */

#include "R3BTofdCal2HistoPar.h"
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

#include "TCanvas.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
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

TFile* hfilename;

R3BTofdCal2HistoPar::R3BTofdCal2HistoPar()
    : FairTask("R3BTofdCal2HistoPar", 1)
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
    , fTofdTotLow(0.)
    , fTofdTotHigh(0.)
    , fTofdSmiley(true)
    , fParaFile("")
    , fHistoFile("")
{
}

R3BTofdCal2HistoPar::R3BTofdCal2HistoPar(const char* name, Int_t iVerbose)
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
    , fTofdTotLow(0.)
    , fTofdTotHigh(0.)
    , fTofdSmiley(true)
    , fParaFile("")
    , fHistoFile("")
{
}

R3BTofdCal2HistoPar::~R3BTofdCal2HistoPar()
{
    if (fCal_Par)
    {
        delete fCal_Par;
    }
}

InitStatus R3BTofdCal2HistoPar::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
    // may be = NULL!
    fCalData = (TClonesArray*)rm->GetObject("TofdCal");
    if (!fCalData)
    {
        return kFATAL;
    }
    if (!fNofModules)
    {
        LOG(ERROR) << "R3BTofdCal2HistoPar::Init() Number of modules not set. ";
        return kFATAL;
    }
    // fCalItemsLos = (TClonesArray*)rm->GetObject("LosCal");
    // if (NULL == fCalItemsLos)
    //    LOG(fatal) << "Branch LosCal not found";
    LOG(INFO) << "Histo filename: " << fHistoFile;
    hfilename = TFile::Open(fHistoFile);
    if (hfilename == 0)
    {
        LOG(ERROR) << "Cannot open Histo file!";
        return kFATAL;
    }
    return kSUCCESS;
}

void R3BTofdCal2HistoPar::SetParContainers()
{
    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCal_Par = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fCal_Par)
    {
        LOG(ERROR) << "R3BTofdCal2HistoPar::Init() Couldn't get handle on TofdHitPar. ";
    }
    //  fCal_Par->setChanged();
}

void R3BTofdCal2HistoPar::Exec(Option_t* option) {}

void R3BTofdCal2HistoPar::FinishEvent() {}

void R3BTofdCal2HistoPar::FinishTask()
{
    if (fParameter == 1)
    {
        // Determine time offset of the 2 PMTs of one paddle. This procedure
        // assumes a sweep run in the middle of the ToF wall horizontally.
        // Since all paddles are mounted vertically one can determine the offset.
        // Half of the offset is added to PM1 and half to PM2.
        LOG(WARNING) << "Calling function calcOffset";
        calcOffset();
        // Determine ToT offset between top and bottom PMT
        LOG(WARNING) << "Calling function calcToTOffset";
        calcToTOffset(fTofdTotLow, fTofdTotHigh);
        // Determine sync offset between paddles
        LOG(WARNING) << "Calling function calcSync";
        calcSync();
        LOG(ERROR) << "Call walk correction before next step!";
    }

    if (fParameter == 2)
    {
        // Determine effective speed of light in [cm/s] for each paddle
        LOG(WARNING) << "Calling function calcVeff";
        calcVeff();
        // Determine light attenuation lambda for each paddle
        LOG(WARNING) << "Calling function calcLambda";
        calcLambda(fTofdTotLow, fTofdTotHigh);
    }

    if (fParameter == 3)
    {
        // calculation of position dependend charge
        if (fTofdSmiley)
        {
            LOG(WARNING) << "Calling function smiley";
            Double_t para2[4];
            Double_t min2 = -50.; // -40 effective bar length
            Double_t max2 = 50.;  // 40 effective bar length = 80 cm
                                  // we will use 50 here for some fit safety margin
            for (Int_t i = 0; i < fNofPlanes; i++)
            {
                for (Int_t j = 0; j < fPaddlesPerPlane; j++)
                {
                    if (hfilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)))
                    {
                        LOG(WARNING) << "Calling Plane " << i + 1 << " Bar " << j + 1;
                        R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                        smiley((TH2F*)hfilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)),
                               min2,
                               max2,
                               para2);
                        Double_t offset1 = par->GetOffset1();
                        Double_t offset2 = par->GetOffset2();
                        Double_t veff = par->GetVeff();
                        Double_t sync = par->GetSync();
                        par->SetPar1a(para2[0]);
                        par->SetPar1b(para2[1]);
                        par->SetPar1c(para2[2]);
                        par->SetPar1d(para2[3]);
                    }
                }
            }
            fCal_Par->setChanged();
        }
        else
        {
            LOG(WARNING) << "Calling function doubleExp";
            Double_t para[4];
            Double_t min = -40.; // effective bar length
            Double_t max = 40.;  // effective bar length = 80 cm

            for (Int_t i = 0; i < fNofPlanes; i++)
            {
                for (Int_t j = 0; j < fPaddlesPerPlane; j++)
                {
                    if (hfilename->Get(Form("Tot1_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
                    {
                        R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                        doubleExp(
                            (TH2F*)hfilename->Get(Form("Tot1_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)), min, max, para);
                        Double_t offset1 = par->GetOffset1();
                        Double_t offset2 = par->GetOffset2();
                        Double_t veff = par->GetVeff();
                        Double_t sync = par->GetSync();
                        par->SetPar1a(para[0]);
                        par->SetPar1b(para[1]);
                        par->SetPar1c(para[2]);
                        par->SetPar1d(para[3]);
                    }
                    if (hfilename->Get(Form("Tot2_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
                    {
                        R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                        doubleExp(
                            (TH2F*)hfilename->Get(Form("Tot2_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)), min, max, para);
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
        LOG(WARNING) << "Calling function zcorr";
        Double_t para[8];
        Double_t pars[3];
        Int_t min = 1, max = 15; // select range for peak search
        for (Int_t i = 0; i < fNofPlanes; i++)
        {
            for (Int_t j = 0; j < fPaddlesPerPlane; j++)
            {
                if (hfilename->Get(Form("Q_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
                {
                    if (i + 1 == 1)
                        continue;
                    if (j + 1 != 32)
                        continue;
                    R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                    std::cout << "Calling Plane: " << i + 1 << " Bar " << j + 1 << "\n";
                    zcorr((TH2F*)hfilename->Get(Form("Q_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)), min, max, pars);
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
                }
            }
        }
        fCal_Par->setChanged();
    }
}

void R3BTofdCal2HistoPar::calcOffset()
{
    TCanvas* cOffset = new TCanvas("cOffset", "cOffset", 10, 10, 1000, 900);
    cOffset->Divide(2, 2);
    R3BTofdHitModulePar* mpar;
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        if (hfilename->Get(Form("Time_Diff_Plane_%i", i + 1)))
        {
            LOG(WARNING) << "Found histo Time_Diff_Plane_" << i + 1;
            auto* h = (TH2F*)hfilename->Get(Form("Time_Diff_Plane_%i", i + 1))->Clone();
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
                LOG(WARNING) << " Plane  " << i + 1 << " Bar " << j + 1 << " Offset  " << offset;
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
void R3BTofdCal2HistoPar::calcToTOffset(Double_t totLow, Double_t totHigh)
{
    TCanvas* cToTOffset = new TCanvas("cToTOffset", "cToTOffset", 10, 10, 1000, 900);
    cToTOffset->Divide(1, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        for (Int_t j = 0; j < fPaddlesPerPlane; j++)
        {
            Double_t offset = 0.;
            R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
            if (hfilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)))
            {
                LOG(WARNING) << "Found histo SqrtQ_vs_PosToT_Plane_" << i + 1 << "_Bar_" << j + 1;
                auto* h = (TH2F*)hfilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1))->Clone();
                cToTOffset->cd(1);
                h->Draw("colz");
                auto* histo_py = (TH2F*)h->ProjectionX("histo_py", totLow, totHigh, "");
                cToTOffset->cd(2);
                histo_py->Rebin(3);
                histo_py->Draw();
                Int_t binmax = histo_py->GetMaximumBin();
                Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
                TF1* fgaus = new TF1(
                    "fgaus", "gaus(0)", Max - 0.2, Max + 0.2); // new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
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
            LOG(WARNING) << " Plane  " << i + 1 << " Bar " << j + 1 << " ToT Offset  " << offset << "\n";
            par->SetToTOffset1(sqrt(exp(offset)));
            par->SetToTOffset2(1. / sqrt(exp(offset)));
        }
    }
    fCal_Par->setChanged();
}

void R3BTofdCal2HistoPar::calcSync()
{
    TCanvas* cSync = new TCanvas("cSync", "cSync", 10, 10, 1000, 900);
    cSync->Divide(2, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        if (hfilename->Get(Form("Time_Sync_Plane_%i", i + 1)))
        {
            LOG(WARNING) << "Found histo Time_Sync_Plane_" << i + 1;
            auto* h = (TH2F*)hfilename->Get(Form("Time_Sync_Plane_%i", i + 1))->Clone();
            for (Int_t j = 0; j < fPaddlesPerPlane; j++)
            {
                cSync->cd(i + 1);
                h->Draw("colz");
                auto* histo_py = (TH1F*)h->ProjectionY("histo_py", j + 2, j + 2, "");
                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                Int_t binmax = histo_py->GetMaximumBin();
                Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
                Double_t MaxEntry = histo_py->GetBinContent(binmax);
                TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.3, Max + 0.3);
                fgaus->SetParameters(MaxEntry, Max, 20);
                histo_py->Fit("fgaus", "QR0");
                Double_t sync = fgaus->GetParameter(1); // histo_py->GetXaxis()->GetBinCenter(binmax);
                par->SetSync(sync);
                LOG(WARNING) << " Plane  " << i + 1 << " Bar " << j + 1 << " Sync  " << sync;
            }
        }
    }
    fCal_Par->setChanged();
}

void R3BTofdCal2HistoPar::calcVeff()
{
    TCanvas* cVeff = new TCanvas("cVeff", "cVeff", 10, 10, 1000, 900);
    cVeff->Divide(2, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        for (Int_t j = 0; j < fPaddlesPerPlane; j++)
        {
            Double_t max = 0.;
            Double_t veff = 7.;
            if (hfilename->Get(Form("Time_Diff_Plane_%i", i + 1)))
            {
                LOG(WARNING) << "Found histo Time_Diff_Plane_" << i + 1;
                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
                if (!par)
                {
                    LOG(INFO) << "Hit par not found, Plane: " << i + 1 << ", Bar: " << j + 1;
                    continue;
                }
                auto* h = (TH2F*)hfilename->Get(Form("Time_Diff_Plane_%i", i + 1))->Clone();
                cVeff->cd(i + 1);
                h->Draw("colz");
                TH1F* histo_py = (TH1F*)h->ProjectionY("histo_py", j + 2, j + 2, "");
                Int_t binmax = histo_py->GetMaximumBin();
                max = histo_py->GetXaxis()->GetBinCenter(binmax);
                Double_t maxEntry = histo_py->GetBinContent(binmax);
                auto* fgaus = new TF1("fgaus", "gaus(0)", max - 0.3, max + 0.3);
                fgaus->SetParameters(maxEntry, max, 20);
                histo_py->Fit("fgaus", "QR0");
                Double_t offset1 = par->GetOffset1();
                Double_t offset2 = par->GetOffset2();
                Double_t sync = par->GetSync();
                max = fgaus->GetParameter(1) + offset1 - offset2; /// TODO: needs to be tested
                // max = max+offset1-offset2;
                veff = fTofdY / max; // effective speed of light in [cm/s]
                LOG(WARNING) << " Plane  " << i + 1 << " Bar " << j + 1 << " offset  " << par->GetOffset1();
                LOG(WARNING) << " Plane  " << i + 1 << " Bar " << j + 1 << " max  " << max;
                LOG(WARNING) << " Plane  " << i + 1 << " Bar " << j + 1 << " veff  " << veff;
                par->SetVeff(veff);
            }
        }
    }
    fCal_Par->setChanged();
}
void R3BTofdCal2HistoPar::calcLambda(Double_t totLow, Double_t totHigh)
{
    TCanvas* cToTOffset = new TCanvas("cLambda", "cLambda", 10, 10, 1000, 900);
    cToTOffset->Divide(1, 2);
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        for (Int_t j = 0; j < fPaddlesPerPlane; j++)
        {
            Double_t offset = 0.;
            R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i + 1, j + 1);
            if (hfilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)))
            {
                LOG(WARNING) << "Found histo SqrtQ_vs_PosToT_Plane_" << i + 1 << "_Bar_" << j + 1;
                auto* h = (TH2F*)hfilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1))->Clone();
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
                LOG(FATAL) << "Missing histo plane " << i + 1 << " bar " << j + 1;
            Double_t lambda = fTofdY / offset;
            LOG(WARNING) << " Plane  " << i + 1 << " Bar " << j + 1 << " ToT Offset  " << offset << " Lambda " << lambda
                         << "\n";
            par->SetLambda(lambda);
        }
    }
    fCal_Par->setChanged();
}
void R3BTofdCal2HistoPar::doubleExp(TH2F* histo, Double_t min, Double_t max, Double_t* para)
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
    cfit_exp->cd(2);
    for (Int_t i = 1; i < histo1->GetNbinsX() - 1; i++)
    {
        TH1F* histo_py = (TH1F*)histo1->ProjectionY("histo_py", i, i, "");
        histo_py->Draw();
        x[n] = histo1->GetXaxis()->GetBinCenter(i);
        Int_t binmax = histo_py->GetMaximumBin();
        y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
        if ((x[n] < -40. || x[n] > 40.) || y[n] < 50.)
        {
            delete histo_py;
            continue;
        }
        if (histo_py->GetMaximum() > 5)
            n++;
        delete histo_py;
    }
    gr1 = new TGraph(n, x, y);
    gr1->Draw("A*");
    TF1* f1 = new TF1("f1", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
    f1->SetParameters(520., 0.001, 17234, -485.);
    f1->SetLineColor(2);
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
    gr2->Draw("A*");
    TF1* f2 = new TF1("f2", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
    f2->SetParameters(para[0], para[1], para[2], para[3]);
    f2->SetLineColor(2);
    gr2->Fit("f2", "", "", min, max);
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = f2->GetParameter(j);
        std::cout << "Parameter: " << para[j] << "\n";
    }
    cfit_exp->Update();
    // gPad->WaitPrimitive();
    gSystem->Sleep(3000);
    delete gr1;
    delete gr2;
    delete f1;
    delete f2;
}
void R3BTofdCal2HistoPar::smiley(TH2F* histo, Double_t min, Double_t max, Double_t* para)
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
    histo2->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
    // histo2->SetAxisRange(fTofdTotLow,fTofdTotHigh,"Y");
    cfit_smiley->cd(2);
    histo2->Draw("colz");
    std::cout << "Searching for points to fit...\n";
    for (Int_t i = 1; i < histo2->GetNbinsX(); i++)
    {
        // std::cout<<"Bin "<<i<<" of "<<histo2->GetNbinsX()<<" with cut: "<<fTofdTotLow<<" < sqrt(q1*q2) <
        // "<<fTofdTotHigh<<"\n";
        cfit_smiley->cd(2);
        TLine* l = new TLine(
            histo2->GetXaxis()->GetBinCenter(i), fTofdTotLow, histo2->GetXaxis()->GetBinCenter(i), fTofdTotHigh);
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

        if ((x[n] < min || x[n] > max) || (y[n] < fTofdTotLow || y[n] > fTofdTotHigh))
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
    gr1->SetTitle("Points found for fitting; x position in cm; sqrt(tot1*tot2)");
    gr1->Draw("A*");
    std::cout << "Start fitting\n";
    TF1* f1 = new TF1("f1", "pol3", min, max);
    f1->SetLineColor(2);
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
    gr2->SetTitle("More information;x position in cm;sqrt(q1*q2)");
    gr2->Draw("A*");
    f1->DrawCopy("SAME");
    TF1* f2 = new TF1("f2", "pol3", min, max);
    f2->SetParameters(para[0], para[1], para[2], para[3]);
    f2->SetLineColor(3);
    gr2->Fit("f2", "0Q", "", min, max);
    f2->Draw("SAME");
    std::cout << "Will write:\n";
    for (Int_t j = 0; j <= 3; j++)
    {
        para[j] = f2->GetParameter(j);
        std::cout << "Parameter: " << para[j] << "\n";
    }
    histo2->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
    auto legend = new TLegend(.9, 0.7, .99, 0.9);
    legend->AddEntry("f1", "First Fit", "l");
    legend->AddEntry("f2", "Second Fit", "l");
    legend->Draw();
    cfit_smiley->Update();
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
void R3BTofdCal2HistoPar::zcorr(TH2F* histo, Int_t min, Int_t max, Double_t* pars)
{
    Double_t par[3000] = { 0 };
    Int_t maxplane = 1, maxbar = 44, nPeaks = 180;
    Double_t x[3000] = { 0 };
    TCanvas* c1 = new TCanvas("c1", "c1", 100, 100, 800, 800);
    c1->Divide(1, 3);
    c1->cd(1);
    auto* h = (TH2F*)histo->Clone();
    h->Draw("colz");
    h->SetAxisRange(min, max, "Y");
    // Projection of charge axis
    auto* h1 = h->ProjectionY("p_y");
    c1->cd(2);
    h1->Draw();
    // Use TSpectrum to find the peak candidates
    TSpectrum* s = new TSpectrum(nPeaks);
    Int_t nfound = s->Search(h1, 1, "", 0.005); // lower threshold than default 0.05
    std::cout << "Found " << nfound << " candidate peaks to fit\n";
    c1->Update();
    // Eliminate background peaks
    nPeaks = 0;
    Double_t* xpeaks = s->GetPositionX();
    for (Int_t p = 0; p <= nfound; p++)
    {
        Float_t xp = xpeaks[p];
        Int_t bin = h1->GetXaxis()->FindBin(xp);
        Float_t yp = h1->GetBinContent(bin);
        if (yp - TMath::Sqrt(yp) < 1.)
            continue;
        par[2 * nPeaks] = yp;
        par[2 * nPeaks + 1] = xp;
        nPeaks++;
    }
    if (nPeaks < 2)
        return;

    Double_t peaks[nPeaks];

    for (Int_t i = 0; i < nPeaks; i++)
    {
        // printf("Found peak @ %f\n",xpeaks[i]);
        peaks[i] = par[2 * i + 1];
    }
    c1->Update();

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
            Int_t z = 0;
            while ((std::cout << " corresponds to Z=") && !(std::cin >> z))
            {
                std::cout << "That's not a number;";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            if (z == 0)
                continue;
            x[nfp] = (Double_t)z;
            zpeaks[nfp] = peaks[i];
            // std::cout<<"z real " << x[nfp] << " z found " << zpeaks[nfp] <<"\n";
            nfp++;
        }
        std::cout << "Do again? (y/n) ";
        std::cin >> doagain;
    } while (doagain != "n");
    if (nfp < 2)
    {
        delete s;
        delete c1;
        return;
    }
    // fit charge axis
    std::cout << "Selected " << nfp << " useful peaks to fit\nStart fitting...\n";
    auto* gr1 = new TGraph();
    TF1* fitz = new TF1("fitz", "[0]*TMath::Power(x,[2])+[1]", min, max);
    fitz->SetParameters(1.5, 2., .1);
    c1->cd(3);
    gr1 = new TGraph(nfp, zpeaks, x);
    gr1->Draw("A*");
    gr1->Fit("fitz", "Q", "", min, max);
    // write parameters
    std::cout << "Optimise the fit, double click to finish\n";
    // gPad->WaitPrimitive();
    auto* fitzr = gr1->GetFunction("fitz");
    for (Int_t j = 0; j < 3; j++)
    {
        pars[j] = fitzr->GetParameter(j);
        // std::cout<<Form("par%i= ",j)<<pars[j]<<"\n";
    }
    // gPad->WaitPrimitive();
    // gSystem->Sleep(3000);
    delete s;
    delete gr1;
    delete c1;
    delete fitz;
}

ClassImp(R3BTofdCal2HistoPar)
