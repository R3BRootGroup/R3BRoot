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
    , fCalTriggerItems()
    , fHitItems(new TClonesArray("R3BBunchedFiberHitData"))
    , fCalPar()
    , fHitPar()
    , fNofHitPars()
    , fNofHitItems()
    , fChannelArray()
    , fh_ToT_MA_Fib()
    , fh_ToT_Single_Fib()
    , fh_ToT_s_Fib()
    , fh_ToT_ToT()
    , fh_dt_Fib()
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
        LOG(ERROR) << "Branch " << name << " not found.";
        return kERROR;
    }
    auto name_trig = fName + "TriggerCal";
    fCalTriggerItems = (TClonesArray*)mgr->GetObject(name_trig);
    if (NULL == fCalTriggerItems)
        LOG(fatal) << "Branch " << name_trig << " not found";
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
    if (fHitPar)
    {
        for (int i = 1; i <= N_FIBER_MAX; i++)
        {
            if (!fIsGain && fIsTsync) // gain already made, tsync will be done now
            {
                R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(i);
                if (par && par->GetGainMA() > 0.)
                    gain_temp[i - 1] = par->GetGainMA();
                else
                    gain_temp[i - 1] = 10.;
            }
            if (fIsGain && !fIsTsync) // tsync already made, gain will be done now
            {
                R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(i);
                if (par)
                    tsync_temp[i - 1] = par->GetSync();
                else
                    tsync_temp[i - 1] = 0.;
            }
        }
    }

    //    }
    // create histograms
    /*
        fh_69_70 = new TH2F("fh_69_70", "69 70", 100, 0., 100., 100, 0., 100.);
        fh_69_59 = new TH2F("fh_69_59", "69 59", 100, 0., 100., 100, 0., 100.);
        fh_69_37 = new TH2F("fh_69_37", "69 37", 100, 0., 100., 100, 0., 100.);
        fh_69_45 = new TH2F("fh_69_45", "69 45", 100, 0., 100., 100, 0., 100.);
        fh_69_77 = new TH2F("fh_69_77", "69 77", 100, 0., 100., 100, 0., 100.);
        fh_69_109 = new TH2F("fh_69_109", "69 109", 100, 0., 100., 100, 0., 100.);
        fh_69_101 = new TH2F("fh_69_101", "69 101", 100, 0., 100., 100, 0., 100.);
        fh_69_123 = new TH2F("fh_69_123", "69 123", 100, 0., 100., 100, 0., 100.);
        fh_69_91 = new TH2F("fh_69_91", "69 91", 100, 0., 100., 100, 0., 100.);
        fh_69_49 = new TH2F("fh_69_49", "69 49", 100, 0., 100., 100, 0., 100.);
        fh_69_71 = new TH2F("fh_69_71", "69 71", 100, 0., 100., 100, 0., 100.);

        fh_69_60 = new TH2F("fh_69_60", "69 60", 100, 0., 100., 100, 0., 100.);
        fh_69_38 = new TH2F("fh_69_38", "69 38", 100, 0., 100., 100, 0., 100.);
        fh_69_46 = new TH2F("fh_69_46", "69 46", 100, 0., 100., 100, 0., 100.);
        fh_69_78 = new TH2F("fh_69_78", "69 78", 100, 0., 100., 100, 0., 100.);
        fh_69_110 = new TH2F("fh_69_110", "69 110", 100, 0., 100., 100, 0., 100.);
        fh_69_102 = new TH2F("fh_69_102", "69 102", 100, 0., 100., 100, 0., 100.);
        fh_69_124 = new TH2F("fh_69_124", "69 124", 100, 0., 100., 100, 0., 100.);
        fh_69_92 = new TH2F("fh_69_92", "69 92", 100, 0., 100., 100, 0., 100.);
        fh_69_50 = new TH2F("fh_69_50", "69 50", 100, 0., 100., 100, 0., 100.);

        fh_multi = new TH1F("multi", "multi", 512, 0., 512.);
    */
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

    // ToT vs ToT SPMT:
    chistName = fName + "_ToT_ToT";
    chistTitle = fName + " ToT vs ToT of single PMTs";
    fh_ToT_ToT = new TH2F(chistName.Data(), chistTitle.Data(), 1000, 0., 100., 1000, 0., 100.);
    fh_ToT_ToT->GetXaxis()->SetTitle("SPMT 1");
    fh_ToT_ToT->GetYaxis()->SetTitle("SPMT 2");

    // time difference SPMT - MAPMT:
    chistName = fName + "_dt";
    chistTitle = fName + " dt of fibers";
    fh_dt_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 512, 0, 512, 1200, -600., 600.);
    fh_dt_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_dt_Fib->GetYaxis()->SetTitle("dt / ns");

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

    auto const c_period = 4096e3 / fClockFreq;
    size_t cal_num = fCalItems->GetEntriesFast();

    auto trig_num = fCalTriggerItems->GetEntries();
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
    for (size_t j = 0; j < cal_num; ++j)
    {
        auto cur_cal = (R3BBunchedFiberCalData const*)fCalItems->At(j);
        if (cur_cal->IsTrailing())
        {
            auto side_i = cur_cal->IsMAPMT() ? 0 : 1;
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
                auto lead_trig_i = fMAPMTTriggerMap[lead->GetChannel()];
                if (cur_cal_trig_i < trig_num && lead_trig_i < trig_num)
                {
                    auto cur_cal_trig = (R3BBunchedFiberCalData const*)fCalTriggerItems->At(cur_cal_trig_i);
                    auto lead_trig = (R3BBunchedFiberCalData const*)fCalTriggerItems->At(lead_trig_i);
                    cur_cal_trig_ns = cur_cal_trig->GetTime_ns();
                    lead_trig_ns = lead_trig->GetTime_ns();
                }
            }

            auto cur_cal_ns = cur_cal->GetTime_ns() - cur_cal_trig_ns;
            auto lead_ns = lead->GetTime_ns() - lead_trig_ns;
            auto tot_ns = fmod(cur_cal_ns - lead_ns + c_period + c_period / 2, c_period) - c_period / 2;
            if (fName == "Fi10" && fnEvents == 17409 && side_i == 0)
                summmpt += 1;
            if (fName == "Fi10" && fnEvents == 17409 && side_i == 1 && ch_i == 0)
                summsm1 += 1;
            if (fName == "Fi10" && fnEvents == 17409 && side_i == 1 && ch_i == 1)
                summsm2 += 1;

            if (fName == "Fi10" && fnEvents == 17409)
                cout << "Input: " << side_i << "; " << ch_i + 1 << ", " << lead->GetTime_ns() << "; " << tot_ns << endl;

            if (tot_ns < 1000)
            {
                // if (side_i==0) tot_ns -= 9.;//taken out for the moment MH

                // channel.tot_list.push_back(ToT(lead, cur_cal, tot_ns)); -> this lead to: t1>t2>t3....; we need
                // t1<t2<t3....
                channel.tot_list.push_front(ToT(lead, cur_cal, lead_ns, cur_cal_ns, tot_ns));
                channel.lead_list.pop_front();
            }
        }
    }
    if (fName == "Fi10" && fnEvents == 17409)
        cout << "Summs: " << summmpt << ", " << summsm1 << ", " << summsm2 << endl;

    //     cout<<"finished list"<<endl;

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
            // MH           for (auto it_spmt = spmt_array.begin(); spmt_array.end() != it_spmt; ++it_spmt)
            // MH           {
            // MH               auto const& spmt = *it_spmt;
            // MH               for (auto it_spmt_tot = spmt.tot_list.begin(); spmt.tot_list.end() != it_spmt_tot;
            // ++it_spmt_tot) MH               { MH                   auto const& spmt_tot = *it_spmt_tot;

            // Check that the combo is inside one sub-det.
            auto mapmt_sub_id = (mapmt_tot.lead->GetChannel() - 1) / fChPerSub[0];
            // MH                   auto spmt_sub_id = (spmt_tot.lead->GetChannel() - 1) / fChPerSub[1];
            // MH                   if (mapmt_sub_id != spmt_sub_id)
            // MH                   {
            // MH                       continue;
            // MH                   }

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
            // MH                    auto fiber_id = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1];
            // MH                                    + ((spmt_tot.lead->GetChannel() - 1) % fChPerSub[1]) + 1;

            auto fiber_id_ch = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] + 1;
            // MH                    single = spmt_tot.lead->GetChannel();

            // cout<<"MA Fiber ch: "<<mapmt_tot.lead->GetChannel()<<" Fiber: "<< fiber_id<<" ToT: "<<
            // mapmt_tot.tot_ns<<endl;
            //                    cout<<"S channels: "<<spmt_tot.lead->GetChannel()<<" ToT: "<<spmt_tot.tot_ns<<" mult:
            //                    "<<s_mult<<endl;

            // TODO: Use it_sub->direction to find real life coordinates.

            // Fix fiber installation mistakes.
            fiber_id = FixMistake(fiber_id);

            // Calibrate hit fiber.
            auto tot_mapmt = mapmt_tot.tot_ns;
            // MH                    auto tot_spmt = spmt_tot.tot_ns;
            Double_t t_mapmt = mapmt_tot.lead_ns;
            // MH                    Double_t t_spmt = spmt_tot.lead->GetTime_ns();

            // "Push" two times in the same clock cycle:
            // MAPMT coarse counter has 4096 channels with 1000/150=6.67 ns or 4 ns each
            // SPMT coarse counter has 2048 channels with 5 ns each

            /* MH
                                Double_t dtime = t_spmt - t_mapmt;
                                while (dtime > 256. / 2. * 1000. / fClockFreq)
                                {
                                    t_mapmt += 256. * 1000. / fClockFreq;
                                    dtime = t_spmt - t_mapmt;
                                    // cout<<"dtime1 "<<dtime<<endl;
                                }
                                while (dtime < -256. / 2. * 1000. / fClockFreq)
                                {
                                    t_mapmt -= 256. * 1000. / fClockFreq;
                                    dtime = t_spmt - t_mapmt;
                                    // cout<<"dtime2 "<<dtime<<endl;
                                }
            */

            // histogram for offset determination
            // MH                    if (fIsCalibrator)
            // MH                        fh_dt_Fib->Fill(fiber_id, t_spmt - t_mapmt);

            // MH                    LOG(DEBUG) << fName << " tS " << tot_spmt << " tM: " << t_mapmt << " totS: " <<
            // tot_spmt MH                               << " totM: " << tot_mapmt << endl;

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
                    // MH                           gainS = par->GetGainS();
                    // MH                           offset1 = par->GetOffset1();
                    // MH                           offset2 = par->GetOffset2();
                }
            }
            // cout<<"offset "<<offset1<<"  "<<offset2<<endl;

            tot_mapmt *= 10. / gainMA;
            // MH                   tot_spmt *= 10. / gainS;
            // t_mapmt += offset1;
            // MH                   t_spmt += offset2;
            t_mapmt += tsync;

            //  cout<<"after "<<tot_mapmt<<", "<<gainMA<<endl;

            // MH                    fh_dt_Fib->Fill(fiber_id, t_spmt - t_mapmt);

            /*
            if (tot_mapmt > tot_mapmt_max) {
              tot_mapmt_max = tot_mapmt;
              tot_mapmt_max_fiber_id = fiber_id;

              tot_spmt_max = tot_spmt;
              tot_spmt_max_fiber_id = fiber_id;

            }
            */
            /*
            if (tot_spmt > tot_spmt_max) {
              tot_spmt_max = tot_spmt;
              tot_spmt_max_fiber_id = fiber_id;
            }
            */

            // Fill histograms for gain match, and for debugging.
            Int_t nHitsTofd = 0;
            if (tofdin)
            {
                auto detTofd = fTofdHitItems;
                nHitsTofd = detTofd->GetEntriesFast();
                for (Int_t ihit = 0; ihit < nHitsTofd; ihit++)
                {
                    R3BTofdHitData* hitTofd = (R3BTofdHitData*)detTofd->At(ihit);
                    t_tofd = hitTofd->GetTime();
                    tofd_id = hitTofd->GetDetId();

                    if (tofd_id == 1 || tofd_id == 2)
                    {

                        while (t_tofd - t_mapmt < 2048. * 1. / 2.)
                        {
                            t_mapmt -= 2048. * 1.;
                        }
                        while (t_tofd - t_mapmt > 2048. * 1. / 2.)
                        {
                            t_mapmt += 2048. * 1.;
                        }

                        tof = t_tofd - t_mapmt;

                        fnEventsfill += 1;

                        fh_Fib_ToF->Fill(fiber_id, tof);
                        /*
                            if(fnEventsfill%1000000	== 0){
                                Double_t mbin = 0;
                                for(Int_t k=1; k<513; k++){
                                    TH1D* proj = fh_Fib_ToF->ProjectionY("", k + 1, k + 1, 0);
                                    mbin = proj->GetBinCenter(proj->GetMaximumBin());
                                    fh_Test->Fill(k,mbin);
                                }
                                fh_Fib_ToF->Reset("ICESM");
                                fnEventsfill = 0.;
                                cout<<"cleared after 1M events!"<<endl;
                            }
                        */
                    }
                }
            }

            fh_ToT_MA_Fib->Fill(fiber_id, tot_mapmt);
            if (s_mult > 0)
            {
                // MH                        fh_ToT_Single_Fib->Fill(fiber_id, tot_spmt);
                // MH                        fh_ToT_s_Fib[single - 1]->Fill(fiber_id, tot_spmt);
            }

            Int_t numFibs = fSubNum * fChPerSub[0];
            // MH                    Int_t numFibs = fSubNum * fChPerSub[0] * fChPerSub[1];
            Double_t x = -10000.;
            Double_t y = -10000.;
            Double_t randx;
            Double_t randy;

            if (fName == "Fi10" || fName == "Fi11" || fName == "Fi12" || fName == "Fi13")
            {
                if (fDirection == VERTICAL)
                {
                    randx = (std::rand() / (float)RAND_MAX);
                    x = ((double)fiber_id - randx - (double)numFibs / 2.) * 0.05 * 2.; // in cm
                    //                            x = ((double)fiber_id - (double)numFibs / 2) * 0.1; // in cm
                    // MH                            y = (t_spmt - t_mapmt) * 3.;
                }
                else
                {
                    // MH                            x = (t_spmt - t_mapmt) * 3.;
                    //                            y = ((double)fiber_id - (double)numFibs / 2) * 0.1; // in cm
                    randy = (std::rand() / (float)RAND_MAX);
                    y = ((double)fiber_id - randy - (double)numFibs / 2.) * 0.05 * 2.; // in cm
                }
            }
            if (fName == "Fi1a" || fName == "Fi1b" || fName == "Fi2a" || fName == "Fi2b" || fName == "Fi3a" ||
                fName == "Fi3b")
            {
                if (fDirection == VERTICAL)
                {
                    randx = (std::rand() / (float)RAND_MAX);
                    x = ((double)fiber_id - randx - (double)numFibs / 2.) * 0.02 * 2.; // in cm
                    //                            x = ((double)fiber_id - (double)numFibs / 2) * 0.04; // in cm
                    // MH                            y = (t_spmt - t_mapmt) * 3.;
                }
                else
                {
                    // MH                            x = (t_spmt - t_mapmt) * 3.;
                    randy = (std::rand() / (float)RAND_MAX);
                    y = ((double)fiber_id - randy - (double)numFibs / 2.) * 0.02 * 2.; // in cm
                    //                            y = ((double)fiber_id - (double)numFibs / 2) * 0.04; // in cm
                }
            }
            // cout<<"Fiber y " << y << endl;
            // MH                    if (y < -100 || y > 100)
            // MH                    {
            // MH                        continue;
            // MH                    }
            // Double_t eloss = sqrt(tot_mapmt * tot_spmt);
            Double_t eloss = tot_mapmt;
            // Double_t t = (t_mapmt + t_spmt) / 2.;
            // Double_t t = t_spmt;
            Double_t t = t_mapmt;

            if (fnEvents == 111026)
            {
                // cout << "FiberID: " << fiber_id << "  " << (double)fiber_id - (double)numFibs << endl;
                // cout << fName<< " x: " << x << " y: " << y << " eloss: " << eloss << " t: " << t << endl;
            }

            energy[fiber_id] = eloss;
            counts[fiber_id] = counts[fiber_id] + 1;
            multi++;

            if (!fIsCalibrator)
                new ((*fHitItems)[fNofHitItems++])
                    // MH                            R3BBunchedFiberHitData(0, x, y, eloss, t, fiber_id, t_mapmt,
                    // t_spmt, tot_mapmt, tot_spmt);
                    R3BBunchedFiberHitData(0, x, y, eloss, t, fiber_id, t_mapmt, 0., tot_mapmt, 0.);
            // MH                }
            // MH            }
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

    //	if(multi>100) cout << "große Multi: " << multi << "  " << fnEvents << endl;
    /*
        fh_multi->Fill(multi);
        multi=0;

        if (counts[69]<0) {
            cout << "-------------------------------------" << endl;
            cout << "69: " << energy[69] << "  " << counts[69] << endl;
            for(Int_t i = 0; i<200; i++){
                if (counts[i] > 0) cout << i << "  " << energy[69] << "  " << counts[69] << endl;
            }
        }
    */
    /*
        fh_69_70->Fill(energy[69],energy[70]);
        fh_69_59->Fill(energy[69],energy[59]);
        fh_69_37->Fill(energy[69],energy[37]);
        fh_69_45->Fill(energy[69],energy[45]);
        fh_69_77->Fill(energy[69],energy[77]);
        fh_69_109->Fill(energy[69],energy[109]);
        fh_69_101->Fill(energy[69],energy[101]);
        fh_69_123->Fill(energy[69],energy[123]);
        fh_69_91->Fill(energy[69],energy[91]);
        fh_69_49->Fill(energy[69],energy[49]);
        fh_69_71->Fill(energy[69],energy[71]);

        fh_69_60->Fill(energy[69],energy[60]);
        fh_69_38->Fill(energy[69],energy[38]);
        fh_69_46->Fill(energy[69],energy[46]);
        fh_69_78->Fill(energy[69],energy[78]);
        fh_69_110->Fill(energy[69],energy[110]);
        fh_69_102->Fill(energy[69],energy[102]);
        fh_69_124->Fill(energy[69],energy[124]);
        fh_69_92->Fill(energy[69],energy[92]);
        fh_69_50->Fill(energy[69],energy[50]);
    */
}

void R3BBunchedFiberCal2Hit::FinishTask()
{
    fh_ToT_MA_Fib->Write();
    fh_ToT_Single_Fib->Write();
    fh_dt_Fib->Write();
    fh_Fib_ToF->Write();
    fh_Test->Write();

    /*
        fh_69_70->Write();
        fh_69_59->Write();
        fh_69_37->Write();
        fh_69_45->Write();
        fh_69_77->Write();
        fh_69_109->Write();
        fh_69_101->Write();
        fh_69_123->Write();
        fh_69_91->Write();
        fh_69_49->Write();
        fh_69_71->Write();

        fh_multi->Write();
    */
    /*
        for (Int_t i = 0; i < 4; i++)
        {
            fh_ToT_s_Fib[i]->Write();
        }
        fh_ToT_ToT->Write();
    */
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
                R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);

                if (!fIsGain)
                {
                    if (par->GetGainMA() == 0)
                        par->SetGainMA(gain_temp[i - 1]);
                }
                TH1D* proj = fh_Fib_ToF->ProjectionY("", i + 1, i + 1, 0);
                // R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
                par->SetSync(proj->GetBinCenter(proj->GetMaximumBin()));

                cout << "MA fiber: " << fName << ", " << i << " tsync: " << proj->GetBinCenter(proj->GetMaximumBin())
                     << ", " << par->GetSync() << ", " << par->GetGainMA() << endl;
            }
        }
        // time offset
        /* MH
                for (UInt_t i = 1; i <= max; i++)
                {
                    TH1D* proj = fh_dt_Fib->ProjectionY("", i + 1, i + 1, 0);
                    R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
                    par->SetOffset1(0.5 * proj->GetBinCenter(proj->GetMaximumBin()));
                    par->SetOffset2(-0.5 * proj->GetBinCenter(proj->GetMaximumBin()));

                    cout << "MA fiber: " << i << " offset: " << 0.5 * proj->GetBinCenter(proj->GetMaximumBin()) << endl;
                }
        */
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
                        // cout<<"MA fiber: "<< i<<" par: "<<proj->GetBinCenter(j)<<endl;
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

ClassImp(R3BBunchedFiberCal2Hit)
