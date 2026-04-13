/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BPWCal2Hit.h"
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

#include "R3BPWCalData.h"
#include <list>
#include <vector>

R3BPWCal2Hit::R3BPWCal2Hit()
    : FairTask("R3B PW Cal to Hit")
{
}

R3BPWCal2Hit::~R3BPWCal2Hit()
{
    LOG(info) << "R3BPWCal2Hit: Delete instance";
    if (fPWHitDataCA)
        delete fPWHitDataCA;
}

InitStatus R3BPWCal2Hit::Init()
{

    // Parameter Container
    // Reading PWHitPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BPWCal2Hit:: FairRuntimeDb not opened";
    }

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(fatal) << "R3BPWCal2Hit::FairRootManager not found";
        return kFATAL;
    }

    fHitPar = dynamic_cast<R3BPWHitPar*>(rtdb->getContainer("PWHitPar"));
    if (!fHitPar)
    {
        LOG(error) << "R3BPWCal2Hit::Init() Couldn't get handle on PWHitPar container";
    }
    else
    {
        LOG(info) << "R3BPWCal2Hit:: PWHitPar container open";
    }

    fPWCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BPWCalData"));
    if (!fPWCalDataCA)
    {
        LOG(error) << "R3BPWCal2HitPar::Init() R3BPWCalData not found";
        return kFATAL;
    }

    // Register output array
    fPWHitDataCA = new TClonesArray("R3BPWHitData");
    rootManager->Register("R3BPWHitData", "PW Bar Hit", fPWHitDataCA, !fOnline);

    fParCont = fHitPar->GetCalParams();

    return kSUCCESS;
}

InitStatus R3BPWCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BPWCal2Hit::Exec(Option_t* opt)
{
    Reset();
    // loop over si data
    Int_t nHits = fPWCalDataCA->GetEntriesFast();
    UInt_t iDetector = 0;
    double charge_left = -1000;
    double charge_right = -1000;
    double time_left = 0;
    double time_right = 0;
    UInt_t ibar = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BPWCalData*>(fPWCalDataCA->At(i));
        iDetector = map1->GetDetId();

        if (iDetector == 0)
        {
            charge_right = map1->GetTot0();
            time_right = map1->GetTime0();
            ibar = map1->GetChannelId();

            charge_left = map1->GetTot1();
            time_left = map1->GetTime1();

            double position = double(ibar - 1) * (2.3 + 0.2) - 6.35; // relative to pad plane coordinates
            double charge = (charge_left + charge_right) / 2.;
            double time = (time_left + time_right) / 2. + fParCont->GetAt(ibar - 1);

            auto tof = time; // will be later relative to start (diamond)
            AddHitBar(iDetector, ibar, time, position, charge, tof);
        }
    }
}

R3BPWHitData* R3BPWCal2Hit::AddHitBar(UInt_t detId, UInt_t channel, double time, double pos, double charge, double tof)
{

    TClonesArray& clref = *fPWHitDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BPWHitData(detId, channel, time, pos, charge, tof);
}

void R3BPWCal2Hit::Reset()
{
    LOG(debug) << "Clearing PWHitStructure Structure";
    if (fPWHitDataCA)
    {
        fPWHitDataCA->Clear();
    }
}

ClassImp(R3BPWCal2Hit);
