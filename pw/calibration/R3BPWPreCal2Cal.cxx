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

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

#include "R3BPWCalData.h"
#include "R3BPWPreCal2Cal.h"
#include "R3BPWPreCalData.h"

R3BPWPreCal2Cal::R3BPWPreCal2Cal()
    : FairTask("R3B PW Calibrator")
{
}

R3BPWPreCal2Cal::~R3BPWPreCal2Cal()
{
    LOG(info) << "R3BPWPreCal2Cal: Delete instance";

    delete fTotCalPar;
}

InitStatus R3BPWPreCal2Cal::Init()
{
    LOG(info) << "R3BPWPreCal2Cal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(fatal) << "R3BPWPreCal2Cal::FairRootManager not found";
        return kFATAL;
    }

    // Parameter Container
    // Reading PWCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BPWPreCal2Cal:: FairRuntimeDb not opened";
    }

    fPreCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BPWPreCalData"));
    if (!fPreCalDataCA)
    {
        LOG(error) << "R3BPWPreCal2CalPar::Init() fPreCalDataCA not found";
        return kFATAL;
    }

    fTotCalPar = dynamic_cast<R3BPWTotCalPar*>(rtdb->getContainer("PWTotCalPar"));
    if (!fTotCalPar)
    {
        LOG(error) << "R3BPWPreCal2CalPar::Init() Couldn't get handle on PWTotCalPar container";
        return kFATAL;
    }

    // fill the TArray with Tot parameters!!!
    fParCont = fTotCalPar->GetCalParams();

    // OUTPUT DATA
    // Calibrated data
    fPWCalDataCA = new TClonesArray("R3BPWCalData", 50);
    rootManager->Register("R3BPWCalData", "PW Cal", fPWCalDataCA, !fOnline);

    return kSUCCESS;
}

InitStatus R3BPWPreCal2Cal::ReInit() { return kSUCCESS; }

void R3BPWPreCal2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays
    Reset();

    // loop over si data
    double time_1;
    double time_2;
    double tot_1;
    double tot_2;
    Int_t nHits = fPreCalDataCA->GetEntries();
    std::vector<int> mask(nHits, 0);
    for (Int_t i = 0; i < nHits; i++)
    {
        if (mask[i] == 1)
        {
            continue;
        }
        auto map1 = dynamic_cast<R3BPWPreCalData*>(fPreCalDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        if (iDetector == 1)
        {
            continue;
        }

        for (Int_t t = i + 1; t < nHits; t++)
        {
            if (mask[t] == 1)
            {
                continue;
            }
            auto nxt_chn = dynamic_cast<R3BPWPreCalData*>(fPreCalDataCA->At(t));
            UInt_t match = 0;
            UInt_t ch1 = map1->GetChannelId();
            UInt_t ch2 = nxt_chn->GetChannelId();
            UInt_t bar = 0;

            if (ch1 % 2 == 1)
            {
                if ((ch1 + 1) == ch2)
                    match = 1;
                bar = ch2 / 2;
            }
            if (ch1 % 2 == 0)
            {
                if ((ch2 + 1) == ch1)
                    match = 2;
                bar = ch1 / 2;
            }
            if (match > 0)
            {
                if (match == 1)
                {
                    tot_1 = map1->GetTot() * fParCont->GetAt(ch1 - 1);
                    tot_2 = nxt_chn->GetTot() * fParCont->GetAt(ch2 - 1);

                    time_1 = map1->GetTime();
                    time_2 = nxt_chn->GetTime();
                }

                else
                {
                    tot_2 = map1->GetTot() * fParCont->GetAt(ch1 - 1);
                    tot_1 = nxt_chn->GetTot() * fParCont->GetAt(ch2 - 1);

                    time_2 = map1->GetTime();
                    time_1 = nxt_chn->GetTime();
                }

                TClonesArray& clref = *fPWCalDataCA;
                Int_t size = clref.GetEntriesFast();
                new (clref[size]) R3BPWCalData(iDetector, bar, time_1, time_2, tot_1, tot_2);
                mask[i] = 1;
                mask[t] = 1;
                break;
            }
        }
    }
    return;
}

void R3BPWPreCal2Cal::Reset()
{
    LOG(debug) << "Clearing PWTotCalData Structure";
    if (fPWCalDataCA)
    {
        fPWCalDataCA->Clear();
    }
}

ClassImp(R3BPWPreCal2Cal)
