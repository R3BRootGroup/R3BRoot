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

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

#include "R3BDMCalData.h"
#include "R3BDMMapped2Cal.h"
#include "R3BDMMappedData.h"

R3BDMMapped2Cal::R3BDMMapped2Cal()
    : FairTask("R3B DM Calibrator")
{
}

R3BDMMapped2Cal::~R3BDMMapped2Cal()
{
    LOG(info) << "R3BDMMapped2Cal: Delete instance";
    if (fDMCalDataCA)
        delete fDMCalDataCA;
}

InitStatus R3BDMMapped2Cal::Init()
{
    LOG(info) << "R3BDMMapped2Cal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(fatal) << "R3BDMMapped2Cal::FairRootManager not found";
        return kFATAL;
    }

    fMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BDMMappedData"));
    if (!fMappedDataCA)
    {
        LOG(error) << "R3BDMMapped2CalPar::Init() fMappedDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BDMMapped2Cal:: FairRuntimeDb not opened";
    }

    fTCalPar = dynamic_cast<R3BTCalPar*>(rtdb->getContainer("DMTCalPar"));
    if (!fTCalPar)
    {
        LOG(error) << "R3BDMMapped2CalPar::Init() Couldn't get handle on DMTCalPar container";
    }
    else
    {
        LOG(info) << "R3BDMMapped2Cal:: DMTCalPar container open";
    }

    // OUTPUT DATA
    fDMCalDataCA = new TClonesArray("R3BDMCalData");

    rootManager->Register("R3BDMCalData", "DM Cal", fDMCalDataCA, !fOnline);

    return kSUCCESS;
}

InitStatus R3BDMMapped2Cal::ReInit() { return kSUCCESS; }

void R3BDMMapped2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays
    Reset();

    if (!fTCalPar)
    {
        LOG(warn) << "R3BDMMapped2Cal::Parameter container not found";
    }

    Entry Ref_vec[4];

    Double_t times_Ref;
    Double_t times_Pad;

    // loop over ref data
    Int_t nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BDMMappedData*>(fMappedDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        if (iDetector != 0)
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
            LOG(info) << "R3BDMMapped2Cal::Exec : Bad time in ns: det= " << iDetector
                      << ", time in channels = " << map1->GetFineTime() << ", time in ns = " << times_Ref;
            continue;
        }

        auto& entry = Ref_vec[map1->GetTDC()];
        entry.time = 5. * (map1->GetCoarseTime() + 2048 * map1->GetEpoch()) - times_Ref;
        entry.Mapped = map1;
    }

    std::vector<Entry> pad_vec[72][2];

    nHits = fMappedDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BDMMappedData*>(fMappedDataCA->At(i));
        UInt_t iDetector = map1->GetDetId();

        // loop over pads data
        if (iDetector == 0)
            continue;

        UInt_t iCh = map1->GetChannelId();
        UInt_t iTDC = map1->GetTDC();
        UInt_t iEdge_Side = map1->GetEdge();
        R3BTCalModulePar* par_Pads = fTCalPar->GetModuleParAt(iDetector + 1, iCh, iEdge_Side + 1);

        if (NULL == par_Pads)
        {
            continue;
        }

        // Convert TDC to [ns]
        times_Pad = par_Pads->GetTimeVFTX(map1->GetFineTime());

        if (times_Pad < 0. || times_Pad > fClockFreq)
        {
            LOG(info) << "R3BDMMapped2Cal::Exec : Bad time in ns: det= " << iDetector << ", ch= " << iCh
                      << ", tdc= " << map1->GetTDC() << ", edge= " << map1->GetEdge()
                      << ", time in channels = " << map1->GetFineTime() << ", time in ns = " << times_Pad;
            continue;
        }
        Entry entry;
        entry.time = 5. * (map1->GetCoarseTime() + 2048 * map1->GetEpoch()) - times_Pad - Ref_vec[iTDC].time;
        entry.Mapped = map1;
        pad_vec[map1->GetChannelId() - 1][iEdge_Side].push_back(entry);
    }

    for (Int_t ch = 0; ch < 72; ch++)
    {
        size_t lead_i = 0, trail_i = 0;
        for (;;)
        {
            if (lead_i >= pad_vec[ch][1].size() || trail_i >= pad_vec[ch][0].size())
            {
                break;
            }

            auto const& entry_lead = pad_vec[ch][1].at(lead_i);
            auto const& entry_trail = pad_vec[ch][0].at(trail_i);

            double tot = entry_trail.time - entry_lead.time;
            UShort_t tdc = entry_lead.Mapped->GetTDC();
            UShort_t det = entry_lead.Mapped->GetDetId();
            if (tot < 0)
            {
                ++trail_i;
                continue;
            }

            TClonesArray& clref = *fDMCalDataCA;
            Int_t size = clref.GetEntriesFast();
            new (clref[size]) R3BDMCalData(det, tdc, ch + 1, entry_lead.time, tot);
            ++lead_i;
            ++trail_i;
        }
    }

    return;
}

void R3BDMMapped2Cal::Reset()
{
    LOG(debug) << "Clearing DMCalData Structure";
    if (fDMCalDataCA)
    {
        fDMCalDataCA->Clear();
    }
}

ClassImp(R3BDMMapped2Cal)
