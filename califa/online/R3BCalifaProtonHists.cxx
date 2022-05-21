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

#include "R3BCalifaProtonHists.h"
#include "R3BCalifaHitData.h"
#include "R3BEventHeader.h"
#include "R3BWRData.h"

#include "R3BLogger.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TFolder.h"
#include "TH2I.h"
#include "THttpServer.h"
#include "TVector3.h"

#include "TClonesArray.h"

#include <ROOT_template_hacks.h>

using roothacks::TypedCollection;

using namespace std;

#define TRIG(i) (1 << (i - 1))

R3BCalifaProtonHists::R3BCalifaProtonHists()
    : FairTask("CALIFAProtonHists")
{
}

InitStatus R3BCalifaProtonHists::Init()
{
    auto mgr = FairRootManager::Instance();

    assert(mgr);

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    assert(header);

    auto run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("", this);

    hits = decltype(hits)(mgr->GetObject("CalifaHitData"));
    mainWrts = decltype(mainWrts)(mgr->GetObject("WRMasterData"));

    run->AddObject(&top);

    return kSUCCESS;
}

InitStatus R3BCalifaProtonHists::ReInit()
{
    assert(0 && "nobody needs this.");
    return kSUCCESS;
}

#if 0
void R3BCalifaProtonHists::Reset_CALIFA_Histo()
{
  assert(0 && "TODO");
}
#endif

void R3BCalifaProtonHists::Exec(Option_t* option)
{
    //    if ((fTrigger >= 0) &&
    //      return;

    R3BCalifaHitData* max[SIDES]{};

    for (auto& h : hits)
    {
        auto side = static_cast<sides_t>(std::abs(h.GetPhi()) > M_PI / 2);
        if (h.GetEnergy() < 40e3)
            continue;
        if (max[side] && h.GetEnergy() < max[side]->GetEnergy())
            continue;
        max[side] = &h;
    }
    all.Fill(max);
    const std::map<int, ProtonCutHists&> trig2hists = { { TRIG(1) | TRIG(7), trigMinBias },
                                                        { TRIG(3) | TRIG(9), trigP2P },
                                                        { TRIG(4) | TRIG(10), trigP2Pv },
                                                        { TRIG(5) | TRIG(11), trigOr } };

    if (header)
    {
        for (const auto& kv : trig2hists)
        {
            if (header->GetTpat() & kv.first)
                kv.second.Fill(max);
        }

        bool p2p = (TRIG(3) | TRIG(9)) & header->GetTpat();
        bool p2pv = (TRIG(4) | TRIG(10)) & header->GetTpat();
        if (p2p && !p2pv)
            trigVetoedP2P.Fill(max);
    }
}

void R3BCalifaProtonHists::FinishTask() {}

ClassImp(R3BCalifaProtonHists);
