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

// -----------------------------------------------------------
// -----          R3BRpcvsLosOnlineSpectra               -----
// -----   Created 23/01/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill RPC online histograms              -----
// -----------------------------------------------------------

/*
 * This task should fill histograms with RPC vs Los online data
 */

#include "R3BRpcvsLosOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BLosHitData.h"
#include "R3BRpcCalData.h"
#include "R3BRpcHitData.h"
#include "R3BRpcMappedData.h"

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
#include "THttpServer.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

R3BRpcvsLosOnlineSpectra::R3BRpcvsLosOnlineSpectra()
    : R3BRpcvsLosOnlineSpectra("RpcvsLosOnlineSpectra", 1)
{
}

R3BRpcvsLosOnlineSpectra::R3BRpcvsLosOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fEventHeader(nullptr)
    , fRpcMappedItems(NULL)
    , fRpcCalItems(NULL)
    , fRpcHitItems(NULL)
    , fLosHitItems(NULL)
    , fTrigger(-1)
    , fNEvents(0)
    , fNbStrips(1)
{
}

R3BRpcvsLosOnlineSpectra::~R3BRpcvsLosOnlineSpectra()
{
    R3BLOG(DEBUG1, "");
    if (fEventHeader)
        delete fEventHeader;
    if (fRpcMappedItems)
        delete fRpcMappedItems;
    if (fRpcCalItems)
        delete fRpcCalItems;
    if (fRpcHitItems)
        delete fRpcHitItems;
    if (fLosHitItems)
        delete fLosHitItems;
}

InitStatus R3BRpcvsLosOnlineSpectra::Init()
{
    R3BLOG(INFO, "");

    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    // Look for the R3BEventHeader
    fEventHeader = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!fEventHeader)
    {
        R3BLOG(WARNING, "EventHeader. not found");
        fEventHeader = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        R3BLOG(INFO, "EventHeader. found");

    // Get access to Mapped data
    fRpcMappedItems = (TClonesArray*)mgr->GetObject("RpcMappedData");
    if (!fRpcMappedItems)
    {
        R3BLOG(FATAL, "RpcMappedData not found");
        return kFATAL;
    }

    // Get access to Cal data
    fRpcCalItems = (TClonesArray*)mgr->GetObject("RpcCalData");
    R3BLOG_IF(WARNING, !fRpcCalItems, "RpcCalData not found");

    // Get access to Hit data
    fRpcHitItems = (TClonesArray*)mgr->GetObject("RpcHitData");
    R3BLOG_IF(WARNING, !fRpcHitItems, "RpcHitData not found");

    // Get access to Los Hit data
    fLosHitItems = (TClonesArray*)mgr->GetObject("LosHit");
    R3BLOG_IF(WARNING, !fLosHitItems, "LosHit not found");

    // Create histograms for all the detectors

    // Energy range for strips
    Double_t binsE = 5000;
    Double_t minE = 0;
    Double_t maxE = 20000;

    char Name1[255];
    char Name2[255];

    // MAIN FOLDER-RPCvsLos
    TFolder* mainfol = new TFolder("RpcvsLos", "RPC vs Los info");

    // Hit data
    if (fRpcHitItems && fLosHitItems)
    {
        fh1_tof.resize(fNbStrips);
        for (Int_t i = 0; i < fNbStrips; i++)
        { // one histo per detector
            sprintf(Name1, "fh1_tof_rpc_%d", i + 1);
            sprintf(Name2, "Tof for RPC strip: %d", i + 1);
            fh1_tof[i] = new TH1F(Name1, Name2, 600, -50., 50.);
            fh1_tof[i]->GetXaxis()->SetTitle("Tof [ns]");
            fh1_tof[i]->GetYaxis()->SetTitle("Counts");
            fh1_tof[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_tof[i]->GetXaxis()->CenterTitle(true);
            fh1_tof[i]->GetYaxis()->CenterTitle(true);
            fh1_tof[i]->Draw("");
            mainfol->Add(fh1_tof[i]);
        }
    }

    // Looking for FairRunOnline
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_RpcvsLos", Form("/Objects/%s/->Reset_RpcvsLos_Histo()", GetName()));

    return kSUCCESS;
}

void R3BRpcvsLosOnlineSpectra::Reset_RpcvsLos_Histo()
{
    R3BLOG(INFO, "");

    // Hit data
    if (fRpcHitItems)
        for (Int_t i = 0; i < fNbStrips; i++)
        {
            fh1_tof[i]->Reset();
        }

    return;
}

void R3BRpcvsLosOnlineSpectra::Exec(Option_t* option)
{
    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
        return;

    // Fill los hit data
    if (fLosHitItems && fLosHitItems->GetEntriesFast() > 0)
    {
        double lostime = 0;
        auto nHits = fLosHitItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto loshit = (R3BLosHitData*)fHitLos->At(ihit);
            if (!loshit)
                continue;

            if (lostime == 0)
                lostime = fHitLos->GetTime();
        }

        // Fill rpc hit data
        if (fRpcHitItems && fRpcHitItems->GetEntriesFast() > 0 && lostime > 0.)
        {
            auto nHits = fRpcHitItems->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                auto rpchit = (R3BRpcHitData*)fRpcHitItems->At(ihit);
                if (!rpchit)
                    continue;
                fh1_tof[rpchit->GetStripId() - 1]->Fill(hit->GetTime() - lostime);
            }
        }
    }

    fNEvents += 1;
    return;
}

void R3BRpcvsLosOnlineSpectra::FinishEvent()
{
    if (fRpcMappedItems)
    {
        fRpcMappedItems->Clear();
    }
    if (fRpcCalItems)
    {
        fRpcCalItems->Clear();
    }
    if (fRpcHitItems)
    {
        fRpcHitItems->Clear();
    }
    if (fLosHitItems)
    {
        fLosHitItems->Clear();
    }
    return;
}

void R3BRpcvsLosOnlineSpectra::FinishTask()
{

    if (fRpcHitItems && fLosHitItems)
        for (Int_t i = 0; i < fNbStrips; i++)
        {
            fh1_tof[i]->Write();
        }
    return;
}

ClassImp(R3BRpcvsLosOnlineSpectra);
