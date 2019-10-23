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

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"

#include <iostream>

R3BBunchedFiberCal2Hit::ToT::ToT(R3BBunchedFiberCalData const* a_lead,
                                 R3BBunchedFiberCalData const* a_trail,
                                 Double_t a_tot)
    : lead(a_lead)
    , trail(a_trail)
    , tot(a_tot)
{
}

R3BBunchedFiberCal2Hit::R3BBunchedFiberCal2Hit(const char* a_name,
                                               Int_t a_verbose,
                                               enum R3BTCalEngine::CTDCVariant a_ctdc_variant,
                                               Direction a_direction,
                                               UInt_t a_sub_num,
                                               UInt_t a_mapmt_per_sub,
                                               UInt_t a_spmt_per_sub,
                                               Bool_t a_is_calibrator)
    : FairTask(TString("R3B") + a_name + "Cal2Hit", a_verbose)
    , fName(a_name)
    , fClockFreq(R3BTCalEngine::CTDC_16_BWD_150 == a_ctdc_variant ? 150 : 250)
    , fDirection(a_direction)
    , fSubNum(a_sub_num)
    , fIsCalibrator(a_is_calibrator)
    , fCalItems()
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
    auto name = fName + "Cal";
    fCalItems = (TClonesArray*)mgr->GetObject(name);
    if (!fCalItems)
    {
        LOG(ERROR) << "Branch " << name << " not found.";
        return kERROR;
    }
    maxevent = mgr->CheckMaxEventNo();

    mgr->Register(fName + "Hit", "Land", fHitItems, kTRUE);
    // Resize per-channel info arrays.
    for (auto side_i = 0; side_i < 2; ++side_i)
    {
        fChannelArray[side_i].resize(fSubNum * fChPerSub[side_i]);
    }

    if (!fIsCalibrator)
    {
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
            fNofHitPars = fHitPar->GetNumModulePar();
            if (0 == fNofHitPars)
            {
                LOG(ERROR) << "No Hit parameters in " << container << " container.";
                fHitPar = nullptr;
            }
        }
    }

    // create histograms
    TString chistName;
    TString chistTitle;
    // ToT MAPMT:
    chistName = fName + "_ToT_MAPMT";
    chistTitle = fName + " ToT of fibers";
    fh_ToT_MA_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 2100, 0., 2100., 100, 0., 41.666667);
    fh_ToT_MA_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_MA_Fib->GetYaxis()->SetTitle("ToT / ns");

    // ToT single PMT:
    chistName = fName + "_ToT_SAPMT";
    chistTitle = fName + " ToT of fibers";
    fh_ToT_Single_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 2100, 0., 2100., 100, 0., 100.);
    fh_ToT_Single_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_Single_Fib->GetYaxis()->SetTitle("ToT / ns");

    // ToT SAPMT:
    for (Int_t i = 0; i < 4; i++)
    {
        char number[15];
        snprintf(number, sizeof(number), "%d", i);
        chistName = fName + "_ToT_SAPMT" + number;
        chistTitle = fName + " ToT of single PMTs " + number;
        fh_ToT_s_Fib[i] = new TH2F(chistName.Data(), chistTitle.Data(), 2100, 0., 2100., 100, 0., 100.);
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
    fh_dt_Fib = new TH2F(chistName.Data(), chistTitle.Data(), 2100, 0., 2100., 1200, -600., 600.);
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
        LOG(ERROR) << "R3BTofdCal2Hit::Init() Couldn't get " << fName << "HitPar. ";
    }
}

void R3BBunchedFiberCal2Hit::Exec(Option_t* option)
{
    // cout<<"in exec"<<endl;
    //	if(fnEvents/10000.==(int)fnEvents/10000) cout<<"Events: "<<fnEvents<<"         \r"<<std::flush;
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

    // cout<<"multihit"<<endl;

    auto const c_period = 4096e3 / fClockFreq;
    size_t cal_num = fCalItems->GetEntriesFast();

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
            auto tot = fmod(cur_cal->GetTime_ns() - lead->GetTime_ns() + c_period, c_period);
            if (tot < 1000)
            {
                // if (side_i==0) tot -= 9.;//taken out for the moment MH

                channel.tot_list.push_back(ToT(lead, cur_cal, tot));
                channel.lead_list.pop_front();
            }
        }
    }

    // cout<<"finished list"<<endl;

    double s1 = 99.;
    double s2 = 99.;
    double s3 = 99.;
    double s4 = 99.;
    int single = 0;

    double tot_mapmt_max = -1;
    double tot_spmt_max = -1;
    int tot_mapmt_max_fiber_id = 0;
    int tot_spmt_max_fiber_id = 0;

    // cout<<"permutations"<<endl;

    // Make every permutation to create fibers.
    auto const& mapmt_array = fChannelArray[0];
    auto const& spmt_array = fChannelArray[1];
    for (auto it_mapmt = mapmt_array.begin(); mapmt_array.end() != it_mapmt; ++it_mapmt)
    {
        auto const& mapmt = *it_mapmt;
        for (auto it_mapmt_tot = mapmt.tot_list.begin(); mapmt.tot_list.end() != it_mapmt_tot; ++it_mapmt_tot)
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

                    auto fiber_id = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] +
                                    ((spmt_tot.lead->GetChannel() - 1) % fChPerSub[1]) + 1;

                    auto fiber_id_ch = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] + 1;
                    single = spmt_tot.lead->GetChannel();

                    /*
                    cout<<"MA Fiber ch: "<<mapmt_tot.lead->GetChannel()<<" Fiber: "<< fiber_id<<" ToT: "<<
                    mapmt_tot.tot<<endl; cout<<"S channels: "<<spmt_tot.lead->GetChannel()<<" ToT: "<<spmt_tot.tot<<"
                    mult: "<<s_mult<<endl;
                    */
                    // TODO: Use it_sub->direction to find real life coordinates.

                    // Fix fiber installation mistakes.
                    fiber_id = FixMistake(fiber_id);

                    // Calibrate hit fiber.
                    auto tot_mapmt = mapmt_tot.tot;
                    auto tot_spmt = spmt_tot.tot;
                    Double_t t_mapmt = mapmt_tot.lead->GetTime_ns();
                    Double_t t_spmt = spmt_tot.lead->GetTime_ns();

                    // "Push" two times in the same clock cycle:
                    // MAPMT coarse counter has 4096 channels with 1000/150=6.67 ns or 4 ns each
                    // SPMT coarse counter has 2048 channels with 5 ns each
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

                    // histogram for offset determination
                    if (fIsCalibrator)
                        fh_dt_Fib->Fill(fiber_id, t_spmt - t_mapmt);

                    // Apply calibration.
                    Double_t gainMA = 10.;
                    Double_t gainS = 10.;
                    Double_t offset1 = 0.;
                    Double_t offset2 = 0.;

                    if (!fIsCalibrator && fHitPar)
                    {
                        R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(fiber_id);
                        if (par)
                        {
                            gainMA = par->GetGainMA();
                            gainS = par->GetGainS();
                            offset1 = par->GetOffset1();
                            offset2 = par->GetOffset2();
                        }
                    }
                    // cout<<"offset "<<offset1<<"  "<<offset2<<endl;
                    // cout<<"before "<<t_spmt<<"  "<<t_mapmt<<endl;

                    tot_mapmt *= 10. / gainMA;
                    tot_spmt *= 10. / gainS;
                    t_mapmt += offset1;
                    t_spmt += offset2;

                    // cout<<"after "<<t_spmt<<"  "<<t_mapmt<<endl;

                    if (!fIsCalibrator)
                        fh_dt_Fib->Fill(fiber_id, t_spmt - t_mapmt);

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
                    fh_ToT_MA_Fib->Fill(fiber_id, tot_mapmt);
                    if (s_mult > 0)
                    {
                        fh_ToT_Single_Fib->Fill(fiber_id, tot_spmt);
                        fh_ToT_s_Fib[single - 1]->Fill(fiber_id, tot_spmt);
                    }

                    Int_t numFibs = fSubNum * fChPerSub[0] * fChPerSub[1];
                    Double_t x = -10000.;
                    Double_t y = -10000.;

                    if (fName == "Fi10" || fName == "Fi11" || fName == "Fi12" || fName == "Fi13")
                    {
                        if (fDirection == VERTICAL)
                        {
                            x = ((double)fiber_id - (double)numFibs / 2) * 0.05; // in cm
                            y = (t_spmt - t_mapmt) * 3.;
                        }
                        else
                        {
                            x = (t_spmt - t_mapmt) * 3.;
                            y = ((double)fiber_id - (double)numFibs / 2) * 0.05; // in cm
                        }
                    }
                    if (fName == "Fi1a" || fName == "Fi1b" || fName == "Fi2a" || fName == "Fi2b" || fName == "Fi3a" ||
                        fName == "Fi3b")
                    {
                        if (fDirection == VERTICAL)
                        {
                            x = ((double)fiber_id - (double)numFibs / 2) * 0.021; // in cm
                            y = (t_spmt - t_mapmt) * 3.;
                        }
                        else
                        {
                            x = (t_spmt - t_mapmt) * 3.;
                            y = ((double)fiber_id - (double)numFibs / 2) * 0.021; // in cm
                        }
                    }
                    Double_t eloss = sqrt(tot_mapmt * tot_spmt);
                    Double_t t = (t_mapmt + t_spmt) / 2.;

                    // cout<<"FiberID: "<<fiber_id << "  "<<(double)fiber_id - (double)numFibs<<endl;
                    // cout<<fName<<" x: "<< x << " y: "<< y << " eloss: " << eloss << " t: " << t << endl;

                    if (!fIsCalibrator)
                        new ((*fHitItems)[fNofHitItems++])
                            R3BBunchedFiberHitData(0, x, y, eloss, t, fiber_id, t_mapmt, t_spmt, tot_mapmt, tot_spmt);
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

        // time offset
        for (UInt_t i = 1; i <= max; i++)
        {
            TH1D* proj = fh_dt_Fib->ProjectionY("", i + 1, i + 1, 0);
            R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
            par->SetOffset1(0.5 * proj->GetBinCenter(proj->GetMaximumBin()));
            par->SetOffset2(-0.5 * proj->GetBinCenter(proj->GetMaximumBin()));

            cout << "MA fiber: " << i << " offset: " << 0.5 * proj->GetBinCenter(proj->GetMaximumBin()) << endl;
        }

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
                    cout << "MA fiber: " << i << " par: " << proj->GetBinCenter(j) << endl;
                    break;
                }
            }
        }

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

        fCalPar->setChanged();
    }
}

ClassImp(R3BBunchedFiberCal2Hit)
