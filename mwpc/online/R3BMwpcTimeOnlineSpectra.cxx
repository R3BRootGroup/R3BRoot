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
// -----                  R3BMwpcTimeOnlineSpectra            -----
// -----    Created 29/09/19 by J.L. Rodriguez-Sanchez    -----
// -----           Fill MWPC online histograms            -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with MWPC online data
 */

#include "R3BMwpcTimeOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BMwpcTimeMappedData.h"
#include "R3BShared.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFolder.h>
#include <TH1F.h>
#include <THttpServer.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TMath.h>
#include <TRandom.h>
#include <TVector3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

R3BMwpcTimeOnlineSpectra::R3BMwpcTimeOnlineSpectra()
    : R3BMwpcTimeOnlineSpectra("R3BMwpcTimeOnlineSpectra", 1, "Mwpc0")
{
}

R3BMwpcTimeOnlineSpectra::R3BMwpcTimeOnlineSpectra(const TString& name, Int_t iVerbose, const TString& namedet)
    : FairTask(name, iVerbose)
    , fNameDet(namedet)
{
}

InitStatus R3BMwpcTimeOnlineSpectra::Init()
{
    R3BLOG(info, "for " << fNameDet);

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3B" + fNameDet + "OnlineSpectra::Init FairRootManager not found";

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(warn, header == nullptr, "EventHeader. not found");
    R3BLOG_IF(info, header, "EventHeader. found");

    // get access to mapped data of mwpcs
    fMapItemsMwpc = dynamic_cast<TClonesArray*>(mgr->GetObject(fNameDet + "TimeMappedData"));
    R3BLOG_IF(fatal, fMapItemsMwpc == nullptr, fNameDet + "TimeMappedData not found");

    // Create histograms for detectors
    TString Name1;
    TString Name2;

    // MWPC: Mapped data
    cMwpcMap = new TCanvas(fNameDet + "_T", fNameDet + "_T", 10, 10, 800, 700);

    if (fNameDet == "Mwpc0" || fNameDet == "Mwpc3") // FIXME FOR MWPC3
    {
        cMwpcMap->Divide(2, 1);

        Name1 = fNameDet + "_Plane1T";
        fh1_mwpc_map_t[0] = R3B::root_owned<TH1I>(Name1, Name1, 2000, -5000, 5000);
        fh1_mwpc_map_t[0]->GetXaxis()->SetTitle("TimeDiff for X MMR64");
        fh1_mwpc_map_t[0]->GetYaxis()->SetTitle("counts");
        cMwpcMap->cd(1);
        fh1_mwpc_map_t[0]->Draw();

        Name1 = fNameDet + "_Plane3T";
        fh1_mwpc_map_t[2] = R3B::root_owned<TH1I>(Name1, Name1, 2000, -5000, 5000);
        fh1_mwpc_map_t[2]->GetXaxis()->SetTitle("TimeDiff for Y MMR64");
        fh1_mwpc_map_t[2]->GetYaxis()->SetTitle("counts");
        cMwpcMap->cd(2);
        fh1_mwpc_map_t[2]->Draw();
    }
    else // if (fNameDet == "Mwpc1" || fNameDet == "Mwpc2")
    {
        cMwpcMap->Divide(3, 1);

        Name1 = fNameDet + "_Plane1T";
        fh1_mwpc_map_t[0] = R3B::root_owned<TH1I>(Name1, Name1, 2000, -5000, 5000);
        fh1_mwpc_map_t[0]->GetXaxis()->SetTitle("TimeDiff for X MMR64");
        fh1_mwpc_map_t[0]->GetYaxis()->SetTitle("counts");
        cMwpcMap->cd(1);
        fh1_mwpc_map_t[0]->Draw();

        Name1 = fNameDet + "_Plane1T";
        fh1_mwpc_map_t[1] = R3B::root_owned<TH1I>(Name1, Name1, 2000, -5000, 5000);
        fh1_mwpc_map_t[1]->GetXaxis()->SetTitle("TimeDiff for X MMR64");
        fh1_mwpc_map_t[1]->GetYaxis()->SetTitle("counts");
        cMwpcMap->cd(2);
        fh1_mwpc_map_t[1]->Draw();

        Name1 = fNameDet + "_Plane3T";
        fh1_mwpc_map_t[2] = R3B::root_owned<TH1I>(Name1, Name1, 2000, -5000, 5000);
        fh1_mwpc_map_t[2]->GetXaxis()->SetTitle("TimeDiff for Y MMR64");
        fh1_mwpc_map_t[2]->GetYaxis()->SetTitle("counts");
        cMwpcMap->cd(3);
        fh1_mwpc_map_t[2]->Draw();
    }

    // MAIN FOLDER-MWPC
    TFolder* mainfolMW = new TFolder(fNameDet + "_T", fNameDet + " time info");
    mainfolMW->Add(cMwpcMap);
    run->AddObject(mainfolMW);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_" + fNameDet + "Time_HIST",
                                          Form("/Objects/%sTime/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BMwpcTimeOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "for " << fNameDet);
    // Mapped data
    if (fMapItemsMwpc)
    {
        if (fNameDet == "Mwpc1" || fNameDet == "Mwpc2")
            for (Int_t i = 0; i < 3; i++)
            {
                fh1_mwpc_map_t[i]->Reset();
            }
        if (fNameDet == "Mwpc0" || fNameDet == "Mwpc3")
        {
            fh1_mwpc_map_t[0]->Reset();
            fh1_mwpc_map_t[2]->Reset();
        }
    }
}

void R3BMwpcTimeOnlineSpectra::Exec(Option_t* /*option*/)
{

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

    if (fMapItemsMwpc && fMapItemsMwpc->GetEntriesFast() > 0)
    {
        Int_t nHits = fMapItemsMwpc->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMwpcTimeMappedData* hit = dynamic_cast<R3BMwpcTimeMappedData*>(fMapItemsMwpc->At(ihit));
            if (!hit)
                continue;
            fh1_mwpc_map_t[hit->GetPlane() - 1]->Fill(hit->GetTime());
        }
    }

    fNEvents += 1;
    return;
}

void R3BMwpcTimeOnlineSpectra::FinishEvent()
{
    if (fMapItemsMwpc)
    {
        fMapItemsMwpc->Clear();
    }
}

void R3BMwpcTimeOnlineSpectra::FinishTask()
{
    if (fMapItemsMwpc)
    {
        cMwpcMap->Write();
    }
}

ClassImp(R3BMwpcTimeOnlineSpectra)
