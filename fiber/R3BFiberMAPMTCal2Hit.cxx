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

#include "mapping_fib23a_trig.hh"
#include "mapping_fib23b_trig.hh"
#include "mapping_fib30_trig.hh"
#include "mapping_fib31_trig.hh"
#include "mapping_fib32_trig.hh"
#include "mapping_fib33_trig.hh"

#include "R3BFiberMAPMTCal2Hit.h"
#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
//#include "R3BFiberMAPMTHitPar.h"
#include "TH1F.h"
#include "TH2F.h"
#include <TClonesArray.h>
#include <cassert>

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"

#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <iostream>

void R3BFiberMAPMTCal2Hit::Fib_Hit::Reset()
{
    for (int i = 0; i < 2; ++i)
    {
        for (int k = 0; k < 2; ++k)
        {
            time[i][k] = -9999;
        }
    }
    channel = -9999;
    return;
}

R3BFiberMAPMTCal2Hit::R3BFiberMAPMTCal2Hit(const char* a_name,
                                           Int_t a_verbose,
                                           // Direction a_direction,
                                           UInt_t a_num_fibers)
    : FairTask(TString("R3B") + a_name + "Cal2Hit", a_verbose)
    , fName(a_name)
    , fClockFreq(150.)
    //, fDirection(a_direction)
    , fNumFibers(a_num_fibers)
    , fCalItems()
    , fCalTriggerItems()
    , fHitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    //, fCalPar()
    //, fNofHitPars()
    , fNofHitItems()
    //, fChannelArray()
    , fnEvents(0)
    , fGate_ns(100.) // make it constructor parameter
{
    // Trigger mappin from included mapping files for each det.
    if (fName == "Fi30")
    {
        fib30_trig_map_setup();
        fTriggerMap[0] = g_fib30_trig_map[0];
        fTriggerMap[1] = g_fib30_trig_map[1];
    }
    if (fName == "Fi31")
    {
        fib31_trig_map_setup();
        fTriggerMap[0] = g_fib31_trig_map[0];
        fTriggerMap[1] = g_fib31_trig_map[1];
    }

    if (fName == "Fi32")
    {
        fib32_trig_map_setup();
        fTriggerMap[0] = g_fib32_trig_map[0];
        fTriggerMap[1] = g_fib32_trig_map[1];
    }
    if (fName == "Fi33")
    {
        fib33_trig_map_setup();
        fTriggerMap[0] = g_fib33_trig_map[0];
        fTriggerMap[1] = g_fib33_trig_map[1];
    }
    if (fName == "Fi23a")
    {
        fib23a_trig_map_setup();
        fTriggerMap[0] = g_fib23a_trig_map[0];
        fTriggerMap[1] = g_fib23a_trig_map[1];
    }
    if (fName == "Fi23b")
    {
        fib23b_trig_map_setup();
        fTriggerMap[0] = g_fib23b_trig_map[0];
        fTriggerMap[1] = g_fib23b_trig_map[1];
    }
}

R3BFiberMAPMTCal2Hit::~R3BFiberMAPMTCal2Hit()
{
    delete fHitItems;
    // delete fCalPar;
}

InitStatus R3BFiberMAPMTCal2Hit::Init()
{
    std::cout << "Init:R3BFiberMAPMTCal2Hit" << std::endl;
    auto mgr = FairRootManager::Instance();
    if (!mgr)
    {
        LOG(ERROR) << "FairRootManager not found.";
        return kERROR;
    }

    auto name = fName + "Cal";
    fCalItems = (TClonesArray*)mgr->GetObject(name);
    if (!fCalItems)
    {
        LOG(fatal) << "Branch " << name << " not found.";
        return kERROR;
    }
    auto name_mapmt_trig = fName + "TriggerCal";
    fCalTriggerItems = (TClonesArray*)mgr->GetObject(name_mapmt_trig);
    if (NULL == fCalTriggerItems)
        LOG(fatal) << "Branch " << name_mapmt_trig << " not found";

    maxevent = mgr->CheckMaxEventNo();

    // TClones branch with Hit items
    mgr->Register(fName + "Hit", "Land", fHitItems, kTRUE);

    // Resize per-channel info arrays.
    // for (auto side_i = 0; side_i < 2; ++side_i)
    //{
    //	fChannelArray[side_i].resize(fNumFibers);
    //}
    return kSUCCESS;
}

InitStatus R3BFiberMAPMTCal2Hit::ReInit() { return kSUCCESS; }

void R3BFiberMAPMTCal2Hit::SetParContainers()
{
    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakelists.txt (in this case in the tof directory)
    // fCalPar = (R3BFiberMAPMTHitPar*)FairRuntimeDb::instance()->getContainer(fName + "HitPar");
    // if (!fCalPar)
    //{
    //    LOG(ERROR) << "R3BFiberCal2Hit::Init() Couldn't get " << fName << "HitPar. ";
    //}
}

void R3BFiberMAPMTCal2Hit::Exec(Option_t* option)
{
    UInt_t cal_num = fCalItems->GetEntriesFast();
    std::cout << "\n\n------ NEW EVENT ------";
    std::cout << "\nTotal Cal items: " << cal_num;
    std::cout << "\nSide"
              << "\tEdge"
              << "\tChannel"
              << "\tTime\n";
    Fib_Hit fhit;
    int ch, edge, side;
    Double_t time;
    std::vector<Fib_Hit> hit_array[520]; // array of multihits in each fiber
    std::vector<int> ch_vec;
    R3BFiberMAPMTCalData* cur_cal;
    for (UInt_t j = 0; j < cal_num; ++j)
    {
        cur_cal = (R3BFiberMAPMTCalData*)fCalItems->At(j);
        side = cur_cal->GetSide();
        edge = cur_cal->IsLeading() ? 0 : 1;
        time = cur_cal->GetTime_ns();
        ch = cur_cal->GetChannel();
        std::cout << "\n" << side << "\t" << edge << "\t" << ch << "\t" << time;
        fhit.Reset();
        fhit.channel = ch;
        fhit.time[side][edge] = time;

        // auto cur_cal_trig_i = fMAPMTTriggerMap[ch_i];
        // auto lead_trig_i = fMAPMTTriggerMap[lead->GetChannel() - 1];

        if (hit_array[ch].size() == 0) // If no hits exisit yet for this fiber
        {
            hit_array[ch].push_back(fhit);
            ch_vec.push_back(ch); // remember fiber number
            continue;
        }
        for (auto& multihit : hit_array[ch]) // Check existing hits
        {
            if (multihit.time[side][edge] >= 0) // the time already exists
            {
                hit_array[ch].push_back(fhit); // add new multihit for this fiber
                break;
            }
            else // otherwise the time does not exist for:
            {
                if (edge == 0 &&                                          // leading timei, and
                    (multihit.time[1 - side][0] < 0 ||                    // no lead on the other side or
                     fabs(multihit.time[1 - side][0] - time) < fGate_ns)) // coinc with other side lead
                {
                    multihit.time[side][edge] = time;
                    break;
                }
                else if (edge == 1 && (multihit.time[side][0] < time || // trail, and lead<trail on the same side or
                                       (multihit.time[side][0] - time) <
                                           (4906 * 1000. / 150 / 2.))) // when trailing is wrapped around tdc range
                {
                    multihit.time[side][edge] = time;
                    break;
                }
                else
                {
                    hit_array[ch].push_back(fhit); // add multihit in this fiber
                    break;
                }
            } // else non existing
        }     // end multihit loop
    }         // end of cal items loop

    double trig_times[8];
    //------ Collecting cal trigger hits --------
    cal_num = fCalTriggerItems->GetEntriesFast();
    Double_t tl, tt; // lead and trile times of the trigger
    for (UInt_t j = 0; j < cal_num; ++j)
    {
        cur_cal = (R3BFiberMAPMTCalData*)fCalTriggerItems->At(j);
        ch = cur_cal->GetChannel();
        tl = cur_cal->GetTime_ns();
        trig_times[ch] = tl;
    }

    std::cout << "\n\t ----- Collected hits: ";
    for (auto& f : ch_vec)
    {
        std::cout << "\n\tFiber" << f << ":";
        for (auto& h : hit_array[f])
        {
            std::cout << "\n\t\tT (b-l)" << h.time[0][0];
            std::cout << "\t\tT (b-t)" << h.time[0][1];
            std::cout << "\t\tT (t-l)" << h.time[1][0];
            std::cout << "\t\tT (t-t)" << h.time[1][1];
        }
    }

    Double_t c_period = 4096. * (1000. / fClockFreq);

    // Filling TClonesArray with hit items
    for (auto& f : ch_vec)
    {
        for (auto& h : hit_array[f])
        {
            if (h.time[0][0] >= 0 && h.time[0][1] >= 0 && h.time[1][0] >= 0 &&
                h.time[1][1] >= 0) // all goood times in a fiiber
            {
                // Bottom
                // auto lead_ns = fmod(h.time[0][0] - trig_time[fTriggerMap[0][h.channel]] + c_period + c_period / 2,
                // c_period) - c_period / 2;//time bottom
                /// auto tot_ns = fmod(h.time[0][1] - h.time[0][0] + c_period + c_period / 2, c_period) - c_period /
                /// 2;//energy bottom

                // Repear the same for the top

                new ((*fHitItems)[fNofHitItems++])
                    // R3BFiberMAPMTHitData(0, x, y, eloss, t, fiber_id, t_mapmt1, 0., tot_mapmt, 0.);
                    R3BFiberMAPMTHitData(0, 0, 0, 0, 0, f, 0, 0., 0, 0.);
            }
        }
        hit_array[f].clear();
    }
    ch_vec.clear();

    //	//auto side_i = cur_cal->IsMAPMT() ? 0 : 1;
    //	//Double_t c_period = 0 == side_i ? 4096. * (1000. / fClockFreq) : 2048. * (1000. / 200.);
    //	//auto cur_cal_ns =
    //	//   fmod(cur_cal->GetTime_ns() - cur_cal_trig_ns + c_period + c_period / 2, c_period) - c_period / 2;
    //	//auto lead_ns = fmod(lead->GetTime_ns() - lead_trig_ns + c_period + c_period / 2, c_period) - c_period / 2;
    //	//auto tot_ns = fmod(cur_cal_ns - lead_ns + c_period + c_period / 2, c_period) - c_period / 2;
    //
    //	if (tot_ns > 0 && tot_ns < 1000)
    //	{
    //		if (side_i == 1)
    //			channel.tot_list.push_back(ToT(lead, cur_cal, lead_ns, cur_cal_ns, tot_ns));
    //		if (side_i == 0)
    //			channel.tot_list.push_front(ToT(lead, cur_cal, lead_ns, cur_cal_ns, tot_ns));
    //
    //		channel.lead_list.pop_front();
    //	}
    //
    //	if (fName == "Fi10" || fName == "Fi11" || fName == "Fi12" || fName == "Fi13")
    //	{
    //		Float_t fiber_thickness = 0.050000 * 2.; // s remove *2 when taking SPMT into analysis
    //		Int_t fiber_nbr = 1024 / 2;              // s remove /2 when taking SPMT into analysis
    //		Float_t dead_layer = 0.9;
    //		Float_t air_layer = 0.01; // relative to fiber_thickness
    //		Float_t detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);
    //
    //		if (fDirection == VERTICAL)
    //		{
    //			x = -detector_width / 2. + fiber_thickness / 2. +
    //				((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
    //			y = 0.;
    //		}
    //		else
    //		{
    //			x = 0.;
    //			y = -detector_width / 2. + fiber_thickness / 2. +
    //				((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
    //		}
    //		if (x > 26.)
    //			std::cout << "fiber_id " << fiber_id << " x " << x << " y " << y << std::endl;
    //	}
    //	Double_t eloss = tot_mapmt;
    //	Double_t t = tof;
    //	energy[fiber_id] = eloss;
    //	counts[fiber_id] = counts[fiber_id] + 1;
    //	multi++;
    //
    //	if (!fIsCalibrator)
    //		new ((*fHitItems)[fNofHitItems++])
    //			R3BFiberMAPMTHitData(0, x, y, eloss, t, fiber_id, t_mapmt1, 0., tot_mapmt, 0.);
    //	fnEvents++;
}

void R3BFiberMAPMTCal2Hit::FinishEvent()
{
    fHitItems->Clear();
    fNofHitItems = 0;
}

void R3BFiberMAPMTCal2Hit::FinishTask() {}

ClassImp(R3BFiberMAPMTCal2Hit)
