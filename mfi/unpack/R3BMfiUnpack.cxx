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

// ---------------------------------------------------------------------------------------
// -----                               R3BMfiUnpack                                  -----
// -----                      Created 30-07-2014 by D.Kresan                         -----
// ---------------------------------------------------------------------------------------

#include <iomanip>
#include <map>

// ROOT headers
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"

// Land headers
#include "R3BMfiRawHit.h"
#include "R3BMfiUnpack.h"

// R3BMfiUnpack: Constructor
R3BMfiUnpack::R3BMfiUnpack(Short_t type, Short_t subType, Short_t procId, Short_t subCrate, Short_t control)
    : FairUnpack(type, subType, procId, subCrate, control)
    , fRawData(new TClonesArray("R3BMfiRawHit"))
    , fNHits(0)
{
}

// Virtual R3BMfiUnpack: Public method
R3BMfiUnpack::~R3BMfiUnpack()
{
    LOG(info) << "R3BMfiUnpack: Delete instance";
    delete fRawData;
}

// Init: Public method
Bool_t R3BMfiUnpack::Init()
{
    Register();
    return kTRUE;
}

// Register: Protected method
void R3BMfiUnpack::Register()
{
    //  LOG(debug) << "Registering";
    LOG(info) << "R3BMfiUnpack : Registering...";
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        return;
    }
    fMan->Register("MfiRawHit", "Mfi", fRawData, kTRUE);
}

// DoUnpack: Public method
Bool_t R3BMfiUnpack::DoUnpack(Int_t* data, Int_t size)
{
    LOG(debug) << "R3BMfiUnpack : Unpacking... size = " << size;

    UInt_t l_i = 0;
    while ((data[l_i] & 0xfff00000) == 0xadd00000)
    {
        l_i += 1;
    }

    std::map<Int_t, Int_t> index;
    Int_t id;

    while (l_i < size)
    {
        // Read packet header
        UInt_t* p1 = (UInt_t*)(data + l_i);
        UInt_t nxtId = (p1[0] & 0xff000000) >> 24;
        UInt_t gemexId = (p1[0] & 0x00ff0000) >> 16;
        UInt_t trigger = (p1[0] & 0x0000ff00) >> 8;
        // UInt_t da_size = (p1[1] & 0xffffffff);
        l_i += 3;
        p1 = (UInt_t*)(data + l_i);
        ULong_t trgTs = (p1[0] & 0x0000ffff);
        ULong_t triggerTs = (trgTs << 32) | (ULong_t)p1[1];
        l_i += 2;

        // Read packet data
        p1 = (UInt_t*)(data + l_i);
        for (Int_t i = 0; i < 5001; i += 2)
        {
            // Check if end of packet data reached
            UInt_t control1 = (p1[i] & 0xff000000);
            UInt_t control2 = (p1[i + 1] & 0xff000000);
            if (control1 == 0xee000000 && control2 == 0xbb000000)
            {
                l_i += 2;
                break;
            }

            // Read a hit
            UInt_t adc = (p1[i] & 0x0fff0000) >> 16;
            UInt_t nxtTs = (p1[i] & 0x00003fff);
            UInt_t chId = (p1[i + 1] & 0x7f000000) >> 24;
            UInt_t epoch = (p1[i + 1] & 0x00ffffff);
            l_i += 2;

            // Internal indexing
            id = 10000 * nxtId + 1000 * gemexId + chId;

            // Create MFI hit
            new ((*fRawData)[fNHits])
                R3BMfiRawHit(nxtId, gemexId, trigger, triggerTs, adc, chId, nxtTs, epoch, index[id]);
            fNHits++;
            index[id] += 1;

            LOG(debug) << nxtId << "  " << gemexId << "  " << trigger << " tot=" << size << "   " << triggerTs;
        }
    }

    LOG(info) << "R3BMfiUnpack : Number of hits in MFI: " << fNHits;
    return kTRUE;
}

// Reset: Public method
void R3BMfiUnpack::Reset()
{
    LOG(debug) << "R3BMfiUnpack : Clearing Data Structure";
    fRawData->Clear();
    fNHits = 0;
}

ClassImp(R3BMfiUnpack)
