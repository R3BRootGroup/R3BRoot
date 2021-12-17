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

#include "R3BFiberMAPMTMapped2CalPar.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "R3BFiberMAPMTMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"
#include "TClonesArray.h"
#include <cassert>

R3BFiberMAPMTMapped2CalPar::R3BFiberMAPMTMapped2CalPar(const char* a_name,
                                                       Int_t a_verbose,
                                                       Int_t a_update_rate,
                                                       Int_t a_min_stats)
    : FairTask(TString("R3B") + a_name + "Mapped2CalPar", a_verbose)
    , fName(a_name)
    , fUpdateRate(a_update_rate)
    , fMinStats(a_min_stats)
{
}

R3BFiberMAPMTMapped2CalPar::~R3BFiberMAPMTMapped2CalPar()
{
    delete fMAPMTTCalPar;
    delete fMAPMTEngine;
    delete fMAPMTTrigTCalPar;
    delete fMAPMTTrigEngine;
}

InitStatus R3BFiberMAPMTMapped2CalPar::Init()
{
    auto rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }

    fMapped = (TClonesArray*)rm->GetObject(fName + "Mapped");
    if (!fMapped)
    {
        return kFATAL;
    }

    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakeLists.txt in the detector directory.
#define GET_TCALPAR(NAME)                                                              \
    do                                                                                 \
    {                                                                                  \
        auto name = fName + #NAME "TCalPar";                                           \
        f##NAME##TCalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer(name); \
        if (!f##NAME##TCalPar)                                                         \
        {                                                                              \
            LOG(ERROR) << "Could not get " << name << '.';                             \
            abort();                                                                   \
            return kFATAL;                                                             \
        }                                                                              \
        f##NAME##TCalPar->setChanged();                                                \
        f##NAME##Engine = new R3BTCalEngine(f##NAME##TCalPar, fMinStats);              \
    } while (0)
    GET_TCALPAR(MAPMT);
    GET_TCALPAR(MAPMTTrig);

    return kSUCCESS;
}

void R3BFiberMAPMTMapped2CalPar::Exec(Option_t* option)
{
    auto mapped_num = fMapped->GetEntriesFast();
    for (auto i = 0; i < mapped_num; i++)
    {
        auto mapped = (R3BFiberMAPMTMappedData*)fMapped->At(i);
        assert(mapped);
        if (!mapped->IsTrigger())
        {
            fMAPMTEngine->Fill(
                1, mapped->GetChannel() * 2 - (mapped->IsLeading() ? 1 : 0), mapped->GetSide() + 1, mapped->GetFine());
        }
        else
        {
            fMAPMTTrigEngine->Fill(1, mapped->GetChannel(), 1, mapped->GetFine());
        }
    }
}

void R3BFiberMAPMTMapped2CalPar::FinishEvent() {}

void R3BFiberMAPMTMapped2CalPar::FinishTask()
{
    fMAPMTEngine->CalculateParamClockTDC(R3BTCalEngine::CTDC_16_BWD_150);
    fMAPMTTrigEngine->CalculateParamClockTDC(R3BTCalEngine::CTDC_16_BWD_150);
    fMAPMTTCalPar->printParams();
    fMAPMTTrigTCalPar->printParams();
}

void R3BFiberMAPMTMapped2CalPar::SetUpdateRate(Int_t a_rate) { fUpdateRate = a_rate; }

void R3BFiberMAPMTMapped2CalPar::SetMinStats(Int_t a_min_stats) { fMinStats = a_min_stats; }

ClassImp(R3BFiberMAPMTMapped2CalPar)
