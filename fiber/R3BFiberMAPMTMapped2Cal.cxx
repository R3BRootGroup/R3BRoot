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
    , fNoCalItems(0)
    , fNoCalTrigItems(0)
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

    // if(mapped_num > 0) cout<<"Mapped Items: "<<fName<<", "<<mapped_num-8<<endl;

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
        /*
                if(fName == "Fi23a" && mapped->GetSide() == 0)
                {
                    if(time_ns > tmaxfib23a[channel-1]) tmaxfib23a[channel-1] = time_ns;
                    if(time_ns < tminfib23a[channel-1]) tminfib23a[channel-1] = time_ns;
                }

        */
        LOG(DEBUG) << " R3BFiberMAPMTMapped2Cal::Exec (" << fName << "): Channel=" << channel << ": Time=" << time_ns
                   << "ns.";

        //       cout<<"Input M2C: "<<fName<<", "<<  i<<", "<<fCalItems->GetEntriesFast()<<", "<<channel<<",
        //       "<<mapped->IsTrigger()<<", "<<mapped->GetSide()<<", "<<mapped->IsLeading()<<endl;

        if (fName == "Fi30" || fName == "Fi31" || fName == "Fi32" || fName == "Fi33")
        {
            if (2 == mapped->GetSide())
            {
                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                    R3BFiberMAPMTCalData(mapped->GetSide(), channel, mapped->IsLeading(), time_ns);
            }
            else
            {
                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                    R3BFiberMAPMTCalData(mapped->GetSide(), channel, mapped->IsLeading(), time_ns);
            }
        }
        if (fName == "Fi23a" || fName == "Fi23b")
        {
            // for Fib23a and Fib23b some anode schannels have two fibers. Here the channel number is not the fiber
            // number!
            if (2 == mapped->GetSide())
            {
                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                    R3BFiberMAPMTCalData(mapped->GetSide(), channel, mapped->IsLeading(), time_ns);
            }
            else
            {
                Int_t iFib = 0;
                if (channel < 65)
                {
                    iFib = channel * 2;
                    new ((*fCalItems)[fNoCalItems++])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                    new ((*fCalItems)[fNoCalItems++])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
                else if (channel > 64 && channel < 193)
                {
                    iFib = 64 + channel;
                    new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
                else if (channel > 192 && channel < 257)
                {
                    iFib = 258 + (channel - 193) * 2;
                    new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                    new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                        R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                }
            }
        }

        /*
                else // FIb23a/23b
                {
                    Int_t iFib = 0;
                    if (mapped->GetSide() != 0) // up channels
                    {
                        if (channel < 65)
                        {
                            iFib = channel * 2;
                            if (2 == mapped->GetSide())
                            {
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                            }
                            else
                            {
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                            }
                        }

                        else if (channel > 64 && channel < 193)
                        {
                            iFib = 64 + channel;
                            if (2 == mapped->GetSide())
                            {
                                new ((*fCalTriggerItems)[fNoCalItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                            }
                            else
                            {
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);

                                if (channel > 123 && channel < 134)
                                {
                                    //	 cout<<"Mapped2Cal orig : "<<fName<<", "<<", "<<iFib<<";
           "<<mapped->GetSide()<<",
                                    //"<<mapped->IsLeading()<<"; "<<time_ns<<endl;

                                    auto time_ghost = time_ns;
                                    auto side_ghost = 1 - mapped->GetSide();
                                    //        new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    //            R3BFiberMAPMTCalData(side_ghost,
                                    //                                 iFib,
                                    //                                 mapped->IsLeading(),
                                    //                                 time_ghost); // add ghost fiber to treat the hole
                                    //
                                    //   cout<<"Mapped2Cal ghost: "<<fName<<", "<<", "<<iFib<<"; "<<side_ghost<<",
                                    //   "<<mapped->IsLeading()<<"; "<<time_ghost<<endl;
                                }
                            }
                        }
                        else if (channel > 192 && channel < 257)
                        {
                            iFib = 258 + (channel - 193) * 2;
                            if (2 == mapped->GetSide())
                            {
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                            }
                            else
                            {
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib - 1, mapped->IsLeading(), time_ns);
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                            }
                        }
                    }
                    else if (mapped->GetSide() != 1) // down channels
                    {
                        if (channel < 65)
                        {
                            iFib = channel + int((channel - 1) / 4) * 4;
                            if (2 == mapped->GetSide())
                            {
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib + 4, mapped->IsLeading(), time_ns);
                            }
                            else
                            {
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib + 4, mapped->IsLeading(), time_ns);
                            }
                        }
                        else if (channel > 64 && channel < 193)
                        {
                            iFib = 64 + channel;
                            if (2 == mapped->GetSide())
                            {
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                            }
                            else
                            {
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);

                                if (channel > 123 && channel < 134)
                                {
                                    //		cout<<"Mapped2Cal orig : "<<fName<<", "<<", "<<iFib<<";
           "<<mapped->GetSide()<<",
                                    //"<<mapped->IsLeading()<<"; "<<time_ns<<endl;

                                    auto time_ghost = time_ns;
                                    auto side_ghost = 1 - mapped->GetSide();
                                    //          new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    //              R3BFiberMAPMTCalData(side_ghost,
                                    //                                   iFib,
                                    //                                   mapped->IsLeading(),
                                    //                                   time_ghost); // add ghost fiber to treat the
           hole
                                    //
                                    //      cout<<"Mapped2Cal ghost: "<<fName<<", "<<", "<<iFib<<"; "<<side_ghost<<",
                                    //      "<<mapped->IsLeading()<<"; "<<time_ghost<<endl;
                                }
                            }
                        }
                        else if (channel > 192 && channel < 257)
                        {
                            iFib = 64 + channel + int((channel - 193) / 4) * 4;
                            if (2 == mapped->GetSide())
                            {
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                                new ((*fCalTriggerItems)[fNoCalTrigItems++]) // fCalTriggerItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib + 4, mapped->IsLeading(), time_ns);
                            }
                            else
                            {
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib, mapped->IsLeading(), time_ns);
                                new ((*fCalItems)[fNoCalItems++]) // fCalItems->GetEntriesFast()])
                                    R3BFiberMAPMTCalData(mapped->GetSide(), iFib + 4, mapped->IsLeading(), time_ns);
                            }
                        }

                    } // end selection top or bottom
                    //	cout<<"CAL "<<fName<<"; "<<channel<<", "<<iFib<<", "<<mapped->GetSide()<<",
           "<<mapped->IsLeading()<<",
                    //"<<time_ns<<endl;
                } // end fibdet selection
                */
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
    //  if (fNoCalItems > 0) cout<<"cal_num in M2C !: "<<fName<<", "<<fNoCalItems<<", "<<endl;
}

void R3BFiberMAPMTMapped2Cal::FinishEvent()
{
    fCalItems->Clear();
    fCalTriggerItems->Clear();
    fNoCalItems = 0;
    fNoCalTrigItems = 0;
}

void R3BFiberMAPMTMapped2Cal::FinishTask() {}

ClassImp(R3BFiberMAPMTMapped2Cal)
