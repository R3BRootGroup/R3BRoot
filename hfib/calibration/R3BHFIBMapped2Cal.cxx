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

#include "R3BHFIBCalData.h"
#include "R3BHFIBMapped2Cal.h"
#include "R3BHFIBMappedData.h"

R3BHFIBMapped2Cal::R3BHFIBMapped2Cal()
    : FairTask("R3B HFIB Calibrator")
{
}

R3BHFIBMapped2Cal::~R3BHFIBMapped2Cal()
{
    LOG(info) << "R3BHFIBMapped2Cal: Delete instance";
    if (fHFIBCalDataCA)
        delete fHFIBCalDataCA;
}

InitStatus R3BHFIBMapped2Cal::Init()
{
    LOG(info) << "R3BHFIBMapped2Cal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(fatal) << "R3BHFIBMapped2Cal::FairRootManager not found";
        return kFATAL;
    }

    fMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BHFIBMappedData"));
    if (!fMappedDataCA)
    {
        LOG(error) << "R3BHFIBMapped2CalPar::Init() fMappedDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BHFIBMapped2Cal:: FairRuntimeDb not opened";
    }

    fTCalPar = dynamic_cast<R3BTCalPar*>(rtdb->getContainer("HFIBTCalPar"));
    if (!fTCalPar)
    {
        LOG(error) << "R3BHFIBMapped2CalPar::Init() Couldn't get handle on HFIBTCalPar container";
    }
    else
    {
        LOG(info) << "R3BHFIBMapped2Cal:: HFIBTCalPar container open";
    }

    // OUTPUT DATA
    fHFIBCalDataCA = new TClonesArray("R3BHFIBCalData");

    rootManager->Register("R3BHFIBCalData", "HFIB Cal", fHFIBCalDataCA, !fOnline);

    return kSUCCESS;
}

InitStatus R3BHFIBMapped2Cal::ReInit() { return kSUCCESS; }

void R3BHFIBMapped2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays
    Reset();

    if (!fTCalPar)
    {
        LOG(warn) << "R3BHFIBMapped2Cal::Parameter container not found";
    }

    Entry Ref_vec[8];

    Double_t times_Ref;
    Double_t times_Fib;

    // loop over ref data
    Int_t nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BHFIBMappedData*>(fMappedDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        if (iDetector != 0)
        {
            continue;
        }

        UInt_t iEdge_Side = 0;
        UInt_t iCh = map1->GetChannelId();
        UInt_t id = iDetector * 10 + iCh;
        fTCalPar->printParams();
        R3BTCalModulePar* par_Refs = fTCalPar->GetModuleParAt(id, iCh, iEdge_Side + 1);
        if (NULL == par_Refs)
        {
            continue;
        }

        // Convert TDC to [ns]
        times_Ref = par_Refs->GetTimeVFTX(map1->GetFineTime());
        if (times_Ref < 0. || times_Ref > fClockFreq)
        {
            LOG(info) << "R3BHFIBMapped2Cal::Exec : Bad time in ns: det= " << iDetector
                      << ", time in channels = " << map1->GetFineTime() << ", time in ns = " << times_Ref;
            continue;
        }

        auto& entry = Ref_vec[map1->GetChannelId() - 1];
        entry.time = (10. / 3.) * (map1->GetCoarseTime() + 4096 * map1->GetEpoch()) - times_Ref;
        entry.Mapped = map1;
    }

    std::vector<Entry> fib_vec[32][8][2];

    nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BHFIBMappedData*>(fMappedDataCA->At(i));
        UInt_t iDetector = map1->GetDetId();

        // loop over fib data
        if (iDetector == 0)
            continue;

        UInt_t iFib = map1->GetChannelId();
        UInt_t iTDC = map1->GetTDC();
        UInt_t iEdge_Side = map1->GetEdge();
        UInt_t id = iDetector * 10 + iTDC;
        R3BTCalModulePar* par_Fibers = fTCalPar->GetModuleParAt(id, iFib, iEdge_Side + 1);

        if (NULL == par_Fibers)
        {
            continue;
        }

        // Convert TDC to [ns]
        times_Fib = par_Fibers->GetTimeVFTX(map1->GetFineTime());
        if (times_Fib < 0. || times_Fib > fClockFreq)
        {
            LOG(info) << "R3BHFIBMapped2Cal::Exec : Bad time in ns: det= " << iDetector << ", ch= " << iFib
                      << ", tdc= " << map1->GetTDC() << ", edge= " << map1->GetEdge()
                      << ", time in channels = " << map1->GetFineTime() << ", time in ns = " << times_Fib;
            continue;
        }
        Entry entry;
        entry.time =
            (10. / 3.) * (map1->GetCoarseTime() + 4096 * map1->GetEpoch()) - times_Fib - Ref_vec[iTDC - 1].time;
        entry.Mapped = map1;
        UInt_t iEdge = map1->GetEdge();
        if (iEdge == 3)
            iEdge = 1;
        fib_vec[map1->GetChannelId() - 1][iTDC - 1][iEdge].push_back(entry);
    }

    for (Int_t ch = 0; ch < 32; ch++)
    {
        for (Int_t tdc = 0; tdc < 8; tdc++)
        {

            size_t lead_i = 0, trail_i = 0;
            for (;;)
            {
                if (lead_i >= fib_vec[ch][tdc][0].size() || trail_i >= fib_vec[ch][tdc][1].size())
                {
                    break;
                }

                auto const& entry_lead = fib_vec[ch][tdc][0].at(lead_i);
                auto const& entry_trail = fib_vec[ch][tdc][1].at(trail_i);

                double tot = entry_trail.time - entry_lead.time;

                if (tot < 0)
                {
                    ++trail_i;
                    continue;
                }

                Int_t det = entry_lead.Mapped->GetDetId();
                TClonesArray& clref = *fHFIBCalDataCA;
                Int_t size = clref.GetEntriesFast();
                new (clref[size]) R3BHFIBCalData(det, tdc + 1, ch + 1, entry_lead.time, tot);
                ++lead_i;
                ++trail_i;
            }
        }
    }

    return;
}

void R3BHFIBMapped2Cal::Reset()
{
    LOG(debug) << "Clearing HFIBCalData Structure";
    if (fHFIBCalDataCA)
    {
        fHFIBCalDataCA->Clear();
    }
}

ClassImp(R3BHFIBMapped2Cal)
