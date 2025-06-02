/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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
// -----                  R3BLosMapped2Cal                -----
// -----          Created Feb 4th 2016 by R.Plag          -----
// ------------------------------------------------------------

#include "R3BLosMapped2Cal.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>

#include <TClonesArray.h>
#include <TMath.h>

#define LOS_COINC_WINDOW_V_NS 200;
#define LOS_COINC_WINDOW_TL_NS 200; // leading
#define LOS_COINC_WINDOW_TT_NS 400; // trailing, longer because of pileup
#define LOS_COINC_WINDOW_M_NS 400;
#define IS_NAN(x) TMath::IsNaN(x)

R3BLosMapped2Cal::R3BLosMapped2Cal()
    : R3BLosMapped2Cal("R3BLosMapped2Cal", 1)
{
}

R3BLosMapped2Cal::R3BLosMapped2Cal(const char* name, int iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fMappedTriggerItems(NULL)
    , fCalItems(new TClonesArray("R3BLosCalData"))
    , fCalTriggerItems(new TClonesArray("R3BLosCalData"))
    , fNofCalItems(0)
    , fNofTcalPars(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BLosMapped2Cal::~R3BLosMapped2Cal()
{
    R3BLOG(debug1, "Destructor");
    if (fCalItems)
        delete fCalItems;
    if (fCalTriggerItems)
        delete fCalTriggerItems;
}

InitStatus R3BLosMapped2Cal::Init()
{
    R3BLOG(info, "");
    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (!header)
        header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("R3BEventHeader"));

    // get access to Mapped data
    fMappedItems = dynamic_cast<TClonesArray*>(mgr->GetObject("LosMapped"));
    if (NULL == fMappedItems)
    {
        R3BLOG(fatal, "LosMapped not found");
        return kFATAL;
    }

    // get access to Trigger Mapped data
    fMappedTriggerItems = dynamic_cast<TClonesArray*>(mgr->GetObject("LosTriggerMapped"));
    R3BLOG_IF(warn, !fMappedTriggerItems, "LosTriggerMapped not found");

    // Request storage of Cal data in output tree
    mgr->Register("LosCal", "LosCal data", fCalItems, !fOnline);
    fCalItems->Clear();

    if (fMappedTriggerItems)
    {
        mgr->Register("LosTriggerCal", "LosTriggerCal data", fCalTriggerItems, !fOnline);
        fCalTriggerItems->Clear();
    }

    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BLosMapped2Cal::SetParContainers()
{
    fTcalPar = dynamic_cast<R3BTCalPar*>(FairRuntimeDb::instance()->getContainer("LosTCalPar"));
    if (!fTcalPar)
    {
        R3BLOG(fatal, "Could not get access to LosTCalPar-Container.");
        fNofTcalPars = 0;
        return;
    }
}

InitStatus R3BLosMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BLosMapped2Cal::Exec(Option_t*)
{
    // check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    auto nHits = fMappedItems->GetEntriesFast();

    if (nHits == 0)
        return;

    for (Int_t ihit = 0; ihit < nHits; ihit++) // nHits = Nchannel_LOS * NTypes = 4 or 8 * 3
    {
        Double_t times_ns = 0. / 0.;
        Double_t times_raw_ns = 0. / 0.;

        R3BLosMappedData* hit = dynamic_cast<R3BLosMappedData*>(fMappedItems->At(ihit));
        if (!hit)
            continue;

        // channel numbers are stored 1-based (1..n)
        UInt_t iDet = hit->GetDetector(); // 1..
        UInt_t iCha = hit->GetChannel();  // 1..
        UInt_t iType = hit->GetType();    // 0,1,2,3

        if ((iDet < 1) || (iDet > fNofDetectors))
        {
            R3BLOG(warn, "Detector number out of range: " << iDet);
            continue;
        }

        if (hit->GetTimeCoarse() > 8192)
        {
            R3BLOG(warn, "Coarse counter > 8192: Det " << iDet << " , Ch: " << iCha << " , type: " << iType);
            continue;
        }

        // Fetch calib data for current channel
        // new:

        if (iType < 3)
        {
            R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iDet, iCha, iType + 1);

            if (!par)
            {
                R3BLOG(warn, "Tcal par not found, Detector: " << iDet << ", Channel: " << iCha << ", Type: " << iType);
                continue;
            }

            // Convert TDC to [ns] ...

            times_raw_ns = par->GetTimeVFTX(hit->GetTimeFine());

            if (times_raw_ns < 0. || times_raw_ns > fClockFreq || IS_NAN(times_raw_ns))
            {

                R3BLOG(warn,
                       "Bad time in ns: det= " << iDet << ", ch= " << iCha << ", type= " << iType
                                               << ", time in channels = " << hit->GetTimeFine()
                                               << ", time in ns = " << times_raw_ns);
                continue;
            }

            // ... and add clock time
            times_ns = fClockFreq - times_raw_ns + hit->GetTimeCoarse() * fClockFreq;
        }
        else
        {
            // MTDC32 time need only a factor
            times_ns = hit->GetTimeFine() / 7.8 / 1000.; // range MTDC 3->7.8ps
        }

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
        R3BLosCalData* calItem = NULL;

        int iCal;
        for (iCal = 0; iCal < fNofCalItems; iCal++)
        {
            auto aCalItem = dynamic_cast<R3BLosCalData*>(fCalItems->At(iCal));
            if (aCalItem->GetDetector() != iDet)
            {
                // Do not consider an item for another detector.
                continue;
            }

            Double_t LOS_COINC_WINDOW_NS;
            Double_t Tdev;
            Bool_t LOS_COINC = false;

            if (iType == 0)
            {
                LOS_COINC_WINDOW_NS = LOS_COINC_WINDOW_V_NS;
                Tdev = fabs(aCalItem->GetMeanTimeVFTX() - times_ns);
                if (Tdev < LOS_COINC_WINDOW_NS)
                    LOS_COINC = true;
                //	if(nHits%48 != 0 ) cout<<"Tdev VFTX: "<<Tdev<<", "<<aCalItem->GetMeanTimeVFTX()<<endl;
            }
            if (iType == 1)
            {
                LOS_COINC_WINDOW_NS = LOS_COINC_WINDOW_TL_NS;
                Tdev = fabs(aCalItem->GetMeanTimeTAMEXL() - times_ns);
                if (Tdev < LOS_COINC_WINDOW_NS && aCalItem->GetTAMEXLNcha() > 0)
                    LOS_COINC = true;
                if (IS_NAN(Tdev) && aCalItem->GetTAMEXLNcha() == 0 && aCalItem->GetVFTXNcha() == 8)
                    LOS_COINC = true; // First Tamex leading time
            }
            if (iType == 2)
            {
                LOS_COINC_WINDOW_NS = LOS_COINC_WINDOW_TT_NS;
                Tdev = fabs(aCalItem->GetMeanTimeTAMEXT() - times_ns);
                if (Tdev < LOS_COINC_WINDOW_NS && aCalItem->GetTAMEXTNcha() > 0)
                    LOS_COINC = true;
                if (IS_NAN(Tdev) && aCalItem->GetTAMEXTNcha() == 0 && aCalItem->GetVFTXNcha() == 8)
                    LOS_COINC = true; // First Tamex trailing time
            }
            if (iType == 3)
            {
                LOS_COINC_WINDOW_NS = LOS_COINC_WINDOW_M_NS;
                Tdev = fabs(aCalItem->GetMeanTimeMTDC32() - times_ns);
                if (Tdev < LOS_COINC_WINDOW_NS && aCalItem->GetMTDC32Ncha() > 0)
                    LOS_COINC = true;
                if (IS_NAN(Tdev) && aCalItem->GetMTDC32Ncha() == 0)
                    LOS_COINC = true; // First MTDC time
            }

            if (LOS_COINC)
            {
                if (!calItem)
                    calItem = aCalItem;
            }
        }
        if (!calItem)
        {
            // there is no detector hit with matching time. Hence, create a new one.
            calItem = new ((*fCalItems)[fCalItems->GetEntriesFast()]) R3BLosCalData(iDet);
            fNofCalItems += 1;
        }
        // set the time to the correct cal item

        if (iType == 0)
        {
            calItem->fTimeV_ns[iCha - 1] = times_ns;
            if (calItem->fTimeV_ns[iCha - 1] < 0. || IS_NAN(calItem->fTimeV_ns[iCha - 1]))
                LOG(info) << "Problem with  fTimeV_ns: " << calItem->fTimeV_ns[iCha - 1] << " " << times_ns << " "
                          << endl;
        }

        if (iType == 1)
        {
            calItem->fTimeL_ns[iCha - 1] = times_ns;
            if (calItem->fTimeL_ns[iCha - 1] < 0. || IS_NAN(calItem->fTimeL_ns[iCha - 1]))
                LOG(info) << "Problem with  fTimeL_ns: " << calItem->fTimeL_ns[iCha - 1] << " " << times_ns << " "
                          << endl;
        }

        if (iType == 2)
        {
            calItem->fTimeT_ns[iCha - 1] = times_ns;
            if (calItem->fTimeT_ns[iCha - 1] < 0. || IS_NAN(calItem->fTimeT_ns[iCha - 1]))
                LOG(info) << "Problem with  fTimeT_ns: " << calItem->fTimeT_ns[iCha - 1] << " " << times_ns << " "
                          << endl;
        }

        if (iType == 3)
        {
            calItem->fTimeM_ns[iCha - 1] = times_ns;
            if (calItem->fTimeM_ns[iCha - 1] < 0. || IS_NAN(calItem->fTimeM_ns[iCha - 1]))
                LOG(info) << "Problem with  fTimeM_ns: " << calItem->fTimeM_ns[iCha - 1] << " " << times_ns << " "
                          << endl;
        }
        continue;
    }

    // Calibrate trigger channels -----------------------------------------------
    if (fMappedTriggerItems && fMappedTriggerItems->GetEntriesFast() > 0)
    {
        auto mapped_num = fMappedTriggerItems->GetEntriesFast();
        UInt_t prevdet = 0;
        R3BLosCalData* caltrigger = NULL;
        for (Int_t mapped_i = 0; mapped_i < mapped_num; mapped_i++)
        {
            auto mapped = dynamic_cast<R3BLosMappedData const*>(fMappedTriggerItems->At(mapped_i));
            UInt_t iDetector = mapped->GetDetector();
            UInt_t iChannel = mapped->GetChannel();
            UInt_t iType = mapped->GetType() + 1; // 1, 2, ... 4

            if (!caltrigger || prevdet != mapped->GetDetector())
            {

                caltrigger = new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()]) R3BLosCalData(iDetector);
                prevdet = mapped->GetDetector();
            }

            // Tcal parameters.
            auto par = fTcalPar->GetModuleParAt(2 + iDetector, iChannel, iType);
            if (!par)
            {
                R3BLOG(warn, "Trigger Tcal par not found.");
                continue;
            }

            // Convert TDC to [ns] ...
            Double_t time_ns = par->GetTimeVFTX(mapped->GetTimeFine());
            // ... and subtract it from the next clock cycle.
            if (time_ns > 0.)
            {
                time_ns = (mapped->GetTimeCoarse() + 1) * fClockFreq - time_ns;
                if (iType == 1)
                    caltrigger->fTimeV_ns[iChannel - 1] = time_ns;
                else if (iType == 2)
                    caltrigger->fTimeL_ns[iChannel - 1] = time_ns;
                else if (iType == 3)
                    caltrigger->fTimeT_ns[iChannel - 1] = time_ns;
                else if (iType == 4)
                    caltrigger->fTimeM_ns[iChannel - 1] = time_ns;
            }
        }
    }

    ++fNEvent;
}

void R3BLosMapped2Cal::FinishEvent()
{
    if (fCalItems)
    {
        fCalItems->Clear();
        fNofCalItems = 0;
    }
    if (fCalTriggerItems)
    {
        fCalTriggerItems->Clear();
    }
}

ClassImp(R3BLosMapped2Cal)
