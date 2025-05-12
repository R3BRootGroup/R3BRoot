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
// -----                R3BRoluOnlineSpectra              -----
// -----          Created April 13th 2016 by M.Heil       -----
// -----       Updated May 11th 2022 by J.L. Rodriguez    -----
// ------------------------------------------------------------

#include "R3BRoluOnlineSpectra.h"
#include "R3BCoarseTimeStitch.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BRoluCalData.h"
#include "R3BRoluMappedData.h"
#include "R3BShared.h"
#include "R3BTCalEngine.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TClonesArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TMath.h>
#include <boost/multi_array.hpp>
#include <vector>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;
namespace
{
    double c_range_ns = 2048 * 5;
} // namespace

R3BRoluOnlineSpectra::R3BRoluOnlineSpectra()
    : R3BRoluOnlineSpectra("RoluOnlineSpectra", 1)
{
}

R3BRoluOnlineSpectra::R3BRoluOnlineSpectra(const char* name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

InitStatus R3BRoluOnlineSpectra::Init()
{
    R3BLOG(info, "");

    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // Get objects for detectors on all levels

    fMappedItems = dynamic_cast<TClonesArray*>(mgr->GetObject("RoluMapped"));
    R3BLOG_IF(fatal, nullptr == fMappedItems, "RoluMapped not found");

    fCalItems = dynamic_cast<TClonesArray*>(mgr->GetObject("RoluCal"));
    R3BLOG_IF(warn, nullptr == fCalItems, "RoluCal not found");

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    // MAIN FOLDER-MWPC
    auto mainfol = new TFolder("ROLU", "ROLU info");

    //------------------------------------------------------------------------
    // Rolu detector
    std::vector<TCanvas*> cRolu;
    TString histName;
    TString canName;

    fh1_rolu_tot.resize(fNofRoluDetectors);
    fh1_rolu_LE_raw.resize(fNofRoluDetectors);

    if (fMappedItems)
    {
        for (int irolucount = 0; irolucount < fNofRoluDetectors; irolucount++)
        {
            histName = Form("ROLU%d", irolucount + 1);
            cRolu.push_back(new TCanvas(canName.Data(), canName.Data(), 10, 10, 1010, 810));

            histName = Form("Rolu%d_channels", irolucount);
            fh1_rolu_channels.push_back(R3B::root_owned<TH1F>(histName.Data(), histName.Data(), 5, 0., 5.));
            fh1_rolu_channels[irolucount]->GetXaxis()->SetTitle("Channel number");
            fh1_rolu_channels[irolucount]->SetFillColor(31);

            histName = Form("Rolu%d_tot", irolucount);
            fh2_rolu_tot.push_back(R3B::root_owned<TH2F>(histName.Data(), histName.Data(), 5, 0, 5, 1500, 0., 300.));
            fh2_rolu_tot[irolucount]->GetXaxis()->SetTitle("PMT number");
            fh2_rolu_tot[irolucount]->GetYaxis()->SetTitle("ToT / ns");

            histName = Form("Rolu%d_multi", irolucount);
            fh1_rolu_multiplicity.push_back(R3B::root_owned<TH1F>(histName.Data(), histName.Data(), 7, -0.5, 6.5));
            fh1_rolu_multiplicity[irolucount]->GetXaxis()->SetTitle("Hits multiplicity");
            fh1_rolu_multiplicity[irolucount]->GetYaxis()->SetTitle("Counts");
            fh1_rolu_multiplicity[irolucount]->SetFillColor(31);

            histName = Form("Rolu%d_hit1vshit2", irolucount);
            fh2_rolu_hit1vshit2.push_back(
                R3B::root_owned<TH2F>(histName.Data(), histName.Data(), 4, 0.5, 4.5, 4, 0.5, 4.5));
            fh2_rolu_hit1vshit2[irolucount]->GetXaxis()->SetTitle("Hit1");
            fh2_rolu_hit1vshit2[irolucount]->GetYaxis()->SetTitle("Hit2");

            int color;
            for (int ichannelcount = 0; ichannelcount < rolu_nb_ch; ichannelcount++)
            {
                color = ichannelcount + 1;
                if (color == 3)
                    color = 8;
                histName = Form("Rolu%d_tot1D_ch%d", irolucount, ichannelcount);
                fh1_rolu_tot[irolucount].push_back(
                    R3B::root_owned<TH1F>(histName.Data(), histName.Data(), 151, 0., 150.));
                fh1_rolu_tot[irolucount][ichannelcount]->GetXaxis()->SetTitle("ToT / ns");
                fh1_rolu_tot[irolucount][ichannelcount]->SetLineColor(color);

                histName = Form("Rolu%d_LE_raw_ch%d", irolucount, ichannelcount);
                fh1_rolu_LE_raw[irolucount].push_back(
                    R3B::root_owned<TH1F>(histName.Data(), histName.Data(), 1050, 0, 10500));
                fh1_rolu_LE_raw[irolucount][ichannelcount]->GetXaxis()->SetTitle("ToT / ns");
                fh1_rolu_LE_raw[irolucount][ichannelcount]->SetLineColor(color);
            }

            cRolu[irolucount]->Divide(2, 3);
            cRolu[irolucount]->cd(1);
            fh1_rolu_channels[irolucount]->Draw();
            gPad->SetLogy();
            cRolu[irolucount]->cd(2);
            gPad->SetLogz();
            fh2_rolu_tot[irolucount]->Draw("colz");
            for (size_t ichannelcount = 0; ichannelcount < rolu_nb_ch; ichannelcount++)
            {
                cRolu[irolucount]->cd(3);
                if (ichannelcount == 0)
                    fh1_rolu_tot[irolucount][ichannelcount]->Draw();
                else
                    fh1_rolu_tot[irolucount][ichannelcount]->Draw("same");
                gPad->SetLogy();
                cRolu[irolucount]->cd(4);
                gPad->SetLogy();
                if (ichannelcount == 0)
                    fh1_rolu_LE_raw[irolucount][ichannelcount]->Draw();
                else
                    fh1_rolu_LE_raw[irolucount][ichannelcount]->Draw("same");
            }
            cRolu[irolucount]->cd(5);
            fh1_rolu_multiplicity[irolucount]->Draw();
            cRolu[irolucount]->cd(6);
            fh2_rolu_hit1vshit2[irolucount]->Draw("colz");
            mainfol->Add(cRolu[irolucount]);
        }
        run->AddObject(mainfol);
        run->GetHttpServer()->RegisterCommand("Reset_ROLU_HIST", Form("/Objects/%s/->Reset_ROLU_Histo()", GetName()));
    }
    return kSUCCESS;
}

void R3BRoluOnlineSpectra::Reset_ROLU_Histo()
{
    R3BLOG(info, "");
    if (fMappedItems)
    {
        for (auto* hist : fh1_rolu_channels)
        {
            hist->Reset();
        }

        for (auto* hist : fh1_rolu_multiplicity)
        {
            hist->Reset();
        }

        for (auto* hist : fh2_rolu_hit1vshit2)
        {
            hist->Reset();
        }
    }
    if (fCalItems)
    {
        for (auto* hist : fh2_rolu_tot)
        {
            hist->Reset();
        }
        for (const auto& tot_vec : fh1_rolu_tot)
        {
            for (auto* hist : tot_vec)
            {
                if (hist)
                    hist->Reset();
            }
        }

        for (const auto& le_vec : fh1_rolu_LE_raw)
        {
            for (auto* hist : le_vec)
            {
                if (hist)
                    hist->Reset();
            }
        }
    }
}

void R3BRoluOnlineSpectra::Exec(Option_t* option)
{
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    if ((fTpat > 0) && (header) && ((header->GetTpat() & fTpat) != fTpat))
        return;

    int nPartROLU = 0;
    double tot = 0;

    if (fMappedItems)
    {
        int nHits = fMappedItems->GetEntriesFast();
        if (nHits > 0)
            nRoluEvents++;

        std::vector<uint16_t> det_mul(fNofRolu, 0);
        std::vector<uint32_t> ch1(fNofRolu, 0);
        std::vector<uint32_t> ch2(fNofRolu, 0);
        for (size_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = dynamic_cast<R3BRoluMappedData*>(fMappedItems->At(ihit));
            if (!hit)
                continue;
            auto detid = hit->GetDetector() - 1;
            auto type = hit->GetType();
            det_mul[detid]++;
            if (type == 0)
            {
                if (ch1[detid] == 0)
                    ch1[detid] = hit->GetChannel();
                else
                    ch2[detid] = hit->GetChannel();
            }
            // channel numbers are stored 1-based (1..n)
            fh1_rolu_channels[detid]->Fill(hit->GetChannel());
        }
        for (size_t det = 0; det < fNofRoluDetectors; det++)
        {
            fh1_rolu_multiplicity[det]->Fill(det_mul[det]);
            if (ch1[det] > 0 && ch2[det] > 0)
                fh2_rolu_hit1vshit2[det]->Fill(ch1[det], ch2[det]);
        }
    }

    if (fCalItems)
    {
        nPartROLU = fCalItems->GetEntriesFast();
        int iDet = 0;

        for (size_t iPart = 0; iPart < nPartROLU; iPart++)
        {
            auto calData = dynamic_cast<R3BRoluCalData*>(fCalItems->At(iPart));
            assert(calData && "dynamic cast failed!");
            auto detid = calData->GetDetector() - 1;
            for (size_t chan = 0; chan < rolu_nb_ch; chan++)
            { // TAMEX leading
                double time_L = calData->GetTimeL_ns(chan);
                double time_T = calData->GetTimeT_ns(chan);
                tot = fmod(time_T - time_L + c_range_ns + c_range_ns / 2., c_range_ns) - c_range_ns / 2.;
                if (!std::isnan(tot))
                {
                    fh2_rolu_tot[detid]->Fill(chan + 1, tot);
                    fh1_rolu_tot[detid][chan]->Fill(tot);
                    fh1_rolu_LE_raw[detid][chan]->Fill(calData->GetTimeL_ns(chan));
                }
            }
        }

        for (size_t det = 0; det < fNofRoluDetectors; det++)
        {
            double maxentries = 0;
            for (size_t i = 0; i < rolu_nb_ch; i++)
                maxentries = max(maxentries, fh1_rolu_tot[det][i]->GetMaximum());
            fh1_rolu_tot[det][0]->SetMaximum(maxentries);
        }
    } // if fCallItems
}

void R3BRoluOnlineSpectra::FinishEvent()
{
    if (fMappedItems)
    {
        fMappedItems->Clear();
    }
    if (fCalItems)
    {
        fCalItems->Clear();
    }
}

void R3BRoluOnlineSpectra::FinishTask()
{
    if (fMappedItems)
    {
        for (auto* hist : fh1_rolu_channels)
        {
            hist->Write();
        }

        for (auto* hist : fh1_rolu_multiplicity)
        {
            hist->Write();
        }

        for (auto* hist : fh2_rolu_hit1vshit2)
        {
            hist->Write();
        }
    }

    if (fCalItems)
    {
        for (auto* hist : fh2_rolu_tot)
        {
            hist->Write();
        }
        for (const auto& tot_vec : fh1_rolu_tot)
        {
            for (auto* hist : tot_vec)
            {
                if (hist)
                    hist->Write();
            }
        }

        for (const auto& le_vec : fh1_rolu_LE_raw)
        {
            for (auto* hist : le_vec)
            {
                if (hist)
                    hist->Write();
            }
        }
    }
}

ClassImp(R3BRoluOnlineSpectra)
