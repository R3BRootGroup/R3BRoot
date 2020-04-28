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

#include "R3BBunchedFiberCal2Hit.h"
#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberHitPar.h"
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

R3BBunchedFiberCal2Hit::ToT::ToT(R3BBunchedFiberCalData const* a_lead,
                                 R3BBunchedFiberCalData const* a_trail,
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

R3BBunchedFiberCal2Hit::R3BBunchedFiberCal2Hit(const char* a_name,
                                               Int_t a_verbose,
                                               enum R3BTCalEngine::CTDCVariant a_ctdc_variant,
                                               Direction a_direction,
                                               UInt_t a_sub_num,
                                               UInt_t a_mapmt_per_sub,
                                               UInt_t a_spmt_per_sub,
                                               Bool_t a_is_calibrator,
                                               Bool_t a_is_gain,
                                               Bool_t a_is_tsync)
    : FairTask(TString("R3B") + a_name + "Cal2Hit", a_verbose)
    , fName(a_name)
    , fClockFreq(R3BTCalEngine::CTDC_16_BWD_150 == a_ctdc_variant ? 150 : 250)
    , fDirection(a_direction)
    , fSubNum(a_sub_num)
    , fIsCalibrator(a_is_calibrator)
    , fIsGain(a_is_gain)
    , fIsTsync(a_is_tsync)
    , fCalItems()
    , fMAPMTCalTriggerItems()
    , fSPMTCalTriggerItems()
    , fHitItems(new TClonesArray("R3BBunchedFiberHitData"))
    , fCalPar()
    , fHitPar()
    , fNofHitPars()
    , fNofHitItems()
    , fChannelArray()
    , fh_ToT_MA_Fib()
    , fh_ToT_Single_Fib()
    , fh_ToT_s_Fib()
    , fh_time_s_Fib()
    , fh_ToT_ToT()
    , fh_dt_Fib()
    , fh_time_Fib()
    , fh_Fib_ToF()
    , fnEvents(0)

{
    fChPerSub[0] = a_mapmt_per_sub;
    fChPerSub[1] = a_spmt_per_sub;
}

R3BBunchedFiberCal2Hit::~R3BBunchedFiberCal2Hit()
{
    delete fHitItems;
    delete fCalPar;
}

InitStatus R3BBunchedFiberCal2Hit::Init()
{
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

    auto name_spmt_trig = "BunchedFiberSPMTTrigCal";
    fSPMTCalTriggerItems = (TClonesArray*)mgr->GetObject(name_spmt_trig);
    if (NULL == fSPMTCalTriggerItems)
        LOG(fatal) << "Branch " << name_spmt_trig << " not found";

    maxevent = mgr->CheckMaxEventNo();

    mgr->Register(fName + "Hit", "Land", fHitItems, kTRUE);
    // Resize per-channel info arrays.
    for (auto side_i = 0; side_i < 2; ++side_i)
    {
        fChannelArray[side_i].resize(fSubNum * fChPerSub[side_i]);
    }

    //    if (!fIsCalibrator )//|| (fIsCalibrator && (!fIsGain || !fIsTsync)) )
    //    {
    // Get calibration parameters if we're not a calibrator.
    auto container = fName + "HitPar";
    fHitPar = (R3BBunchedFiberHitPar*)FairRuntimeDb::instance()->getContainer(container);

    if (!fHitPar)
    {
        LOG(ERROR) << "Could not get " << container << " container.";
        fNofHitPars = 0;
    }
    else
    {
        // if(!fIsCalibrator)
        fNofHitPars = fHitPar->GetNumModulePar();
        if (0 == fNofHitPars)
        {
            LOG(ERROR) << "No Hit parameters in " << container << " container.";
            fHitPar = nullptr;
        }
    }

    for (int i = 1; i <= N_FIBER_MAX; i++)
    {
        gain_temp[i - 1] = 10.;
        tsync_temp[i - 1] = 0.;
    }

    if (fHitPar)
    {
        for (int i = 1; i <= N_FIBER_MAX; i++)
        {
            if (!fIsGain && fIsTsync) // gain already made, tsync will be done now
            {
                R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(i);
                if (par && par->GetGainMA() > 0.)
                    gain_temp[i - 1] = par->GetGainMA();
            }
            if (fIsGain && !fIsTsync) // tsync already made, gain will be done now
            {
                R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(i);
                if (par)
                    tsync_temp[i - 1] = par->GetSync();
            }
        }
    }

    //    }
    // create histograms
    TString chistName;
    TString chistTitle;
    // ToT MAPMT:
    chistName = fName + "_ToT_MAPMT";
    chistTitle = fName + " ToT of fibers";
    //    fh_ToT_MA_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 2100, 0., 2100., 100, 0., 41.666667);
    fh_ToT_MA_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 1000, 0., 400.);
    fh_ToT_MA_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_MA_Fib->GetYaxis()->SetTitle("ToT / ns");

    // ToF Tofd -> Fiber:
    chistName = fName + "_tof";
    chistTitle = fName + " ToF to ToFD";
    fh_Fib_ToF = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 44000, -1100., 1100.);
    fh_Fib_ToF->GetYaxis()->SetTitle("ToF / ns");
    fh_Fib_ToF->GetXaxis()->SetTitle("Fiber ID");

    // Test:
    chistName = fName + "_test";
    chistTitle = fName + " Tsync test";
    fh_Test = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 50, -1024., 1024.);
    fh_Test->GetYaxis()->SetTitle("Tsync / ns");
    fh_Test->GetXaxis()->SetTitle("Fiber ID");

    // ToT single PMT:
    chistName = fName + "_ToT_SAPMT";
    chistTitle = fName + " ToT of fibers";
    fh_ToT_Single_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 100, 0., 100.);
    fh_ToT_Single_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_Single_Fib->GetYaxis()->SetTitle("ToT / ns");

    // ToT SAPMT:
    for (Int_t i = 0; i < 4; i++)
    {
        char number[15];
        snprintf(number, sizeof(number), "%d", i);
        chistName = fName + "_ToT_SAPMT" + number;
        chistTitle = fName + " ToT of single PMTs " + number;
        fh_ToT_s_Fib[i] = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 100, 0., 100.);
        fh_ToT_s_Fib[i]->GetXaxis()->SetTitle("Fiber number");
        fh_ToT_s_Fib[i]->GetYaxis()->SetTitle("ToT / ns");
    }

    chistName = fName + "_time_SAPMT";
    chistTitle = fName + " time of single PMTs ";
    fh_time_s_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 6, 0, 6, 4000, -2000., 2000.);
    fh_time_s_Fib->GetXaxis()->SetTitle("single number");
    fh_time_s_Fib->GetYaxis()->SetTitle("time / ns");

    // ToT vs ToT SPMT:
    chistName = fName + "_ToT_ToT";
    chistTitle = fName + " ToT vs ToT of single PMTs";
    fh_ToT_ToT = new TH2F(chistName.Data(), chistTitle.Data(), 1000, 0., 100., 1000, 0., 100.);
    fh_ToT_ToT->GetXaxis()->SetTitle("SPMT 1");
    fh_ToT_ToT->GetYaxis()->SetTitle("SPMT 2");

    // time difference SPMT - MAPMT:
    chistName = fName + "_dt";
    chistTitle = fName + " dt of fibers";
    fh_dt_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 1000, -5000., 5000.);
    fh_dt_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_dt_Fib->GetYaxis()->SetTitle("dt / ns");

    // time of MAPMT:
    chistName = fName + "_time";
    chistTitle = fName + " time of fibers";
    fh_time_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 4000, -2000., 2000.);
    fh_time_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_time_Fib->GetYaxis()->SetTitle("time / ns");

    return kSUCCESS;
}

InitStatus R3BBunchedFiberCal2Hit::ReInit() { return kSUCCESS; }

void R3BBunchedFiberCal2Hit::SetParContainers()
{
    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCalPar = (R3BBunchedFiberHitPar*)FairRuntimeDb::instance()->getContainer(fName + "HitPar");
    if (!fCalPar)
    {
        LOG(ERROR) << "R3BFiberCal2Hit::Init() Couldn't get " << fName << "HitPar. ";
    }
}

void R3BBunchedFiberCal2Hit::Exec(Option_t* option)
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
    std::vector<R3BBunchedFiberCalData const*> mapmt_trig_table(fSubNum * fChPerSub[0] / 128);
    for (size_t j = 0; j < mapmt_trig_num; ++j)
    {
        auto cal = (R3BBunchedFiberCalData const*)fMAPMTCalTriggerItems->At(j);
        mapmt_trig_table.at(cal->GetChannel() - 1) = cal;
    }

    // TODO: This will create a map for every fiber detector... Urg.
    // Also since it's shared between many detectors it must be dynamic, for now.
    size_t spmt_trig_num = fSPMTCalTriggerItems->GetEntries();
    std::vector<R3BBunchedFiberCalData const*> spmt_trig_table;
    for (size_t j = 0; j < spmt_trig_num; ++j)
    {
        auto cal = (R3BBunchedFiberCalData const*)fSPMTCalTriggerItems->At(j);
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
        auto cur_cal = (R3BBunchedFiberCalData const*)fCalItems->At(j);
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
        auto cur_cal = (R3BBunchedFiberCalData const*)fCalItems->At(j);
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
    // auto cur_cal = (R3BBunchedFiberCalData const*)fCalItems->At(j);
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
            for (auto it_spmt = spmt_array.begin(); spmt_array.end() != it_spmt; ++it_spmt)
            {
                auto const& spmt = *it_spmt;
                for (auto it_spmt_tot = spmt.tot_list.begin(); spmt.tot_list.end() != it_spmt_tot; ++it_spmt_tot)
                {
                    auto const& spmt_tot = *it_spmt_tot;

                    // Check that the combo is inside one sub-det.
                    auto mapmt_sub_id = (mapmt_tot.lead->GetChannel() - 1) / fChPerSub[0];
                    auto spmt_sub_id = (spmt_tot.lead->GetChannel() - 1) / fChPerSub[1];
                    if (mapmt_sub_id != spmt_sub_id)
                    {
                        continue;
                    }

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

                    // auto fiber_id = mapmt_tot.lead->GetChannel();
                    auto fiber_id = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] +
                                    ((spmt_tot.lead->GetChannel() - 1) % fChPerSub[1]) + 1;

                    auto fiber_id_ch = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] + 1;
                    single = spmt_tot.lead->GetChannel();

                    // TODO: Use it_sub->direction to find real life coordinates.

                    // Fix fiber installation mistakes.
                    fiber_id = FixMistake(fiber_id);

                    // Calibrate hit fiber.
                    auto tot_mapmt = mapmt_tot.tot_ns;
                    auto tot_spmt = spmt_tot.tot_ns;
                    Double_t t_mapmt = mapmt_tot.lead_ns;
                    Double_t t_spmt = spmt_tot.lead_ns;

                    // only accept hits which are at the right time:

                    fh_time_Fib->Fill(fiber_id, t_mapmt);
                    fh_time_s_Fib->Fill(single, t_spmt);

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
                        R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(fiber_id);
                        if (par)
                        {
                            gainMA = par->GetGainMA();
                            tsync = par->GetSync();
                            gainS = par->GetGainS();
                            offset1 = par->GetOffset1();
                            offset2 = par->GetOffset2();
                        }
                    }
                    // cout<<"offset "<<offset1<<"  "<<offset2<<endl;

                    tot_mapmt *= 10. / gainMA;
                    tot_spmt *= 10. / gainS;
                    t_mapmt += offset1;
                    t_spmt += offset2;
                    tof = (t_mapmt + t_spmt) / 2.;
                    tof += tsync;

                    // histogram for offset determination
                    fh_dt_Fib->Fill(fiber_id, t_spmt - t_mapmt);

                    // Fill histograms for gain match, and for debugging.
                    fh_Fib_ToF->Fill(fiber_id, tof);
                    fh_ToT_MA_Fib->Fill(fiber_id, tot_mapmt);
                    if (s_mult > 0)
                    {
                        fh_ToT_Single_Fib->Fill(fiber_id, tot_spmt);
                        fh_ToT_s_Fib[single - 1]->Fill(fiber_id, tot_spmt);
                    }

                    // Int_t numFibs = fSubNum * fChPerSub[0];
                    Int_t numFibs = fSubNum * fChPerSub[0] * fChPerSub[1];
                    Double_t x = -10000.;
                    Double_t y = -10000.;
                    Double_t randx;
                    Double_t randy;

                    if (fName == "Fi10" || fName == "Fi11" || fName == "Fi12" || fName == "Fi13")
                    {
                        Float_t fiber_thickness = 0.050000; // MH remove *2 when taking SPMT into analysis
                        Int_t fiber_nbr = 1024;             // MH remove /2 when taking SPMT into analysis
                        Float_t dead_layer = 0.9;
                        Float_t air_layer = 0.01; // relative to fiber_thickness
                        Float_t detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);

                        if (fDirection == VERTICAL)
                        {
                            x = -detector_width / 2. + fiber_thickness / 2. +
                                ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                            y = (t_spmt - t_mapmt) * 3.;
                        }
                        else
                        {
                            x = (t_spmt - t_mapmt) * 3.;
                            y = -detector_width / 2. + fiber_thickness / 2. +
                                ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                        }
                        // cout << "fiber_id " << fiber_id << " pos " << x << endl;
                    }
                    if (fName == "Fi1a" || fName == "Fi1b" || fName == "Fi2a" || fName == "Fi2b" || fName == "Fi3a" ||
                        fName == "Fi3b")
                    {
                        Float_t fiber_thickness = 0.021000; // MH remove *2 when taking SPMT into analysis
                        Int_t fiber_nbr = 512;              // MH remove /2 when taking SPMT into analysis
                        Float_t dead_layer = 0.9;
                        Float_t air_layer = 0.01; // relative to fiber_thickness
                        Float_t detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);
                        if (fDirection == VERTICAL)
                        {
                            x = -detector_width / 2. + fiber_thickness / 2. +
                                ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                            y = (t_spmt - t_mapmt) * 3.;
                        }
                        else
                        {
                            x = (t_spmt - t_mapmt) * 3.;
                            y = -detector_width / 2. + fiber_thickness / 2. +
                                ((fiber_id - 1) + ((fiber_id - 1) * air_layer)) * fiber_thickness;
                        }
                    }
                    // cout<<"Fiber y " << y << endl;
                    if (y < -100 || y > 100)
                    {
                        continue;
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

                    if (!fIsCalibrator)
                        new ((*fHitItems)[fNofHitItems++])
                            R3BBunchedFiberHitData(0, x, y, eloss, t, fiber_id, t_mapmt, t_spmt, tot_mapmt, tot_spmt);
                    // R3BBunchedFiberHitData(0, x, y, eloss, t, fiber_id, t_mapmt, 0., tot_mapmt, 0.);
                }
            }
        }
    }
    fh_ToT_ToT->Fill(s1, s2);
    fnEvents++;

    // cout<<"end exec"<<endl;
}

void R3BBunchedFiberCal2Hit::FinishEvent()
{
    fHitItems->Clear();
    fNofHitItems = 0;
}

void R3BBunchedFiberCal2Hit::FinishTask()
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

        R3BBunchedFiberHitModulePar* mpar;

        UInt_t max = N_FIBER_MAX;
        if (fh_ToT_MA_Fib->GetNbinsX() < N_FIBER_MAX)
            max = fh_ToT_MA_Fib->GetNbinsX();

        for (UInt_t i = 1; i <= max; i++)
        {
            mpar = new R3BBunchedFiberHitModulePar();
            mpar->SetFiber(i);
            fCalPar->AddModulePar(mpar);
        }

        // tsync
        if (fIsTsync)
        {
            for (UInt_t i = 1; i <= max; i++)
            {
                // R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);

                if (!fIsGain)
                {
                    R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
                    par->SetGainMA(gain_temp[i - 1]);
                }
                R3BBunchedFiberHitModulePar* par1 = fCalPar->GetModuleParAt(i);
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
            R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
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
                        R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
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
                            R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
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

void R3BBunchedFiberCal2Hit::MAPMTTriggerMapSet(unsigned const* a_map, size_t a_map_bytes)
{
    size_t const c_length = a_map_bytes / sizeof *a_map;
    if (c_length != fSubNum * fChPerSub[0])
    {
        LOG(fatal) << "MAPMT trigger map length=" << c_length << " not compatible with sub#=" << fSubNum
                   << " mapmt_per_sub=" << fChPerSub[0] << "!";
    }
    fMAPMTTriggerMap = a_map;
}

void R3BBunchedFiberCal2Hit::SPMTTriggerMapSet(unsigned const* a_map, size_t a_map_bytes)
{
    size_t const c_length = a_map_bytes / sizeof *a_map;
    if (c_length != fSubNum * fChPerSub[1])
    {
        LOG(fatal) << "SPMT trigger map length=" << c_length << " not compatible with sub#=" << fSubNum
                   << " spmt_per_sub=" << fChPerSub[1] << "!";
    }
    fSPMTTriggerMap = a_map;
}

ClassImp(R3BBunchedFiberCal2Hit)
