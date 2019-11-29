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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                      R3BCalifaMappedDataAnalysis                  -----
// -----                           Version 1.0                             -----
// -----                    Created 03.02.2014 by Y.Gonzalez               -----
// -----                   Modified 20.03.2017 by P.Cabanelas              -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"

#include "R3BCalifaMappedData.h"
#include "R3BCalifaMappedDataAnalysis.h"

R3BCalifaMappedDataAnalysis::R3BCalifaMappedDataAnalysis() { fnEvents = 0; }

R3BCalifaMappedDataAnalysis::~R3BCalifaMappedDataAnalysis() {}

InitStatus R3BCalifaMappedDataAnalysis::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fRawData = (TClonesArray*)fMan->GetObject("CalifaMappedData");
    CreateHistos();

    return kSUCCESS;
}

void R3BCalifaMappedDataAnalysis::Exec(Option_t* option)
{
    Int_t nHits = fRawData->GetEntries();
    R3BCalifaMappedData* hit;
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BCalifaMappedData*)fRawData->At(i);
        thc->Fill(hit->GetCrystalId());
        the->Fill(hit->GetEnergy());
        tht->Fill(hit->GetTime());
    }
    if (0 == (fnEvents % 100))
    {
        LOG(INFO) << fnEvents << " events, multiplicity:  " << nHits;
    }
    fnEvents += 1;
}

void R3BCalifaMappedDataAnalysis::FinishTask() {}

void R3BCalifaMappedDataAnalysis::CreateHistos()
{
    thc = new TH1F("Crystal ID", "", 9, -0.5, 8.5);
    the = new TH1F("Energy", "", 30, 320., 350.);
    tht = new TH1F("Time", "", 100, 0., 4500000000.);
    FairRunOnline* run = FairRunOnline::Instance();
    run->AddObject(thc);
    run->AddObject(the);
    run->AddObject(tht);
}

void R3BCalifaMappedDataAnalysis::WriteHistos()
{
    thc->Write();
    the->Write();
    tht->Write();
    LOG(INFO) << "R3BCalifaMappedDataAnalysis --- Histograms written to Root File ...";
}

ClassImp(R3BCalifaMappedDataAnalysis)
