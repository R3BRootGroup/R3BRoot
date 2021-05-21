/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
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
// -----                  R3BFiberMAPMTMapped2TCal           -----
// -----          Created May 13th 2021 by V.Panin           -----
// ------------------------------------------------------------

#include "R3BFiberMAPMTMapped2Cal.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTMappedData.h"
#include "R3BTCalEngine.h"
#include "TClonesArray.h"
#include <cassert>

R3BFiberMAPMTMapped2Cal::R3BFiberMAPMTMapped2Cal(const char* a_name, Int_t a_verbose)
    : FairTask(TString("R3B") + a_name + "Mapped2Cal", a_verbose)
    , fName(a_name)
    , fMAPMTTCalPar(nullptr)
    , fMAPMTTrigTCalPar(nullptr)
    , fMappedItems(nullptr)
    , fCalItems(new TClonesArray("R3BFiberMAPMTCalData"))
    , fCalTriggerItems(new TClonesArray("R3BFiberMAPMTCalData"))
    , fClockFreq(1000. / 150)
    , fnEvents(0)
{
}

R3BFiberMAPMTMapped2Cal::~R3BFiberMAPMTMapped2Cal()
{
    delete fCalItems;
    delete fCalTriggerItems;
}

InitStatus R3BFiberMAPMTMapped2Cal::Init()
{
    if (!fMAPMTTCalPar)
    {
        LOG(ERROR) << "TCal parameter containers missing, "
                      "did you forget SetParContainers?";
        return kERROR;
    }
    if (0 == fMAPMTTCalPar->GetNumModulePar())
    {
        LOG(ERROR) << "No TCal parameters in containers " << fMAPMTTCalPar->GetName() << ".";
        return kERROR;
    }
    auto mgr = FairRootManager::Instance();
    if (!mgr)
    {
        LOG(ERROR) << "FairRootManager not found.";
        return kERROR;
    }
    auto name = fName + "Mapped";
    fMappedItems = (TClonesArray*)mgr->GetObject(name);
    if (!fMappedItems)
    {
        LOG(ERROR) << "Branch " << name << " not found.";
        return kERROR;
    }
    mgr->Register(fName + "Cal", "Land", fCalItems, kTRUE);
    mgr->Register(fName + "TriggerCal", "Land", fCalTriggerItems, kTRUE);
    return kSUCCESS;
}

void R3BFiberMAPMTMapped2Cal::SetParContainers()
{
#define GET_TCALPAR(NAME)                                                              \
    do                                                                                 \
    {                                                                                  \
        auto name = fName + #NAME "TCalPar";                                           \
        f##NAME##TCalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer(name); \
        if (!f##NAME##TCalPar)                                                         \
        {                                                                              \
            LOG(ERROR) << "Could not get access to " << name << " container.";         \
        }                                                                              \
    } while (0)
    GET_TCALPAR(MAPMT);
    GET_TCALPAR(MAPMTTrig);
}

InitStatus R3BFiberMAPMTMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BFiberMAPMTMapped2Cal::Exec(Option_t* option)
{
    auto mapped_num = fMappedItems->GetEntriesFast();
    LOG(DEBUG) << "R3BFiberMAPMTMapped2Cal::Exec:fMappedItems=" << fMappedItems->GetName() << '.';
    for (auto i = 0; i < mapped_num; i++)
    {
        auto mapped = (R3BFiberMAPMTMappedData*)fMappedItems->At(i);
        assert(mapped);

        auto channel = mapped->GetChannel();
        LOG(DEBUG) << " R3BFiberMAPMTMapped2Cal::Exec:Channel=" << channel << ":Side=" << mapped->GetSide()
                   << ":Edge=" << (mapped->IsLeading() ? "Leading" : "Trailing") << '.';

        // Fetch tcal parameters.
        R3BTCalModulePar* par;
        if (mapped->IsTrigger())
        {
            par = fMAPMTTrigTCalPar->GetModuleParAt(1, channel, 1);
        }
        else
        {
            auto tcal_channel_i = channel * 2 - (mapped->IsLeading() ? 1 : 0);
            par = fMAPMTTCalPar->GetModuleParAt(1, tcal_channel_i, mapped->GetSide() + 1);
        }
        if (!par)
        {
            LOG(WARNING) << "R3BFiberMAPMTMapped2Cal::Exec (" << fName << "): Channel=" << channel
                         << ": TCal par not found.";
            continue;
        }

        // Calibrate fine time.
        auto fine_raw = mapped->GetFine();
        if (-1 == fine_raw)
        {
            // TODO: Is this really ok?
            continue;
        }
        auto fine_ns = par->GetTimeClockTDC(fine_raw);
        LOG(DEBUG) << " R3BFiberMAPMTMapped2Cal::Exec: Fine raw=" << fine_raw << " -> ns=" << fine_ns << '.';

        // we have to differ between single PMT which is on Tamex and MAPMT which is on clock TDC
        Double_t time_ns = -1;
        if (fine_ns < 0. || fine_ns >= fClockFreq)
        {
            LOG(ERROR) << "R3BFiberMAPMTMapped2Cal::Exec (" << fName << "): Channel=" << channel
                       << ": Bad CTDC fine time (raw=" << fine_raw << ",ns=" << fine_ns << ").";
            continue;
        }

        // Calculate final time with clock cycles.
        //		time_ns = mapped->GetCoarse() * fClockFreq +
        //		(mapped->IsLeading() ? -fine_ns : fine_ns);
        // new clock TDC firmware need here a minus
        time_ns = mapped->GetCoarse() * fClockFreq - fine_ns;

        LOG(DEBUG) << " R3BFiberMAPMTMapped2Cal::Exec (" << fName << "): Channel=" << channel << ": Time=" << time_ns
                   << "ns.";
        if (fName == "Fi30" || fName == "Fi31" || fName == "Fi32" || fName == "Fi33")
        {
            if (2 == mapped->GetSide())
            {
                new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()])
                    R3BFiberMAPMTCalData(mapped->GetSide(), channel, mapped->IsLeading(), time_ns);
            }
            else
            {
                new ((*fCalItems)[fCalItems->GetEntriesFast()])
                    R3BFiberMAPMTCalData(mapped->GetSide(), channel, mapped->IsLeading(), time_ns);
            }
        }
        else
        {
            Int_t iFib = 0;
            if (channel < 65)
            {
                iFib = channel * 2;
                if (2 == mapped->GetSide())
                {
                    new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                    new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
                else
                {
                    new ((*fCalItems)[fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                    new ((*fCalItems)[fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
            }
            else if (channel > 64 && channel < 193)
            {
                iFib = 64 + channel;
                if (2 == mapped->GetSide())
                {
                    new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
                else
                {
                    new ((*fCalItems)[fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
            }
            else if (channel > 192 && channel < 257)
            {
                iFib = 258 + (channel - 193) * 2;
                if (2 == mapped->GetSide())
                {
                    new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                    new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
                else
                {
                    new ((*fCalItems)[fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                    new ((*fCalItems)[fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
            }
            //	cout<<"CAL "<<fName<<"; "<<channel<<", "<<iFib<<", "<<mapped->GetSide()<<", "<<mapped->IsLeading()<<",
            //"<<time_ns<<endl;
        } // end fibdet selection
          /*
                          if (mapped->IsTrigger())
                          {
                              new ((*fCalTriggerItems)[fCalTriggerItems->GetEntriesFast()])
                                  R3BFiberMAPMTCalData(mapped->GetSide(), channel, mapped->IsLeading(), time_ns);
                          }
                          else
                          {
                              new ((*fCalItems)[fCalItems->GetEntriesFast()])
                                  R3BFiberMAPMTCalData(mapped->GetSide(), channel, mapped->IsLeading(), time_ns);
                          }
          */
    }
    fnEvents++;
}

void R3BFiberMAPMTMapped2Cal::FinishEvent()
{
    fCalItems->Clear();
    fCalTriggerItems->Clear();
}

void R3BFiberMAPMTMapped2Cal::FinishTask() {}

ClassImp(R3BFiberMAPMTMapped2Cal)
