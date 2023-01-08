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

// ----------------------------------------------------------------
// -----            R3BFi6Mapped2TCalPar (clock TDC)             -----
// ----------------------------------------------------------------

#include "R3BFi6Mapped2CalPar.h"
//#include "R3BPaddleTamexMappedData.h"
#include "R3BEventHeader.h"
#include "R3BFibMappedData.h"
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

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BFi6Mapped2CalPar::R3BFi6Mapped2CalPar()
    : FairTask("R3BFi6Mapped2CalPar", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fCal_Par(NULL)
{
}

R3BFi6Mapped2CalPar::R3BFi6Mapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fCal_Par(NULL)
{
}

R3BFi6Mapped2CalPar::~R3BFi6Mapped2CalPar()
{
    delete fCal_Par;
    delete fEngine;
}

/*  !!!!!!!!!!!!!
 * Define new container in:
 * /R3BRoot/tcal/R3BTCalContFact.cxx
 */
InitStatus R3BFi6Mapped2CalPar::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }

    fMapped = (TClonesArray*)rm->GetObject("Fi6Mapped");
    if (!fMapped)
    {
        return kFATAL;
    }

    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("Fi6TCalPar");
    if (!fCal_Par)
    {
        LOG(error) << "R3BFi6Mapped2CalPar::Init() Couldn't get handle on Fi6TCalPar. ";
        return kFATAL;
    }

    fCal_Par->setChanged();

    fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

    return kSUCCESS;
}

void R3BFi6Mapped2CalPar::Exec(Option_t* option)
{

    Int_t nHits = fMapped->GetEntries();

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {

        R3BFibMappedData* hit = (R3BFibMappedData*)fMapped->At(i);
        if (!hit)
            continue; // should not happen

        Int_t iCh = hit->GetFiberId(); // 1..n

        fEngine->Fill(1, iCh, 1, hit->GetFineTime1LE());
        fEngine->Fill(1, iCh, 2, hit->GetFineTime1TE());
    }
}

void R3BFi6Mapped2CalPar::FinishEvent() {}

void R3BFi6Mapped2CalPar::FinishTask()
{
    fEngine->CalculateParamVFTX();
    fCal_Par->printParams();
}

ClassImp(R3BFi6Mapped2CalPar)
