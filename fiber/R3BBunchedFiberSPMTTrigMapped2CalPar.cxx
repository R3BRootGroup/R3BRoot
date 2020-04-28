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

#include "R3BBunchedFiberSPMTTrigMapped2CalPar.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "R3BBunchedFiberMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"
#include "TClonesArray.h"
#include <cassert>

R3BBunchedFiberSPMTTrigMapped2CalPar::R3BBunchedFiberSPMTTrigMapped2CalPar(
    Int_t a_verbose,
    Int_t a_update_rate,
    Int_t a_min_stats)
    : FairTask("R3BBunchedFiberSMPTTrigMapped2CalPar", a_verbose)
    , fUpdateRate(a_update_rate)
    , fMinStats(a_min_stats)
{
}

R3BBunchedFiberSPMTTrigMapped2CalPar::~R3BBunchedFiberSPMTTrigMapped2CalPar()
{
    delete fTCalPar;
    delete fEngine;
}

InitStatus R3BBunchedFiberSPMTTrigMapped2CalPar::Init()
{
    auto rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }

    fMapped = (TClonesArray*)rm->GetObject("BunchedFiberSPMTTrigMapped");
    if (!fMapped)
    {
        return kFATAL;
    }

    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakeLists.txt in the detector directory.
    auto name =  "BunchedFiberSPMTTrigTCalPar";
    fTCalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer(name);
    if (!fTCalPar)
    {
      LOG(ERROR) << "Could not get " << name << '.';
      abort();
      return kFATAL;
    }
    fTCalPar->setChanged();
    fEngine = new R3BTCalEngine(fTCalPar, fMinStats);

    return kSUCCESS;
}

void R3BBunchedFiberSPMTTrigMapped2CalPar::Exec(Option_t* option)
{
    auto mapped_num = fMapped->GetEntriesFast();
    for (auto i = 0; i < mapped_num; i++)
    {
        auto mapped = (R3BBunchedFiberMappedData*)fMapped->At(i);
        fEngine->Fill(1, mapped->GetChannel(), 1, mapped->GetFine());
    }
}

void R3BBunchedFiberSPMTTrigMapped2CalPar::FinishEvent() {}

void R3BBunchedFiberSPMTTrigMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fTCalPar->printParams();
}

void R3BBunchedFiberSPMTTrigMapped2CalPar::SetUpdateRate(Int_t a_rate) { fUpdateRate = a_rate; }

void R3BBunchedFiberSPMTTrigMapped2CalPar::SetMinStats(Int_t a_min_stats) { fMinStats = a_min_stats; }

ClassImp(R3BBunchedFiberSPMTTrigMapped2CalPar)
