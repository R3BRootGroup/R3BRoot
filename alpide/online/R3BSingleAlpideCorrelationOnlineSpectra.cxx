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

// ------------------------------------------------------------
// -----      R3BSingleAlpideCorrelationOnlineSpectra     -----
// -----    Created 28/10/22 by J.L. Rodriguez-Sanchez    -----
// ----- Fill Single ALPIDE correlation online histograms -----
// ------------------------------------------------------------

#include "R3BSingleAlpideCorrelationOnlineSpectra.h"
#include "R3BAlpideHitData.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"

#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TClonesArray.h"
#include "TMath.h"

R3BSingleAlpideCorrelationOnlineSpectra::R3BSingleAlpideCorrelationOnlineSpectra()
    : R3BSingleAlpideCorrelationOnlineSpectra("SingleAlpideCorrelationOnlineSpectra", 1, "1", 1, "2", 2)
{
}

R3BSingleAlpideCorrelationOnlineSpectra::R3BSingleAlpideCorrelationOnlineSpectra(const TString& name,
                                                                                 Int_t iVerbose,
                                                                                 const TString& namedet1,
                                                                                 const int id1,
                                                                                 const TString& namedet2,
                                                                                 const int id2)
    : FairTask(name, iVerbose)
    , fHitItems(NULL)
    , fNameDet1(namedet1)
    , fId1(id1)
    , fNameDet2(namedet2)
    , fId2(id2)
    , fNEvents(0)
{
}

R3BSingleAlpideCorrelationOnlineSpectra::~R3BSingleAlpideCorrelationOnlineSpectra() { R3BLOG(debug, ""); }

InitStatus R3BSingleAlpideCorrelationOnlineSpectra::Init()
{
    R3BLOG(info, "");

    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    fHitItems = (TClonesArray*)mgr->GetObject("AlpideHitData");
    if (!fHitItems)
    {
        R3BLOG(fatal, "AlpideHitData not found");
        return kFATAL;
    }

    // Create histograms for detectors
    TString Name1;
    TString Name2;

    cHit2D = new TCanvas(fNameDet1 + "-" + fNameDet2 + "_hit",
                         "Alpide" + fNameDet1 + "-Alpide" + fNameDet2 + " hit info",
                         10,
                         10,
                         800,
                         700);
    cHit2D->Divide(2, 1);

    // X Hit data
    Name1 = "fh2_" + fNameDet1 + "-" + fNameDet2 + "_hitx";
    Name2 = "Position-X: Alpide" + fNameDet1 + " vs Alpide" + fNameDet2;
    fh2_hitx = new TH2F(Name1, Name2, 200, -15, 15, 200, -15, 15);
    fh2_hitx->GetXaxis()->SetTitle("X" + fNameDet1 + " [mm]");
    fh2_hitx->GetYaxis()->SetTitle("X" + fNameDet2 + " [mm]");
    fh2_hitx->GetYaxis()->SetTitleOffset(1.1);
    fh2_hitx->GetXaxis()->CenterTitle(true);
    fh2_hitx->GetYaxis()->CenterTitle(true);
    fh2_hitx->GetXaxis()->SetLabelSize(0.045);
    fh2_hitx->GetXaxis()->SetTitleSize(0.045);
    fh2_hitx->GetYaxis()->SetLabelSize(0.045);
    fh2_hitx->GetYaxis()->SetTitleSize(0.045);
    cHit2D->cd(1);
    fh2_hitx->Draw("col");

    // Y Hit data
    Name1 = "fh2_" + fNameDet1 + "-" + fNameDet2 + "_hity";
    Name2 = "Position-Y: Alpide" + fNameDet1 + " vs Alpide" + fNameDet2;
    fh2_hity = new TH2F(Name1, Name2, 100, -7.5, 7.5, 100, -7.5, 7.5);
    fh2_hity->GetXaxis()->SetTitle("Y" + fNameDet1 + " [mm]");
    fh2_hity->GetYaxis()->SetTitle("Y" + fNameDet2 + " [mm]");
    fh2_hity->GetYaxis()->SetTitleOffset(1.1);
    fh2_hity->GetXaxis()->CenterTitle(true);
    fh2_hity->GetYaxis()->CenterTitle(true);
    fh2_hity->GetXaxis()->SetLabelSize(0.045);
    fh2_hity->GetXaxis()->SetTitleSize(0.045);
    fh2_hity->GetYaxis()->SetLabelSize(0.045);
    fh2_hity->GetYaxis()->SetTitleSize(0.045);
    cHit2D->cd(2);
    fh2_hity->Draw("col");

    // MAIN FOLDER
    TFolder* mainfol = new TFolder("Alpide" + fNameDet1 + "-Alpide" + fNameDet2,
                                   "Alpide" + fNameDet1 + "-Alpide" + fNameDet2 + " info");
    if (fHitItems)
    {
        mainfol->Add(cHit2D);
    }
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Alpide" + fNameDet1 + "_" + fNameDet2 + "_HIST",
                                          Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BSingleAlpideCorrelationOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");
    fh2_hitx->Reset();
    fh2_hity->Reset();
}

void R3BSingleAlpideCorrelationOnlineSpectra::Exec(Option_t* option)
{
    // Fill hit data
    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        float x1 = NAN, x2 = NAN, y1 = NAN, y2 = NAN;
        auto nHits = fHitItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BAlpideHitData*)fHitItems->At(ihit);
            if (!hit)
                continue;
            Int_t senid = hit->GetSensorId();
            if (senid == fId1)
            {
                x1 = hit->GetPosl();
                y1 = hit->GetPost();
            }
            else if (senid == fId2)
            {
                x2 = hit->GetPosl();
                y2 = hit->GetPost();
            }
        }
        fh2_hitx->Fill(x1, x2);
        fh2_hity->Fill(y1, y2);
    }

    fNEvents++;
    return;
}

void R3BSingleAlpideCorrelationOnlineSpectra::FinishEvent()
{
    if (fHitItems)
    {
        fHitItems->Clear();
    }
}

void R3BSingleAlpideCorrelationOnlineSpectra::FinishTask()
{

    if (fHitItems)
    {
        cHit2D->Write();
    }
}

ClassImp(R3BSingleAlpideCorrelationOnlineSpectra);
