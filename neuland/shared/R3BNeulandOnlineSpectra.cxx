#include "R3BNeulandOnlineSpectra.h"
#include "FairRunOnline.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THttpServer.h"
#include <iostream>
#include <limits>

R3BNeulandOnlineSpectra::R3BNeulandOnlineSpectra()
    : FairTask("R3BNeulandOnlineSpectra", 0)
    , fNeulandCalData("NeulandCalData")
    , fNeulandHits("NeulandHits")
    , fLosCalData("LosCal")
{
}

InitStatus R3BNeulandOnlineSpectra::Init()
{
    auto run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("/Tasks", this);
    run->GetHttpServer()->RegisterCommand("Reset_Neuland", Form("/Tasks/%s/->ResetHistos()", GetName()));

    fNeulandCalData.Init();
    fNeulandHits.Init();
    fLosCalData.Init();

    auto canvasCal = new TCanvas("NeulandCal", "NeulandCal", 10, 10, 850, 850);
    canvasCal->Divide(2, 3);

    hTstart = new TH1D("hTstart", "Tstart", 1000, 0., 41000.);
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

    ahCalEvsBar[0] = new TH2D("hCalE1vsBar", "CalLevel: Energy1 vs Bars", fNBars, 0.5, fNBars + 0.5, 1000, 0, 1000);
    canvasCal->cd(5);
    ahCalEvsBar[0]->Draw("colz");

    ahCalEvsBar[1] = new TH2D("hCalE2vsBar", "CalLevel: Energy2 vs Bars", fNBars, 0.5, fNBars + 0.5, 1000, 0, 1000);
    canvasCal->cd(6);
    ahCalEvsBar[1]->Draw("colz");

    canvasCal->cd(0);
    run->AddObject(canvasCal);

    auto canvasHit = new TCanvas("NeulandHit", "NeulandHit", 10, 10, 850, 850);
    canvasHit->Divide(3, 3);

    ahHitTvsBar[0] = new TH2D("hHitTvsBar", "HitLevel: Time vs Bars   ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    canvasHit->cd(1);
    ahHitTvsBar[0]->Draw("colz");

    ahHitTvsBar[1] =
        new TH2D("hHitTLvsBar", "HitLevel: TimeL vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    canvasHit->cd(2);
    ahHitTvsBar[1]->Draw("colz");

    ahHitTvsBar[2] =
        new TH2D("hHitTRvsBar", "HitLevel: TimeR vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    canvasHit->cd(3);
    ahHitTvsBar[2]->Draw("colz");

    ahHitEvsBar[0] = new TH2D("hHitEvsBar", "HitLevel: Energy vs Bars ", fNBars, 0.5, fNBars + 0.5, 1000, 0, 1000);
    canvasHit->cd(4);
    ahHitEvsBar[0]->Draw("colz");

    ahHitEvsBar[1] = new TH2D("hHitELvsBar", "HitLevel: EnergyL vs Bars", fNBars, 0.5, fNBars + 0.5, 1000, 0, 1000);
    canvasHit->cd(5);
    ahHitEvsBar[1]->Draw("colz");

    ahHitEvsBar[2] = new TH2D("hHitERvsBar", "HitLevel: EnergyR vs Bars", fNBars, 0.5, fNBars + 0.5, 1000, 0, 1000);
    canvasHit->cd(6);
    ahHitEvsBar[2]->Draw("colz");

    hTdiffvsBar = new TH2D("hTdiffvsBar", "Tdiff vs Bars", fNBars, 0.5, fNBars + 0.5, 1000, -1100, 1100);
    canvasHit->cd(7);
    hTdiffvsBar->Draw("colz");

    hToFvsBar = new TH2D("hTofvsBar", "Tof vs Bars", fNBars, 0.5, fNBars + 0.5, 1000, 20, 200);
    canvasHit->cd(8);
    hToFvsBar->Draw("colz");

    hTofvsEhit = new TH2D("hTofvsEhit", "Tof vs Ehit", 1000, 0, 100, 2000, 40, 200);
    canvasHit->cd(9);
    hTofvsEhit->Draw("colz");

    canvasHit->cd(0);
    run->AddObject(canvasHit);

    auto canvasPlaneXY = new TCanvas("NeulandPlaneXY", "NeulandPlaneXY", 10, 10, 850, 850);
    canvasPlaneXY->Divide(3, 3);
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
    run->AddObject(canvasPlaneXY);

    return kSUCCESS;
}

void R3BNeulandOnlineSpectra::Exec(Option_t*)
{
    const double start = GetTstart();

    const auto calData = fNeulandCalData.Retrieve();
    const auto hits = fNeulandHits.Retrieve();

    for (const auto& data : calData)
    {
        const auto side = data->GetSide() - 1; // [1,2] -> [0,1]
        const auto bar = data->GetBarId();
        ahCalTvsBar[side]->Fill(bar, data->GetTime());
        ahCalEvsBar[side]->Fill(bar, data->GetQdc());
    }

    for (const auto& hit : hits)
    {
        const auto bar = hit->GetPaddle();
        ahHitTvsBar[0]->Fill(bar, hit->GetT());
        ahHitTvsBar[1]->Fill(bar, hit->GetTdcL());
        ahHitTvsBar[2]->Fill(bar, hit->GetTdcR());

        ahHitEvsBar[0]->Fill(bar, hit->GetE());
        ahHitEvsBar[1]->Fill(bar, hit->GetQdcL());
        ahHitEvsBar[2]->Fill(bar, hit->GetQdcR());

        hTdiffvsBar->Fill(bar, hit->GetTdcL() - hit->GetTdcR());
        hToFvsBar->Fill(bar, hit->GetT() - start);
        hTofvsEhit->Fill(hit->GetE(), hit->GetT() - start);

        const int paddle = static_cast<const int>(std::floor(hit->GetPaddle() / 50));
        ahXYperPlane[paddle]->Fill(hit->GetPosition().X(), hit->GetPosition().Y());
    }
}

void R3BNeulandOnlineSpectra::FinishTask()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("R3BNeulandOnlineSpectra");
    gDirectory->cd("R3BNeulandOnlineSpectra");

    hTstart->Write();
    hNstart->Write();

    ahCalTvsBar[0]->Write();
    ahCalTvsBar[1]->Write();
    ahCalEvsBar[0]->Write();
    ahCalEvsBar[1]->Write();

    ahHitTvsBar[0]->Write();
    ahHitTvsBar[1]->Write();
    ahHitTvsBar[2]->Write();
    ahHitEvsBar[0]->Write();
    ahHitEvsBar[1]->Write();
    ahHitEvsBar[2]->Write();

    hTdiffvsBar->Write();
    hToFvsBar->Write();
    hTofvsEhit->Write();

    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlane[i]->Write();
    }

    gDirectory = tmp;
}

void R3BNeulandOnlineSpectra::ResetHistos()
{
    hTstart->Reset();
    hNstart->Reset();

    ahCalTvsBar[0]->Reset();
    ahCalTvsBar[1]->Reset();
    ahCalEvsBar[0]->Reset();
    ahCalEvsBar[1]->Reset();

    ahHitTvsBar[0]->Reset();
    ahHitTvsBar[1]->Reset();
    ahHitTvsBar[2]->Reset();
    ahHitEvsBar[0]->Reset();
    ahHitEvsBar[1]->Reset();
    ahHitEvsBar[2]->Reset();

    hTdiffvsBar->Reset();
    hToFvsBar->Reset();
    hTofvsEhit->Reset();

    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlane[i]->Reset();
    }
}

double R3BNeulandOnlineSpectra::GetTstart() const
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

bool R3BNeulandOnlineSpectra::IsBeam() const { return !fLosCalData.Retrieve().empty(); }

ClassImp(R3BNeulandOnlineSpectra)
