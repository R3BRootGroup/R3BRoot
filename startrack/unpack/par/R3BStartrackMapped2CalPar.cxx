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
// -----            R3BStartrackMapped2CalPar                 -----
// -----           Created July 2017 by M.Labiche              -----
// ----------------------------------------------------------------

/* Some notes:
 *
 *
 */

#include "R3BStartrackMapped2CalPar.h"
#include "R3BStartrackMappedData.h"

#include "FairLogger.h"
#include "FairParIo.h"
#include "FairParamList.h"
#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TF1.h"
#include "TH1F.h"

#include "TMath.h"
#include "TVector3.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

R3BStartrackMapped2CalPar::R3BStartrackMapped2CalPar()
    : FairTask("R3B Startrack Mapped2CalPar", 1)
    , fStartrackMappedDataCA(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fNumEvents(NULL)
    , NEvents(0)
    , fOutputFile(NULL)
{
}

R3BStartrackMapped2CalPar::R3BStartrackMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fStartrackMappedDataCA(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofInLadder(0)
    , fNofOutLadder(0)
    , NEvents(0)
    , fOutputFile(NULL)
{
}

R3BStartrackMapped2CalPar::~R3BStartrackMapped2CalPar()
{
    if (fNumEvents)
    {
        delete fNumEvents;
    }
    /*   if (fEngine)
        {
            delete fEngine;
        }
    */
}

InitStatus R3BStartrackMapped2CalPar::Init()
{

    LOG(debug) << "Registering";
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        LOG(fatal) << "Init: No FairRootManager";
        return kFATAL;
    }

    fStartrackMappedDataCA = (TClonesArray*)fMan->GetObject("StartrackMappedData");

    /*
      // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
      // to be set as dependency in CMakelists.txt (in this case in the tof directory)
      fCal_Par = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
      if (!fCal_Par)
      {
          LOG(error) << "R3BTofdMapped2TCalPar::Init() Couldn't get handle on TofdTCalPar. ";
          return kFATAL;

      }

      fCal_Par->setChanged();

      if (!fNofModules)
      {
          LOG(error) << "R3BTofdMapped2TCalPar::Init() Number of modules not set. ";
          return kFATAL;
      }

      fEngine = new R3BTCalEngine(fCal_Par, fMinStats);
  */

    return kSUCCESS;
}

void R3BStartrackMapped2CalPar::Exec(Option_t* option)
{

    if (++NEvents % 10000 == 0)
        LOG(info) << NEvents;

    Int_t nHits = fStartrackMappedDataCA->GetEntries();
    if (!nHits)
        return;

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BStartrackMappedData* hit = (R3BStartrackMappedData*)fStartrackMappedDataCA->At(i);

        Int_t iLadder = hit->GetLadderId(); // 1..n
        Int_t iAsic = hit->GetAsicId();     // 1..n
        Int_t iSide = hit->GetSide();       // 1 or 2
        Int_t iStrip = hit->GetStripId();   // 1..128

        if (iLadder >= (fNofInLadder + fNofOutLadder)) // this also errors for iDetector==0
        {
            LOG(error) << "R3BStartrackMapped2CalPar::Exec() : more Ladders than expected! Det: " << iLadder
                       << " allowed are 1.." << fNofInLadder + fNofOutLadder;
            continue;
        }

        // for (Int_t edge=0;edge<2;edge++)
        //{
        // Fill Charge hitogram
        // fEngine->Fill(iLadder, iSide, (iAsic-1)*128 + iStrip+1, hit->GetADCdata());
        //}
    }

    // Increment events
    NEvents += 1;
}

void R3BStartrackMapped2CalPar::FinishEvent() {}

void R3BStartrackMapped2CalPar::FinishTask()
{
    FILE* fout = NULL;
    if (fOutputFile)
    {
        fout = fopen(fOutputFile, "w");
        if (!fout)
        {
            cerr << "Could not open " << fOutputFile << " for writing!\n";
        }
        else
            fprintf(fout, "# StripId NumEvents\n");
    }
}

void R3BStartrackMapped2CalPar::SetOutputFile(const char* outFile) { fOutputFile = const_cast<char*>(outFile); }

ClassImp(R3BStartrackMapped2CalPar)
