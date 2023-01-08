/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStartrackRawAna                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.04.2014 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"

#include "R3BStartrackRawAna.h"
#include "R3BStartrackRawHit.h"

R3BStartrackRawAna::R3BStartrackRawAna() { fnEvents = 0; }

R3BStartrackRawAna::~R3BStartrackRawAna() {}

InitStatus R3BStartrackRawAna::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fRawData = (TClonesArray*)fMan->GetObject("StartrackRawHit");
    CreateHistos();

    return kSUCCESS;
}

void R3BStartrackRawAna::Exec(Option_t* option)
{
    Int_t nHits = fRawData->GetEntries();
    // cout << "-I- R3BStartrackRawAna::Exec : event " << fnEvents << ", multiplicity " << nHits << endl;
    R3BStartrackRawHit* hit;

    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BStartrackRawHit*)fRawData->At(i);
        thw->Fill(hit->GetWordType());
        thh->Fill(hit->GetHitBit());
        thm->Fill(hit->GetModuleId());
        thsd->Fill(hit->GetSide());
        tha->Fill(hit->GetAsicId());
        thst->Fill(hit->GetStripId());
        the->Fill(hit->GetADCdata());

        thts->Fill(hit->GetTimelb() * 1e-6);
        // cout << "WR lb=" << hit->GetWRlb() << endl;
        // cout << "Time lb=" << hit->GetTimelb() << endl;
        // cout << "XXXXX" << endl;
        thtslbdiff->Fill((hit->GetWRlb() - hit->GetTimelb()) * 1e-6); // in msec
        thtsExt->Fill(hit->GetTimeExtlb() * 1e-6);
        thtsExtlbdiff->Fill((hit->GetTimeExtlb() - hit->GetTimelb()) * 1e-6); // in msec

        thif->Fill(hit->GetInfoField());
        thic->Fill(hit->GetInfoCode());
    }

    if (0 == (fnEvents % 100))
    {
        cout << "nEvents:" << fnEvents << "  nHits= " << nHits << endl;
    }
    fnEvents += 1;
}

void R3BStartrackRawAna::FinishTask() {}

void R3BStartrackRawAna::CreateHistos()
{
    thw = new TH1F("Word type 10->2 11->3", "", 20, -0.5, 20.5);
    thh = new TH1F("hit value", "", 9, -0.5, 8.5);
    thm = new TH1F("Module ID", "", 30, -0.5, 30.5);
    thsd = new TH1F("Side", "", 9, -0.5, 8.5);
    tha = new TH1F("Asic ID", "", 17, -0.5, 16.5);
    thst = new TH1F("Strip ID", "", 131, -0.5, 130.5);
    the = new TH1F("Energy", "", 500, 0., 5000.);
    thts = new TH1F("Silicon Time stamp", "", 1000, 0., 1000.);
    thtslbdiff = new TH1F(" WR time - Si time  ", "", 550, -50, 500.);
    thtsExt = new TH1F("Master time Trigger", "", 1000, 0., 10.);
    thtsExtlbdiff = new TH1F("Ext Mster Time - Si time  ", "", 2000, 0, 20.);
    thif = new TH1F("InfoField", "", 21, 0., 20.);
    thic = new TH1F("InfoCode", "", 21, 0., 20.);
    FairRunOnline* run = FairRunOnline::Instance();
    run->AddObject(thw);
    run->AddObject(thh);
    run->AddObject(thm);
    run->AddObject(thsd);
    run->AddObject(tha);
    run->AddObject(thst);
    run->AddObject(the);
    run->AddObject(thts);
    run->AddObject(thtslbdiff);
    run->AddObject(thtsExt);
    run->AddObject(thtsExtlbdiff);
    run->AddObject(thif);
    run->AddObject(thic);
}

void R3BStartrackRawAna::WriteHistos()
{
    thw->Write();
    thh->Write();
    thm->Write();
    thsd->Write();
    tha->Write();
    thst->Write();
    the->Write();
    thts->Write();
    thtslbdiff->Write();
    thtsExt->Write();
    thtsExtlbdiff->Write();
    thif->Write();
    thic->Write();
    LOG(info) << "R3BStartrackRawAna --- Histograms writed to the Root File ...";
}

ClassImp(R3BStartrackRawAna)
