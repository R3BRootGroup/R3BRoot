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

#include "R3BRpcCal2Hit.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "R3BRpcCalData.h"
#include <list>
#include <vector>

R3BRpcCal2Hit::R3BRpcCal2Hit()
    : FairTask("R3B RPC Cal to Hit")
    , fCalDataCA(NULL)
    , fHitDataCA(NULL)
    , fOnline(kFALSE)
{
}

R3BRpcCal2Hit::~R3BRpcCal2Hit()
{
    LOG(INFO) << "R3BRpcCal2Hit: Delete instance";
    if (fHitDataCA)
        delete fHitDataCA;
}

void R3BRpcCal2Hit::SetParContainers()
{
    // Parameter Container
    // Reading RPCHitPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcCal2Hit:: FairRuntimeDb not opened";
    }

    fHitPar = (R3BRpcHitPar*)rtdb->getContainer("RPCHitPar");
    if (!fHitPar)
    {
        LOG(ERROR) << "R3BRpcCal2Hit::Init() Couldn't get handle on RPCHitPar container";
    }
    else
    {
        LOG(INFO) << "R3BRpcCal2Hit:: RPCHitPar container open";
    }
}

InitStatus R3BRpcCal2Hit::Init()
{
    LOG(INFO) << "R3BRpcCal2Hit::Init()";
    assert(!fHitDataCA); // in case someone calls Init() twice.
    FairRootManager* ioManager = FairRootManager::Instance();
    if (!ioManager)
        LOG(fatal) << "R3BRpcCal2Hit::Init() FairRootManager not found";

    fCalDataCA = (TClonesArray*)ioManager->GetObject("RPCCalData");

    // Register output array
    fHitDataCA = new TClonesArray("R3BRPCHitData");
    ioManager->Register("RPCHitData", "RPC Hit", fHitDataCA, !fOnline);

    return kSUCCESS;
}

InitStatus R3BRpcCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BRpcCal2Hit::Exec(Option_t* opt)
{
    Reset(); // Reset entries in output arrays, local arrays

    if (!fHitPar)
    {
        LOG(WARNING) << "R3BRpcCal2Hit::Parameter container not found";
    }

    // Reading the Input -- Cal Data --
    Int_t nHits = fCalDataCA->GetEntries();
    if (!nHits)
        return;

    R3BRpcCalData** calData;
    calData = new R3BRpcCalData*[nHits];

    for (Int_t i = 0; i < nHits; i++)
    {
        calData[i] = (R3BRpcCalData*)(fCalDataCA->At(i));
        Int_t channelId = 0; // DUMMY FILLING---> CALL HERE YOUR CALIB
        Double_t energy = 0;
        uint64_t time = 0;
        AddHit(channelId, energy, time);
    }

    if (calData)
        delete[] calData; // FTFY
    return;
}

void R3BRpcCal2Hit::Reset()
{
    // Clear the CA structure
    LOG(DEBUG) << "Clearing RPCHitData Structure";
    if (fHitDataCA)
        fHitDataCA->Clear();
}

R3BRpcHitData* R3BRpcCal2Hit::AddHit(UInt_t channel, Double_t ene, ULong64_t time)
{
    TClonesArray& clref = *fHitDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BRpcHitData(channel, ene, time);
}

ClassImp(R3BRpcCal2Hit);
