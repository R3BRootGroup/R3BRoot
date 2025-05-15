/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BMwpcvsFootOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BFootHitData.h"
#include "R3BLogger.h"
#include "R3BMwpcHitData.h"
#include "R3BShared.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TFolder.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TVector3.h>

#include <TClonesArray.h>
#include <sstream>
#include <string>

R3BMwpcvsFootOnlineSpectra::R3BMwpcvsFootOnlineSpectra()
    : R3BMwpcvsFootOnlineSpectra("R3BMwpcvsFootOnlineSpectra", 1)
{
}

R3BMwpcvsFootOnlineSpectra::R3BMwpcvsFootOnlineSpectra(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

void R3BMwpcvsFootOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading amsStripCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");
}

InitStatus R3BMwpcvsFootOnlineSpectra::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(fatal, fHeader == nullptr, "EventHeader. not found");

    // get access to Hit data
    fHitItemsMw = dynamic_cast<TClonesArray*>(mgr->GetObject("Mwpc0HitData"));
    R3BLOG_IF(fatal, !fHitItemsMw, "Mwpc0HitData not found");

    fHitItemsFoot = dynamic_cast<TClonesArray*>(mgr->GetObject("FootHitData"));
    R3BLOG_IF(fatal, !fHitItemsFoot, "FootHitData not found");

    // Create histograms for detectors
    cCMwF = std::make_unique<TCanvas>("Mwpc_vs_Foot", "Mwpc vs Foot correlations", 10, 10, 500, 500);
    cCMwF->Divide(2, 3);

    std::vector<std::string> hist_names;
    hist_names.push_back("fh2_MwpcX_Foot1X");
    hist_names.push_back("fh2_MwpcX_Foot3X");
    hist_names.push_back("fh2_MwpcY_Foot2Y");
    hist_names.push_back("fh2_MwpcY_Foot4Y");
    hist_names.push_back("fh2_MwpcX_Foot2Y");
    hist_names.push_back("fh2_MwpcY_Foot1X");

    std::vector<std::string> hist_titles;
    hist_titles.push_back("MwpcX vs Foot1X");
    hist_titles.push_back("MwpcX vs Foot3X");
    hist_titles.push_back("MwpcY vs Foot2Y");
    hist_titles.push_back("MwpcY vs Foot4Y");
    hist_titles.push_back("MwpcX vs Foot2Y");
    hist_titles.push_back("MwpcY vs Foot1X");

    std::vector<std::string> hist_xaxis_titles;
    hist_xaxis_titles.push_back("Foot1X [mm]");
    hist_xaxis_titles.push_back("Foot3X [mm]");
    hist_xaxis_titles.push_back("Foot2Y [mm]");
    hist_xaxis_titles.push_back("Foot4Y [mm]");
    hist_xaxis_titles.push_back("MwpcX [mm]");
    hist_xaxis_titles.push_back("Foot1X [mm]");

    std::vector<std::string> hist_yaxis_titles;
    hist_yaxis_titles.push_back("MwpcX [mm]");
    hist_yaxis_titles.push_back("MwpcX [mm]");
    hist_yaxis_titles.push_back("MwpcY [mm]");
    hist_yaxis_titles.push_back("MwpcY [mm]");
    hist_yaxis_titles.push_back("Foot2Y [mm]");
    hist_yaxis_titles.push_back("MwpcY [mm]");

    fh2_cor.resize(hist_names.size());
    fNbHist = hist_names.size();
    for (size_t i = 0; i < hist_names.size(); i++)
    {
        cCMwF->cd(i + 1);
        gPad->SetLogz();
        std::stringstream ss1;
        ss1 << "fh2_Mwpc_Tttx_Xcor_" << i;
        fh2_cor[i] = R3B::root_owned<TH2F>(hist_names[i].c_str(), hist_titles[i].c_str(), 400, -100, 100, 67, -50, 50);
        fh2_cor[i]->GetXaxis()->SetTitle(hist_xaxis_titles[i].c_str());
        fh2_cor[i]->GetYaxis()->SetTitle(hist_yaxis_titles[i].c_str());
        fh2_cor[i]->GetYaxis()->SetTitleOffset(1.2);
        fh2_cor[i]->GetXaxis()->CenterTitle(true);
        fh2_cor[i]->GetYaxis()->CenterTitle(true);
        fh2_cor[i]->Draw("COLZ");
    }

    // MAIN FOLDER-Mwpc-Foot
    auto mainfol = new TFolder("Mwpc_vs_Foot", "MWPC vs Foot info");

    if (fHitItemsMw && fHitItemsFoot)
    {
        mainfol->Add(cCMwF.get());
    }

    auto run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Mwpc_vs_Foot", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BMwpcvsFootOnlineSpectra::ReInit()
{
    SetParContainers();
    // SetParameter();
    return kSUCCESS;
}

void R3BMwpcvsFootOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");
    for (const auto& hist : fh2_cor)
    {
        hist->Reset();
    }
}

void R3BMwpcvsFootOnlineSpectra::Exec(Option_t* /*option*/)
{
    if ((fTpat > 0) && (fHeader) && ((fHeader->GetTpat() & fTpat) != fTpat))
        return;

    int nHitsMw = fHitItemsMw->GetEntriesFast();
    int nHitsFo = fHitItemsFoot->GetEntriesFast();

    if (nHitsMw == 0 || nHitsFo == 0)
        return;

    std::vector<TVector3> VMw;
    for (size_t ihit = 0; ihit < nHitsMw; ihit++)
    {
        auto hit = dynamic_cast<R3BMwpcHitData*>(fHitItemsMw->At(ihit));
        VMw.push_back(TVector3(hit->GetX(), hit->GetY(), 0));
    }

    for (size_t ihit = 0; ihit < nHitsFo; ihit++)
    {
        auto hit = dynamic_cast<R3BFootHitData*>(fHitItemsFoot->At(ihit));
        auto idet = hit->GetDetId() - 1;
        auto pos = hit->GetPos();
        if (idet < 0)
            continue;

        if (idet == 0 || idet == 2) // Foot detectors in X
            for (const auto& vtemp : VMw)
            {
                auto index = idet == 0 ? 0 : 1;
                if (abs(vtemp.X() - pos) < fMinDist)
                    fh2_cor[index]->Fill(pos, vtemp.X());
            }
        else if (idet == 1 || idet == 3) // Foot detectors in Y
            for (const auto& vtemp : VMw)
            {
                auto index = idet == 1 ? 2 : 3;
                if (abs(vtemp.Y() - pos) < fMinDist)
                    fh2_cor[index]->Fill(pos, vtemp.Y());
            }

        if (idet == 1) // Foot detector in Y closer to MW
            for (const auto& vtemp : VMw)
            {
                fh2_cor[fNbHist - 2]->Fill(vtemp.X(), pos);
            }

        if (idet == 0) // Foot detector in X closer to MW
            for (const auto& vtemp : VMw)
            {
                fh2_cor[fNbHist - 1]->Fill(pos, vtemp.Y());
            }
    }
    fNEvents++;
}

void R3BMwpcvsFootOnlineSpectra::FinishEvent()
{
    if (fHitItemsMw)
    {
        fHitItemsMw->Clear();
    }
    if (fHitItemsFoot)
    {
        fHitItemsFoot->Clear();
    }
}

void R3BMwpcvsFootOnlineSpectra::FinishTask()
{
    // Write canvas for Hit data
    if (fHitItemsMw && fHitItemsFoot)
    {
        for (const auto& hist : fh2_cor)
        {
            hist->Write();
        }
    }
}

ClassImp(R3BMwpcvsFootOnlineSpectra)
