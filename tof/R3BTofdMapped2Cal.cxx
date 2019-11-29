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
// -----                  R3BTofdMapped2Cal                -----
// -----          Created Feb 4th 2016 by R.Plag          -----
// ------------------------------------------------------------

/*
 * March 2016
 * Reconstructing data into PaddleItems. Each paddle holds two times,
 * (for PM1 and PM2). This allows to plot
 * the time differences e.g. via evt->Draw(...) interactively (aka without
 * looping over all channels) which is crucial for a quick check of the
 * detector status during the experiment.
 */

#include <assert.h>

#include "R3BTofdMapped2Cal.h"

#include "TClonesArray.h"
#include "TMath.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "R3BTCalEngine.h"
#include "R3BTofdCalData.h"
#include "R3BTofdMappedData.h"

#include "R3BEventHeader.h"

#define IS_NAN(x) TMath::IsNaN(x)

namespace
{
    double const c_range_ns = 2048 * 5;
};

R3BTofdMapped2Cal::R3BTofdMapped2Cal()
    : FairTask("R3BTofdMapped2Cal", 1)
    , fMappedItems(nullptr)
    , fCalItems(new TClonesArray("R3BTofdCalData"))
    , fNofCalItems(0)
    , fTcalPar(0)
    , fNofTcalPars(0)
    , fNofPlanes(0)
    , fPaddlesPerPlane(0)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fCalLookup()
{
}

R3BTofdMapped2Cal::R3BTofdMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(nullptr)
    , fCalItems(new TClonesArray("R3BTofdCalData"))
    , fNofCalItems(0)
    , fTcalPar(0)
    , fNofTcalPars(0)
    , fNofPlanes(0)
    , fPaddlesPerPlane(0)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fCalLookup()
{
}

R3BTofdMapped2Cal::~R3BTofdMapped2Cal() { delete fCalItems; }

size_t R3BTofdMapped2Cal::GetCalLookupIndex(R3BTofdMappedData const& a_mapped) const
{
    size_t i = ((a_mapped.GetDetectorId() - 1) * fPaddlesPerPlane + (a_mapped.GetBarId() - 1)) * 2 +
               (a_mapped.GetSideId() - 1);

    //  cout<<i<<";"<<fCalLookup.size()<<"; "<<a_mapped.GetDetectorId()<<"; "<<a_mapped.GetBarId()<<endl   ;

    assert(i < fCalLookup.size());
    return i;
}

InitStatus R3BTofdMapped2Cal::Init()
{
    fNofTcalPars = fTcalPar->GetNumModulePar();
    if (fNofTcalPars == 0)
    {
        LOG(ERROR) << "There are no TCal parameters in container TofdTCalPar";
        return kFATAL;
    }

    LOG(INFO) << "R3BTofdMapped2Cal::Init : read " << fNofTcalPars << " modules";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    // get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("TofdMapped");
    if (NULL == fMappedItems)
        LOG(fatal) << "Branch TofdMapped not found";

    // request storage of Cal data in output tree
    mgr->Register("TofdCal", "Land", fCalItems, kTRUE);

    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BTofdMapped2Cal::SetParContainers()
{
    fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
    if (!fTcalPar)
    {
        LOG(ERROR) << "Could not get access to TofdTCalPar-Container.";
        fNofTcalPars = 0;
    }
}

InitStatus R3BTofdMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BTofdMapped2Cal::Exec(Option_t* option)
{
    // check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    for (auto it = fCalLookup.begin(); fCalLookup.end() != it; ++it)
    {
        auto& vec = *it;
        vec.clear();
    }

    Int_t mapped_num = fMappedItems->GetEntriesFast();

    // Calibrate time to nanoseconds.
    struct Cal
    {
        Cal(R3BTofdMappedData const* a_mapped, double a_time_ns)
            : mapped(a_mapped)
            , time_ns(a_time_ns)
        {
        }
        R3BTofdMappedData const* mapped;
        double time_ns;
    };
    std::vector<std::vector<Cal>> cal_vec(fNofPlanes * fPaddlesPerPlane * 2);
    for (Int_t mapped_i = 0; mapped_i < mapped_num; mapped_i++)
    {
        auto mapped = (R3BTofdMappedData const*)fMappedItems->At(mapped_i);

        if ((mapped->GetDetectorId() < 1) || (mapped->GetDetectorId() > fNofPlanes))
        {
            LOG(DEBUG) << "R3BTofdMapped2Cal::Exec : Plane number out of range: " << mapped->GetDetectorId();
            continue;
        }
        if ((mapped->GetBarId() < 1) || (mapped->GetBarId() > fPaddlesPerPlane))
        {
            LOG(DEBUG) << "R3BTofdMapped2Cal::Exec : Bar number out of range: " << mapped->GetBarId() << ", "
                       << fPaddlesPerPlane;
            continue;
        }

        // Tcal parameters.
        auto* par = fTcalPar->GetModuleParAt(
            mapped->GetDetectorId(), mapped->GetBarId(), 2 * mapped->GetSideId() + mapped->GetEdgeId() - 2);
        if (!par)
        {
            LOG(INFO) << "R3BTofdMapped2Cal::Exec : Tcal par not found, Plane: " << mapped->GetDetectorId()
                      << ", Bar: " << mapped->GetBarId() << ", Side: " << mapped->GetSideId()
                      << ", Edge: " << mapped->GetEdgeId();
            continue;
        }

        // Convert TDC to [ns] ...
        Double_t time_ns = par->GetTimeVFTX(mapped->GetTimeFine());
        // ... and subtract it from the next clock cycle.
        time_ns = (mapped->GetTimeCoarse() + 1) * fClockFreq - time_ns;

        auto& entry = cal_vec.at(((mapped->GetDetectorId() - 1) * fPaddlesPerPlane + mapped->GetBarId() - 1) * 2 +
                                 mapped->GetSideId() - 1);
        entry.push_back(Cal(mapped, time_ns));
    }

    // Iterate through calibrated times and match leading/trailing pairs.
    // Note that the reader saves all leading edges first, then appends the trailing.
    //
    // TODO: This algo would not properly handle e.g. double leading before a trailing,
    //       should be fixed to be future proof!
    //
    for (auto it = cal_vec.begin(); cal_vec.end() != it; ++it)
    {
        auto& ch = *it;
        if (ch.empty())
            continue;
        // for (auto it2 = ch.begin(); ch.end() != it2; ++it2) {
        // std::cout << it2->mapped->GetEdgeId() << ": " << it2->time_ns << '\n';
        //}
        size_t lead_i = 0;
        size_t trail_i;
        for (trail_i = 0; trail_i < ch.size() && 2 != ch.at(trail_i).mapped->GetEdgeId(); ++trail_i)
            ;
        // std::cout << "Trail=" << trail_i << '\n';
        for (;;)
        {
            for (; trail_i < ch.size(); ++trail_i)
            {
                auto dt = ch.at(trail_i).time_ns - ch.at(lead_i).time_ns;
                if (dt < -c_range_ns / 2)
                {
                    // Wrap-around.
                    break;
                }
                if (dt > c_range_ns / 2)
                {
                    // std::cout << lead_i << ' ' << trail_i << ": " << ch.at(lead_i).time_ns << ' ' <<
                    // ch.at(trail_i).time_ns << '\n';
                    // Missing leading edge with wrap-around at the same time.
                    continue;
                }
                if (dt > 0)
                {
                    break;
                }
            }
            if (trail_i == ch.size() || 1 != ch.at(lead_i).mapped->GetEdgeId())
            {
                break;
            }

            auto lead = ch.at(lead_i).mapped;
            auto cal = new ((*fCalItems)[fNofCalItems++]) R3BTofdCalData(lead->GetDetectorId(),
                                                                         lead->GetBarId(),
                                                                         lead->GetSideId(),
                                                                         ch.at(lead_i).time_ns,
                                                                         ch.at(trail_i).time_ns);

            ++lead_i;
            ++trail_i;
        }
    }
}

void R3BTofdMapped2Cal::FinishEvent()
{
    if (fCalItems)
    {
        fCalItems->Clear();
        fNofCalItems = 0;
    }
}

void R3BTofdMapped2Cal::FinishTask() {}

void R3BTofdMapped2Cal::SetNofModules(Int_t planes, Int_t ppp)
{
    fNofPlanes = planes;
    fPaddlesPerPlane = ppp;
    // #planes * #bars * 2, each entry pair is a full bar, i.e. merged sides +
    // edges.
    fCalLookup.resize(planes * ppp * 2);
}

ClassImp(R3BTofdMapped2Cal)
