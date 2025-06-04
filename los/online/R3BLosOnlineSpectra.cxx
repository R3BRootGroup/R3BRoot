/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------
// -----                R3BLosOnlineSpectra               -----
// -----          Created April 13th 2016 by M.Heil       -----
// -----       Updated May 11th 2022 by J.L. Rodriguez    -----
// ------------------------------------------------------------

#include "R3BLosOnlineSpectra.h"
#include "R3BCoarseTimeStitch.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
#include "R3BShared.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

R3BLosOnlineSpectra::R3BLosOnlineSpectra()
    : R3BLosOnlineSpectra("LosOnlineSpectra", 1)
{
}

R3BLosOnlineSpectra::R3BLosOnlineSpectra(const char* name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

InitStatus R3BLosOnlineSpectra::Init()
{
    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    R3BLOG(info, "");

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // Get objects for detectors on all levels

    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    for (int det = 0; det < DET_MAX; det++)
    {
        fMappedItems.push_back(dynamic_cast<TClonesArray*>(mgr->GetObject(Form("%sMapped", fDetectorNames[det]))));
        R3BLOG_IF(fatal, NULL == fMappedItems.at(det), "LosMapped not found");
        fCalItems.push_back(dynamic_cast<TClonesArray*>(mgr->GetObject(Form("%sCal", fDetectorNames[det]))));
        R3BLOG_IF(warn, NULL == fCalItems.at(det), "LosCal not found");
    }

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    // Trigger and Tpat
    fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fhTpat->GetXaxis()->SetTitle("Tpat value");

    fhTrigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fhTrigger->GetXaxis()->SetTitle("Trigger value");

    auto cTrigg = new TCanvas("Trigg", "Triggers", 10, 10, 650, 350);
    cTrigg->Divide(2, 1);
    cTrigg->cd(1);
    gPad->SetLogy();
    fhTrigger->Draw();
    cTrigg->cd(2);
    gPad->SetLogy();
    fhTpat->Draw();
    cTrigg->cd(0);

    // MAIN FOLDER-MWPC
    auto mainfol = new TFolder("LOS", "LOS info");

    // Los detector  ---------------------------------------------------
    std::vector<TCanvas*> cLos;
    std::vector<TCanvas*> cLos_diagnosis;
    std::vector<TCanvas*> cLos_mapped_channels;
    if (fMappedItems.at(DET_LOS))
    {
        for (Int_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        {
            auto detName = Form("LOS%d", iloscount + 1);
            cLos.push_back(new TCanvas(detName, detName, 10, 10, 1010, 810));

            cLos_diagnosis.push_back(new TCanvas(
                Form("LOS%d_diagnosis", iloscount + 1), Form("LOS%d diagnosis", iloscount + 1), 10, 10, 1010, 810));

            cLos_mapped_channels.push_back(new TCanvas(Form("LOS_mapped_channels%d", iloscount + 1),
                                                       Form("LOS mapped channels %d", iloscount + 1),
                                                       10,
                                                       10,
                                                       1010,
                                                       810));

            fh_los_channels[iloscount] =
                R3B::root_owned<TH1F>(Form("%s_channels", detName), Form("%s channels", detName), 20, 0., 20.);
            fh_los_channels[iloscount]->GetXaxis()->SetTitle("Channel number");
            fh_los_channels[iloscount]->SetFillColor(31);

            fh_los_multihit[iloscount] = R3B::root_owned<TH1F>(
                Form("%s_multihit", detName), Form("%s multihit && all 8 PMs", detName), 30, 0., 30.);
            fh_los_multihit[iloscount]->GetXaxis()->SetTitle("Multihit");
            fh_los_multihit[iloscount]->SetFillColor(31);

            fh_los_pos_MCFD[iloscount] = R3B::root_owned<TH2F>(
                Form("%s_pos_MCFD", detName), Form("%s MCFD Position ", detName), 500, -5., 5., 500, -5., 5.);
            fh_los_pos_MCFD[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_MCFD[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_TAMEX[iloscount] = R3B::root_owned<TH2F>(
                Form("%s_pos_TAMEX", detName), Form("%s TAMEX Position ", detName), 500, -5., 5., 500, -5., 5.);
            fh_los_pos_TAMEX[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_TAMEX[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_ToT[iloscount] = R3B::root_owned<TH2F>(
                Form("%s_pos_ToT", detName), Form("%s ToT Position ", detName), 500, -5., 5., 500, -5., 5.);
            fh_los_pos_ToT[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_ToT[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_dt_hits_ToT[iloscount] = R3B::root_owned<TH2F>(
                Form("%s_dt_ToT", detName), Form("%s ToT dt ", detName), 4000, -4., 4., 1000, 0, 1000.);
            fh_los_dt_hits_ToT[iloscount]->GetXaxis()->SetTitle("Time MCFD / ns"); // dt between two hits / ns
            fh_los_dt_hits_ToT[iloscount]->GetYaxis()->SetTitle("ToT / ns");       // ToT / ns

            fh_los_tres_MCFD[iloscount] =
                R3B::root_owned<TH1F>(Form("%s_dt_4vs4_MCFD", detName),
                                      Form("%s MCFD Time resolution - 4pmts vs 4pmts", detName),
                                      4000,
                                      -4.,
                                      4.);
            fh_los_tres_MCFD[iloscount]->GetXaxis()->SetTitle("Time MCFD / ns");
            fh_los_tres_MCFD[iloscount]->SetFillColor(31);

            fh_los_tres_TAMEX[iloscount] =
                R3B::root_owned<TH1F>(Form("%s_dt_4vs4_TAMEX", detName),
                                      Form("%s TAMEX Time resolution - 4pmts vs 4pmts ", detName),
                                      4000,
                                      -4.,
                                      4.);
            fh_los_tres_TAMEX[iloscount]->GetXaxis()->SetTitle("Time TAMEX / ns");
            fh_los_tres_TAMEX[iloscount]->SetFillColor(31);

            fh_los_tot[iloscount] = R3B::root_owned<TH2F>(
                Form("%s_tot", detName), Form("%s ToT vs PMT", detName), 10, 0, 10, 2000, 0., 1000.);
            fh_los_tot[iloscount]->GetXaxis()->SetTitle("PMT number");
            fh_los_tot[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            fh_los_tot_mean[iloscount] =
                R3B::root_owned<TH1F>(Form("%s_tot_mean", detName), Form("%s mean ToT", detName), 1500, 0., 1000.);
            fh_los_tot_mean[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_tot_mean[iloscount]->GetXaxis()->SetTitle("ToT / ns");
            fh_los_tot_mean[iloscount]->SetFillColor(31);

            fh_los_ihit_ToT[iloscount] = R3B::root_owned<TH2F>(
                Form("%s_tot_ihit", detName), Form("%s ToT vs ihit", detName), 10, 0, 10, 600, 0., 1000.);
            fh_los_ihit_ToT[iloscount]->GetXaxis()->SetTitle("iHit");
            fh_los_ihit_ToT[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            fh_losToT_vs_Events[iloscount] = R3B::root_owned<TH2F>(Form("%s_tot_vs_event", detName),
                                                                   Form("%s ToT vs. Event #", detName),
                                                                   10000,
                                                                   0,
                                                                   10000000,
                                                                   1000,
                                                                   0.,
                                                                   1000.);
            fh_losToT_vs_Events[iloscount]->GetYaxis()->SetTitle("ToT / ns");
            fh_losToT_vs_Events[iloscount]->GetXaxis()->SetTitle("Event number");

            fh_losTAMEX_vs_Events[iloscount] = R3B::root_owned<TH2F>(Form("%s_dtTAMEX_vs_event", detName),
                                                                     Form("%s dtTAMEX vs. Event #", detName),
                                                                     10000,
                                                                     0,
                                                                     10000000,
                                                                     1000,
                                                                     -4.,
                                                                     4.);
            fh_losTAMEX_vs_Events[iloscount]->GetYaxis()->SetTitle("Time TAMEX  / ns");
            fh_losTAMEX_vs_Events[iloscount]->GetXaxis()->SetTitle("Event number");

            fh_losMCFD_vs_Events[iloscount] = R3B::root_owned<TH2F>(Form("%s_dtMCFD_vs_event", detName),
                                                                    Form("%s dtMCFD vs. Event #", detName),
                                                                    10000,
                                                                    0,
                                                                    10000000,
                                                                    1000,
                                                                    -4.,
                                                                    4.);
            fh_losMCFD_vs_Events[iloscount]->GetYaxis()->SetTitle("Time MCFD  / ns");
            fh_losMCFD_vs_Events[iloscount]->GetXaxis()->SetTitle("Event number");

            fh_los_vftx_tamex[iloscount] = R3B::root_owned<TH1F>(
                Form("%s_vftx_tamex", detName), Form("%s vftx_tamex", detName), 100000, -5000, 5000.);
            fh_los_vftx_tamex[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_vftx_tamex[iloscount]->GetXaxis()->SetTitle("Ttamex-Tvftx / ns");
            fh_los_vftx_tamex[iloscount]->SetFillColor(31);

            fh1_los_mapped.resize(nb_pmts * 3);
            const char* names[] = { "VFTX", "TMX_L", "TMX_T" };
            for (Int_t t = 0; t < fh1_los_mapped.size(); t++)
            {
                TString title = Form("%s_%s_los_channels_%d", detName, names[t / nb_pmts], t % nb_pmts);
                fh1_los_mapped[t] = R3B::root_owned<TH1F>(title, title, 2048, 0, 4 * 2048 * 5.);
            }

            cLos[iloscount]->Divide(3, 3);
            cLos[iloscount]->cd(1);
            fh_los_channels[iloscount]->Draw();
            cLos[iloscount]->cd(2);
            gPad->SetLogy();
            fh_los_multihit[iloscount]->Draw();
            cLos[iloscount]->cd(3);
            gPad->SetLogz();
            fh_los_tot[iloscount]->Draw("colz");
            cLos[iloscount]->cd(4); // gPad->SetLogy();
            fh_los_tot_mean[iloscount]->Draw();
            cLos[iloscount]->cd(5); // gPad->SetLogy();
            fh_los_tres_MCFD[iloscount]->Draw();
            cLos[iloscount]->cd(6);
            gPad->SetLogz();
            fh_los_dt_hits_ToT[iloscount]->Draw("colz");
            cLos[iloscount]->cd(7);
            gPad->SetLogz();
            fh_los_pos_ToT[iloscount]->Draw("colz");
            cLos[iloscount]->cd(8);
            gPad->SetLogz();
            fh_los_pos_MCFD[iloscount]->Draw("colz");
            cLos[iloscount]->cd(9);
            gPad->SetLogz();
            fh_los_ihit_ToT[iloscount]->Draw("colz");
            cLos[iloscount]->cd(0);
            mainfol->Add(cLos[iloscount]);

            cLos_diagnosis[iloscount]->Divide(2, 3);
            cLos_diagnosis[iloscount]->cd(1);
            gPad->SetLogz();
            fh_los_pos_TAMEX[iloscount]->Draw("colz");
            cLos_diagnosis[iloscount]->cd(2);
            gPad->SetLogy();
            fh_los_tres_TAMEX[iloscount]->Draw();
            cLos_diagnosis[iloscount]->cd(3);
            gPad->SetLogz();
            fh_losToT_vs_Events[iloscount]->Draw("colz");
            cLos_diagnosis[iloscount]->cd(4);
            gPad->SetLogz();
            fh_losTAMEX_vs_Events[iloscount]->Draw("colz");
            cLos_diagnosis[iloscount]->cd(5);
            gPad->SetLogz();
            fh_losMCFD_vs_Events[iloscount]->Draw("colz");
            cLos_diagnosis[iloscount]->cd(6);
            gPad->SetLogy();
            fh_los_vftx_tamex[iloscount]->Draw();
            mainfol->Add(cLos_diagnosis[iloscount]);

            cLos_mapped_channels[iloscount]->Divide(nb_pmts, 3);

            for (size_t t = 0; t < fh1_los_mapped.size(); t++)
            {
                cLos_mapped_channels[iloscount]->cd(t + 1);
                fh1_los_mapped[t]->SetFillColor(31);
                fh1_los_mapped[t]->Draw();
            }
            mainfol->Add(cLos_mapped_channels[iloscount]);
        }
        run->AddObject(mainfol);
        run->GetHttpServer()->RegisterCommand("Reset_LOS_HIST", Form("/Objects/%s/->Reset_LOS_Histo()", GetName()));
    }

    // Definition of a time stich object to correlate times coming from different systems
    fTimeStitch = new R3BCoarseTimeStitch();

    return kSUCCESS;
}

void R3BLosOnlineSpectra::Reset_LOS_Histo()
{
    R3BLOG(info, "");
    if (fMappedItems.at(DET_LOS))
    {
        for (size_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        {
            fh_los_channels[iloscount]->Reset();
            fh_los_tres_MCFD[iloscount]->Reset();
            fh_los_tres_TAMEX[iloscount]->Reset();
            fh_los_pos_ToT[iloscount]->Reset();
            fh_los_tot[iloscount]->Reset();
            fh_los_tot_mean[iloscount]->Reset();
            fh_los_pos_MCFD[iloscount]->Reset();
            fh_los_pos_TAMEX[iloscount]->Reset();
            fh_los_multihit[iloscount]->Reset();
            fh_los_ihit_ToT[iloscount]->Reset();
            fh_losTAMEX_vs_Events[iloscount]->Reset();
            fh_losToT_vs_Events[iloscount]->Reset();
            fh_losMCFD_vs_Events[iloscount]->Reset();
            fh_los_dt_hits_ToT[iloscount]->Reset();
        }
        for (auto* hist : fh1_los_mapped)
        {
            hist->Reset();
        }
    }
}

void R3BLosOnlineSpectra::Exec(Option_t* /*option*/)
{
    time = header->GetTimeStamp();
    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }

    if (header->GetTrigger() == 12)
        time_spill_start = time; // header->GetTimeStamp();    // spill start in nsec
    if (header->GetTrigger() == 13)
        time_spill_end = time; // header->GetTimeStamp();    // spill end  in nsec

    fhTrigger->Fill(header->GetTrigger());

    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    if (header->GetTrigger() == 12)
        cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
    if (header->GetTrigger() == 13)
        cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec" << endl;

    Int_t tpatbin = 0;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0)
        {
            fhTpat->Fill(i + 1);
        }
    }

    if ((fTpat > 0) && (header != nullptr) && ((header->GetTpat() & fTpat) != fTpat))
        return;

    //----------------------------------------------------------------------
    // LOS detector
    //----------------------------------------------------------------------
    // Time nomenclature for LOS:
    // lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7
    //   t / \lt
    //   rt| |l
    //   r | |lb
    //   rb\ /b

    Double_t timeTofd = 0;

    Double_t timeLosV[maxnb_los_det][mult_hit];
    Double_t LosTresV[maxnb_los_det][mult_hit];
    Double_t timeLosT[maxnb_los_det][mult_hit];
    Double_t LosTresT[maxnb_los_det][mult_hit];
    Double_t timeLos[maxnb_los_det][mult_hit];
    Double_t totsum[maxnb_los_det][mult_hit];
    Double_t xT_cm[maxnb_los_det][mult_hit];
    Double_t yT_cm[maxnb_los_det][mult_hit];
    Double_t xToT_cm[maxnb_los_det][mult_hit];
    Double_t yToT_cm[maxnb_los_det][mult_hit];
    Double_t xV_cm[maxnb_los_det][mult_hit];
    Double_t yV_cm[maxnb_los_det][mult_hit];

    Double_t time_V[maxnb_los_det][mult_hit][nb_pmts]; // [det][multihit][pm]
    Double_t time_L[maxnb_los_det][mult_hit][nb_pmts];
    Double_t time_T[maxnb_los_det][mult_hit][nb_pmts];
    Double_t tot[maxnb_los_det][mult_hit][nb_pmts];
    Double_t time_MTDC[mult_hit][nb_pmts] = { { 0. } };

    for (size_t idet = 0; idet < fNofLosDetectors; idet++)
    {
        for (size_t imult = 0; imult < mult_hit; imult++)
        {
            timeLosV[idet][imult] = 0.0;
            LosTresV[idet][imult] = 0.0 / 0.0;
            timeLosT[idet][imult] = 0.0;
            LosTresT[idet][imult] = 0.0 / 0.0;
            timeLos[idet][imult] = 0.0;
            totsum[idet][imult] = 0.0;
            xT_cm[idet][imult] = 0.0 / 0.0;
            yT_cm[idet][imult] = 0.0 / 0.0;
            xToT_cm[idet][imult] = -100000.;
            yToT_cm[idet][imult] = -100000.;
            xV_cm[idet][imult] = 0.0 / 0.0;
            yV_cm[idet][imult] = 0.0 / 0.0;
            for (Int_t icha = 0; icha < nb_pmts; icha++)
            {
                time_V[idet][imult][icha] = 0.0 / 0.0; // [det][multihit][pm]
                time_L[idet][imult][icha] = 0.0 / 0.0;
                time_T[idet][imult][icha] = 0.0 / 0.0;
                tot[idet][imult][icha] = 0.0 / 0.0;
            }
        }
    }
    Int_t nPartLOS = 0;
    std::vector<Int_t> nPartc(fNofLosDetectors, 0);

    Bool_t iLOSType[maxnb_los_det][mult_hit];
    Bool_t iLOSPileUp[maxnb_los_det][mult_hit];
    for (size_t idet = 0; idet < fNofLosDetectors; idet++)
    {
        for (size_t imult = 0; imult < mult_hit; imult++)
        {
            iLOSType[idet][imult] = false;
            iLOSPileUp[idet][imult] = false;
        }
    }

    if (fMappedItems.at(DET_LOS))
    {
        auto det = fMappedItems.at(DET_LOS);
        auto nHits = det->GetEntriesFast();

        if (nHits > 0)
            nLosEvents++;

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = dynamic_cast<R3BLosMappedData*>(det->At(ihit));
            if (!hit)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hit->GetDetector(); // 1..
            Int_t iCha = hit->GetChannel();  // 1..
            Int_t iTyp = hit->GetType();     // 0,1,2,3
            // Int_t iFT = hit->GetTimeFine();
            Int_t iCT = hit->GetTimeCoarse();
            if (iTyp == 0 || iTyp == 1)
                fh_los_channels[iDet - 1]->Fill(nb_pmts * iTyp + iCha); // exclude MTDC data
            fh1_los_mapped[nb_pmts * iTyp + iCha - 1]->Fill(iCT * 5);
        }
    }

    if (fCalItems.at(DET_LOS))
    {
        auto det = fCalItems.at(DET_LOS);
        nPartLOS = det->GetEntriesFast();

        Int_t iDet = 0;
        Double_t time_V_LOS1[mult_hit][nb_pmts] = { { 0. } };
        Double_t time_V_LOS2[mult_hit][nb_pmts] = { { 0. } };

        for (size_t iPart = 0; iPart < nPartLOS; iPart++)
        {
            /*
             * nPart is the number of particle passing through LOS detector in one event
             */
            auto calData = dynamic_cast<R3BLosCalData*>(det->At(iPart));
            iDet = calData->GetDetector();

            Double_t sumvtemp = 0, sumltemp = 0, sumttemp = 0;
            for (size_t iCha = 0; iCha < nb_pmts; iCha++)
            {
                sumvtemp += calData->GetTimeV_ns(iCha);
                sumltemp += calData->GetTimeL_ns(iCha);
                sumttemp += calData->GetTimeT_ns(iCha);
            }
            if (!(IS_NAN(sumvtemp)) && !(IS_NAN(sumltemp)) && !(IS_NAN(sumltemp)))
            {
                nPartc[iDet - 1]++;
            }
            else
            {
                continue;
            }

            for (size_t iCha = 0; iCha < nb_pmts; iCha++)
            {
                if (iDet == 1)
                {
                    time_V_LOS1[nPartc[iDet - 1] - 1][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V_LOS1[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeV_ns(iCha);
                    }
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeL_ns(iCha);
                    }

                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeT_ns(iCha);
                    }
                    time_MTDC[nPartc[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeM_ns(iCha))))
                    { // MTDC
                        time_MTDC[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeM_ns(iCha);
                    }
                }
                if (iDet == 2)
                {
                    time_V_LOS2[nPartc[iDet - 1] - 1][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V_LOS2[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeV_ns(iCha);
                    }
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeL_ns(iCha);
                    }
                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeT_ns(iCha);
                    }
                }
            }

            if (!calData)
            {
                cout << "LOS !calData" << endl;
                continue; // can this happen?
            }
        }

        // Sorting VFTX data:

        // detector 1
        if (nPartc[0] > 0)
        {
            std::qsort(time_V_LOS1,
                       nPartc[0],
                       sizeof(*time_V_LOS1),
                       [](const void* arg1, const void* arg2) -> int
                       {
                           double const* lhs = static_cast<double const*>(arg1);
                           double const* rhs = static_cast<double const*>(arg2);

                           return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
                       });
            for (Int_t iPart = 0; iPart < nPartc[0]; iPart++)
            {
                for (size_t ipm = 0; ipm < nb_pmts; ipm++)
                {
                    time_V[0][iPart][ipm] = time_V_LOS1[iPart][ipm];
                }
            }
        }

        // detector 2
        if (fNofLosDetectors > 1 && nPartc[1] > 0)
        {
            std::qsort(time_V_LOS2,
                       nPartc[1],
                       sizeof(*time_V_LOS2),
                       [](const void* arg1, const void* arg2) -> int
                       {
                           double const* lhs = static_cast<double const*>(arg1);
                           double const* rhs = static_cast<double const*>(arg2);

                           return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
                       });
            for (Int_t iPart = 0; iPart < nPartc[1]; iPart++)
            {
                for (size_t ipm = 0; ipm < nb_pmts; ipm++)
                {
                    time_V[1][iPart][ipm] = time_V_LOS2[iPart][ipm];
                }
            }
        }

        // End sorting

        std::vector<double_t> time_first(fNofLosDetectors, -1.);
        std::vector<double_t> time0(fNofLosDetectors, -1.);
        std::vector<double_t> time1(fNofLosDetectors, -1.);
        std::vector<double_t> time_abs(fNofLosDetectors, -1.);

        for (iDet = 1; iDet <= fNofLosDetectors; iDet++)
        {
            for (size_t iPart = 0; iPart < nPartc[iDet - 1]; iPart++)
            {
                Bool_t iLOSTypeMCFD = false;
                Bool_t iLOSTypeTAMEX = false;

                if (time_V[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][0])) &&
                    time_V[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][1])) &&
                    time_V[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][2])) &&
                    time_V[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][3])) &&
                    time_V[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][4])) &&
                    time_V[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][5])) &&
                    time_V[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][6])) &&
                    time_V[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][7])))
                {
                    iLOSTypeMCFD = true; // all 8 MCFD times
                }

                if (time_L[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][0])) &&
                    time_L[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][1])) &&
                    time_L[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][2])) &&
                    time_L[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][3])) &&
                    time_L[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][4])) &&
                    time_L[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][5])) &&
                    time_L[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][6])) &&
                    time_L[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][7])) &&

                    time_T[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][0])) &&
                    time_T[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][1])) &&
                    time_T[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][2])) &&
                    time_T[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][3])) &&
                    time_T[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][4])) &&
                    time_T[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][5])) &&
                    time_T[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][6])) &&
                    time_T[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][7])))
                {
                    iLOSTypeTAMEX = true; // all 8 leading and trailing times
                }

                // We will consider only events in which booth MCFD and TAMEX see same number of channels:
                if (iLOSTypeTAMEX && iLOSTypeMCFD)
                    iLOSType[iDet - 1][iPart] = true;

                if (iLOSType[iDet - 1][iPart])
                {
                    int nPMT = 0;
                    int nPMV = 0;

                    for (size_t ipm = 0; ipm < nb_pmts; ipm++)
                    {

                        if (time_T[iDet - 1][iPart][ipm] > 0. && time_L[iDet - 1][iPart][ipm] > 0. &&
                            !(IS_NAN(time_T[iDet - 1][iPart][ipm])) && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                        {
                            while (time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm] <= 0.)
                            {
                                time_T[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] + 2048. * fClockFreq;
                            }

                            nPMT = nPMT + 1;
                            tot[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm];

                            // pileup rejection
                            if (tot[iDet - 1][iPart][ipm] > fEpileup)
                                iLOSPileUp[iDet - 1][iPart] = true;
                        }

                        if (tot[iDet - 1][iPart][ipm] != 0. && !(IS_NAN(tot[iDet - 1][iPart][ipm])))
                            totsum[iDet - 1][iPart] += tot[iDet - 1][iPart][ipm];

                        if (time_L[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                            timeLosT[iDet - 1][iPart] += time_L[iDet - 1][iPart][ipm];

                        // Calculate detector time
                        if (time_V[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][ipm])))
                        {
                            timeLosV[iDet - 1][iPart] += time_V[iDet - 1][iPart][ipm];
                            nPMV = nPMV + 1;
                        }
                    }

                    totsum[iDet - 1][iPart] = totsum[iDet - 1][iPart] / nPMT;

                    timeLosV[iDet - 1][iPart] = timeLosV[iDet - 1][iPart] / nPMV;

                    timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] / nPMT;

                    timeLos[iDet - 1][iPart] = timeLosV[iDet - 1][iPart];

                    LosTresV[iDet - 1][iPart] = ((time_V[iDet - 1][iPart][0] + time_V[iDet - 1][iPart][2] +
                                                  time_V[iDet - 1][iPart][4] + time_V[iDet - 1][iPart][6]) -
                                                 (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][3] +
                                                  time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][7])) /
                                                4.;

                    LosTresT[iDet - 1][iPart] = ((time_L[iDet - 1][iPart][0] + time_L[iDet - 1][iPart][2] +
                                                  time_L[iDet - 1][iPart][4] + time_L[iDet - 1][iPart][6]) -
                                                 (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][3] +
                                                  time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][7])) /
                                                4.;

                    // right koord.-system, Z-axis beam direction:
                    // Position from tamex:
                    xT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][2]) / 2. -
                                             (time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][6]) / 2.;
                    yT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][3] + time_L[iDet - 1][iPart][4]) / 2. -
                                             (time_L[iDet - 1][iPart][7] + time_L[iDet - 1][iPart][0]) / 2.;
                    xT_cm[iDet - 1][iPart] = (xT_cm[iDet - 1][iPart] - flosOffsetXT[iDet - 1]) * flosVeffXT[iDet - 1];
                    yT_cm[iDet - 1][iPart] = (yT_cm[iDet - 1][iPart] - flosOffsetYT[iDet - 1]) * flosVeffYT[iDet - 1];

                    // Position from VFTX:
                    xV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][2]) / 2. -
                                             (time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][6]) / 2.;
                    yV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][3] + time_V[iDet - 1][iPart][4]) / 2. -
                                             (time_V[iDet - 1][iPart][7] + time_V[iDet - 1][iPart][0]) / 2.;
                    xV_cm[iDet - 1][iPart] = (xV_cm[iDet - 1][iPart] - flosOffsetXV[iDet - 1]) * flosVeffXV[iDet - 1];
                    yV_cm[iDet - 1][iPart] = (yV_cm[iDet - 1][iPart] - flosOffsetYV[iDet - 1]) * flosVeffYV[iDet - 1];

                    //// Position from ToT:
                    // if (tot[iDet - 1][iPart][1] > 0. && tot[iDet - 1][iPart][2] > 0. && tot[iDet - 1][iPart][5] > 0.
                    // &&
                    //     tot[iDet - 1][iPart][6] > 0. && tot[iDet - 1][iPart][0] > 0. && tot[iDet - 1][iPart][3] > 0.
                    //     && tot[iDet - 1][iPart][4] > 0. && tot[iDet - 1][iPart][7] > 0.)
                    //{
                    //     xToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) / 2. -
                    //                                  (tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2]) / 2.) /
                    //                                 ((tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2] +
                    //                                   tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) /
                    //                                  4.));

                    //    yToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7]) / 2. -
                    //                                 (tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) / 2.) /
                    //                                ((tot[iDet - 1][iPart][7] + tot[iDet - 1][iPart][0] +
                    //                                  tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) /
                    //                                 4.));
                    //}

                    // Position from ToT:
                    if (tot[iDet - 1][iPart][1] > 0. && tot[iDet - 1][iPart][5] > 0. && tot[iDet - 1][iPart][3] > 0. &&
                        tot[iDet - 1][iPart][7] > 0.)
                    {
                        xToT_cm[iDet - 1][iPart] = (tot[iDet - 1][iPart][5] - tot[iDet - 1][iPart][1]) /
                                                   ((tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][1]) / 2.);
                        yToT_cm[iDet - 1][iPart] = (tot[iDet - 1][iPart][7] - tot[iDet - 1][iPart][3]) /
                                                   ((tot[iDet - 1][iPart][7] + tot[iDet - 1][iPart][3]) / 2.);
                    }

                    xToT_cm[iDet - 1][iPart] =
                        (xToT_cm[iDet - 1][iPart] - flosOffsetXQ[iDet - 1]) * flosVeffXQ[iDet - 1];
                    yToT_cm[iDet - 1][iPart] =
                        (yToT_cm[iDet - 1][iPart] - flosOffsetYQ[iDet - 1]) * flosVeffYQ[iDet - 1];

                    if (timeLosV[iDet - 1][iPart] > 0. && timeLosV[iDet - 1][iPart] < 8192. * 5. &&
                        !(IS_NAN(timeLosV[iDet - 1][iPart])))
                    {
                        while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] < 2048. * 5. / 2.)
                        {
                            timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] + 2048. * 5.;
                        }
                        while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] > 2048. * 5. / 2.)
                        {
                            timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] - 2048. * 5.;
                        }

                        fh_los_vftx_tamex[iDet - 1]->Fill(fTimeStitch->GetTime(
                            timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart], "tamex", "vftx"));
                    }

                    fh_losMCFD_vs_Events[iDet - 1]->Fill(fNEvents, LosTresV[iDet - 1][iPart]);
                    fh_losTAMEX_vs_Events[iDet - 1]->Fill(fNEvents, LosTresT[iDet - 1][iPart]);
                    fh_losToT_vs_Events[iDet - 1]->Fill(fNEvents, totsum[iDet - 1][iPart]);

                    if (!(iLOSPileUp[iDet - 1][iPart])) // desregard pile-up events
                    {
                        for (size_t ipm = 0; ipm < nb_pmts; ipm++)
                        {
                            fh_los_tot[iDet - 1]->Fill(ipm + 1, tot[iDet - 1][iPart][ipm]);
                        }
                        fh_los_dt_hits_ToT[iDet - 1]->Fill(LosTresV[iDet - 1][iPart], totsum[iDet - 1][iPart]);
                        fh_los_tot_mean[iDet - 1]->Fill(totsum[iDet - 1][iPart]);
                        fh_los_tres_MCFD[iDet - 1]->Fill(LosTresV[iDet - 1][iPart]);
                        fh_los_tres_TAMEX[iDet - 1]->Fill(LosTresT[iDet - 1][iPart]);
                        fh_los_pos_MCFD[iDet - 1]->Fill(xV_cm[iDet - 1][iPart], yV_cm[iDet - 1][iPart]);
                        fh_los_pos_TAMEX[iDet - 1]->Fill(xT_cm[iDet - 1][iPart], yT_cm[iDet - 1][iPart]);
                        fh_los_pos_ToT[iDet - 1]->Fill(xToT_cm[iDet - 1][iPart], yToT_cm[iDet - 1][iPart]);
                        fh_los_ihit_ToT[iDet - 1]->Fill(iPart + 1, totsum[iDet - 1][iPart]);
                        fh_los_multihit[iDet - 1]->Fill(iPart + 1);
                    }
                } // if iLosType
            }     // for iPart
        }         // for iDet

    } // if fCallItems

    fNEvents++;
}

void R3BLosOnlineSpectra::FinishEvent()
{
    for (size_t det = 0; det < DET_MAX; det++)
    {
        if (fMappedItems.at(det))
        {
            fMappedItems.at(det)->Clear();
        }
        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
    }
}

void R3BLosOnlineSpectra::FinishTask()
{
    if (fMappedItems.at(DET_LOS))
    {
        for (size_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        {
            fh_los_channels[iloscount]->Write();
            fh_los_tot[iloscount]->Write();
            fh_los_dt_hits_ToT[iloscount]->Write();
            fh_los_ihit_ToT[iloscount]->Write();
            fh_los_tot_mean[iloscount]->Write();
            fh_los_tres_MCFD[iloscount]->Write();
            fh_los_tres_TAMEX[iloscount]->Write();
            fh_los_pos_MCFD[iloscount]->Write();
            fh_los_pos_TAMEX[iloscount]->Write();
            fh_los_pos_ToT[iloscount]->Write();
        }
        for (auto* hist : fh1_los_mapped)
        {
            hist->Write();
        }
    }
    fhTpat->Write();
    fhTrigger->Write();

    R3BLOG(info, "All events: " << fNEvents << ", LOS events: " << nLosEvents);
}

ClassImp(R3BLosOnlineSpectra)
