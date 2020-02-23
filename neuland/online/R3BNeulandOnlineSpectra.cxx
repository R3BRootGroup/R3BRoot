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

#include "R3BNeulandOnlineSpectra.h"
#include "FairRunOnline.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THttpServer.h"
#include <TFile.h>
#include <iostream>
#include <limits>

R3BNeulandOnlineSpectra::R3BNeulandOnlineSpectra()
    : FairTask("R3BNeulandOnlineSpectra", 0)
    , fNeulandMappedData("NeulandMappedData")
    , fNeulandCalData("NeulandCalData")
    , fNeulandHits("NeulandHits")
    , fLosCalData("LosCal")
#ifdef SOFIA
    , fSofSciCalData("SofSciTcalData")
#endif // SOFIA
{
}

InitStatus R3BNeulandOnlineSpectra::Init()
{
    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    auto run = FairRunOnline::Instance();
    if (run)
    {
        run->GetHttpServer()->Register("/Tasks", this);
        run->GetHttpServer()->RegisterCommand("Reset_Neuland", Form("/Tasks/%s/->ResetHistos()", GetName()));
        run->GetHttpServer()->RegisterCommand("Reset_Neuland_Mapped",
                                              Form("/Tasks/%s/->ResetHistosMapped()", GetName()));
    }

    fNeulandMappedData.Init();
    fNeulandCalData.Init();
    fNeulandHits.Init();
    fLosCalData.Init();
#ifdef SOFIA
    fSofSciCalData.Init();
#endif // SOFIA

    auto canvasMapped = new TCanvas("NeulandMapped", "NeulandMapped", 10, 10, 850, 850);
    canvasMapped->Divide(1, 2);

    canvasMapped->cd(1);
    ahMappedBar1[0] = new TH1D("hMappedBar1fLE", "Mapped: Bars fine 1LE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar1[0]->Draw();
    ahMappedBar1[1] = new TH1D("hMappedBar1fTE", "Mapped: Bars fine 1TE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar1[1]->SetLineColor(1);
    ahMappedBar1[1]->Draw("same");
    ahMappedBar1[2] = new TH1D("hMappedBar1cLE", "Mapped: Bars coarse 1LE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar1[2]->SetLineColor(2);
    ahMappedBar1[2]->Draw("same");
    ahMappedBar1[3] = new TH1D("hMappedBar1cTE", "Mapped: Bars coarse 1TE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar1[3]->SetLineColor(6);
    ahMappedBar1[3]->Draw("same");

    canvasMapped->cd(2);
    ahMappedBar2[0] = new TH1D("hMappedBar2fLE", "Mapped: Bars fine 2LE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar2[0]->Draw();
    ahMappedBar2[1] = new TH1D("hMappedBar2fTE", "Mapped: Bars fine 2TE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar2[1]->SetLineColor(1);
    ahMappedBar2[1]->Draw("same");
    ahMappedBar2[2] = new TH1D("hMappedBar2cLE", "Mapped: Bars coarse 2LE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar2[2]->SetLineColor(2);
    ahMappedBar2[2]->Draw("same");
    ahMappedBar2[3] = new TH1D("hMappedBar2cTE", "Mapped: Bars coarse 2TE", fNBars, 0.5, fNBars + 0.5);
    ahMappedBar2[3]->SetLineColor(6);
    ahMappedBar2[3]->Draw("same");

    canvasMapped->cd(0);
    if (run)
    {
        run->AddObject(canvasMapped);
    }

    auto canvasCal = new TCanvas("NeulandCal", "NeulandCal", 10, 10, 850, 850);
    canvasCal->Divide(2, 3);

    hTstart = new TH1D("hTstart", "Tstart", 1000, -10000., 50000.);
    canvasCal->cd(1);
    hTstart->Draw();

    hNstart = new TH1D("hNstart", "Nstart", 20, 0.5, 20.5);
    canvasCal->cd(2);
    hNstart->Draw();

    ahCalTvsBar[0] =
        new TH2D("hCalT1vsBar", "CalLevel: Time1 vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    canvasCal->cd(3);
    ahCalTvsBar[0]->Draw("colz");

    ahCalTvsBar[1] =
        new TH2D("hCalT2vsBar", "CalLevel: Time2 vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    canvasCal->cd(4);
    ahCalTvsBar[1]->Draw("colz");

    ahCalEvsBar[0] = new TH2D("hCalE1vsBar", "CalLevel: Energy1 vs Bars", fNBars, 0.5, fNBars + 0.5, 600, 0, 600);
    canvasCal->cd(5);
    ahCalEvsBar[0]->Draw("colz");

    ahCalEvsBar[1] = new TH2D("hCalE2vsBar", "CalLevel: Energy2 vs Bars", fNBars, 0.5, fNBars + 0.5, 600, 0, 600);
    canvasCal->cd(6);
    ahCalEvsBar[1]->Draw("colz");

    canvasCal->cd(0);
    if (run)
    {
        run->AddObject(canvasCal);
    }

    auto canvasHit = new TCanvas("NeulandHit", "NeulandHit", 10, 10, 850, 850);
    canvasHit->Divide(2, 2);

    hHitEvsBar = new TH2D("hHitEvsBar", "HitLevel: Energy vs Bars ", fNBars, 0.5, fNBars + 0.5, 1000, 0, 60);
    canvasHit->cd(1);
    hHitEvsBar->Draw("colz");

    hTdiffvsBar = new TH2D("hTdiffvsBar", "Tdiff vs Bars", fNBars, 0.5, fNBars + 0.5, 1000, -60, 60);
    canvasHit->cd(2);
    hTdiffvsBar->Draw("colz");

    hToFvsBar = new TH2D("hTofvsBar", "Tof vs Bars", fNBars, 0.5, fNBars + 0.5, 6000, -100, 400);
    hToFcvsBar = new TH2D("hTofcvsBar", "Tofc vs Bars", fNBars, 0.5, fNBars + 0.5, 6000, -100, 400);
    canvasHit->cd(3);
    hToFcvsBar->Draw("colz");

    hTofvsEhit = new TH2D("hTofvsEhit", "Tof vs Ehit", 1000, 0, 60, 6000, -100, 400);
    hTofcvsEhit = new TH2D("hTofcvsEhit", "Tofc vs Ehit", 1000, 0, 60, 6000, -100, 400);
    canvasHit->cd(4);
    hTofcvsEhit->Draw("colz");

    hTofvsX = new TH2D("hTofvsX", "Tof vs X", 3000, -200., 200., 6000, 0, 300);
    hTofcvsX = new TH2D("hTofcvsX", "Tofc vs X", 3000, -200., 200., 6000, 0, 300);
    hTofvsY = new TH2D("hTofvsY", "Tof vs Y", 3000, -200., 200., 6000, 0, 300);
    hTofcvsY = new TH2D("hTofcvsY", "Tofc vs Y", 3000, -200., 200., 6000, 0, 300);

    canvasHit->cd(0);
    if (run)
    {
        run->AddObject(canvasHit);
    }

    auto canvasHitCosmics = new TCanvas("NeulandHitCosmics", "NeulandHitCosmics", 10, 10, 850, 850);
    canvasHitCosmics->Divide(2, 2);

    hHitEvsBarCosmics =
        new TH2D("hHitEvsBarCosmics", "HitLevel: Energy vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 1000, 0, 60);
    canvasHitCosmics->cd(1);
    hHitEvsBarCosmics->Draw("colz");

    hTdiffvsBarCosmics =
        new TH2D("hTdiffvsBarCosmics", "Tdiff vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 1000, -60, 60);
    canvasHitCosmics->cd(2);
    hTdiffvsBarCosmics->Draw("colz");

    hDT375 = new TH2D("hDT375", "Thit - Thit375 vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 1000, -20, 20);
    canvasHitCosmics->cd(3);
    hDT375->Draw("colz");

    hDT425 = new TH2D("hDT425", "Thit - Thit425 vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 1000, -20, 20);
    canvasHitCosmics->cd(4);
    hDT425->Draw("colz");

    canvasHitCosmics->cd(0);
    if (run)
    {
        run->AddObject(canvasHitCosmics);
    }

    auto canvasPlaneXY = new TCanvas("NeulandPlaneXY", "NeulandPlaneXY", 10, 10, 850, 850);
    canvasPlaneXY->Divide(4, 4);
    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlane[i] = new TH2D("hHitXYPlane" + TString::Itoa(i, 10),
                                   "Hit XY Plane" + TString::Itoa(i, 10),
                                   300,
                                   -150,
                                   150,
                                   300,
                                   -150,
                                   150);
        canvasPlaneXY->cd(i + 1);
        ahXYperPlane[i]->Draw("colz");
    }
    canvasPlaneXY->cd(0);
    if (run)
    {
        run->AddObject(canvasPlaneXY);
    }

    auto canvasPlaneSofia = new TCanvas("Timing!", "Timing!", 10, 10, 850, 850);
    canvasPlaneSofia->Divide(2, 2);

    hSofiaTime = new TH1D("hSofiaTime", "hSofiaTime", 50000, 0, 50000);
    canvasPlaneSofia->cd(1);
    hSofiaTime->Draw();

    hNeuLANDvsSOFIA = new TH2D("hNeuLANDvsSOFIA", "hNeuLANDvsSOFIA", 4000, 0, 40000, 1050, 0, 10500);
    canvasPlaneSofia->cd(2);
    hNeuLANDvsSOFIA->Draw("colz");

    return kSUCCESS;
}

void R3BNeulandOnlineSpectra::Exec(Option_t*)
{
    const double clight = 29.9792458;

    const double start = GetTstart();

    const auto mappedData = fNeulandMappedData.Retrieve();
    const auto calData = fNeulandCalData.Retrieve();
    const auto hits = fNeulandHits.Retrieve();

    for (const auto& mapped : mappedData)
    {
        const auto plane = mapped->GetPlaneId();
        const auto barp = mapped->GetBarId();
        const auto bar = (plane - 1) * 50 + barp;

        if (mapped->GetFineTime1LE() > 0)
            ahMappedBar1[0]->Fill(bar);
        if (mapped->GetFineTime1TE() > 0)
            ahMappedBar1[1]->Fill(bar);
        if (mapped->GetCoarseTime1LE() > 0)
            ahMappedBar1[2]->Fill(bar);
        if (mapped->GetCoarseTime1TE() > 0)
            ahMappedBar1[3]->Fill(bar);
        if (mapped->GetFineTime2LE() > 0)
            ahMappedBar2[0]->Fill(bar);
        if (mapped->GetFineTime2TE() > 0)
            ahMappedBar2[1]->Fill(bar);
        if (mapped->GetCoarseTime2LE() > 0)
            ahMappedBar2[2]->Fill(bar);
        if (mapped->GetCoarseTime2TE() > 0)
            ahMappedBar2[3]->Fill(bar);
    }

    for (const auto& data : calData)
    {
        const auto side = data->GetSide() - 1; // [1,2] -> [0,1]
        const auto bar = data->GetBarId();
        ahCalTvsBar[side]->Fill(bar, data->GetTime());
        ahCalEvsBar[side]->Fill(bar, data->GetQdc());
        hNeuLANDvsSOFIA->Fill(start, data->GetTime());
    }

    Double_t randx;

    for (const auto& hit : hits)
    {
        const auto bar = hit->GetPaddle();

        if (IsBeam())
        {
            if (std::isnan(hit->GetT()))
                continue;
            hHitEvsBar->Fill(bar, hit->GetE());
            hTdiffvsBar->Fill(bar, hit->GetTdcL() - hit->GetTdcR());

            // const Double_t tadj = fDistanceToTarget / hit->GetPosition().Mag() *
            // hit->GetT();
            const Double_t tcorr = hit->GetT() - (hit->GetPosition().Mag() - fDistanceToTarget) / clight;
            const Double_t tadj = hit->GetT();

            if (hit->GetE() > 0.)
            { // 7.
                hToFvsBar->Fill(bar, tadj);
                hToFcvsBar->Fill(bar, tcorr);
            }

            hTofvsEhit->Fill(hit->GetE(), tadj);
            hTofcvsEhit->Fill(hit->GetE(), tcorr);

            randx = (std::rand() / (float)RAND_MAX);
            const int plane = static_cast<const int>(std::floor((hit->GetPaddle() - 1) / 50));
            ahXYperPlane[plane]->Fill(hit->GetPosition().X() + (plane % 2) * 5. * (randx - 0.5),
                                      hit->GetPosition().Y() + ((plane + 1) % 2) * 5. * (randx - 0.5));

            hTofvsX->Fill(hit->GetPosition().X() + (plane % 2) * 5. * (randx - 0.5), tadj);
            hTofcvsX->Fill(hit->GetPosition().X() + (plane % 2) * 5. * (randx - 0.5), tcorr);
            hTofvsY->Fill(hit->GetPosition().Y() + ((plane + 1) % 2) * 5. * (randx - 0.5), tadj);
            hTofcvsY->Fill(hit->GetPosition().Y() + ((plane + 1) % 2) * 5. * (randx - 0.5), tcorr);
        }
        else
        {
            hHitEvsBarCosmics->Fill(bar, hit->GetE());
            hTdiffvsBarCosmics->Fill(bar, hit->GetTdcL() - hit->GetTdcR());

            for (const auto& hitref : hits)
            {
                if ((hitref->GetPaddle() == 375) && (bar != 375))
                    hDT375->Fill(bar,
                                 (hit->GetTdcL() + hit->GetTdcR()) / 2. - (hitref->GetTdcL() + hitref->GetTdcR()) / 2.);
                if ((hitref->GetPaddle() == 425) && (bar != 425))
                    hDT425->Fill(bar,
                                 (hit->GetTdcL() + hit->GetTdcR()) / 2. - (hitref->GetTdcL() + hitref->GetTdcR()) / 2.);
            }
        }
    }
}

void R3BNeulandOnlineSpectra::FinishTask()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("R3BNeulandOnlineSpectra");
    gDirectory->cd("R3BNeulandOnlineSpectra");

    ahMappedBar1[0]->Write();
    ahMappedBar1[1]->Write();
    ahMappedBar1[2]->Write();
    ahMappedBar1[3]->Write();
    ahMappedBar2[0]->Write();
    ahMappedBar2[1]->Write();
    ahMappedBar2[2]->Write();
    ahMappedBar2[3]->Write();

    hTstart->Write();
    hNstart->Write();

    ahCalTvsBar[0]->Write();
    ahCalTvsBar[1]->Write();
    ahCalEvsBar[0]->Write();
    ahCalEvsBar[1]->Write();

    hHitEvsBar->Write();

    hHitEvsBarCosmics->Write();

    hTdiffvsBar->Write();
    hToFvsBar->Write();
    hTofvsEhit->Write();
    hToFcvsBar->Write();
    hTofcvsEhit->Write();

    hTofvsX->Write();
    hTofcvsX->Write();
    hTofvsY->Write();
    hTofcvsY->Write();

    hTdiffvsBarCosmics->Write();

    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlane[i]->Write();
    }

    hSofiaTime->Write();
    hNeuLANDvsSOFIA->Write();

    gDirectory = tmp;
}

void R3BNeulandOnlineSpectra::ResetHistos()
{
    ahMappedBar1[0]->Reset();
    ahMappedBar1[1]->Reset();
    ahMappedBar1[2]->Reset();
    ahMappedBar1[3]->Reset();
    ahMappedBar2[0]->Reset();
    ahMappedBar2[1]->Reset();
    ahMappedBar2[2]->Reset();
    ahMappedBar2[3]->Reset();

    hTstart->Reset();
    hNstart->Reset();

    ahCalTvsBar[0]->Reset();
    ahCalTvsBar[1]->Reset();
    ahCalEvsBar[0]->Reset();
    ahCalEvsBar[1]->Reset();

    hHitEvsBar->Reset();

    hHitEvsBarCosmics->Reset();

    hTdiffvsBar->Reset();
    hToFvsBar->Reset();
    hTofvsEhit->Reset();
    hToFcvsBar->Reset();
    hTofcvsEhit->Reset();

    hTofvsX->Reset();
    hTofcvsX->Reset();
    hTofvsY->Reset();
    hTofcvsY->Reset();

    hTdiffvsBarCosmics->Reset();

    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlane[i]->Reset();
    }

    hSofiaTime->Reset();
    hNeuLANDvsSOFIA->Reset();
}

void R3BNeulandOnlineSpectra::ResetHistosMapped()
{
    ahMappedBar1[0]->Reset();
    ahMappedBar1[1]->Reset();
    ahMappedBar1[2]->Reset();
    ahMappedBar1[3]->Reset();
    ahMappedBar2[0]->Reset();
    ahMappedBar2[1]->Reset();
    ahMappedBar2[2]->Reset();
    ahMappedBar2[3]->Reset();
}

double R3BNeulandOnlineSpectra::GetLosTstart() const
{
    const auto losCalData = fLosCalData.Retrieve();

    if (losCalData.empty())
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    hNstart->Fill(losCalData.size());
    for (const auto& los : fLosCalData.Retrieve())
    {
        hTstart->Fill(los->GetMeanTimeVFTX());
    }

    return losCalData.back()->GetMeanTimeVFTX();
}

#ifdef SOFIA
double R3BNeulandOnlineSpectra::GetSofTstart() const
{
    const auto sofsci = fSofSciCalData.Retrieve();

    if (sofsci.empty())
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    hNstart->Fill(sofsci.size());

    double ts = 0.;
    int ns = 0;
    for (const auto& sof : sofsci)
    {
        hSofiaTime->Fill(sof->GetRawTimeNs());
        if (sof->GetDetector() == 4)
        {
            if (sof->GetPmt() == 1 || sof->GetPmt() == 2)
            {
                ts = ts + sof->GetRawTimeNs();
                ns = ns + 1;
            }
        }
    }

    if (ns == 2)
    {
        hTstart->Fill(ts / 2.);
        return ts / 2.;
    }
    else
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    // evt->Draw("NeulandCalData.fTime:SofSciTcalData.fRawTimeNs>>h1(200,0,40000,200,0,10000)","",
    // "colz",10000000); if (sofsci.back()->GetRawTimeNs()==-1000000)
    //{
    //   return std::numeric_limits<double>::quiet_NaN();
    //}
    // else
    //{
    //  return sofsci.back()->GetRawTimeNs();
    //}
}
#endif // SOFIA

bool R3BNeulandOnlineSpectra::IsBeam() const { return !std::isnan(GetTstart()); }

double R3BNeulandOnlineSpectra::GetTstart() const
{
#ifdef SOFIA
    return GetSofTstart();
#endif // SOFIA
    return GetLosTstart();
}

ClassImp(R3BNeulandOnlineSpectra)
