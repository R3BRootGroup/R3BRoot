/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BLosMapped2CalPar.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BLosMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRtdbRun.h>
#include <FairRunIdGenerator.h>
#include <FairRuntimeDb.h>

#include <TClonesArray.h>
#include <TF1.h>
#include <TH1F.h>
#include <math.h>

#include <iostream>
#include <stdlib.h>

R3BLosMapped2CalPar::R3BLosMapped2CalPar()
    : R3BLosMapped2CalPar("R3BLosMapped2CalPar", 1)
{
}

R3BLosMapped2CalPar::R3BLosMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

R3BLosMapped2CalPar::~R3BLosMapped2CalPar()
{
    R3BLOG(debug1, "Destructor");
    if (fCal_Par)
    {
        delete fCal_Par;
    }
    if (fEngine)
    {
        delete fEngine;
    }
}

InitStatus R3BLosMapped2CalPar::Init()
{
    R3BLOG(info, "");
    for (size_t i = 0; i < fNofChannels; i++)
    {
        for (size_t k = 0; k < fNofTypes; k++)
        {
            for (size_t d = 0; d < fNofDetectors; d++)
                Icount[d][i][k] = 0;
            Icounttrig[i][k] = 0;
        }
    }
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    header = dynamic_cast<R3BEventHeader*>(rm->GetObject("EventHeader."));
    R3BLOG_IF(error, !header, "EventHeader. not found");

    fMapped = dynamic_cast<TClonesArray*>(rm->GetObject("LosMapped"));
    R3BLOG_IF(fatal, !fMapped, "LosMapped not found");

    // get access to Trigger Mapped data
    fMappedTriggerItems = dynamic_cast<TClonesArray*>(rm->GetObject("LosTriggerMapped"));
    R3BLOG_IF(warn, !fMappedTriggerItems, "LosTriggerMapped not found");

    fCal_Par = dynamic_cast<R3BTCalPar*>(FairRuntimeDb::instance()->getContainer("LosTCalPar"));
    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BLosMapped2CalPar::Exec(Option_t* option)
{
    // test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    auto nHits = fMapped->GetEntriesFast();
    // Loop over mapped hits
    for (size_t i = 0; i < nHits; i++)
    {
        auto hit = dynamic_cast<R3BLosMappedData*>(fMapped->At(i));
        if (!hit)
        {
            continue; // should not happen
        }

        // channel numbers are supposed to be 1-based (1..n)
        auto iDetector = hit->GetDetector() - 1; // now 0..n-1
        auto iChannel = hit->GetChannel() - 1;   // now 0..n-1
        auto iType = hit->GetType();             // 0,1,2,3

        if (iType < 3)
        {
            if (iDetector > (fNofDetectors - 1))
            {
                R3BLOG(error,
                       "More detectors than expected! Det: " << (iDetector + 1) << " allowed are 1.." << fNofDetectors);
                continue;
            }
            if (iChannel > (fNofChannels - 1))
            {
                R3BLOG(
                    error,
                    "More channels than expected! Channel: " << (iChannel + 1) << " allowed are 1.." << fNofChannels);
                continue;
            }
            if (iType > 3)
            {
                R3BLOG(error, "More time-types than expected! Type: " << iType << " allowed are 0..3");
                continue;
            }
            Icount[iDetector][iChannel][iType]++;
            fEngine->Fill(iDetector + 1, iChannel + 1, iType + 1, hit->GetTimeFine());
        }
    }

    // Calibrate trigger channels -----------------------------------------------
    if (fMappedTriggerItems && fMappedTriggerItems->GetEntriesFast() > 0)
    {
        auto mapped_num = fMappedTriggerItems->GetEntriesFast();
        for (Int_t mapped_i = 0; mapped_i < mapped_num; mapped_i++)
        {
            auto mapped = dynamic_cast<R3BLosMappedData const*>(fMappedTriggerItems->At(mapped_i));

            auto iDetector = mapped->GetDetector() - 1; // now 0..n-1
            auto iChannel = mapped->GetChannel();
            auto iType = mapped->GetType() + 1; // 1,2,3...
            R3BLOG(debug1, "Det: " << iDetector << " channel" << iChannel << " raw " << mapped->GetTimeFine());
            fEngine->Fill(3 + iDetector, iChannel, iType, mapped->GetTimeFine());
            Icounttrig[iChannel - 1][iType - 1]++;
        }
    }
    // Increment events
    fNEvents++;
}

void R3BLosMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fCal_Par->setChanged();

    R3BLOG(info, "Calibration of LOS detector");
    for (size_t d = 0; d < fNofDetectors; d++)
    {
        for (size_t i = 0; i < fNofChannels; i++)
        {
            for (size_t k = 0; k < fNofTypes; k++)
            {
                if (Icount[d][i][k] > fMinStats)
                {
                    R3BLOG(info,
                           "Detector: " << d + 1 << ", Channel: " << i + 1 << ", Type: " << k
                                        << ", Count: " << Icount[d][i][k]);
                }
            }
        }
    }

    R3BLOG(info, "Calibration of trigger signals from LOS detector");
    for (size_t i = 0; i < fNofChannels; i++)
    {
        for (size_t k = 0; k < fNofTypes; k++)
        {
            if (Icounttrig[i][k] > fMinStats)
            {
                if (k == 0)
                    R3BLOG(info, "VFTX channel: " << i + 1 << ", Counts: " << Icounttrig[i][k]);
                else
                    R3BLOG(info, "Tamex Channel: " << i + 1 << ", Type: " << k << ", Counts: " << Icounttrig[i][k]);
            }
        }
    }
}

ClassImp(R3BLosMapped2CalPar)
