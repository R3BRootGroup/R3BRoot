/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BHFIBCal2Hit.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "R3BHFIBCalData.h"
#include "R3BHFIBHitData.h"
#include <list>
#include <vector>

R3BHFIBCal2Hit::R3BHFIBCal2Hit()
    : FairTask("R3B HFIB Cal to Hit")
{
}

R3BHFIBCal2Hit::~R3BHFIBCal2Hit()
{
    LOG(info) << "R3BHFIBCal2Hit: Delete instance";
    if (fTotCalPar)
        delete fTotCalPar;
    if (fHFIBHitDataCA)
        delete fHFIBHitDataCA;
}

InitStatus R3BHFIBCal2Hit::Init()
{

    LOG(info) << "R3BHFIBCal2Hit::Init()";

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BHFIBCal2Hit:: FairRuntimeDb not opened";
    }

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(fatal) << "R3BHFIBCal2Hit::FairRootManager not found";
        return kFATAL;
    }

    fHFIBCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BHFIBCalData"));
    if (!fHFIBCalDataCA)
    {
        LOG(error) << "R3BHFIBCal2Hit::Init() R3BHFIBCalData not found";
        return kFATAL;
    }

    fTotCalPar = dynamic_cast<R3BHFIBTotCalPar*>(rtdb->getContainer("HFIBTotCalPar"));
    if (!fTotCalPar)
    {
        LOG(error) << "R3BHFIBCal2HitPar::Init() Couldn't get handle on HFIBTotCalPar container";
        return kFATAL;
    }

    // fill the TArray with Tot parameters!!!
    fParCont = fTotCalPar->GetCalParams();

    // Register output array
    fHFIBHitDataCA = new TClonesArray("R3BHFIBHitData");
    rootManager->Register("R3BHFIBHitData", "HFIB Diamond Hit", fHFIBHitDataCA, !fOnline);

    // TRandom3* rnd;
    rnd = new TRandom3();

    return kSUCCESS;
}

InitStatus R3BHFIBCal2Hit::ReInit() { return kSUCCESS; }

void R3BHFIBCal2Hit::Exec(Option_t* opt)
{
    Reset();

    // mapping tdc chnnels -> fibers
    Int_t MAP_TDC1[32] = { 56,  58,  50, 42, 32, 48, 4,  54, 2,   10,  8,  18, 12,  28,  62,  38,
                           110, 116, 98, 90, 94, 70, 88, 78, 104, 106, 74, 86, 102, 108, 124, 100 };
    Int_t MAP_TDC2[32] = { 84, 82, 76, 92, 68, 66, 126, 122, 128, 120, 114, 118, 72, 112, 96, 80,
                           16, 14, 36, 44, 40, 52, 46,  60,  24,  22,  64,  30,  26, 20,  34, 6 };
    Int_t MAP_TDC3[32] = { 57,  51,  49, 41, 35, 39, 5,  45, 11,  17, 21, 25,  29,  31,  63,  19,
                           119, 127, 65, 83, 97, 77, 87, 79, 111, 99, 81, 101, 117, 123, 125, 67 };
    Int_t MAP_TDC4[32] = { 73, 85, 89, 93, 69, 95, 121, 109, 115, 113, 105, 103, 75, 91, 107, 71,
                           9,  1,  43, 53, 37, 59, 47,  55,  15,  33,  61,  23,  13, 7,  27,  3 };
    Int_t MAP_TDC5[32] = { 170, 182, 178, 184, 190, 192, 166, 186, 150, 156, 140, 146, 136, 138, 160, 142,
                           238, 244, 218, 212, 216, 206, 208, 198, 232, 234, 202, 226, 228, 236, 252, 222 };
    Int_t MAP_TDC6[32] = { 194, 210, 204, 220, 230, 214, 254, 250, 256, 248, 242, 246, 200, 240, 224, 196,
                           154, 158, 180, 172, 174, 164, 168, 130, 148, 144, 176, 132, 134, 152, 188, 162 };
    Int_t MAP_TDC7[32] = { 159, 167, 169, 177, 185, 179, 157, 173, 153, 149, 145, 139, 133, 129, 171, 137,
                           247, 255, 225, 215, 211, 207, 205, 199, 239, 227, 195, 229, 245, 251, 253, 219 };
    Int_t MAP_TDC8[32] = { 193, 203, 209, 213, 221, 217, 249, 237, 243, 241, 233, 231, 197, 223, 235, 201,
                           147, 161, 191, 187, 183, 181, 175, 165, 141, 131, 163, 135, 143, 151, 189, 155 };

    Int_t nHits = fHFIBCalDataCA->GetEntriesFast();
    UInt_t iDetector = 0;
    double charge = 0;
    double time = 0;
    UInt_t ichn = 0;
    UInt_t fiber = 0;
    UInt_t tdc = 0;
    double position = -1;
    UInt_t inum = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BHFIBCalData*>(fHFIBCalDataCA->At(i));
        iDetector = map1->GetDetId();
        tdc = map1->GetTDC();
        ichn = map1->GetChannelId();

        if (iDetector == 0)
            continue;

        time = map1->GetTime();
        inum = (iDetector - 1) + (tdc - 1) + (ichn - 1);
        charge = map1->GetTot() - fParCont->GetAt(inum);

        if (tdc == 1)
            fiber = MAP_TDC1[ichn - 1];
        if (tdc == 2)
            fiber = MAP_TDC2[ichn - 1];
        if (tdc == 3)
            fiber = MAP_TDC3[ichn - 1];
        if (tdc == 4)
            fiber = MAP_TDC4[ichn - 1];
        if (tdc == 5)
            fiber = MAP_TDC5[ichn - 1];
        if (tdc == 6)
            fiber = MAP_TDC6[ichn - 1];
        if (tdc == 7)
            fiber = MAP_TDC7[ichn - 1];
        if (tdc == 8)
            fiber = MAP_TDC8[ichn - 1];
        position = double(fiber - 1) * 0.05 + rnd->Uniform(0, 1) * (0.05);

        AddHitFiber(iDetector, fiber, position, time, charge);
    }
}

R3BHFIBHitData* R3BHFIBCal2Hit::AddHitFiber(UInt_t detId, UInt_t fibId, double pos, double time, double charge)
{
    TClonesArray& clref = *fHFIBHitDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BHFIBHitData(detId, fibId, pos, time, charge);
}

void R3BHFIBCal2Hit::Reset()
{
    LOG(debug) << "Clearing HFIBHitStructure Structure";
    if (fHFIBHitDataCA)
    {
        fHFIBHitDataCA->Clear();
    }
}

ClassImp(R3BHFIBCal2Hit);
