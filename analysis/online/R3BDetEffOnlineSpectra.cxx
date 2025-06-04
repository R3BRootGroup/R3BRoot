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

#include "R3BDetEffOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BFootHitData.h"
#include "R3BLogger.h"
#include "R3BLosHitData.h"
#include "R3BShared.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TFolder.h>
#include <TH1F.h>
#include <THttpServer.h>
#include <TVector3.h>

#include <TClonesArray.h>
#include <sstream>
#include <string>

R3BDetEffOnlineSpectra::R3BDetEffOnlineSpectra()
    : R3BDetEffOnlineSpectra("R3BDetEffOnlineSpectra", 1)
{
}

R3BDetEffOnlineSpectra::R3BDetEffOnlineSpectra(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

void R3BDetEffOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading amsStripCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");
}

InitStatus R3BDetEffOnlineSpectra::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(fatal, fHeader == nullptr, "EventHeader. not found");

    // get access to mappped data
    size_t index = 0;
    for (const auto& clone_name : fNames)
    {
        fMapItems.push_back(dynamic_cast<TClonesArray*>(mgr->GetObject(clone_name)));
        R3BLOG_IF(fatal, !fMapItems[index], clone_name + " not found");
        index++;
    }

    counter.resize(fNames.size());
    for (size_t i = 0; i < fNames.size(); i++)
        counter[i] = 0;

    // MAIN FOLDER
    auto mainfol = new TFolder("Efficiency_det", "Detector efficiency info");

    auto cDet = new TCanvas("Detector_efficiency", "detector efficiency info", 10, 10, 500, 500);
    fh1_det_eff = R3B::root_owned<TH1F>("fh1_det_eff", "Detector efficiency", fNames.size(), 0, fNames.size());
    fh1_det_eff->GetXaxis()->SetTitle("Detectors");
    fh1_det_eff->GetYaxis()->SetTitle("Counts");
    fh1_det_eff->GetYaxis()->SetTitleOffset(1.1);
    fh1_det_eff->GetXaxis()->CenterTitle(true);
    fh1_det_eff->GetYaxis()->CenterTitle(true);
    fh1_det_eff->SetLineColor(1);
    fh1_det_eff->SetFillColor(31);
    fh1_det_eff->Draw();

    mainfol->Add(cDet);

    auto run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Det_Efficiency", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BDetEffOnlineSpectra::ReInit()
{
    SetParContainers();
    // SetParameter();
    return kSUCCESS;
}

void R3BDetEffOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");
    fh1_det_eff->Reset();
}

void R3BDetEffOnlineSpectra::Exec(Option_t* /*option*/)
{
    if ((fTpat > 0) && (fHeader != nullptr) && ((fHeader->GetTpat() & fTpat) != fTpat))
        return;
    fNEvents++;

    size_t index = 0;
    for (const auto& clone : fMapItems)
    {
        if (clone->GetEntriesFast() > 0)
            counter[index]++;
        index++;
    }

    for (size_t i = 0; i < fNames.size(); i++)
    {
        fh1_det_eff->SetBinContent(i + 1, counter[i] / fNEvents);
    }
}

void R3BDetEffOnlineSpectra::FinishEvent()
{
    for (const auto& clone : fMapItems)
    {
        clone->Clear();
    }
}

void R3BDetEffOnlineSpectra::FinishTask() {}

ClassImp(R3BDetEffOnlineSpectra)
