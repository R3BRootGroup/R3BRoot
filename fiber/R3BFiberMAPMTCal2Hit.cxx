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

#include "R3BFiberMAPMTCal2Hit.h"
#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTHitPar.h"
#include "R3BTCalEngine.h"
#include "TH1F.h"
#include "TH2F.h"
#include <TClonesArray.h>
#include <cassert>

#include "R3BTofdHitData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"

#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <iostream>

R3BFiberMAPMTCal2Hit::ToT::ToT(R3BFiberMAPMTCalData const* a_lead,
                                 R3BFiberMAPMTCalData const* a_trail,
                                 Double_t a_lead_ns,
                                 Double_t a_tail_ns,
                                 Double_t a_tot_ns)
    : lead(a_lead)
    , trail(a_trail)
    , lead_ns(a_lead_ns)
    , tail_ns(a_tail_ns)
    , tot_ns(a_tot_ns)
{
}

R3BFiberMAPMTCal2Hit::R3BFiberMAPMTCal2Hit(const char* a_name,
                                               Int_t a_verbose,
                                               Direction a_direction,
                                               UInt_t a_fiber_num,
                                               )
    : FairTask(TString("R3B") + a_name + "Cal2Hit", a_verbose)
    , fName(a_name)
    , fDirection(a_direction)
		, fFiberNum(a_fiber_num)
    , fCalItems()
    , fMAPMTCalTriggerItems()
    , fHitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fCalPar()
    , fNofHitPars()
    , fNofHitItems()
    , fChannelArray()
    , fh_ToT_MA_Fib()
    , fh_ToT_ToT()
    , fh_dt_Fib()
    , fh_time_Fib()
    , fh_Fib_ToF()
    , fnEvents(0)

{
    fChPerSub[0] = a_mapmt_per_sub;
    fChPerSub[1] = a_spmt_per_sub;
}

R3BFiberMAPMTCal2Hit::~R3BFiberMAPMTCal2Hit()
{
    delete fHitItems;
    delete fCalPar;
}

InitStatus R3BFiberMAPMTCal2Hit::Init()
{
    cout << "Init:R3BFiberMAPMTCal2Hit" << endl;
    auto mgr = FairRootManager::Instance();
    if (!mgr)
    {
        LOG(ERROR) << "FairRootManager not found.";
        return kERROR;
    }

    tofdin = true;
    fTofdHitItems = (TClonesArray*)mgr->GetObject("TofdHit");
    if (!fTofdHitItems)
    {
        LOG(INFO) << "Branch TofdHit not found.";
        tofdin = false;
    }

    auto name = fName + "Cal";
    fCalItems = (TClonesArray*)mgr->GetObject(name);
    if (!fCalItems)
    {
        LOG(fatal) << "Branch " << name << " not found.";
        return kERROR;
    }
    auto name_mapmt_trig = fName + "TriggerCal";
    fMAPMTCalTriggerItems = (TClonesArray*)mgr->GetObject(name_mapmt_trig);
    if (NULL == fMAPMTCalTriggerItems)
        LOG(fatal) << "Branch " << name_mapmt_trig << " not found";

    maxevent = mgr->CheckMaxEventNo();

    mgr->Register(fName + "Hit", "Land", fHitItems, kTRUE);
    
		// Resize per-channel info arrays.
    for (auto side_i = 0; side_i < 2; ++side_i)
    {
        fChannelArray[side_i].resize(iFiberNum);
    }

    return kSUCCESS;
}

InitStatus R3BFiberMAPMTCal2Hit::ReInit() { return kSUCCESS; }

void R3BFiberMAPMTCal2Hit::SetParContainers()
{
    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCalPar = (R3BFiberMAPMTHitPar*)FairRuntimeDb::instance()->getContainer(fName + "HitPar");
    if (!fCalPar)
    {
        LOG(ERROR) << "R3BFiberCal2Hit::Init() Couldn't get " << fName << "HitPar. ";
    }
}

void R3BFiberMAPMTCal2Hit::Exec(Option_t* option)
{
    // cout<<"in exec"<<endl;
    //	if(fnEvents/10000.==(int)fnEvents/10000) cout<<"Events: "<<fnEvents<<"         \r"<<std::flush;
    multi = 0;
    if (fnEvents / 100000. == (int)fnEvents / 100000)
        std::cout << "\rEvents: " << fnEvents << " / " << maxevent << " (" << (int)(fnEvents * 100. / maxevent)
                  << " %) " << std::flush;

    for (auto side_i = 0; side_i < 2; ++side_i)
    {
        // Clear local helper containers.
        auto& array = fChannelArray[side_i];
        for (auto it = array.begin(); array.end() != it; ++it)
        {
            it->lead_list.clear();
            it->tot_list.clear();
        }
    }
    for (Int_t i = 0; i < 1025; i++)
    {
        energy[i] = 0.;
        counts[i] = 0;
    }
    //  cout<<"multihit"<<endl;

    size_t cal_num = fCalItems->GetEntriesFast();

    // Make direct mapping tables for trigger items.
    size_t mapmt_trig_num = fMAPMTCalTriggerItems->GetEntries();
    std::vector<R3BFiberMAPMTCalData const*> mapmt_trig_table(fSubNum * fChPerSub[0] / 128);
    for (size_t j = 0; j < mapmt_trig_num; ++j)
    {
        auto cal = (R3BFiberMAPMTCalData const*)fMAPMTCalTriggerItems->At(j);
        mapmt_trig_table.at(cal->GetChannel() - 1) = cal;
    }

    // TODO: This will create a map for every fiber detector... Urg.
    // Also since it's shared between many detectors it must be dynamic, for now.
    size_t spmt_trig_num = fSPMTCalTriggerItems->GetEntries();
    std::vector<R3BFiberMAPMTCalData const*> spmt_trig_table;
    for (size_t j = 0; j < spmt_trig_num; ++j)
    {
        auto cal = (R3BFiberMAPMTCalData const*)fSPMTCalTriggerItems->At(j);
        auto idx = cal->GetChannel() - 1;
        if (idx >= spmt_trig_table.size())
            spmt_trig_table.resize(idx + 1);
        spmt_trig_table.at(idx) = cal;
    }

    // Find multi-hit ToT for every channel.
    // The easiest safe way to survive ugly cases is to record all
    // leading edges per channel, and then pair up with whatever
    // trailing we have.
    // Not super efficient, but shouldn't crash if the data is not
    // perfect.
    unsigned n_lead = 0;
    unsigned n_trail = 0;
    int s_mult = 0;
    for (size_t j = 0; j < cal_num; ++j)
    {
        auto cur_cal = (R3BFiberMAPMTCalData const*)fCalItems->At(j);
        if (cur_cal->IsLeading())
        {
            ++n_lead;
            auto side_i = cur_cal->IsMAPMT() ? 0 : 1;
            auto ch_i = cur_cal->GetChannel() - 1;
            auto& channel = fChannelArray[side_i].at(ch_i);
            channel.lead_list.push_back(cur_cal);
            if (side_i == 1)
                s_mult++;
        }
        else
        {
            ++n_trail;
        }
    }
    if (n_lead != n_trail)
    {
        //    return;
    }
    Int_t summmpt = 0;
    Int_t summsm1 = 0;
    Int_t summsm2 = 0;
    //
    // puts("Event");
    // bool do_print = false;

    for (size_t j = 0; j < cal_num; ++j)
    {
        auto cur_cal = (R3BFiberMAPMTCalData const*)fCalItems->At(j);
        if (cur_cal->IsTrailing())
        {
            auto side_i = cur_cal->IsMAPMT() ? 0 : 1;
            Double_t c_period = 0 == side_i ? 4096. * (1000. / fClockFreq) : 2048. * (1000. / 200.);

            auto ch_i = cur_cal->GetChannel() - 1;
            auto& channel = fChannelArray[side_i].at(ch_i);
            if (channel.lead_list.empty())
            {
                continue;
            }
            auto lead = channel.lead_list.front();

            Double_t cur_cal_trig_ns = 0;
            Double_t lead_trig_ns = 0;
            if (cur_cal->IsMAPMT() && fMAPMTTriggerMap)
            {
                auto cur_cal_trig_i = fMAPMTTriggerMap[ch_i];
                auto lead_trig_i = fMAPMTTriggerMap[lead->GetChannel() - 1];
                // if(fName=="Fi3a") printf("3a trig curr %8u %8u lead %8u %8u  %8u\n", ch_i, cur_cal_trig_i,
                // lead->GetChannel() - 1, lead_trig_i, mapmt_trig_table.size()); if(fName=="Fi3b") printf("3b trig curr
                // %8u %8u lead %8u %8u\n", ch_i, cur_cal_trig_i, lead->GetChannel() - 1, lead_trig_i);
                if (cur_cal_trig_i < mapmt_trig_table.size() && lead_trig_i < mapmt_trig_table.size())
                {
                    auto cur_cal_trig = mapmt_trig_table.at(cur_cal_trig_i);
                    auto lead_trig = mapmt_trig_table.at(lead_trig_i);
                    cur_cal_trig_ns = cur_cal_trig->GetTime_ns();
                    lead_trig_ns = lead_trig->GetTime_ns();
                }
            }
            else if (cur_cal->IsSPMT() && fSPMTTriggerMap)
            {
                auto cur_cal_trig_i = fSPMTTriggerMap[ch_i];
                auto lead_trig_i = fSPMTTriggerMap[lead->GetChannel() - 1];
                // if(fName=="Fi3a") printf("3a trig curr %8u %8u lead %8u %8u  %8u\n", ch_i, cur_cal_trig_i,
                // lead->GetChannel() - 1, lead_trig_i, spmt_trig_table.size()); if(fName=="Fi3b") printf("3b trig curr
                // %8u %8u lead %8u %8u\n", ch_i, cur_cal_trig_i, lead->GetChannel() - 1, lead_trig_i);
                if (cur_cal_trig_i < spmt_trig_table.size() && lead_trig_i < spmt_trig_table.size())
                {
                    auto cur_cal_trig = spmt_trig_table.at(cur_cal_trig_i);
                    auto lead_trig = spmt_trig_table.at(lead_trig_i);
                    cur_cal_trig_ns = cur_cal_trig->GetTime_ns();
                    lead_trig_ns = lead_trig->GetTime_ns();
                }
            }

            auto cur_cal_ns =
                fmod(cur_cal->GetTime_ns() - cur_cal_trig_ns + c_period + c_period / 2, c_period) - c_period / 2;
            auto lead_ns = fmod(lead->GetTime_ns() - lead_trig_ns + c_period + c_period / 2, c_period) - c_period / 2;
            auto tot_ns = fmod(cur_cal_ns - lead_ns + c_period + c_period / 2, c_period) - c_period / 2;

            // if (fName=="Fi3a" && side_i==0) {
            // printf("3a curr %8.2f %8.2f\n", cur_cal->GetTime_ns(), cur_cal_trig_ns);
            // printf("3a lead %8.2f %8.2f\n", lead->GetTime_ns(), lead_trig_ns);
            //}
            //
            // if (fName=="Fi3b" && side_i==0) {
            // printf("3b curr %8.2f %8.2f\n", cur_cal->GetTime_ns(), cur_cal_trig_ns);
            // printf("3b lead %8.2f %8.2f\n", lead->GetTime_ns(), lead_trig_ns);
            //}
            // if (cur_cal->IsMAPMT() && tot_ns < 0) {
            //  printf("%8.2f %8.2f %8.2f\n", lead->GetTime_ns(), cur_cal->GetTime_ns(), tot_ns);
            //  do_print = true;
            //}

            if (tot_ns > 0 && tot_ns < 1000)
            {
                // if (side_i==0) tot_ns -= 9.;//taken out for the moment MH
                if (side_i == 1)
                    channel.tot_list.push_back(ToT(lead, cur_cal, lead_ns, cur_cal_ns, tot_ns));
                if (side_i == 0)
                    channel.tot_list.push_front(ToT(lead, cur_cal, lead_ns, cur_cal_ns, tot_ns));

                channel.lead_list.pop_front();
            }
        }
    }
    // if (do_print) for (size_t j = 0; j < cal_num; ++j) {
    // auto cur_cal = (R3BFiberMAPMTCalData const*)fCalItems->At(j);
    // if (cur_cal->IsMAPMT())
    // printf("%3u: %3u %u %f\n", j, cur_cal->GetChannel(), cur_cal->IsLeading(), cur_cal->GetTime_ns());
    //}

    double s1 = 99.;
    double s2 = 99.;
    double s3 = 99.;
    double s4 = 99.;
    int single = 0;

    double tot_mapmt_max = -1;
    double tot_spmt_max = -1;
    int tot_mapmt_max_fiber_id = 0;
    int tot_spmt_max_fiber_id = 0;
    int tofd_id = 0;
    double tof;
    double t_tofd;
    // cout<<"permutations"<<endl;

    // Make every permutation to create fibers.
    auto const& mapmt_array = fChannelArray[0];
    auto const& spmt_array = fChannelArray[1];

    for (auto it_mapmt = mapmt_array.begin(); mapmt_array.end() != it_mapmt; ++it_mapmt) // over fiber_number 0...nmax-1
    {
        auto const& mapmt = *it_mapmt;
        for (auto it_mapmt_tot = mapmt.tot_list.begin(); mapmt.tot_list.end() != it_mapmt_tot;
             ++it_mapmt_tot) // over ihit(fiber)
        {
            auto const& mapmt_tot = *it_mapmt_tot;
            // s             for (auto it_spmt = spmt_array.begin(); spmt_array.end() != it_spmt; ++it_spmt)
            // s             {
            // s                auto const& spmt = *it_spmt;
            // s                for (auto it_spmt_tot = spmt.tot_list.begin(); spmt.tot_list.end() != it_spmt_tot;
            // ++it_spmt_tot) s                { s                    auto const& spmt_tot = *it_spmt_tot;

            // Check that the combo is inside one sub-det.
            auto mapmt_sub_id = (mapmt_tot.lead->GetChannel() - 1) / fChPerSub[0];
            // s                    auto spmt_sub_id = (spmt_tot.lead->GetChannel() - 1) / fChPerSub[1];
            // s                    if (mapmt_sub_id != spmt_sub_id)
            // s                    {
            // s                        continue;
            // s                    }

            /*
             * How to get a fiber ID for a fiber detector defined as:
             *  SubNum = 2
             *  MAPMT = 256
             *  SPMT = 2
             * This means we'll receive 512 MAPMT channels as 1..512, and 4 SPMT
             * channels, but the first half (sub-detector) is completely
             * decoupled from the second half. The sequence of all fibers in
             * order is then, as (MAPMT,SPMT)-pairs:
             *  (1,1), (1,2), (2,1), ... (256,1), (256,2),
             *  (257,3), (257,4), (258,3), ... (512,3), (512,4)
             */

            auto fiber_id = mapmt_tot.lead->GetChannel();
            // s					auto fiber_id = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1]
            // s                                                + ((spmt_tot.lead->GetChannel() - 1) % fChPerSub[1]) +
            // 1;

            // auto fiber_id_ch = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] + 1;
            // s                    single = spmt_tot.lead->GetChannel();

            // TODO: Use it_sub->direction to find real life coordinates.

            // Fix fiber installation mistakes.
            fiber_id = FixMistake(fiber_id);

            // Calibrate hit fiber.
            auto tot_mapmt = mapmt_tot.tot_ns;
            // s                    auto tot_spmt = spmt_tot.tot_ns;
            Double_t t_mapmt = mapmt_tot.lead_ns;

            Double_t t_mapmt1 = mapmt_tot.lead->GetTime_ns(); // MAPMT time without subtraction of trigger time

            // s                    Double_t t_spmt = spmt_tot.lead_ns;
            // only accept hits which are at the right time:
            Bool_t simu = true;
            if (!simu)
            {
                if (fName == "Fi3a" || fName == "Fi3b")
                {
                    // s                            if (t_spmt < -400 || t_spmt > -270)
                    // s                                continue;
                    if (t_mapmt < -550 || t_mapmt > -350)
                        continue;
                }
                if (fName == "Fi10" || fName == "Fi11" || fName == "Fi12" || fName == "Fi13")
                {
                    // s                            if (t_spmt < -300 || t_spmt > -160)
                    // s                                continue;
                    if (t_mapmt < -500 || t_mapmt > -300)
                        continue;
                }
            }

            fh_time_Fib->Fill(fiber_id, t_mapmt);
            // s                    fh_time_s_Fib->Fill(single, t_spmt);

            // cout << fName << " tS " << t_spmt << " tM: " << t_mapmt << " totS: " <<
            //	tot_spmt << " totM: " << tot_mapmt << endl;

            // Apply calibration.
            Double_t gainMA = gain_temp[fiber_id - 1]; // 10.;
            Double_t gainS = 10.;
            Double_t offset1 = 0.;
            Double_t offset2 = 0.;
            Double_t tsync = tsync_temp[fiber_id - 1]; // 0.;

            //  cout<<"before "<<fiber_id<<", "<<tsync<<"  "<<gainMA<<", "<<endl;

            if (!fIsCalibrator && fHitPar)
            {
                R3BFiberMAPMTHitModulePar* par = fHitPar->GetModuleParAt(fiber_id);
                if (par)
                {
                    gainMA = par->GetGainMA();
                    tsync = par->GetSync();
                    // gainS = par->GetGainS();
                    offset1 = par->GetOffset1();
                    offset2 = par->GetOffset2();
                }
            }
            // cout<<"offset "<<offset1<<"  "<<offset2<<endl;
            // s we do not use single PMT, therefore no offset needed
            offset1 = 0.;
            offset2 = 0.;

            tot_mapmt *= 10. / gainMA;
            // s                    tot_spmt *= 10. / gainS;
            t_mapmt += offset1;
            // s                    t_spmt += offset2;
            // s                    tof = (t_mapmt + t_spmt) / 2.;
            tof = t_mapmt;
            tof -= tsync;

            // histogram for offset determination
            // s                    fh_dt_Fib->Fill(fiber_id, t_spmt - t_mapmt);

            // Fill histograms for gain match, and for debugging.
            fh_Fib_ToF->Fill(fiber_id, tof);
            fh_ToT_MA_Fib->Fill(fiber_id, tot_mapmt);
            // s                    if (s_mult > 0)
            // s                    {
            // s                        fh_ToT_Single_Fib->Fill(fiber_id, tot_spmt);
            // s                        fh_ToT_s_Fib[single - 1]->Fill(fiber_id, tot_spmt);
            // s                    }

            // Int_t numFibs = fSubNum * fChPerSub[0];
            Int_t numFibs = fSubNum * fChPerSub[0] * fChPerSub[1];
            Double_t x = -10000.;
            Double_t y = -10000.;
            Double_t randx;
            Double_t randy;

            if (fName == "Fi10" || fName == "Fi11" || fName == "Fi12" || fName == "Fi13")
            {
                Float_t fiber_thickness = 0.050000 * 2.; // s remove *2 when taking SPMT into analysis
                Int_t fiber_nbr = 1024 / 2;              // s remove /2 when taking SPMT into analysis
                Float_t dead_layer = 0.9;
                Float_t air_layer = 0.01; // relative to fiber_thickness
                Float_t detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);

                if (fDirection == VERTICAL)
                {
                    x = -detector_width / 2. + fiber_thickness / 2. +
                        ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                    // s                            y = (t_spmt - t_mapmt) * 3.;
                    y = 0.;
                }
                else
                {
                    // S                            x = (t_spmt - t_mapmt) * 3.;
                    x = 0.;
                    y = -detector_width / 2. + fiber_thickness / 2. +
                        ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                }
                if (x > 26.)
                    cout << "fiber_id " << fiber_id << " x " << x << " y " << y << endl;
            }
            if (fName == "Fi1a" || fName == "Fi1b" || fName == "Fi2a" || fName == "Fi2b" || fName == "Fi3a" ||
                fName == "Fi3b")
            {
                Float_t fiber_thickness = 0.021000 * 2.; // s remove *2 when taking SPMT into analysis
                Int_t fiber_nbr = 512 / 2;               // s remove /2 when taking SPMT into analysis
                Float_t dead_layer = 0.9;
                Float_t air_layer = 0.01; // relative to fiber_thickness
                Float_t detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);
                if (fDirection == VERTICAL)
                {
                    x = -detector_width / 2. + fiber_thickness / 2. +
                        ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                    // s                            y = (t_spmt - t_mapmt) * 3.;
                    y = 0.;
                }
                else
                {
                    // s                            x = (t_spmt - t_mapmt) * 3.;
                    x = 0.;
                    y = -detector_width / 2. + fiber_thickness / 2. +
                        ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                }
            }
            // cout<<"Fiber y " << y << endl;
            if (y < -60 || y > 60)
            {
                // continue;
            }
            if (tof < -20 || tof > 20)
            {
                // continue;
            }
            // Double_t eloss = sqrt(tot_mapmt * tot_spmt);
            Double_t eloss = tot_mapmt;
            // Double_t t = (t_mapmt + t_spmt) / 2.;
            // Double_t t = t_spmt;
            // Double_t t = t_mapmt;
            Double_t t = tof;

            energy[fiber_id] = eloss;
            counts[fiber_id] = counts[fiber_id] + 1;
            multi++;

            // if(fiber_id>256) cout << "save fiber_id " << fiber_id << " pos " << x << endl;
            if (!fIsCalibrator)
                new ((*fHitItems)[fNofHitItems++])
                    // s                            R3BFiberMAPMTHitData(0, x, y, eloss, t, fiber_id, t_mapmt, t_spmt,
                    // tot_mapmt, tot_spmt);
                    R3BFiberMAPMTHitData(0, x, y, eloss, t, fiber_id, t_mapmt1, 0., tot_mapmt, 0.);
            // s                }
            // s            }
        }
    }
    fh_ToT_ToT->Fill(s1, s2);
    fnEvents++;

    // cout<<"end exec"<<endl;
}

void R3BFiberMAPMTCal2Hit::FinishEvent()
{
    fHitItems->Clear();
    fNofHitItems = 0;
}

void R3BFiberMAPMTCal2Hit::FinishTask()
{
    fh_ToT_MA_Fib->Write();
    fh_ToT_Single_Fib->Write();
    fh_dt_Fib->Write();
    fh_time_Fib->Write();
    fh_Fib_ToF->Write();
    fh_Test->Write();
    fh_time_s_Fib->Write();

    /*

        fh_multi->Write();
    */

    for (Int_t i = 0; i < 4; i++)
    {
        fh_ToT_s_Fib[i]->Write();
    }
    fh_ToT_ToT->Write();

    if (fIsCalibrator)
    {

        R3BFiberMAPMTHitModulePar* mpar;

        UInt_t max = N_FIBER_MAX;
        if (fh_ToT_MA_Fib->GetNbinsX() < N_FIBER_MAX)
            max = fh_ToT_MA_Fib->GetNbinsX();

        for (UInt_t i = 1; i <= max; i++)
        {
            mpar = new R3BFiberMAPMTHitModulePar();
            mpar->SetFiber(i);
            fCalPar->AddModulePar(mpar);
        }

        // tsync
        if (fIsTsync)
        {
            for (UInt_t i = 1; i <= max; i++)
            {
                // R3BFiberMAPMTHitModulePar* par = fCalPar->GetModuleParAt(i);

                if (!fIsGain)
                {
                    R3BFiberMAPMTHitModulePar* par = fCalPar->GetModuleParAt(i);
                    par->SetGainMA(gain_temp[i - 1]);
                }
                R3BFiberMAPMTHitModulePar* par1 = fCalPar->GetModuleParAt(i);
                TH1D* proj = fh_Fib_ToF->ProjectionY("", i + 1, i + 1, 0);
                par1->SetSync(proj->GetBinCenter(proj->GetMaximumBin()));

                cout << "MA fiber: " << fName << ", " << i << " tsync: " << proj->GetBinCenter(proj->GetMaximumBin())
                     << ", " << par1->GetSync() << ", " << par1->GetGainMA() << endl;
            }
        }
        // time offset

        for (UInt_t i = 1; i <= max; i++)
        {
            TH1D* proj = fh_dt_Fib->ProjectionY("", i + 1, i + 1, 0);
            R3BFiberMAPMTHitModulePar* par = fCalPar->GetModuleParAt(i);
            par->SetOffset1(0.5 * proj->GetBinCenter(proj->GetMaximumBin()));
            par->SetOffset2(-0.5 * proj->GetBinCenter(proj->GetMaximumBin()));

            cout << "MA fiber: " << i << " offset: " << 0.5 * proj->GetBinCenter(proj->GetMaximumBin()) << endl;
        }

        if (fIsGain)
        {
            for (UInt_t i = 1; i <= max; i++)
            {
                TH1D* proj = fh_ToT_MA_Fib->ProjectionY("", i + 1, i + 1, 0);
                for (UInt_t j = proj->GetNbinsX() - 2; j > 2; j--)
                {
                    if (j == 2)
                    {
                        // could not find maximum
                    }
                    if (proj->GetBinContent(j) > proj->GetMaximum() / 100.)
                    {
                        R3BFiberMAPMTHitModulePar* par = fCalPar->GetModuleParAt(i);
                        par->SetGainMA(proj->GetBinCenter(j));
                        cout << "MA fiber: " << i << " par: " << proj->GetBinCenter(j) << endl;
                        // par->SetGainMA(j - 1);
                        //	cout << "MA fiber: " << i << " par: " << proj->GetBinCenter(j) << endl;
                        break;
                    }
                }
            }
        }
        /* MH
                for (UInt_t i = 1; i <= max; i++)
                {
                    TH1D* proj = fh_ToT_Single_Fib->ProjectionY("", i + 1, i + 1, 0);
                    for (UInt_t j = proj->GetNbinsX() - 2; j > 2; j--)
                    {
                        if (j == 2)
                        {
                            // could not find maximum
                        }

                        if (proj->GetBinContent(j) > proj->GetMaximum() / 10.)
                        {
                            R3BFiberMAPMTHitModulePar* par = fCalPar->GetModuleParAt(i);
                            par->SetGainS(proj->GetBinCenter(j));
                            // cout<<"S fiber: "<< i<<" par: "<<proj->GetBinCenter(j)<<endl;
                            // par->SetGainS(j - 1);
                            cout << "S fiber: " << i << " par: " << proj->GetBinCenter(j) << endl;
                            break;
                        }
                    }
                }
        */
        fCalPar->setChanged();
    }
}

void R3BFiberMAPMTCal2Hit::MAPMTTriggerMapSet(unsigned const* a_map, size_t a_map_bytes)
{
    size_t const c_length = a_map_bytes / sizeof *a_map;
    if (c_length != fSubNum * fChPerSub[0])
    {
        LOG(fatal) << "MAPMT trigger map length=" << c_length << " not compatible with sub#=" << fSubNum
                   << " mapmt_per_sub=" << fChPerSub[0] << "!";
    }
    fMAPMTTriggerMap = a_map;
}

void R3BFiberMAPMTCal2Hit::SPMTTriggerMapSet(unsigned const* a_map, size_t a_map_bytes)
{
    size_t const c_length = a_map_bytes / sizeof *a_map;
    if (c_length != fSubNum * fChPerSub[1])
    {
        LOG(fatal) << "SPMT trigger map length=" << c_length << " not compatible with sub#=" << fSubNum
                   << " spmt_per_sub=" << fChPerSub[1] << "!";
    }
    fSPMTTriggerMap = a_map;
}

ClassImp(R3BFiberMAPMTCal2Hit)
