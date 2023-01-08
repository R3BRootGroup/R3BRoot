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

// ---------------------------------------------------------------
// -----                R3BRoluMapped2CalPar                 -----
// -----        Created July 2019 by A. Kelic-Heil           -----
// -----          Following R3BLosMapped2CalPar              -----
// ---------------------------------------------------------------

/* Some notes:
 *
 * This file handles 7ps VFTX and TAMEX2, hence we have three times per channel.
 *
 */

#include "R3BRoluMapped2CalPar.h"
#include "R3BEventHeader.h"
#include "R3BRoluMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TF1.h"
#include "TH1F.h"
#include "math.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BRoluMapped2CalPar::R3BRoluMapped2CalPar()
    : R3BRoluMapped2CalPar("R3BRoluMapped2CalPar", 1)
{
}

R3BRoluMapped2CalPar::R3BRoluMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofDetectors(0)
    , fNofChannels(0)
    , fNofTypes(0)
    , fNEvents(0)
    , fCal_Par(NULL)
{
}

R3BRoluMapped2CalPar::~R3BRoluMapped2CalPar()
{
    if (fCal_Par)
    {
        delete fCal_Par;
    }
    if (fEngine)
    {
        delete fEngine;
    }
}

InitStatus R3BRoluMapped2CalPar::Init()
{

    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        LOG(fatal) << "R3BRoluMapped2CalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    header = (R3BEventHeader*)rm->GetObject("EventHeader.");
    // may be = NULL!
    if (!header)
    {
        LOG(warn) << "R3BRoluMapped2CalPar::Init() EventHeader. not found";
        header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
    }
    else
        LOG(info) << "R3BRoluMapped2CalPar::Init() R3BEventHeader found";

    fMapped = (TClonesArray*)rm->GetObject("RoluMapped");
    if (!fMapped)
    {
        LOG(fatal) << "R3BRoluMapped2CalPar::Branch RoluMapped not found";
        return kFATAL;
    }

    fMappedTrigger = (TClonesArray*)rm->GetObject("RoluTriggerMapped");
    if (!fMappedTrigger)
    {
        LOG(warn) << "R3BRoluMapped2CalPar::Branch RoluMapped not found";
    }

    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("RoluTCalPar");
    fCal_Par->setChanged();

    if (!fNofModules)
    {
        LOG(fatal) << "R3BRoluMapped2CalPar::Init() Number of modules not set. ";
        return kFATAL;
    }

    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BRoluMapped2CalPar::Exec(Option_t* option)
{
    // test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    UInt_t nHits = fMapped->GetEntries();

    // Loop over mapped hits
    for (UInt_t i = 0; i < nHits; i++)
    {
        R3BRoluMappedData* hit = (R3BRoluMappedData*)fMapped->At(i);
        if (!hit)
        {
            continue; // should not happen
        }

        // channel numbers are supposed to be 1-based (1..n)
        UInt_t iDetector = hit->GetDetector() - 1; // now 0..n-1
        UInt_t iChannel = hit->GetChannel() - 1;   // now 0..n-1
        UInt_t iType = hit->GetType();             // 0,1

        if (iDetector > (fNofDetectors - 1))
        {
            LOG(error) << "R3BRoluMapped2CalPar::Exec() : more detectors than expected! Det: " << (iDetector + 1)
                       << " allowed are 1.." << fNofDetectors;
            continue;
        }
        if (iChannel > (fNofChannels - 1))
        {
            LOG(error) << "R3BRoluMapped2CalPar::Exec() : more channels than expected! Channel: " << (iChannel + 1)
                       << " allowed are 1.." << fNofChannels;
            continue;
        }

        if (iType > (fNofTypes - 1))
        {
            LOG(error) << "R3BRoluMapped2CalPar::Exec() : more time-types than expected! Type: " << iType
                       << " allowed are 0.." << (fNofTypes - 1);
            continue;
        }

        // Fill TAC histogram for VFTX and TAMEX

        fEngine->Fill(iDetector + 1, iChannel + 1, iType + 1, hit->GetTimeFine());
    }

    if (fMappedTrigger)
    {
        nHits = fMappedTrigger->GetEntries();
        // Loop over mapped triggers
        for (Int_t i = 0; i < nHits; i++)
        {
            auto mapped = (R3BRoluMappedData const*)fMappedTrigger->At(i);
            fEngine->Fill(3, 1, 1, mapped->GetTimeFine());
        }
    }

    // Increment events
    fNEvents += 1;
}

void R3BRoluMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();

    fCal_Par->printParams();
}

ClassImp(R3BRoluMapped2CalPar);
