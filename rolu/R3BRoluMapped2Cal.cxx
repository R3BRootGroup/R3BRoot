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

// ------------------------------------------------------------
// -----                  R3BRoluMapped2Cal               -----
// -----          Created July 2019 A. Kelic-Heil         -----
// -----                   following LOS                  -----
// ------------------------------------------------------------

#include "R3BRoluMapped2Cal.h"
#include "R3BEventHeader.h"
#include "R3BRoluCalData.h"
#include "R3BRoluMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>

#define Rolu_COINC_WINDOW_T_NS 200
#define IS_NAN(x) TMath::IsNaN(x)

R3BRoluMapped2Cal::R3BRoluMapped2Cal()
    : R3BRoluMapped2Cal("RoluTcal", 1)
{
}

R3BRoluMapped2Cal::R3BRoluMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fMappedTriggerItems(NULL)
    , fCalItems(new TClonesArray("R3BRoluCalData"))
    , fCalTriggerItems(new TClonesArray("R3BRoluCalData"))
    , fNofCalItems(0)
    , fNofTcalPars(0)
    , fNofModules(0)
    , fTcalPar(NULL)
    , fTrigger(1) // trigger 1 - onspill, 2 - offspill, -1 - all events
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvent(0)
    , fOnline(kFALSE)
{
}

R3BRoluMapped2Cal::~R3BRoluMapped2Cal()
{
    if (fCalItems)
    {
        delete fCalItems;
    }
    if (fCalTriggerItems)
    {
        delete fCalTriggerItems;
    }
}

InitStatus R3BRoluMapped2Cal::Init()
{
    fNofTcalPars = fTcalPar->GetNumModulePar();
    if (fNofTcalPars == 0)
    {
        LOG(fatal) << "There are no TCal parameters in container RoluTCalPar";
        return kFATAL;
    }

    LOG(info) << "R3BRoluMapped2Cal::Init()::read " << fNofModules << " modules";

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(fatal) << "R3BRoluMapped2Cal::Init() FairRootManager not found";
        return kFATAL;
    }

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!header)
    {
        LOG(warn) << "R3BRoluMapped2Cal::Init() EventHeader. not found";
        header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        LOG(info) << "R3BRoluMapped2Cal::Init() R3BEventHeader found";

    // get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("RoluMapped");
    if (NULL == fMappedItems)
    {
        LOG(fatal) << "R3BRoluMapped2Cal::Branch RoluMapped not found";
        return kFATAL;
    }

    // get access to Trigger Mapped data
    fMappedTriggerItems = (TClonesArray*)mgr->GetObject("RoluTriggerMapped");
    if (!fMappedTriggerItems)
        LOG(warn) << "R3BRoluMapped2Cal::Branch RoluTriggerMapped not found";

    // request storage of Cal data in output tree
    mgr->Register("RoluCal", "RoluCal data", fCalItems, !fOnline);
    fCalItems->Clear();

    if (fMappedTriggerItems)
    {
        mgr->Register("RoluTriggerCal", "RoluTriggerCal data", fCalTriggerItems, !fOnline);
        fCalTriggerItems->Clear();
    }

    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BRoluMapped2Cal::SetParContainers()
{
    fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("RoluTCalPar");

    LOG(info) << "R3BRoluMapped2Cal::SetParContainers() ROLU TCAL PARAMETERS SET";

    if (!fTcalPar)
    {
        LOG(error) << "Could not get access to RoluTCalPar-Container.";
        fNofTcalPars = 0;
        return;
    }
}

InitStatus R3BRoluMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BRoluMapped2Cal::Exec(Option_t* option)
{
    // check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    Int_t nHits = fMappedItems->GetEntriesFast();

    if (nHits < 1)
        return;

    // cout<<"Rolu Mapped2Cal nHits: "<<nHits<<endl;

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        Double_t times_ns = 0. / 0.;
        Double_t times_raw_ns = 0. / 0.;

        R3BRoluMappedData* hit = (R3BRoluMappedData*)fMappedItems->At(ihit);
        if (!hit)
            continue;

        // channel numbers are stored 1-based (1..n)
        UInt_t iDet = hit->GetDetector(); // 1..
        UInt_t iCha = hit->GetChannel();  // 1..4
        UInt_t iType = hit->GetType();    // 0,1

        if ((iDet < 1) || (iDet > fNofDetectors))
        {
            LOG(error) << "R3BRoluMapped2Cal::Exec : Detector number out of range: " << iDet;
            continue;
        }

        // Fetch calib data for current channel
        R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iDet, iCha, iType + 1);
        if (!par)
        {
            LOG(warn) << "R3BRoluMapped2Cal::Exec : Tcal par not found, Detector: " << iDet << ", Channel: " << iCha
                      << ", Type: " << iType;
            continue;
        }

        // Convert TDC to [ns] ...
        times_raw_ns = par->GetTimeVFTX(hit->GetTimeFine());

        if (times_raw_ns < 0. || times_raw_ns > fClockFreq || IS_NAN(times_raw_ns))
        {

            LOG(warn) << "R3BRoluMapped2Cal::Exec : Bad time in ns: det= " << iDet << ", ch= " << iCha
                      << ", type= " << iType << ", time in channels = " << hit->GetTimeFine()
                      << ", time in ns = " << times_raw_ns;
            continue;
        }

        // ... and add clock time
        times_ns = fClockFreq - times_raw_ns + hit->GetTimeCoarse() * fClockFreq;

        // cout<<"M2Cal: "<<iCha<<", "<<iType<<"; "<<hit->GetTimeFine()<<", "<<hit->GetTimeCoarse()<<",
        // "<<times_ns<<endl;

        /* Note: we have multi-hit data...
         *
         * So the map needs to have one item per detector and (multi-)hit
         * Then we need to establish a time window
         * Here, we have the hits unsorted in time and channel. If we
         * reconstruct a detector hit using a time window, So:
         *
         * For each single hit, search the list of detector hits. If a
         * matching hit is found (dt < window and item not yet set), add
         * item. Else create new detector hit.
         *
         * This way, we theoretically *might* end up with two calItems
         * which are actually just one. Hm... this should be very rare.
         * Care about that later if it becomes necessary.
         *
         */

        // see if there is already a detector hit around that time
        R3BRoluCalData* calItem = NULL;

        int iCal;
        for (iCal = 0; iCal < fNofCalItems; iCal++)
        {
            R3BRoluCalData* aCalItem = (R3BRoluCalData*)fCalItems->At(iCal);

            if (aCalItem->GetDetector() != iDet)
            {
                // Do not consider an item for another detector.
                continue;
            }

            Double_t Rolu_COINC_WINDOW_NS;
            Double_t Tdev;
            Bool_t Rolu_COINC = false;

            if (iType == 0)
            {
                Rolu_COINC_WINDOW_NS = Rolu_COINC_WINDOW_T_NS;
                Tdev = fabs(aCalItem->GetMeanTimeTAMEXL() - times_ns);
                if (Tdev < Rolu_COINC_WINDOW_NS && aCalItem->GetTAMEXLNcha() > 0)
                    Rolu_COINC = true;
                if (IS_NAN(Tdev) && aCalItem->GetTAMEXLNcha() == 0)
                    Rolu_COINC = true; // First Tamex leading time
            }
            if (iType == 1)
            {
                Rolu_COINC_WINDOW_NS = Rolu_COINC_WINDOW_T_NS;
                Tdev = fabs(aCalItem->GetMeanTimeTAMEXT() - times_ns);
                if (Tdev < Rolu_COINC_WINDOW_NS && aCalItem->GetTAMEXTNcha() > 0)
                    Rolu_COINC = true;
                if (IS_NAN(Tdev) && aCalItem->GetTAMEXTNcha() == 0)
                    Rolu_COINC = true; // First Tamex trailing time
            }

            if (Rolu_COINC)
            {
                // check if item is already set. If so, we need to skip this event!
                switch (iCha)
                {

                    case 1:
                    { // R
                        if (iType == 0 && !IS_NAN(aCalItem->GetTimeL_ns(0)))
                            goto skip_event_pileup;
                        if (iType == 1 && !IS_NAN(aCalItem->GetTimeT_ns(0)))
                            goto skip_event_pileup;
                    }
                    break;

                    case 3:
                    { // L
                        if (iType == 0 && !IS_NAN(aCalItem->GetTimeL_ns(2)))
                            goto skip_event_pileup;
                        if (iType == 1 && !IS_NAN(aCalItem->GetTimeT_ns(2)))
                            goto skip_event_pileup;
                    }
                    break;

                    case 2:
                    { // O
                        if (iType == 0 && !IS_NAN(aCalItem->GetTimeL_ns(1)))
                            goto skip_event_pileup;
                        if (iType == 1 && !IS_NAN(aCalItem->GetTimeT_ns(1)))
                            goto skip_event_pileup;
                    }
                    break;

                    case 4:
                    { // U
                        if (iType == 0 && !IS_NAN(aCalItem->GetTimeL_ns(3)))
                            goto skip_event_pileup;
                        if (iType == 1 && !IS_NAN(aCalItem->GetTimeT_ns(3)))
                            goto skip_event_pileup;
                    }
                    break;
                }
                if (!calItem)
                    calItem = aCalItem;
            }
        }
        if (!calItem)
        {
            // there is no detector hit with matching time. Hence, create a new one.
            calItem = new ((*fCalItems)[fNofCalItems++]) R3BRoluCalData(iDet);
            // fNofCalItems += 1;
        }
        // set the time to the correct cal item

        if (iType == 0)
        {
            calItem->fTimeL_ns[iCha - 1] = times_ns;
            if (calItem->fTimeL_ns[iCha - 1] < 0. || IS_NAN(calItem->fTimeL_ns[iCha - 1]))
                LOG(warn) << "Problem with fTimeL_ns: " << calItem->fTimeL_ns[iCha - 1] << " " << times_ns << " "
                          << endl;

            //		cout<<" TEST2: "<<iCha<<", "<<iType<<", "<<calItem->fTimeL_ns[iCha-1]<<endl;
        }

        if (iType == 1)
        {
            calItem->fTimeT_ns[iCha - 1] = times_ns;
            if (calItem->fTimeT_ns[iCha - 1] < 0. || IS_NAN(calItem->fTimeT_ns[iCha - 1]))
                LOG(warn) << "Problem with fTimeT_ns: " << calItem->fTimeT_ns[iCha - 1] << " " << times_ns << " "
                          << endl;

            //		cout<<" TEST3: "<<iCha<<", "<<iType<<", "<<calItem->fTimeT_ns[iCha-1]<<endl;
        }

        continue;
    skip_event_pileup:
        LOG(warn) << "R3BRoluMapped2Cal::Exec : " << fNEvent << " iCha: " << iCha << " iType: " << iType
                  << " iCal: " << iCal << " Skip event because of pileup.";
    }

    // Calibrate trigger channels.
    if (fMappedTriggerItems)
    {
        auto mapped_num = fMappedTriggerItems->GetEntriesFast();
        for (Int_t mapped_i = 0; mapped_i < mapped_num; mapped_i++)
        {
            auto mapped = (R3BRoluMappedData const*)fMappedTriggerItems->At(mapped_i);

            // Tcal parameters.
            auto* par = fTcalPar->GetModuleParAt(3, 1, 1);
            if (!par)
            {
                LOG(info) << "R3BRoluMapped2Cal::Exec : Trigger Tcal par not found.";
                continue;
            }

            // Convert TDC to [ns] ...
            Double_t time_ns = par->GetTimeVFTX(mapped->GetTimeFine());
            // ... and subtract it from the next clock cycle.
            time_ns = (mapped->GetTimeCoarse() + 1) * fClockFreq - time_ns;

            auto cal = new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()]) R3BRoluCalData(1);
            cal->fTimeL_ns[0] = time_ns;
        }
    }

    ++fNEvent;
}

void R3BRoluMapped2Cal::FinishEvent()
{
    fNofCalItems = 0;
    if (fCalItems)
    {
        fCalItems->Clear();
    }

    if (fCalTriggerItems)
    {
        fCalTriggerItems->Clear();
    }
}

ClassImp(R3BRoluMapped2Cal);
