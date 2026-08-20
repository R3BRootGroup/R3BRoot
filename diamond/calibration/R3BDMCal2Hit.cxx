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

#include "R3BDMCal2Hit.h"
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

#include "R3BDMCalData.h"
#include "R3BDMHitData.h"
#include <list>
#include <vector>

R3BDMCal2Hit::R3BDMCal2Hit()
    : FairTask("R3B DM Cal to Hit")
{
}

R3BDMCal2Hit::~R3BDMCal2Hit()
{
    LOG(info) << "R3BDMCal2Hit: Delete instance";
    if (fTotCalPar)
        delete fTotCalPar;
    if (fDMHitDataCA)
        delete fDMHitDataCA;
}

InitStatus R3BDMCal2Hit::Init()
{

    LOG(info) << "R3BDMCal2Hit::Init()";

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BDMCal2Hit:: FairRuntimeDb not opened";
    }

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(fatal) << "R3BDMCal2Hit::FairRootManager not found";
        return kFATAL;
    }

    fDMCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BDMCalData"));
    if (!fDMCalDataCA)
    {
        LOG(error) << "R3BDMCal2Hit::Init() R3BDMCalData not found";
        return kFATAL;
    }

    fTotCalPar = dynamic_cast<R3BDMTotCalPar*>(rtdb->getContainer("DMTotCalPar"));
    if (!fTotCalPar)
    {
        LOG(error) << "R3BDMCal2HitPar::Init() Couldn't get handle on DMTotCalPar container";
        return kFATAL;
    }

    // fill the TArray with Tot parameters!!!
    fParCont = fTotCalPar->GetCalParams();

    // Register output array
    fDMHitDataCA = new TClonesArray("R3BDMHitData");
    rootManager->Register("R3BDMHitData", "DM Diamond Hit", fDMHitDataCA, !fOnline);

    // TRandom3* rnd;
    rnd = new TRandom3();

    return kSUCCESS;
}

InitStatus R3BDMCal2Hit::ReInit() { return kSUCCESS; }

void R3BDMCal2Hit::Exec(Option_t* opt)
{
    Reset();

    // mapping tdc chnnels -> pads
    Int_t TDC0_X[18] = { 3, 3, 2, 2, 1, 1, 3, 1, 2, 3, 3, 2, 2, 1, 1, 2, 3, 1 };
    Int_t TDC0_Y[18] = { 6, 4, 6, 4, 6, 4, 2, 2, 1, 5, 3, 5, 3, 5, 3, 2, 1, 1 };
    Int_t TDC1_X[18] = { 5, 6, 4, 6, 6, 5, 5, 4, 4, 6, 4, 5, 6, 6, 5, 5, 4, 4 };
    Int_t TDC1_Y[18] = { 1, 2, 2, 4, 6, 4, 6, 4, 6, 1, 1, 2, 3, 5, 3, 5, 3, 5 };
    Int_t TDC2_Y[18] = { 3, 3, 2, 2, 1, 1, 3, 1, 2, 3, 3, 2, 2, 1, 1, 2, 3, 1 };
    Int_t TDC2_X[18] = { 6, 4, 6, 4, 6, 4, 2, 2, 1, 5, 3, 5, 3, 5, 3, 2, 1, 1 };
    Int_t TDC3_Y[18] = { 5, 6, 4, 6, 6, 5, 5, 4, 4, 6, 4, 5, 6, 6, 5, 5, 4, 4 };
    Int_t TDC3_X[18] = { 1, 2, 2, 4, 6, 4, 6, 4, 6, 1, 1, 2, 3, 5, 3, 5, 3, 5 };

    Int_t nHits = fDMCalDataCA->GetEntriesFast();
    UInt_t iDetector = 0;
    double charge = 0;
    double time = 0;
    UInt_t ichn = 0;
    UInt_t pad = 0;
    UInt_t tdc = 0;
    double pos_x = -1;
    double pos_y = -1;
    int pad_x = 0;
    int pad_y = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BDMCalData*>(fDMCalDataCA->At(i));
        iDetector = map1->GetDetId();
        tdc = map1->GetTDC();
        ichn = map1->GetChannelId() - 1;

        if (iDetector == 0)
            continue;

        if (tdc == 0)
        {
            pad_x = TDC0_X[ichn];
            pad_y = TDC0_Y[ichn];
        }
        if (tdc == 1)
        {
            pad_x = TDC1_X[ichn - 18];
            pad_y = TDC1_Y[ichn - 18];
        }
        if (tdc == 2)
        {
            pad_x = TDC2_X[ichn - 36];
            pad_y = TDC2_Y[ichn - 36];
        }
        if (tdc == 3)
        {
            pad_x = TDC3_X[ichn - 54];
            pad_y = TDC3_Y[ichn - 54];
        }

        pad = pad_x + (pad_y - 1) * 6;

        charge = map1->GetTot() - fParCont->GetAt(ichn);
        time = map1->GetTime();
        pos_x = double(pad_x - 1) * (1. / 6.) + rnd->Uniform(0, 1) * (1. / 6.);
        pos_y = double(pad_y - 1) * (1. / 6.) + rnd->Uniform(0, 1) * (1. / 6.);

        AddHitDiamond(iDetector, pad, pos_x, pos_y, time, charge);
    }
}

R3BDMHitData* R3BDMCal2Hit::AddHitDiamond(UInt_t detId,
                                          UInt_t pad,
                                          double pos_x,
                                          double pos_y,
                                          double time,
                                          double charge)
{
    TClonesArray& clref = *fDMHitDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BDMHitData(detId, pad, pos_x, pos_y, time, charge);
}

void R3BDMCal2Hit::Reset()
{
    LOG(debug) << "Clearing DMHitStructure Structure";
    if (fDMHitDataCA)
    {
        fDMHitDataCA->Clear();
    }
}

ClassImp(R3BDMCal2Hit);
