/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BScalersOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BShared.h"
#include "R3BTrloiiData.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TPaveText.h>
#include <TVector3.h>

#include <TClonesArray.h>
#include <iomanip>
#include <iostream>
#include <sstream>

R3BScalersOnlineSpectra::R3BScalersOnlineSpectra()
    : R3BScalersOnlineSpectra("R3BScalersOnlineSpectra", 1)
{
}

R3BScalersOnlineSpectra::R3BScalersOnlineSpectra(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

InitStatus R3BScalersOnlineSpectra::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(fatal, fHeader == nullptr, "EventHeader. not found");

    // get access to Hit data
    fScalerItems = dynamic_cast<TClonesArray*>(mgr->GetObject("TrloiiData"));
    R3BLOG_IF(fatal, !fScalerItems, "TrloiiData not found");

    // Create histograms for detectors

    cRate = std::make_unique<TCanvas>("Rates_per_spill", "Rates_per_spill", 10, 10, 500, 500);
    cRate->Divide(3, 2);
    fh1_rate.resize(6);

    for (int i = 0; i < 6; i++)
    {
        cRate->cd(i + 1);
        std::stringstream ss1;
        ss1 << "fh1_Rate_" << i + 1;
        std::stringstream ss2;
        ss2 << "Rate for TPAT " << i + 1;
        fh1_rate[i] = R3B::root_owned<TH1F>(ss1.str().c_str(), ss2.str().c_str(), 1000, 0, 10000);
        fh1_rate[i]->GetXaxis()->SetTitle("Rate per spill");
        fh1_rate[i]->GetYaxis()->SetTitle("Counts");
        fh1_rate[i]->GetYaxis()->SetTitleOffset(1.1);
        fh1_rate[i]->GetXaxis()->CenterTitle(true);
        fh1_rate[i]->GetYaxis()->CenterTitle(true);
        fh1_rate[i]->SetFillColor(4);
        fh1_rate[i]->SetLineColor(1);
        fh1_rate[i]->Draw("");
    }

    cDts = std::make_unique<TCanvas>("Dead_times", "Dead times per tpat", 10, 10, 500, 500);
    cDts->Divide(4, 4);
    fh1_dt.resize(fNbHistScalers);
    tpatextra.resize(fNbHistScalers);

    for (int i = 0; i < fNbHistScalers; i++)
    {
        cDts->cd(i + 1);
        std::stringstream ss1;
        ss1 << "fh1_DT_" << i + 1;
        if (i < 6)
            fh1_dt[i] = R3B::root_owned<TH1F>(ss1.str().c_str(), ss1.str().c_str(), 1000, 0, 50);
        else
            fh1_dt[i] = R3B::root_owned<TH1F>(ss1.str().c_str(), ss1.str().c_str(), 1000, 0, 100);
        fh1_dt[i]->GetXaxis()->SetTitle("DT (%)");
        fh1_dt[i]->GetYaxis()->SetTitle("Counts");
        fh1_dt[i]->GetYaxis()->SetTitleOffset(1.1);
        fh1_dt[i]->GetXaxis()->CenterTitle(true);
        fh1_dt[i]->GetYaxis()->CenterTitle(true);
        fh1_dt[i]->SetFillColor(2);
        fh1_dt[i]->SetLineColor(1);
        fh1_dt[i]->Draw("");
        tpatextra[i] = 0.;
    }

    cDts2 = std::make_unique<TCanvas>("Dead_times_Stefanos", "Dead times", 10, 10, 500, 500);
    cDts2->Divide(3, 2);
    fh1_dt2.resize(6);

    for (int i = 0; i < 6; i++)
    {
        cDts2->cd(i + 1);
        std::stringstream ss1;
        ss1 << "fh1_DT2_" << i + 1;

        fh1_dt2[i] = R3B::root_owned<TH1F>(ss1.str().c_str(), ss1.str().c_str(), 500, 0, 60);
        fh1_dt2[i]->GetXaxis()->SetTitle("DT (%)");
        fh1_dt2[i]->GetYaxis()->SetTitle("Counts");
        fh1_dt2[i]->GetYaxis()->SetTitleOffset(1.1);
        fh1_dt2[i]->GetXaxis()->CenterTitle(true);
        fh1_dt2[i]->GetYaxis()->CenterTitle(true);
        fh1_dt2[i]->SetFillColor(2);
        fh1_dt2[i]->SetLineColor(1);
        fh1_dt2[i]->Draw("");
    }

    cDts_time = std::make_unique<TCanvas>("Dead_times_vs_spill", "Dead times vs Spill", 10, 10, 500, 500);
    cDts_time->Divide(3, 2);
    fh2_dt_time.resize(6);
    for (int i = 0; i < 6; i++)
    {
        cDts_time->cd(i + 1);
        std::stringstream ss1;
        ss1 << "fh2_DT_time" << i + 1;
        std::stringstream ss2;
        ss2 << "Dead time for TPAT " << i + 1;

        fh2_dt_time[i] = R3B::root_owned<TH2F>(ss1.str().c_str(), ss1.str().c_str(), 2000, 0, 2000, 500, 0, 60);
        fh2_dt_time[i]->GetXaxis()->SetTitle("Spill number");
        fh2_dt_time[i]->GetYaxis()->SetTitle("DT (%)");
        fh2_dt_time[i]->GetYaxis()->SetTitleOffset(1.1);
        fh2_dt_time[i]->GetXaxis()->CenterTitle(true);
        fh2_dt_time[i]->GetYaxis()->CenterTitle(true);
        fh2_dt_time[i]->SetMarkerColor(2);
        fh2_dt_time[i]->SetMarkerStyle(21);
        fh2_dt_time[i]->SetMarkerSize(1.5);
        fh2_dt_time[i]->Draw("p");
    }

    // MAIN FOLDER
    auto mainfol = new TFolder("Scalers", "Scalers info");

    if (fScalerItems)
    {
        mainfol->Add(cRate.get());
        mainfol->Add(cDts.get());
        mainfol->Add(cDts2.get());
        mainfol->Add(cDts_time.get());
    }

    auto run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Scalers", Form("/Objects/%s/->Reset_Histo()", GetName()));

    for (Int_t i = 0; i < 4; i++)
        for (Int_t j = 0; j < fNbHistScalers; j++)
        {
            prevscaler[i][j] = 0;
        }
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BScalersOnlineSpectra::ReInit() { return kSUCCESS; }

void R3BScalersOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");
    for (const auto& hist : fh2_dt_time)
    {
        hist->Reset();
    }
    for (const auto& hist : fh1_rate)
    {
        hist->Reset();
    }
    for (const auto& hist : fh1_dt)
    {
        hist->Reset();
    }
    for (const auto& hist : fh1_dt2)
    {
        hist->Reset();
    }
}

void R3BScalersOnlineSpectra::Exec(Option_t* /*option*/)
{
    int tpatbin = 0;
    if (fHeader->GetTpat() > 0)
    {
        for (int i = 0; i < fNbHistScalers; i++)
        {
            tpatbin = (fHeader->GetTpat() & (1 << i));
            if (tpatbin != 0)
            {
                tpatextra[i]++;
            }
        }
    }

    if (fHeader->GetTrigger() == triggerSpillOn && !is_starting)
    {
        is_starting = true;
    }

    if (is_starting == false)
    {
        return;
    }

    int nHits = fScalerItems->GetEntriesFast();

    if (nHits > 0)
    {
        for (Int_t j = 0; j < fNbHistScalers; j++)
        {
            dt[j] = 0;
            ds[j] = 0;
            for (Int_t k = 0; k < 4; k++)
                nb_proj[k][j] = 0;
        }

        R3BTrloiiData** trloData = new R3BTrloiiData*[nHits];
        for (int j = 0; j < nHits; j++)
        {
            trloData[j] = dynamic_cast<R3BTrloiiData*>(fScalerItems->At(j));
            auto counts = trloData[j]->GetCounts();
            auto ch = trloData[j]->GetCh() - 1;
            auto ty = trloData[j]->GetType() - 1;

            nb_proj[ty][ch] += (counts - prevscaler[ty][ch]);
            prevscaler[ty][ch] = counts;
        }
        if (trloData)
            delete[] trloData;

        if (fHeader->GetTrigger() == triggerSpillOff)
        {
            is_starting = false;

            std::stringstream fprint;
            fprint << "\nMean values as a function of the TPAT" << std::endl;
            fprint << "TPAT \t\t Dead Time(%) \t Downscaling \tTpats before DT      \tTpats after DT \tTpats after DS "
                      "\t Tpats from RootFile \t DT-Stefanos (%)"
                   << std::endl;

            for (int s = 0; s < fNbHistScalers; s++)
            {
                dt[s] = (1. - nb_proj[2][s] / nb_proj[1][s]) * 100.;
                ds[s] = nb_proj[2][s] / nb_proj[3][s];
                fh1_dt[s]->Fill(dt[s]);
                if (s < 6)
                {
                    auto vdt = nb_proj[1][s + 6] / (nb_proj[1][s] + nb_proj[1][s + 6]) * 100.;
                    fprint << "# " << std::setw(2) << s + 1 << "\t\t" << std::fixed << std::setw(4)
                           << std::setprecision(1) << dt[s] << "\t\t" << std::fixed << std::setw(4)
                           << std::setprecision(0) << ds[s] << "\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << nb_proj[1][s] << "\t\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << nb_proj[2][s] << "\t\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << nb_proj[3][s] << "\t\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << tpatextra[s] << "\t\t\t" << std::fixed << std::setw(3)
                           << std::setprecision(2) << vdt << std::endl;
                    if (vdt > 0)
                    {
                        fh1_dt2[s]->Fill(vdt);
                        fh2_dt_time[s]->Fill(fNEvents, vdt);
                    }
                    if (nb_proj[3][s] > 0)
                        fh1_rate[s]->Fill(nb_proj[3][s]);
                }
                else
                    fprint << "# " << std::setw(2) << s + 1 << "\t\t" << std::fixed << std::setw(4)
                           << std::setprecision(1) << dt[s] << "\t\t" << std::fixed << std::setw(4)
                           << std::setprecision(0) << ds[s] << "\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << nb_proj[1][s] << "\t\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << nb_proj[2][s] << "\t\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << nb_proj[3][s] << "\t\t\t" << std::fixed << std::setw(5)
                           << std::setprecision(0) << tpatextra[s] << "\t\t\t" << std::fixed << std::setw(3)
                           << std::setprecision(2) << (1. - tpatextra[s] / nb_proj[3][s]) * 100. << std::endl;
            }
            fNEvents += 1;
            fprint << "\nNumber of spills: " << fNEvents << std::endl;
            std::cout << fprint.str() << std::endl;
            fprint.clear();

            for (int i = 0; i < fNbHistScalers; i++)
                tpatextra[i] = 0.;
        }
    }
}

void R3BScalersOnlineSpectra::FinishEvent()
{
    if (fScalerItems)
    {
        fScalerItems->Clear();
    }
}

void R3BScalersOnlineSpectra::FinishTask()
{
    // Write canvas
    if (fScalerItems)
    {
        for (const auto& hist : fh2_dt_time)
        {
            hist->Write();
        }
        for (const auto& hist : fh1_rate)
        {
            hist->Write();
        }
        for (const auto& hist : fh1_dt)
        {
            hist->Write();
        }
        for (const auto& hist : fh1_dt2)
        {
            hist->Write();
        }
    }
}

ClassImp(R3BScalersOnlineSpectra)
