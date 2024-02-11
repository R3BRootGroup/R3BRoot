/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

/*
 * This task should fill histograms with MWPC online data correlations
 */

#include "R3BMwpcVsMusliCorrelationOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BMusliCalData.h"
#include "R3BMwpcCalData.h"
#include "R3BShared.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TMath.h>
#include <TRandom.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

R3BMwpcVsMusliCorrelationOnlineSpectra::R3BMwpcVsMusliCorrelationOnlineSpectra()
    : R3BMwpcVsMusliCorrelationOnlineSpectra("MwpcVsMusliCorrelationOnlineSpectra", 1, "Mwpc2")
{
}

R3BMwpcVsMusliCorrelationOnlineSpectra::R3BMwpcVsMusliCorrelationOnlineSpectra(const TString& name,
                                                                               Int_t iVerbose,
                                                                               const TString& namedet)
    : FairTask(name, iVerbose)
    , fNameDet(namedet)
{
}

InitStatus R3BMwpcVsMusliCorrelationOnlineSpectra::Init()
{
    LOG(info) << "R3B" + fNameDet + "VsMusliCorrelationOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3B" + fNameDet + "VsMusliCorrelationOnlineSpectra::Init FairRootManager not found";

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to CAL data of the MWPC detector
    fCalItemsMwpc = dynamic_cast<TClonesArray*>(mgr->GetObject(fNameDet + "CalData"));
    if (!fCalItemsMwpc)
    {
        LOG(info) << "R3B" + fNameDet + "VsMusliCorrelationOnlineSpectra: " + fNameDet + "CalData not found";
    }

    // get access to the MAPPED data of the MUSLI
    fCalItemsMusli = dynamic_cast<TClonesArray*>(mgr->GetObject("MusliCalData"));
    if (!fCalItemsMusli)
    {
        LOG(info) << "R3BMusliOnlineSpectra::Init() No MusliCalData found";
    }

    // Create histograms for detectors
    TString Name1;
    TString Name2;

    // MAIN FOLDER
    TFolder* mainfol = new TFolder(fNameDet + "VsMusli", fNameDet + "VsMusli info");

    if (fCalItemsMwpc && fCalItemsMusli)
    {
        cCal = new TCanvas(fNameDet + "CalVsMusliMap", fNameDet + " vs Musli map info", 10, 10, 800, 700);
        Name1 = "fh2_" + fNameDet + "_calx_Musli_DTraw_A7_A8";
        Name2 = "Position-X: " + fNameDet + " vs Musli DTraw for the pair A07/A08";
        fh2_calx = new TH2F(Name1, Name2, 2000, -10000, 10000, 64, 0.5, 64.5);
        fh2_calx->GetXaxis()->SetTitle(fNameDet + "-X [pads]");
        fh2_calx->GetYaxis()->SetTitle("Musli A07/A08 - DTraw [100ps]");
        fh2_calx->GetYaxis()->SetTitleOffset(1.1);
        fh2_calx->GetXaxis()->CenterTitle(true);
        fh2_calx->GetYaxis()->CenterTitle(true);
        fh2_calx->GetXaxis()->SetLabelSize(0.045);
        fh2_calx->GetXaxis()->SetTitleSize(0.045);
        fh2_calx->GetYaxis()->SetLabelSize(0.045);
        fh2_calx->GetYaxis()->SetTitleSize(0.045);
        cCal->cd();
        fh2_calx->Draw("col");
        mainfol->Add(cCal);
    }

    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_" + fNameDet + "_Musli_HIST",
                                          Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BMwpcVsMusliCorrelationOnlineSpectra::Reset_Histo()
{
    LOG(info) << "R3B" + fNameDet + "VsMusliCorrelationOnlineSpectra::Reset_Histo";
    if (fCalItemsMwpc && fCalItemsMusli)
    {
        fh2_calx->Reset();
    }
}

void R3BMwpcVsMusliCorrelationOnlineSpectra::Exec(Option_t* /*option*/)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3B" + fNameDet + "VsMusliCorrelationOnlineSpectra::Exec FairRootManager not found";

    if (fCalItemsMwpc && fCalItemsMwpc->GetEntriesFast() > 0 && fCalItemsMusli && fCalItemsMusli->GetEntriesFast() > 0)
    {
        UInt_t nHits;
        UInt_t qx[64];
        UInt_t qmax = 0;
        Short_t padx = -1;
        for (UShort_t i = 0; i < 64; i++)
        {
            qx[i] = 0;
        }
        // Read Mwpc - Cal data
        nHits = fCalItemsMwpc->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMwpcCalData* hit = dynamic_cast<R3BMwpcCalData*>(fCalItemsMwpc->At(ihit));
            if (!hit)
                continue;
            if (hit->GetPlane() == 1 || hit->GetPlane() == 2)
            {
                qx[hit->GetPad() - 1] += hit->GetQ();
                if (qx[hit->GetPad() - 1] > qmax)
                {
                    padx = hit->GetPad();
                    qmax = qx[padx - 1];
                }
            }
        }
        // Read Musli = Mapped data
        nHits = fCalItemsMusli->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusliCalData* hit = dynamic_cast<R3BMusliCalData*>(fCalItemsMusli->At(ihit));
            if (!hit)
                continue;
            if (hit->GetSignal() != 4)
                continue;
            else
            {
                fh2_calx->Fill(padx, hit->GetDT());
            }
        }
    }

    fNEvents += 1;
}

void R3BMwpcVsMusliCorrelationOnlineSpectra::FinishEvent()
{
    if (fCalItemsMwpc)
    {
        fCalItemsMwpc->Clear();
    }
    if (fCalItemsMusli)
    {
        fCalItemsMusli->Clear();
    }
}

void R3BMwpcVsMusliCorrelationOnlineSpectra::FinishTask()
{

    if (fCalItemsMwpc && fCalItemsMusli)
    {
        cCal->Write();
    }
}

ClassImp(R3BMwpcVsMusliCorrelationOnlineSpectra)
