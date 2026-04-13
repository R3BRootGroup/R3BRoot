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

#include "R3BPWMapped2PreCal.h"
#include "R3BPWMappedData.h"
#include "R3BPWPreCalData.h"

R3BPWMapped2PreCal::R3BPWMapped2PreCal()
    : FairTask("R3B PW Calibrator")
{
}

R3BPWMapped2PreCal::~R3BPWMapped2PreCal()
{
    LOG(info) << "R3BPWMapped2PreCal: Delete instance";
    delete fPWPreCalDataCA;
}

InitStatus R3BPWMapped2PreCal::Init()
{
    LOG(info) << "R3BPWMapped2PreCal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(fatal) << "R3BPWMapped2PreCal::FairRootManager not found";
        return kFATAL;
    }

    fMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BPWMappedData"));
    if (!fMappedDataCA)
    {
        LOG(error) << "R3BPWMapped2PreCalPar::Init() fMappedDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BPWMapped2PreCal:: FairRuntimeDb not opened";
    }

    fTCalPar = dynamic_cast<R3BTCalPar*>(rtdb->getContainer("PWTCalPar"));
    if (!fTCalPar)
    {
        LOG(error) << "R3BPWMapped2PreCalPar::Init() Couldn't get handle on PWTCalPar container";
    }
    else
    {
        LOG(info) << "R3BPWMapped2PreCal:: PWTCalPar container open";
    }

    // OUTPUT DATA
    fPWPreCalDataCA = new TClonesArray("R3BPWPreCalData");

    rootManager->Register("R3BPWPreCalData", "PW Pre Cal", fPWPreCalDataCA, !fOnline);

    return kSUCCESS;
}

InitStatus R3BPWMapped2PreCal::ReInit() { return kSUCCESS; }

void R3BPWMapped2PreCal::Exec(Option_t* option)
{
    // Reset entries in output arrays
    Reset();

    if (!fTCalPar)
    {
        LOG(warn) << "R3BPWMapped2PreCal::Parameter container not found";
    }

    Entry Ref_vec[7];

    Double_t times_Ref;
    Double_t times_Sipm;

    // loop over ref data
    Int_t nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BPWMappedData*>(fMappedDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        if (iDetector != 1)
        {
            continue;
        }

        UInt_t iEdge_Side = 0;
        UInt_t iCh = map1->GetChannelId();
        R3BTCalModulePar* par_Refs = fTCalPar->GetModuleParAt(iDetector + 1, iCh, iEdge_Side + 1);

        if (NULL == par_Refs)
        {
            continue;
        }

        // Convert TDC to [ns]
        times_Ref = par_Refs->GetTimeVFTX(map1->GetFineTime());

        if (times_Ref < 0. || times_Ref > fClockFreq)
        {

            LOG(info) << "R3BPWMapped2PreCal::Exec : Bad time in ns: det= " << iDetector
                      << ", time in channels = " << map1->GetFineTime() << ", time in ns = " << times_Ref;
            continue;
        }

        auto& entry = Ref_vec[map1->GetChannelId() - 1];
        entry.time = 5. * (map1->GetCoarseTime() + 2048 * map1->GetEpoch()) - times_Ref;
        entry.Mapped = map1;
    }

    std::vector<Entry> si_vec[32][2];

    nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BPWMappedData*>(fMappedDataCA->At(i));
        UInt_t iDetector = map1->GetDetId();
        if (iDetector == 1)
        {
            TClonesArray& clref = *fPWPreCalDataCA;
            Int_t size = clref.GetEntriesFast();
            new (clref[size]) R3BPWPreCalData(1, map1->GetChannelId(), Ref_vec[map1->GetChannelId() - 1].time, 0);
        }
        // loop over si data
        if (iDetector == 0)
        {
            UInt_t iSiPM = map1->GetChannelId();
            UInt_t iEdge_Side = map1->GetEdge() * 2;
            R3BTCalModulePar* par_Sipm = fTCalPar->GetModuleParAt(iDetector + 1, iSiPM, iEdge_Side + 1);

            if (NULL == par_Sipm)
            {
                continue;
            }

            // Convert TDC to [ns]
            times_Sipm = par_Sipm->GetTimeVFTX(map1->GetFineTime());

            if (times_Sipm < 0. || times_Sipm > fClockFreq)
            {

                LOG(info) << "R3BPWMapped2PreCal::Exec : Bad time in ns: det= " << iDetector << ", si= " << iSiPM
                          << ", edge= " << map1->GetEdge() << ", time in channels = " << map1->GetFineTime()
                          << ", time in ns = " << times_Sipm;
                continue;
            }
            Entry entry;
            entry.time = 5. * (map1->GetCoarseTime() + 2048 * map1->GetEpoch()) - times_Sipm - Ref_vec[0].time;
            entry.Mapped = map1;
            si_vec[map1->GetChannelId() - 1][map1->GetEdge()].push_back(entry);
        }
    }

    for (Int_t si = 0; si < 32; si++)
    {
        size_t lead_i = 0, trail_i = 0;
        for (;;)
        {
            if (lead_i >= si_vec[si][1].size() || trail_i >= si_vec[si][0].size())
            {
                break;
            }

            auto const& entry_lead = si_vec[si][1].at(lead_i);
            auto const& entry_trail = si_vec[si][0].at(trail_i);

            double tot = entry_trail.time - entry_lead.time;
            if (tot < 0)
            {
                ++trail_i;
                continue;
            }

            TClonesArray& clref = *fPWPreCalDataCA;
            Int_t size = clref.GetEntriesFast();
            new (clref[size]) R3BPWPreCalData(0, si + 1, entry_lead.time, tot);
            ++lead_i;
            ++trail_i;
        }
    }

    return;
}

void R3BPWMapped2PreCal::Reset()
{
    LOG(debug) << "Clearing PWCalData Structure";
    if (fPWPreCalDataCA)
    {
        fPWPreCalDataCA->Clear();
    }
}

ClassImp(R3BPWMapped2PreCal)
