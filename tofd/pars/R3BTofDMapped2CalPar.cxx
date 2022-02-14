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

// ------------------------------------------------------------
// -----               R3BTofDMapped2CalPar               -----
// -----    Created 31/03/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#include "R3BTofDMapped2CalPar.h"

#include "TClonesArray.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "R3BLogger.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"
#include "R3BTofdMappedData.h"

R3BTofDMapped2CalPar::R3BTofDMapped2CalPar()
    : R3BTofDMapped2CalPar("R3BTofDMapped2CalPar", 1)
{
}

R3BTofDMapped2CalPar::R3BTofDMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(1)
    , fNofPlanes(4)
    , fPaddlesPerPlane(44)
    , fNofModules(fNofPlanes * fPaddlesPerPlane * 4)
    , fCalPar(nullptr)
    , fMapped(nullptr)
    , fMappedTrigger(nullptr)
    , fEngine(nullptr)
{
}

R3BTofDMapped2CalPar::~R3BTofDMapped2CalPar()
{
    if (fCalPar)
        delete fCalPar;
    if (fEngine)
        delete fEngine;
}

InitStatus R3BTofDMapped2CalPar::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        R3BLOG(FATAL, "FairRootManager not found");
        return kFATAL;
    }

    fMapped = (TClonesArray*)rm->GetObject("TofdMapped");
    if (!fMapped)
    {
        R3BLOG(FATAL, "TofdMapped not found");
        return kFATAL;
    }
    fMappedTrigger = (TClonesArray*)rm->GetObject("TofdTriggerMapped");
    if (!fMappedTrigger)
    {
        R3BLOG(WARNING, "TofdTriggerMapped not found");
        fMappedTrigger = NULL;
    }

    fCalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
    if (!fCalPar)
    {
        R3BLOG(ERROR, "Couldn't get handle on TofdTCalPar. ");
        return kFATAL;
    }

    if (!fNofModules)
    {
        R3BLOG(ERROR, "Number of modules not set.");
        return kFATAL;
    }

    fEngine = new R3BTCalEngine(fCalPar, fMinStats);

    for (UInt_t d = 0; d < 5; d++)
        for (UInt_t i = 0; i < 48; i++)
            for (UInt_t k = 0; k < 4; k++)
            {
                Icount[d][i][k] = 0;
            }

    return kSUCCESS;
}

void R3BTofDMapped2CalPar::Exec(Option_t* option)
{
    Int_t nHits = fMapped->GetEntriesFast();
    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        auto mapped = (R3BTofdMappedData const*)fMapped->At(i);

        if (mapped->GetDetectorId() > fNofPlanes)
        {
            R3BLOG(ERROR,
                   "More planes than expected! Plane: " << mapped->GetDetectorId() << " allowed are 1.." << fNofPlanes);
            continue;
        }
        if (mapped->GetBarId() > fPaddlesPerPlane)
        {
            R3BLOG(ERROR,
                   "More bars then expected! Det: " << mapped->GetBarId() << " allowed are 1.." << fPaddlesPerPlane);
            continue;
        }

        Int_t edge = mapped->GetSideId() * 2 + mapped->GetEdgeId() - 2; // 1..4
        // std::cout << mapped->GetDetectorId() <<" "<< mapped->GetBarId() << " "<< mapped->GetTimeFine() <<std::endl;
        fEngine->Fill(mapped->GetDetectorId(), mapped->GetBarId(), edge, mapped->GetTimeFine());

        Icount[mapped->GetDetectorId() - 1][mapped->GetBarId() - 1][edge - 1]++;
    }

    if (fMappedTrigger)
    {
        nHits = fMappedTrigger->GetEntriesFast();
        // Loop over mapped triggers
        for (Int_t i = 0; i < nHits; i++)
        {
            auto mapped = (R3BTofdMappedData const*)fMappedTrigger->At(i);

            if (mapped->GetDetectorId() != fNofPlanes + 1)
            {
                R3BLOG(ERROR,
                       "Trigger plane incorrect! Plane: " << mapped->GetDetectorId() << " not " << fNofPlanes + 1);
                continue;
            }
            fEngine->Fill(mapped->GetDetectorId(), mapped->GetBarId(), 1, mapped->GetTimeFine());
            Icount[mapped->GetDetectorId() - 1][mapped->GetBarId() - 1][0]++;
        }
    }
}

void R3BTofDMapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fCalPar->setChanged();
    fCalPar->printParams();

    R3BLOG(INFO, "Calibration of TofD detector");
    for (Int_t p = 0; p < 5; p++)
        for (Int_t i = 0; i < 48; i++)
            for (Int_t k = 0; k < 4; k++)
                if (Icount[p][i][k] > fMinStats)
                {
                    if (p < 4)
                        R3BLOG(INFO,
                               "Plane: " << p + 1 << ", paddle: " << i + 1 << ", side: " << k + 1
                                         << ", Count: " << Icount[p][i][k]);
                    else
                        R3BLOG(INFO,
                               "Trigger plane: " << p + 1 << ", paddle: " << i + 1 << ", side: " << k + 1
                                                 << ", Count: " << Icount[p][i][k]);
                }
}

void R3BTofDMapped2CalPar::SetUpdateRate(Int_t rate) { fUpdateRate = rate; }

void R3BTofDMapped2CalPar::SetMinStats(Int_t minStats) { fMinStats = minStats; }

void R3BTofDMapped2CalPar::SetNofModules(Int_t nDets, Int_t nCh)
{
    fNofPlanes = nDets;
    fPaddlesPerPlane = nCh;
    fNofModules = nDets * nCh * 4;
}

ClassImp(R3BTofDMapped2CalPar);
