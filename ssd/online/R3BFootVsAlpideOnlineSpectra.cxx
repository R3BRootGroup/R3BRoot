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

// --------------------------------------------------------------
// -----       R3BFootVsAlpideOnlineSpectra             --
// -----    Created 17/02/19  by J.L. Rodriguez-Sanchez        --
// ----- Fill FOOT and ALPIDE correlations in online histograms --
// --------------------------------------------------------------

/*
 *  This taks reads hit data from FOOT and ALPIDE detectors and plots
 *  online histograms
 */

#include "R3BFootVsAlpideOnlineSpectra.h"
#include "R3BAlpideHitData.h"
#include "R3BEventHeader.h"
#include "R3BFootHitData.h"
#include "R3BLogger.h"
#include "R3BShared.h"
#include "THttpServer.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TVector3.h"

#include "R3BTCalEngine.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#define IS_NAN(x) TMath::IsNaN(x)

using namespace std;

R3BFootVsAlpideOnlineSpectra::R3BFootVsAlpideOnlineSpectra()
    : FairTask("FootVsAlpideOnlineSpectra", 1)
    , fHitItemsFoot(NULL)
    , fHitItemsAlpide(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTrigger(-1)
    , fNEvents(0)
    , fNbDet(8)
{
}

R3BFootVsAlpideOnlineSpectra::R3BFootVsAlpideOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fHitItemsFoot(NULL)
    , fHitItemsAlpide(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTrigger(-1)
    , fNEvents(0)
    , fNbDet(8)
{
}

R3BFootVsAlpideOnlineSpectra::~R3BFootVsAlpideOnlineSpectra() {}

InitStatus R3BFootVsAlpideOnlineSpectra::Init()
{

    LOG(info) << "R3BFootVsAlpideOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3BFootVsAlpideOnlineSpectra::Init FairRootManager not found";
    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // create histograms of all detectors

    // get access to Hit data
    fHitItemsFoot = dynamic_cast<TClonesArray*>(mgr->GetObject("FootHitData"));
    if (!fHitItemsFoot)
    {
        LOG(info) << "R3BFootVsAlpideOnlineSpectra::Init FootHitData not found";
    }

    // get access to Hit data
    fHitItemsAlpide = dynamic_cast<TClonesArray*>(mgr->GetObject("AlpideHitData"));
    if (!fHitItemsAlpide)
    {
        LOG(info) << "R3BFootVsAlpideOnlineSpectra::Init AlpideHitData not found";
    }

    // Name variables
    char Name1[255];
    char Name2[255];

    // Folders
    TFolder* mainfol = new TFolder("Foot-Alpide", "Foot-Alpide correlation info");

    // ================ Position X correlation canvas =====================
    cPosCorr = new TCanvas("Foot_vs_Alpide_pos_corr", "position correlation info", 10, 10, 500, 500);
    cPosCorr->Divide(4, 2);
    mainfol->Add(cPosCorr);

    fh2_foot_alpide_pos_corr.resize(fNbDet);
    int i_pad = 1;

    for (int i = 0; i < fNbDet; i++)
    {
        sprintf(Name1, "fh2_foot_vs_alpide_posCorr_det_%d", i + 1);
        sprintf(Name2, "FOOT vs ALPIDE position correlation for FOOT: %d", i + 1);

        fh2_foot_alpide_pos_corr[i] = R3B::root_owned<TH2F>(Name1, Name2, 100, -50, 50, 100, -50, 50);
        fh2_foot_alpide_pos_corr[i]->GetXaxis()->SetTitle("FOOT [mm]");
        fh2_foot_alpide_pos_corr[i]->GetYaxis()->SetTitle("ALPIDE [mm]");
        fh2_foot_alpide_pos_corr[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_foot_alpide_pos_corr[i]->GetXaxis()->CenterTitle(true);
        int foot_num = i + 1;
        if (foot_num < 9)
        {
            cPosCorr->cd(i_pad);
            gPad->SetLogz(true);
            fh2_foot_alpide_pos_corr[i]->Draw("col");
            i_pad++;
        }
    }

    // ================ Energy correlation canvas =====================
    cCharCorr = new TCanvas("Foot_vs_Alpide_char_corr", "Charge correlation info", 10, 10, 500, 500);
    cCharCorr->Divide(4, 2);
    mainfol->Add(cCharCorr);

    fh2_foot_alpide_char_corr.resize(fNbDet);
    i_pad = 1;

    for (int i = 0; i < fNbDet; i++)
    {
        sprintf(Name1, "fh2_foot_vs_alpide_char_det_%d", i + 1);
        sprintf(Name2, "FOOT vs ALPIDE charge correlation Y for FOOT: %d", i + 1);

        fh2_foot_alpide_char_corr[i] = R3B::root_owned<TH2F>(Name1, Name2, 1000, 0, 5000, 100, 0, 100);
        fh2_foot_alpide_char_corr[i]->GetXaxis()->SetTitle("FOOT Energy [channels]");
        fh2_foot_alpide_char_corr[i]->GetYaxis()->SetTitle("ALPIDE Cluster Size");
        fh2_foot_alpide_char_corr[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_foot_alpide_char_corr[i]->GetXaxis()->CenterTitle(true);
        int foot_num = i + 1;
        if (foot_num < 9)
        {
            cCharCorr->cd(i_pad);
            fh2_foot_alpide_char_corr[i]->Draw("col");
            i_pad++;
        }
    }

    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset", Form("/Objects/%s/->Reset_FOOT_ALPIDE_Histo()", GetName()));

    return kSUCCESS;
}

void R3BFootVsAlpideOnlineSpectra::Reset_FOOT_ALPIDE_Histo()
{
    LOG(info) << "R3BFootVsAlpideOnlineSpectra::Reset_FOOT_ALPIDE_Histo";
    for (int i = 0; i < fNbDet; i++)
    {
        fh2_foot_alpide_char_corr[i]->Reset();
        fh2_foot_alpide_pos_corr[i]->Reset();
    }

    return;
}

void R3BFootVsAlpideOnlineSpectra::Exec(Option_t* option)
{

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3BFootVsAlpideOnlineSpectra::Exec FairRootManager not found";

    // if(header->GetTrigger()!=1){cout << header->GetTrigger()<<endl;}
    // if ((fTrigger >= 0) && (header) && (header->GetTrigger() != 1))
    // return;

    // Check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header != nullptr) && (header->GetTrigger() != fTrigger))
        return;

    if (fTpat1 >= 0 && fTpat2 >= 0 && (header))
    {
        // fTpat = 1-16; fTpat_bit = 0-15
        Int_t fTpat_bit1 = fTpat1 - 1;
        Int_t fTpat_bit2 = fTpat2 - 1;
        Int_t tpatbin = 0;
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
            {
                return;
            }
        }
    }

    //  ============= FOOT Hit data ==============

    // Vector to store the info of the hits of each event
    std::vector<std::vector<double>> footEnergies(fNbDet);
    std::vector<std::vector<double>> footPositions(fNbDet);

    if (fHitItemsFoot && fHitItemsFoot->GetEntriesFast() > 0)
    {
        auto nHits = fHitItemsFoot->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootHitData* hit = dynamic_cast<R3BFootHitData*>(fHitItemsFoot->At(ihit));
            if (!hit)
                continue;
            if ((hit->GetEta() < 0.3) || (hit->GetEta() > 0.7))
                continue;
            footEnergies[hit->GetDetId() - 1].push_back(hit->GetEnergy());
            footPositions[hit->GetDetId() - 1].push_back(hit->GetPos());
        }
    }

    //  ============= ALPIDE Hit data ==============

    // Vector to store the info of the hits of each event
    std::vector<double> alpideEnergies;
    std::vector<double> alpidePositionsX;
    std::vector<double> alpidePositionsY;

    if (fHitItemsAlpide && fHitItemsAlpide->GetEntriesFast() > 0)
    {
        auto nHits = fHitItemsAlpide->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BAlpideHitData* hit = dynamic_cast<R3BAlpideHitData*>(fHitItemsAlpide->At(ihit));
            if (!hit)
                continue;
            alpideEnergies.push_back(hit->GetClusterSize());
            alpidePositionsX.push_back(hit->GetX());
            alpidePositionsY.push_back(hit->GetY());
        }
    }

    // =============== Calculate the correlations =======
    if (alpideEnergies.size() == 0)
    {
        // R3BLOG("info", "Different number of hits in FOOT and ALPIDE");
        std::cout << "No ALPIDE hits";
    }
    else
    {
        for (int i = 0; i < alpidePositionsX.size(); i++)
        {
            for (int j = 0; j < fNbDet; j++)
            {
                for (int k = 0; k < footPositions[j].size(); k++)
                {
                    fh2_foot_alpide_char_corr[j]->Fill(footEnergies[j][k], alpideEnergies[i]);

                    if ((j % 2) == 0) // Y
                    {
                        fh2_foot_alpide_pos_corr[j]->Fill(footPositions[j][k], alpidePositionsY[i]);
                    }

                    else
                    {
                        fh2_foot_alpide_pos_corr[j]->Fill(footPositions[j][k], alpidePositionsX[i]);
                    }
                }
            }
        }
    }
    fNEvents += 1;
}

void R3BFootVsAlpideOnlineSpectra::FinishEvent()
{

    if (fHitItemsFoot)
    {
        fHitItemsFoot->Clear();
    }
    if (fHitItemsAlpide)
    {
        fHitItemsAlpide->Clear();
    }
}

void R3BFootVsAlpideOnlineSpectra::FinishTask()
{

    if (fHitItemsFoot && fHitItemsAlpide)
    {
        cPosCorr->Write();
        cCharCorr->Write();
    }
}

ClassImp(R3BFootVsAlpideOnlineSpectra)
