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

#include "R3BEventHeader.h"
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
                                 Double_t a_lead_raw,
                                 Double_t a_trail_raw,
                                 Double_t a_lead_ns,
                                 Double_t a_trail_ns,
                                 Double_t a_tot_ns,
                                 Double_t a_lead_trig,
                                 Double_t a_trail_trig)
    : lead(a_lead)
    , trail(a_trail)
    , lead_raw(a_lead_raw)
    , trail_raw(a_trail_raw)
    , lead_ns(a_lead_ns)
    , trail_ns(a_trail_ns)
    , tot_ns(a_tot_ns)
    , lead_trig_ns(a_lead_trig)
    , trail_trig_ns(a_trail_trig)
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
    , fh_ToT1_ToT2()
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
        LOG(fatal) << "FairRootManager not found.";
        //        return kError;
    }
    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    auto name = fName + "Cal";
    fCalItems = (TClonesArray*)mgr->GetObject(name);
    if (!fCalItems)
    {
        LOG(fatal) << "Branch " << name << " not found.";
        //        return kError;
    }
    auto name_mapmt_trig = fName + "TriggerCal";
    fMAPMTCalTriggerItems = (TClonesArray*)mgr->GetObject(name_mapmt_trig);
    if (NULL == fMAPMTCalTriggerItems)
        LOG(fatal) << "Branch " << name_mapmt_trig << " not found";

    auto name_spmt_trig = "BunchedFiberSPMTTrigCal";
    fSPMTCalTriggerItems = (TClonesArray*)mgr->GetObject(name_spmt_trig);
    if (NULL == fSPMTCalTriggerItems && fChPerSub[1] > 0)
        LOG(fatal) << "Branch " << name_spmt_trig << " not found";

    maxevent = mgr->CheckMaxEventNo();

    mgr->Register(fName + "Hit", "Land", fHitItems, kTRUE);
    // Resize per-channel info arrays.
    for (auto side_i = 0; side_i < 2; ++side_i)
    {
        fChannelArray[side_i].resize(fSubNum * fChPerSub[side_i]);
    }

    // Get calibration parameters if we're not a calibrator.
    auto container = fName + "HitPar";
    fHitPar = (R3BBunchedFiberHitPar*)FairRuntimeDb::instance()->getContainer(container);
    if (!fHitPar)
    {
        LOG(info) << "Could not get " << container << " container.";
        fNofHitPars = 0;
    }
    else
    {
        fNofHitPars = fHitPar->GetNumModulePar();

        //  cout<<"fNoHitPar: "<<fNofHitPars<<endl;

        if (0 == fNofHitPars)
        {
            LOG(info) << "No Hit parameters in " << container << " container.";
            fHitPar = nullptr;
        }
    }

    for (int i = 1; i <= N_FIBER_MAX; i++)
    {
        gainM_temp[i - 1] = 20.;
        gainS_temp[i - 1] = 20.;
        tsync_temp[i - 1] = 0.;
        offset1_temp[i - 1] = 0.;
        offset2_temp[i - 1] = 0.;
    }

    if (fHitPar)
    {
        for (int i = 1; i <= N_FIBER_MAX; i++)
        {
            if (!fIsGain && fIsTsync) // gain already made, tsync will be done now
            {
                R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(i);
                if (par && par->GetGainMA() > 0.)
                {
                    gainM_temp[i - 1] = par->GetGainMA();
                    gainS_temp[i - 1] = par->GetGainS();
                }
            }
            if (fIsGain && !fIsTsync) // tsync already made, gain will be done now
            {
                R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(i);
                if (par)
                {
                    tsync_temp[i - 1] = par->GetSync();
                    offset1_temp[i - 1] = par->GetOffset1();
                    offset2_temp[i - 1] = par->GetOffset2();
                }
            }
        }
    }

    TString chistName;
    TString chistTitle;

    UInt_t Nmax = 2e6;
    UInt_t NChaMax = 256;
    UInt_t NFibMax = 512;

    chistName = fName + "_hit_mult";
    chistTitle = fName + " hit mult";
    fh_multi = new TH1F(chistName.Data(), chistTitle.Data(), 300, 0, 300);
    fh_multi->GetXaxis()->SetTitle("mutltiplicity");
    fh_multi->GetYaxis()->SetTitle("Counts");

    // ch correl
    chistName = fName + "_ch_correl";
    chistTitle = fName + "ch correl";
    fh_ch_corr = new TH2F(chistName.Data(), chistTitle.Data(), 10, 0, 10, NChaMax, 0, NChaMax);
    fh_ch_corr->GetXaxis()->SetTitle("MA channel");
    fh_ch_corr->GetYaxis()->SetTitle("SA channel");

    chistName = fName + "_ToT_FibNum";
    chistTitle = fName + "ToT FibNum";
    fh_ToT_ifib = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 500, 0., 100.);
    fh_ToT_ifib->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_ifib->GetYaxis()->SetTitle("ToT / ns");

    chistName = fName + "_MA_hit";
    chistTitle = fName + "MA hit";
    fh_iFib_nHit = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 100, 0, 100);
    fh_iFib_nHit->GetXaxis()->SetTitle("Fiber number");
    fh_iFib_nHit->GetYaxis()->SetTitle("nHit");

    chistName = fName + "_MA_hit_ac";
    chistTitle = fName + "MA hit ac";
    fh_iFib_nHit_ac = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 100, 0, 100);
    fh_iFib_nHit_ac->GetXaxis()->SetTitle("Fiber number");
    fh_iFib_nHit_ac->GetYaxis()->SetTitle("nHit");

    // nhit correl
    chistName = fName + "_nhit_MAvsSA";
    chistTitle = fName + "nhit MAvsSA";
    fh_hit1 = new TH2F(chistName.Data(), chistTitle.Data(), 200, 0, 200, 200, 0, 200);
    fh_hit1->GetXaxis()->SetTitle("nhitMA");
    fh_hit1->GetYaxis()->SetTitle("nhit(SA1+SA2*sA3+SA4)");

    chistName = fName + "_nhit_SAvsSA";
    chistTitle = fName + "nhit SAvsSA";
    fh_hit2 = new TH2F(chistName.Data(), chistTitle.Data(), 200, 0, 200, 200, 0, 200);
    fh_hit2->GetXaxis()->SetTitle("nhit(SA1+SA3)");
    fh_hit2->GetYaxis()->SetTitle("nhit(SA2+SA4)");

    chistName = fName + "_nhit_MAvsSA_ac";
    chistTitle = fName + "nhit MAvsSA ac";
    fh_hit1_ac = new TH2F(chistName.Data(), chistTitle.Data(), 200, 0, 200, 200, 0, 200);
    fh_hit1_ac->GetXaxis()->SetTitle("nhitMA");
    fh_hit1_ac->GetYaxis()->SetTitle("nhit(SA1+SA2*sA3+SA4)");

    chistName = fName + "_nhit_SAvsSA_ac";
    chistTitle = fName + "nhit SAvsSA ac";
    fh_hit2_ac = new TH2F(chistName.Data(), chistTitle.Data(), 200, 0, 200, 200, 0, 200);
    fh_hit2_ac->GetXaxis()->SetTitle("nhit(SA1+SA3)");
    fh_hit2_ac->GetYaxis()->SetTitle("nhit(SA2+SA4)");

    // ToT MAPMT:
    chistName = fName + "_ToT_MAPMT";
    chistTitle = fName + " ToT MA  of fibers";
    fh_ToT_MA_Fib = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 500, 0., 100.);
    fh_ToT_MA_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_MA_Fib->GetYaxis()->SetTitle("ToT / ns");

    chistName = fName + "_ToT_MAPMT_raw";
    chistTitle = fName + " ToT MA of fibers raw";
    fh_ToT_MA_Fib_raw = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 500, 0., 100.);
    fh_ToT_MA_Fib_raw->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_MA_Fib_raw->GetYaxis()->SetTitle("ToT / ns");

    chistName = fName + "_ToT_MAPMT_ac";
    chistTitle = fName + " ToT MA of fibers ac";
    fh_ToT_MA_Fib_ac = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 500, 0., 100.);
    fh_ToT_MA_Fib_ac->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_MA_Fib_ac->GetYaxis()->SetTitle("ToT / ns");

    // ToT MAPMT max:
    chistName = fName + "_ToT_MAPMT_max";
    chistTitle = fName + " max ToT of fibers";
    fh_ToT_MA_Fib_max = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 500, 0., 100.);
    fh_ToT_MA_Fib_max->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_MA_Fib_max->GetYaxis()->SetTitle("ToT / ns");

    chistName = fName + "_ToT_SAPMT_raw";
    chistTitle = fName + " ToT SA of fibers raw";
    fh_ToT_SA_Fib_raw = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 500, 0., 100.);
    fh_ToT_SA_Fib_raw->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_SA_Fib_raw->GetYaxis()->SetTitle("ToT / ns");

    // ToF Tofd -> Fiber:
    chistName = fName + "_tof";
    chistTitle = fName + " ToF";
    fh_Fib_ToF = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_Fib_ToF->GetYaxis()->SetTitle("ToF / ns");
    fh_Fib_ToF->GetXaxis()->SetTitle("Fiber ID");

    chistName = fName + "_tof_ac";
    chistTitle = fName + " ToF ac";
    fh_Fib_ToF_ac = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_Fib_ToF_ac->GetYaxis()->SetTitle("ToF / ns");
    fh_Fib_ToF_ac->GetXaxis()->SetTitle("Fiber ID");

    // tmapmt:
    chistName = fName + "_MAPMT_time";
    chistTitle = fName + " MAPMT time";
    fh_tmapmt = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_tmapmt->GetYaxis()->SetTitle("tMAPMT / ns");
    fh_tmapmt->GetXaxis()->SetTitle("Fiber ID");

    chistName = fName + "_MAPMT_time_ac";
    chistTitle = fName + " MAPMT time ac";
    fh_tmapmt_ac = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_tmapmt_ac->GetYaxis()->SetTitle("tMAPMT / ns");
    fh_tmapmt_ac->GetXaxis()->SetTitle("Fiber ID");

    // tsapmt:
    chistName = fName + "_SAPMT_time";
    chistTitle = fName + " SAPMT time";
    fh_tsapmt = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_tsapmt->GetYaxis()->SetTitle("tSAPMT / ns");
    fh_tsapmt->GetXaxis()->SetTitle("Fiber ID");

    chistName = fName + "_SAPMT_time_ac";
    chistTitle = fName + " SAPMT time ac";
    fh_tsapmt_ac = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_tsapmt_ac->GetYaxis()->SetTitle("tSAPMT / ns");
    fh_tsapmt_ac->GetXaxis()->SetTitle("Fiber ID");

    // tcorell:
    chistName = fName + "_t_corell";
    chistTitle = fName + " t corell";
    fh_tcorell = new TH2F(chistName.Data(), chistTitle.Data(), 2000, -1000., 1000., 2000, -1000., 1000.);
    fh_tcorell->GetYaxis()->SetTitle("time MA / ns");
    fh_tcorell->GetXaxis()->SetTitle("time SA / ns ");

    // ecorell:
    chistName = fName + "_SAtot_vs_MAtot";
    chistTitle = fName + " SAtot vs MAtot";
    fh_ecorell = new TH2F(chistName.Data(), chistTitle.Data(), 50, 0., 10., 50, 0., 10.);
    fh_ecorell->GetXaxis()->SetTitle("sqrt(tot) SA /sqrt(ns)");
    fh_ecorell->GetYaxis()->SetTitle("sqrt(tot) MA /sqrt(ns) ");

    // ToT single PMT:
    chistName = fName + "_ToT_SAPMT";
    chistTitle = fName + " ToT of fibers";
    fh_ToT_Single_Fib = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 2500, 0., 500.);
    fh_ToT_Single_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_Single_Fib->GetYaxis()->SetTitle("ToT / ns");

    chistName = fName + "_ToT_SAPMT_ac";
    chistTitle = fName + " ToT SA of fibers ac";
    fh_ToT_Single_Fib_ac = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 2500, 0., 500.);
    fh_ToT_Single_Fib_ac->GetXaxis()->SetTitle("Fiber number");
    fh_ToT_Single_Fib_ac->GetYaxis()->SetTitle("ToT / ns");

    chistName = fName + "_ToT_SAPMT_ch";
    chistTitle = fName + " ToT SA of fibers ch";
    fh_tot_SA_ch = new TH2F(chistName.Data(), chistTitle.Data(), 5, 0, 5, 2500, 0., 500.);
    fh_tot_SA_ch->GetXaxis()->SetTitle("SA channel");
    fh_tot_SA_ch->GetYaxis()->SetTitle("ToT / ns");

    chistName = fName + "_x_vs_y";
    chistTitle = fName + " x_vs y";
    fh_x_vs_y = new TH2F(chistName.Data(), chistTitle.Data(), 5000, -25., 25., 5000, -25., 25.);
    fh_x_vs_y->GetXaxis()->SetTitle("x / cm");
    fh_x_vs_y->GetYaxis()->SetTitle("y / cm");

    // ToT SAPMT:
    for (Int_t i = 0; i < 4; i++)
    {
        char number[15];
        snprintf(number, sizeof(number), "%d", i);
        chistName = fName + "_ToT_SAPMT" + number;
        chistTitle = fName + " ToT of single PMTs " + number;
        fh_ToT_s_Fib[i] = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 2500, 0., 500.);
        fh_ToT_s_Fib[i]->GetXaxis()->SetTitle("Fiber number");
        fh_ToT_s_Fib[i]->GetYaxis()->SetTitle("ToT / ns");
    }

    // ToT vs ToT SPMT:
    chistName = fName + "_ToT1_ToT2";
    chistTitle = fName + " raw ToT1 vs ToT2 of single PMTs";
    fh_ToT1_ToT2 = new TH2F(chistName.Data(), chistTitle.Data(), 500, 0., 100., 500, 0., 100.);
    fh_ToT1_ToT2->GetXaxis()->SetTitle("SPMT 1");
    fh_ToT1_ToT2->GetYaxis()->SetTitle("SPMT 2");

    chistName = fName + "_ToT3_ToT4";
    chistTitle = fName + " raw ToT3 vs ToT4 of single PMTs";
    fh_ToT1_ToT3 = new TH2F(chistName.Data(), chistTitle.Data(), 500, 0., 100., 500, 0., 100.);
    fh_ToT1_ToT3->GetXaxis()->SetTitle("SPMT 3");
    fh_ToT1_ToT3->GetYaxis()->SetTitle("SPMT 4");

    chistName = fName + "_ToT1_ToT4";
    chistTitle = fName + " raw ToT1 vs ToT4 of single PMTs";
    fh_ToT1_ToT4 = new TH2F(chistName.Data(), chistTitle.Data(), 500, 0., 100., 500, 0., 100.);
    fh_ToT1_ToT4->GetXaxis()->SetTitle("SPMT 1");
    fh_ToT1_ToT4->GetYaxis()->SetTitle("SPMT 4");

    chistName = fName + "_ToT1_ToT2_ac";
    chistTitle = fName + " calib ToT1 vs ToT2 of single PMTs ac";
    fh_ToT1_ToT2_ac = new TH2F(chistName.Data(), chistTitle.Data(), 500, 0., 100., 500, 0., 100.);
    fh_ToT1_ToT2_ac->GetXaxis()->SetTitle("SPMT 1");
    fh_ToT1_ToT2_ac->GetYaxis()->SetTitle("SPMT 2");

    chistName = fName + "_ToT3_ToT4_ac";
    chistTitle = fName + " calib ToT3 vs ToT4 of single PMTs ac";
    fh_ToT1_ToT3_ac = new TH2F(chistName.Data(), chistTitle.Data(), 500, 0., 100., 500, 0., 100.);
    fh_ToT1_ToT3_ac->GetXaxis()->SetTitle("SPMT 3");
    fh_ToT1_ToT3_ac->GetYaxis()->SetTitle("SPMT 4");

    chistName = fName + "_ToT1_ToT4_ac";
    chistTitle = fName + " calib ToT1 vs ToT4 of single PMTs ac";
    fh_ToT1_ToT4_ac = new TH2F(chistName.Data(), chistTitle.Data(), 500, 0., 100., 500, 0., 100.);
    fh_ToT1_ToT4_ac->GetXaxis()->SetTitle("SPMT 1");
    fh_ToT1_ToT4_ac->GetYaxis()->SetTitle("SPMT 4");

    // time difference SPMT - MAPMT:
    chistName = fName + "_dt";
    chistTitle = fName + " dt of fibers";
    fh_dt_Fib = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_dt_Fib->GetXaxis()->SetTitle("Fiber number");
    fh_dt_Fib->GetYaxis()->SetTitle("dt / ns");

    chistName = fName + "_dt_ac";
    chistTitle = fName + " dt of fibers ac";
    fh_dt_Fib_ac = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_dt_Fib_ac->GetXaxis()->SetTitle("Fiber number");
    fh_dt_Fib_ac->GetYaxis()->SetTitle("dt / ns");

    // trigger time difference SPMT - MAPMT:
    chistName = fName + "_dt_trigg";
    chistTitle = fName + " dt trig all events";
    fh_dttrig_all = new TH2F(chistName.Data(), chistTitle.Data(), NFibMax, 0, NFibMax, 5000, -10000., 10000.);
    fh_dttrig_all->GetXaxis()->SetTitle("Fiber number");
    fh_dttrig_all->GetYaxis()->SetTitle("dt trigger/ ns");

    chistName = fName + "_Eloss_vs_tof";
    chistTitle = fName + " Eloss vs ToF";
    fh_lowMtot = new TH2F(chistName.Data(), chistTitle.Data(), 500, 0., 100., 2000, -1000., 1000.);
    fh_lowMtot->GetXaxis()->SetTitle("Eloss /ns");
    fh_lowMtot->GetYaxis()->SetTitle("ToF / ns");

    chistName = fName + "_Mtot_vs_NEvents";
    chistTitle = fName + " Mtot vs NEvents";
    fh_Mtot_vs_NEvents = new TH2F(chistName.Data(), chistTitle.Data(), 100000, 0, Nmax, 500, 0., 100.);
    fh_Mtot_vs_NEvents->GetXaxis()->SetTitle("Event number");
    fh_Mtot_vs_NEvents->GetYaxis()->SetTitle("Eloss / ns");

    chistName = fName + "_fibId_vs_NEvents";
    chistTitle = fName + " fibId vs NEvents";
    fh_fibId_vs_NEvents = new TH2F(chistName.Data(), chistTitle.Data(), 100000, 0, Nmax, 1024, 0., 1024.);
    fh_fibId_vs_NEvents->GetXaxis()->SetTitle("Event number");
    fh_fibId_vs_NEvents->GetYaxis()->SetTitle("fiber ID");

    chistName = fName + "_dtot_vs_dt";
    chistTitle = fName + " dToT vs dt";
    fh_dt_dtot = new TH2F(chistName.Data(), chistTitle.Data(), 1000, 0., 1000., 500, -250., 250.);
    fh_dt_dtot->GetXaxis()->SetTitle("dtime two hits /ns");
    fh_dt_dtot->GetYaxis()->SetTitle("dtot two hits / ns");

    chistName = fName + "_time_vs_hit";
    chistTitle = fName + " time vs hit num";
    fh_t_ihit = new TH2F(chistName.Data(), chistTitle.Data(), 1000, 0., 1000., 2000, -1000, 1000);
    fh_t_ihit->GetXaxis()->SetTitle("hit number");
    fh_t_ihit->GetYaxis()->SetTitle("time / ns");

    chistName = fName + "_tot_vs_hit";
    chistTitle = fName + " tot vs hit num";
    fh_tot_ihit = new TH2F(chistName.Data(), chistTitle.Data(), 1000, 0., 1000., 500, 0, 250);
    fh_tot_ihit->GetXaxis()->SetTitle("hit number");
    fh_tot_ihit->GetYaxis()->SetTitle("tot / ns");

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
        LOG(error) << "R3BFiberCal2Hit::Init() Couldn't get " << fName << "HitPar. ";
    }
}

void R3BBunchedFiberCal2Hit::Exec(Option_t* option)
{

    // cout<<"in exec "<<fName<<endl;
    //	if(fnEvents/10000.==(int)fnEvents/10000) cout<<"Events: "<<fnEvents<<"         \r"<<std::flush;

    if (fnEvents / 100000. == (int)fnEvents / 100000)
    {
        std::cout << "\rEvents: " << fnEvents << " / " << maxevent << " (" << (int)(fnEvents * 100. / maxevent)
                  << " %) " << std::flush;
        cout << " " << endl;
    }

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

    /*size_t*/ cal_num = fCalItems->GetEntriesFast();
    //  cout << "calnum: " << cal_num << endl;

    // if(cal_num < 1) return;
    // if(cal_num%2 == 1) return;

    size_t mapmt_trig_num = fMAPMTCalTriggerItems->GetEntries();
    // cout<<"mapmt_trig_num = "<<mapmt_trig_num<<endl;

    double trig_time[8];
    Double_t tl, tt; // lead and trile times of the trigger

    std::vector<R3BBunchedFiberCalData const*> mapmt_trig_table(fSubNum * fChPerSub[0] / 128);
    // cout<<"mapmt_trig_table size: "<<fName<<", "<<fSubNum * fChPerSub[0] / 128<<endl;

    for (size_t j = 0; j < mapmt_trig_num; ++j)
    {
        auto cal = (R3BBunchedFiberCalData const*)fMAPMTCalTriggerItems->At(j);
        // cout<<"Trigger Map: "<<fName<<", "<<j<<"; "<<cal->GetChannel()<<endl;
        mapmt_trig_table.at(cal->GetChannel() - 1) = cal;
        tl = cal->GetTime_ns();
        trig_time[j] = tl;
        // cout << "mapmt trig time " << fName << " j: " << j << " time: " << tl << endl;
    }

    // cout << "mapmt trig read " << fName << " " << mapmt_trig_num << endl;

    // TODO: This will create a map for every fiber detector... Urg.
    // Also since it's shared between many detectors it must be dynamic, for now.
    size_t spmt_trig_num = 0;
    if (fChPerSub[1] > 0)
    {
        spmt_trig_num = fSPMTCalTriggerItems->GetEntries();
    }
    // cout << "singleCalTrigger: " << spmt_trig_num << endl;
    std::vector<R3BBunchedFiberCalData const*> spmt_trig_table;
    for (size_t j = 0; j < spmt_trig_num; ++j)
    {
        auto cal = (R3BBunchedFiberCalData const*)fSPMTCalTriggerItems->At(j);
        auto idx = cal->GetChannel() - 1;
        if (idx >= spmt_trig_table.size())
            spmt_trig_table.resize(idx + 1);
        spmt_trig_table.at(idx) = cal;
    }

    //  cout << "sapmt trig read " << fName << "  " << spmt_trig_num << endl;

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

    // cout<<"channel side read "<<fName<<endl;

    if (n_lead != n_trail)
    {
        //    return;
    }
    summmpt = 0;
    summsm1 = 0;
    summsm2 = 0;
    summsm3 = 0;
    summsm4 = 0;

    for (size_t j = 0; j < cal_num; ++j)
    {
        auto cur_cal = (R3BBunchedFiberCalData const*)fCalItems->At(j);

        //   cout<<"Cal2Hit *** "<<fName<<", Nev="<<fnEvents<<", iHit="<<j<<
        // ", iCha="<<cur_cal->GetChannel() <<", isLead="<<cur_cal->IsLeading()<<
        //", time="<<cur_cal->GetTime_ns()<<endl;

        if (cur_cal->IsTrailing())
        {
            auto side_i = cur_cal->IsMAPMT() ? 0 : 1;

            Double_t c_period = 0 == side_i ? 4096. * (1000. / fClockFreq) // CTDC
                                            : 2048. * (1000. / 200.);      // Tamex

            auto ch_i = cur_cal->GetChannel() - 1;

            auto& channel = fChannelArray[side_i].at(ch_i);
            if (channel.lead_list.empty())
            {
                continue;
            }
            auto lead = channel.lead_list.front();

            Double_t cur_cal_trig_ns = 0;
            Double_t lead_trig_ns = 0;
            Double_t lead_raw = 0;
            Double_t trail_raw = 0;
            Double_t trail_trig_ns = 0;
            if (cur_cal->IsMAPMT() && fMAPMTTriggerMap)
            {
                auto cur_cal_trig_i = fMAPMTTriggerMap[ch_i];
                auto lead_trig_i = fMAPMTTriggerMap[lead->GetChannel() - 1];
                if (cur_cal_trig_i < mapmt_trig_table.size() && lead_trig_i < mapmt_trig_table.size())
                {
                    auto cur_cal_trig = mapmt_trig_table.at(cur_cal_trig_i);
                    auto lead_trig = mapmt_trig_table.at(lead_trig_i);
                    cur_cal_trig_ns = cur_cal_trig->GetTime_ns();
                    lead_trig_ns = lead_trig->GetTime_ns();

                    //  cout<<"Detector: "<<fName<<", trig times: "<<lead_trig_ns<<", "<<cur_cal_trig_ns<<", times: "<<
                    //       lead->GetTime_ns()<<", "<<cur_cal->GetTime_ns()<<endl;
                }
            }
            else if (cur_cal->IsSPMT() && fSPMTTriggerMap && 1 == 1) // Don't use this for s454
            {
                auto cur_cal_trig_i = fSPMTTriggerMap[ch_i];
                auto lead_trig_i = fSPMTTriggerMap[lead->GetChannel() - 1];
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

            //	if(side_i==0) cout<<"Trigger "<<fName<<", "<<lead->GetTime_ns()<<", "<<lead_trig_ns<<",
            //"<<lead->GetChannel()<<
            //	", "<<fMAPMTTriggerMap[lead->GetChannel() - 1]<<", "<<cur_cal->GetChannel()<<endl;

            lead_raw = lead->GetTime_ns();
            trail_raw = cur_cal->GetTime_ns();
            trail_trig_ns = cur_cal_trig_ns;

            // auto lead_ns = cur_cal->GetTime_ns();
            // auto tot_ns = lead->GetTime_ns();//cur_cal_ns;
            // if(tot_ns < 0.) tot_ns = 0./0.;
            // cout << fName << " side: " << side_i << " trigger time " << cur_cal_trig_ns  << " trig time: "
            //	 << lead_trig_ns << " time " << lead_ns << endl;

            if (side_i == 0)
                summmpt += 1;
            if (side_i == 1 && ch_i == 0)
                summsm1 += 1;
            if (side_i == 1 && ch_i == 1)
                summsm2 += 1;
            if (side_i == 1 && ch_i == 2)
                summsm3 += 1;
            if (side_i == 1 && ch_i == 3)
                summsm4 += 1;

            if (tot_ns < 0)
                tot_ns = -1. * tot_ns;
            if (tot_ns < 1000. && tot_ns > 0.)
            {
                // channel.tot_list.push_back(ToT(lead, cur_cal, tot_ns)); -> this lead to: t1>t2>t3....; we need
                // t1<t2<t3....

                //    if(fName=="Fi1b" && side_i==1) cout<<"First: "<<fName<<", "<<side_i<<"; "<<lead->GetChannel()<<",
                //    "<<lead_ns<<"; "<<tot_ns<<endl;

                if (side_i == 0)
                    channel.tot_list.push_front(ToT(
                        lead, cur_cal, lead_raw, trail_raw, lead_ns, cur_cal_ns, tot_ns, lead_trig_ns, trail_trig_ns));
                else
                    channel.tot_list.push_back(ToT(
                        lead, cur_cal, lead_raw, trail_raw, lead_ns, cur_cal_ns, tot_ns, lead_trig_ns, trail_trig_ns));
                channel.lead_list.pop_front();
            }
        }
    }

    cond = true;
    if (summsm1 + summsm2 + summsm3 + summsm4 < 1)
    {
        cond = false;
        //	return;
    }

    fh_hit1->Fill(summmpt, summsm1 + summsm2 + summsm3 + summsm4);
    fh_hit2->Fill(summsm1 + summsm3, summsm2 + summsm4);

    tot_mapmt_max = -1;
    tot_spmt_max = -1;
    tot_mapmt_max_fiber_id = 0;
    tot_spmt_max_fiber_id = 0;

    double tof;
    // cout<<"permutations"<<endl;
    s1 = -99.;
    s2 = -99.;
    s3 = -99.;
    s4 = -99.;
    s1_ac = -99.;
    s2_ac = -99.;
    s3_ac = -99.;
    s4_ac = -99.;
    summsm1_ac = 0;
    summsm2_ac = 0;
    summsm3_ac = 0;
    summsm4_ac = 0;
    for (int i = 0; i < 512; i++)
    {
        mtemp[i] = 0;
        for (int j = 0; j < 100; j++)
        {
            ttemp[j][i] = 0. / 0.;
            etemp[j][i] = 0.;
        }
    }

    // Make every permutation to create fibers.
    auto const& mapmt_array = fChannelArray[0];
    auto const& spmt_array = fChannelArray[1];
    multi = 0;
    summmpt_ac = 0;
    Int_t isumNMA = 0;
    for (auto it_mapmt = mapmt_array.begin(); mapmt_array.end() != it_mapmt; ++it_mapmt) // over MA channel 0...nmax-1
    {
        if (!(mapmt_array.begin() == mapmt_array.end()))
            isumNMA += 1;
        auto const& mapmt = *it_mapmt;
        Double_t tlmem0 = 0. / 0.;
        Double_t tdiff0 = 0. / 0.;
        Int_t isumNhitMA = 0;
        Double_t emem0 = 0. / 0.;
        Double_t ttmem0 = 0. / 0.;
        // ohne Trigger
        Double_t tlmem1 = 0. / 0.;
        Double_t emem1 = 0. / 0.;
        Double_t ttmem1 = 0. / 0.;
        Double_t triglmem = 0. / 0.;
        Double_t trigtmem = 0. / 0.;
        // cout<<"list size "<<mapmt.tot_list.size()<<endl;
        for (auto it_mapmt_tot = mapmt.tot_list.begin(); mapmt.tot_list.end() != it_mapmt_tot;
             ++it_mapmt_tot) // over ihit(fiber)
        {
            auto const& mapmt_tot = *it_mapmt_tot;

            if (isumNhitMA == 0)
            {
                tlmem0 = mapmt_tot.lead_ns;
                emem0 = mapmt_tot.tot_ns;
                ttmem0 = mapmt_tot.trail_ns;
                tlmem1 = mapmt_tot.lead_raw;
                ttmem1 = mapmt_tot.trail_raw;
                emem1 = ttmem1 - tlmem1;
                triglmem = mapmt_tot.lead_trig_ns;
                trigtmem = mapmt_tot.trail_trig_ns;
            }

            int mch = mapmt_tot.lead->GetChannel() - 1;

            mtemp[mch]++;
            ttemp[isumNhitMA][mch] = mapmt_tot.lead_ns;
            etemp[isumNhitMA][mch] = mapmt_tot.tot_ns;

            isumNhitMA += 1;

            // if(isumNhitMA > 1) continue;    // only first hit in each event

            auto fiber_id_ch = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] + 1;

            tdiff0 = (mapmt_tot.lead_ns - tlmem0);

            if (mapmt_tot.lead_raw - tlmem1 < 0 && fnEvents == 499)
            {
                cout << "Leading " << fName << ", " << fnEvents << ", " << mch + 1 << ", " << tlmem1 << ", "
                     << mapmt_tot.lead_raw << ", " << triglmem << ", " << mapmt_tot.lead_trig_ns << endl;
                cout << "Trailin " << fName << ", " << fnEvents << ", " << mch + 1 << ", " << ttmem1 << ", "
                     << mapmt_tot.trail_raw << ", " << trigtmem << ", " << mapmt_tot.trail_trig_ns << endl;
            }
            tlmem0 = mapmt_tot.lead_ns;
            emem0 = mapmt_tot.tot_ns;
            ttmem0 = mapmt_tot.trail_ns;
            tlmem1 = mapmt_tot.lead_raw;
            ttmem1 = mapmt_tot.trail_raw;
            triglmem = mapmt_tot.lead_trig_ns;
            trigtmem = mapmt_tot.trail_trig_ns;

            //   if(fName=="Fi1b") cout<<"Second: "<<fName<<", "<<"0; "<<mapmt_tot.lead->GetChannel()<<",
            //   "<<mapmt_tot.lead_ns<<"; "<<mapmt_tot.tot_ns<<endl;

            int single = 0;
            Int_t isumNSA = 0;
            // if there is a single PMT:
            //            if(fChPerSub[1] > 0)
            // for (auto it_spmt = spmt_array.begin(); spmt_array.end() != it_spmt; ++it_spmt) // over No of SAPMT
            {

                // auto const& spmt = *it_spmt;
                Int_t isumNhitSA = 0;
                Double_t tmem = 0. / 0.;
                Double_t tdiff = 0. / 0.;
                // if (!(spmt.tot_list.end() == spmt.tot_list.begin()))
                // isumNSA += 1;
                // if(isumNSA > 1) continue;

                // for (auto it_spmt_tot = spmt.tot_list.begin(); spmt.tot_list.end() != it_spmt_tot;
                //     ++it_spmt_tot) // over ihit in each SAPMT
                {
                    // auto const& spmt_tot = *it_spmt_tot;

                    // if (isumNhitSA == 0)
                    //    tmem = spmt_tot.lead_ns;

                    // isumNhitSA += 1;
                    /*
                                        tdiff = spmt_tot.lead_ns - tmem;

                                        if (spmt_tot.lead->GetChannel() == 1)
                                            s1 = spmt_tot.tot_ns;
                                        if (spmt_tot.lead->GetChannel() == 2)
                                            s2 = spmt_tot.tot_ns;
                                        if (spmt_tot.lead->GetChannel() == 3)
                                            s3 = spmt_tot.tot_ns;
                                        if (spmt_tot.lead->GetChannel() == 4)
                                            s4 = spmt_tot.tot_ns;
                    */
                    // fh_ToT_SA_Fib_raw->Fill(spmt_tot.lead->GetChannel(), spmt_tot.tot_ns);

                    // Check that the combo is inside one sub-det.
                    auto mapmt_sub_id = (mapmt_tot.lead->GetChannel() - 1) / fChPerSub[0];
                    // auto spmt_sub_id = (spmt_tot.lead->GetChannel() - 1) / fChPerSub[1];
                    // if (mapmt_sub_id != spmt_sub_id)
                    //{
                    //    continue;
                    //}

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
                    // auto fiber_id = (mapmt_tot.lead->GetChannel() - 1) * fChPerSub[1] +
                    //                ((spmt_tot.lead->GetChannel() - 1) % fChPerSub[1]) + 1;

                    // single = spmt_tot.lead->GetChannel();

                    // fh_ch_corr->Fill(single, mapmt_tot.lead->GetChannel());
                    // if (mapmt_tot.lead->GetChannel() == 2)
                    //    fh_ToT_s_Fib[single - 1]->Fill(tdiff, spmt_tot.tot_ns);
                    // tmem = spmt_tot.lead_ns;

                    // Fix fiber installation mistakes.
                    fiber_id = FixMistake(fiber_id);

                    //   if(fiber_id==173 || fiber_id==181 || fiber_id==185) continue;

                    fh_dt_dtot->Fill(tdiff0, mapmt_tot.tot_ns - emem0);
                    fh_t_ihit->Fill(isumNhitMA, mapmt_tot.lead_ns);
                    fh_tot_ihit->Fill(isumNhitMA, mapmt_tot.tot_ns);

                    // Calibrate hit fiber.
                    /*
                    auto tot_spmt = spmt_tot.tot_ns;
                    Double_t tot_spmt_raw = spmt_tot.tot_ns;
                    Double_t t_spmt = spmt_tot.lead_ns;
                    Double_t t_spmt_raw = spmt_tot.lead_raw;
                    Double_t t_trig_sapmt = spmt_tot.lead_trig_ns;
                    */
                    auto tot_mapmt = mapmt_tot.tot_ns;
                    Double_t tot_mapmt_raw = mapmt_tot.tot_ns;
                    Double_t t_mapmt = mapmt_tot.lead_ns;
                    Double_t t_mapmt_raw = mapmt_tot.lead_raw;
                    Double_t t_trig_mapmt = mapmt_tot.lead_trig_ns;

                    // cout<<fName<<", "<<fiber_id<<", "<<t_mapmt_raw<<", "<<t_trig_mapmt<<", "<<t_mapmt<<endl;

                    /*
                    Double_t dt_trig = t_trig_sapmt - t_trig_mapmt;

                    while (dt_trig > 256. / 2. * 1000. / fClockFreq)
                    {
                        t_trig_mapmt += 256. * 1000. / fClockFreq;
                        dt_trig = t_trig_sapmt - t_trig_mapmt;
                    }
                    while (dt_trig < -256. / 2. * 1000. / fClockFreq)
                    {
                        t_trig_mapmt -= 256. * 1000. / fClockFreq;
                        dt_trig = t_trig_sapmt - t_trig_mapmt;
                    }

                    Double_t dtime = t_spmt - t_mapmt;

                    while (dtime > 256. / 2. * 1000. / fClockFreq)
                    {
                        t_mapmt += 256. * 1000. / fClockFreq;
                        dtime = t_spmt - t_mapmt;
                    }
                    while (dtime < -256. / 2. * 1000. / fClockFreq)
                    {
                        t_mapmt -= 256. * 1000. / fClockFreq;
                        dtime = t_spmt - t_mapmt;
                    }
                    */
                    // tof = (t_mapmt + t_spmt) / 2.;
                    tof = t_mapmt;

                    // fh_dttrig_all->Fill(fiber_id, dt_trig);

                    fh_ToT_MA_Fib_raw->Fill(fiber_id, tot_mapmt_raw);
                    // fh_ToT_SA_Fib_raw->Fill(fiber_id, tot_spmt_raw);

                    fh_Fib_ToF->Fill(fiber_id, tof);
                    // fh_dt_Fib->Fill(fiber_id, dtime);

                    // Apply calibration.
                    Double_t gainMA = gainM_temp[fiber_id - 1]; // 10.;
                    // Double_t gainSA = gainS_temp[fiber_id - 1];    // 10.;
                    Double_t offset1 = offset1_temp[fiber_id - 1]; // 0.;
                    Double_t offset2 = offset2_temp[fiber_id - 1]; // 0.;
                    Double_t tsync = tsync_temp[fiber_id - 1];     // 0.;

                    if (!fIsCalibrator && fHitPar)
                    {
                        R3BBunchedFiberHitModulePar* par = fHitPar->GetModuleParAt(fiber_id);
                        if (par)
                        {
                            gainMA = par->GetGainMA();
                            // gainSA = par->GetGainS();
                            tsync = par->GetSync();
                            offset1 = par->GetOffset1();
                            offset2 = par->GetOffset2();
                        }
                    }

                    // t_mapmt += offset1;
                    // t_spmt += offset2;
                    tot_mapmt *= 20. / gainMA;
                    // tot_spmt *= 20. / gainSA;
                    // tof  -= tsync;
                    // tof = (t_spmt + t_mapmt) / 2. - tsync;
                    t_mapmt -= tsync;
                    tof = t_mapmt;

                    // fh_tot_SA_ch->Fill(single, tot_spmt);

                    fh_tmapmt->Fill(fiber_id, t_mapmt);
                    // fh_tsapmt->Fill(fiber_id, t_spmt);

                    fh_ToT_MA_Fib->Fill(fiber_id, tot_mapmt);

                    if (s_mult > 0)
                    {
                        // fh_ToT_Single_Fib->Fill(fiber_id, tot_spmt);
                        // fh_ToT_s_Fib[single - 1]->Fill(fiber_id, tot_spmt);
                    }

                    // Int_t numFibs = fSubNum * fChPerSub[0];
                    Int_t numFibs = fSubNum * fChPerSub[0] * fChPerSub[1];
                    Double_t x = -10000.;
                    Double_t y = -10000.;
                    Double_t randx;
                    Double_t randy;
                    Double_t t_spmt = 0.;

                    if (fName == "Fi10" || fName == "Fi11" || fName == "Fi12" || fName == "Fi13")
                    {
                        Float_t fiber_thickness = 0.050000; // MH remove *2 when taking SPMT into analysis
                        Int_t fiber_nbr = 1024;             // MH remove /2 when taking SPMT into analysis
                        Float_t dead_layer = 0.9;
                        Float_t air_layer = 0.01; // relative to fiber_thickness
                        Float_t detector_width = fiber_nbr * fiber_thickness * (1. + air_layer);

                        if (fDirection == VERTICAL)
                        {
                            x = -detector_width / 2. + fiber_thickness / 2. +
                                ((double)(fiber_id - 1) + ((double)(fiber_id - 1) * air_layer)) * fiber_thickness;
                            y = (t_spmt - t_mapmt) * 3.;
                        }
                        else
                        {
                            x = (t_spmt - t_mapmt) * 3.;
                            y = -detector_width / 2. + fiber_thickness / 2. +
                                ((double)(fiber_id - 1) + ((double)(fiber_id - 1) * air_layer)) * fiber_thickness;
                        }
                        // cout << "fiber_id " << fiber_id << " pos " << x << endl;
                    }
                    if (fName == "Fi1a" || fName == "Fi1b" || fName == "Fi2a" || fName == "Fi2b" || fName == "Fi3a" ||
                        fName == "Fi3b")
                    {
                        Float_t fiber_thickness = 0.021000; // MH remove *2 when taking SPMT into analysis
                        Int_t fiber_nbr = 256;              // MH remove /2 when taking SPMT into analysis
                        Float_t dead_layer = 0.9;
                        Float_t air_layer = 0.01; // relative to fiber_thickness
                        Float_t detector_width = fiber_nbr * fiber_thickness * (1. + air_layer);
                        if (fDirection == VERTICAL)
                        {
                            x = -detector_width / 2. + fiber_thickness / 2. +
                                ((double)(fiber_id - 1) + ((double)(fiber_id - 1) * air_layer)) * fiber_thickness;
                            y = (t_spmt - t_mapmt) * 3.;
                        }
                        else
                        {
                            x = (t_spmt - t_mapmt) * 3.;
                            y = -detector_width / 2. + fiber_thickness / 2. +
                                ((double)(fiber_id - 1) + ((double)(fiber_id - 1) * air_layer)) * fiber_thickness;
                        }
                    }
                    if (fName == "Fi0")
                    {
                        Float_t fiber_thickness = 0.2; // MH remove *2 when taking SPMT into analysis
                        Int_t fiber_nbr = 128;         // MH remove /2 when taking SPMT into analysis
                        Float_t dead_layer = 0.9;
                        Float_t air_layer = 0.01; // relative to fiber_thickness
                        Float_t detector_width = fiber_nbr * fiber_thickness * (1. + air_layer);
                        if (fDirection == VERTICAL)
                        {
                            x = -detector_width / 2. + fiber_thickness / 2. +
                                ((double)(fiber_id - 1) + ((double)(fiber_id - 1) * air_layer)) * fiber_thickness;
                            y = 0.;
                        }
                        else
                        {
                            x = 0.;
                            y = -detector_width / 2. + fiber_thickness / 2. +
                                ((double)(fiber_id - 1) + ((double)(fiber_id - 1) * air_layer)) * fiber_thickness;
                        }
                    }

                    // cout<<"Fiber y " << y << endl;
                    // MH                    if (y < -100 || y > 100)
                    // MH                    {
                    // MH                        continue;
                    // MH                    }
                    // Double_t eloss = sqrt(tot_mapmt * tot_spmt);
                    Double_t eloss = tot_mapmt;
                    // Double_t t = (t_spmt_raw + t_mapmt_raw) / 2.; // tof;
                    //  Double_t t = t_spmt;
                    Double_t t = tof;

                    /*
                    if ((tof > -30. && tof < 30.) && (dtime > -20. && dtime < 20.))
                    {
                        if (single == 1 && tot_spmt > 0.)
                            s1_ac = tot_spmt;
                        if (single == 2 && tot_spmt > 0.)
                            s2_ac = tot_spmt;
                        if (single == 3 && tot_spmt > 0.)
                            s3_ac = tot_spmt;
                        if (single == 4 && tot_spmt > 0.)
                            s3_ac = tot_spmt;
                    }
                    */
                    energy[fiber_id] = eloss;
                    counts[fiber_id] = counts[fiber_id] + 1;

                    fh_iFib_nHit->Fill(fiber_id, isumNhitMA);

                    //   if((tof > -30. && tof < 30.) && (dtime > -20. && dtime < 20.) && eloss > 0. && tot_spmt < 50.){
                    if (eloss > tot_mapmt_max)
                    {
                        tot_mapmt_max = eloss;
                        tot_mapmt_max_fiber_id = fiber_id;

                        // tot_spmt_max = tot_spmt;
                        // tot_spmt_max_fiber_id = fiber_id;
                    }

                    //  if(fName=="Fi1a" || (fName=="Fi1b" && fiber_id > 115 && fiber_id < 135))
                    {
                        fh_Mtot_vs_NEvents->Fill(fnEvents, eloss);
                        fh_fibId_vs_NEvents->Fill(fnEvents, fiber_id);
                        fh_lowMtot->Fill(eloss, tof);
                    }
                    fh_x_vs_y->Fill(x, y);
                    // fh_ecorell->Fill(sqrt(tot_spmt), sqrt(tot_mapmt));
                    fh_ToT_ifib->Fill(fiber_id, eloss);
                    fh_tcorell->Fill(t_spmt, t_mapmt);
                    fh_tmapmt_ac->Fill(fiber_id, t_mapmt);
                    fh_tsapmt_ac->Fill(fiber_id, t_spmt);
                    fh_Fib_ToF_ac->Fill(fiber_id, tof);
                    // fh_dt_Fib_ac->Fill(fiber_id, dtime);
                    // fh_ToT_Single_Fib_ac->Fill(fiber_id, tot_spmt);
                    fh_ToT_MA_Fib_ac->Fill(fiber_id, tot_mapmt);
                    fh_iFib_nHit_ac->Fill(fiber_id, isumNhitMA);

                    if (tot_mapmt > 0. && isumNhitSA == 1 && isumNSA == 1)
                        summmpt_ac++;

                    /*
                    if (isumNhitMA == 1 && isumNMA == 1)
                    {
                        if (single == 1 && tot_spmt > 0.)
                            summsm1_ac++;
                        if (single == 2 && tot_spmt > 0.)
                            summsm2_ac++;
                        if (single == 3 && tot_spmt > 0.)
                            summsm3_ac++;
                        if (single == 4 && tot_spmt > 0.)
                            summsm4_ac++;
                    }
                    */
                    multi++;
                    mpmt_ch.push_back(fiber_id_ch);
                    // spmt_ch.push_back(single);
                    //  }
                    Double_t t_spmt_raw = 0.;
                    Double_t tot_spmt = 0.;
                    if (!fIsCalibrator)
                    {
                        new ((*fHitItems)[fNofHitItems++])
                            R3BBunchedFiberHitData(0, x, y, eloss, t, fiber_id, t_mapmt, t_spmt, tot_mapmt, tot_spmt);
                        // cout << "saved hit: " << fiber_id << "  " << t << endl;

                        //   if(fName=="Fi1b")  cout<<"Third: "<<fName<<", "<<"0; "<<fiber_id<<", "<<t<<";
                        //   "<<eloss<<endl;
                    }
                } // over it_mapmt_tot
            }     // it_mapmt
        }
    }

    fh_ToT1_ToT2->Fill(s1, s2);
    fh_ToT1_ToT3->Fill(s3, s4);
    fh_ToT1_ToT4->Fill(s1, s4);
    fh_ToT1_ToT2_ac->Fill(s1_ac, s2_ac);
    fh_ToT1_ToT3_ac->Fill(s3_ac, s4_ac);
    fh_ToT1_ToT4_ac->Fill(s1_ac, s4_ac);

    fnEvents++;

    //  cout<<"order "<<tfirst<<", "<<tsecond<<", "<<tthird<<", "<<tfourth<<endl;

    // cout<<"end exec"<<endl;
}

void R3BBunchedFiberCal2Hit::FinishEvent()
{
    fHitItems->Clear();
    fNofHitItems = 0;
    Double_t t1, t2, t3, t4;
    // if (cal_num > 0 && (summsm1_ac + summsm2_ac + summsm3_ac + summsm4_ac) > 0)
    {
        Int_t norm = summmpt_ac;
        if (norm < 1)
            norm = 1;
        // fh_hit1_ac->Fill(summmpt_ac, (summsm1_ac + summsm2_ac + summsm3_ac + summsm4_ac));
        // fh_hit2_ac->Fill(summsm1_ac + summsm3_ac, summsm2_ac + summsm4_ac);

        //  cout<<"max values: "<<fName<<", "<<tot_mapmt_max_fiber_id<<", "<< tot_mapmt_max<<endl;
        fh_ToT_MA_Fib_max->Fill(tot_mapmt_max_fiber_id, tot_mapmt_max);

        if (multi > 0)
            fh_multi->Fill(multi);
    }

    multi = 0;
    summsm1_ac = 0;
    summsm2_ac = 0;
    summsm3_ac = 0;
    summsm4_ac = 0;
    summmpt_ac = 0;
    mpmt_ch.clear();
    spmt_ch.clear();
}

void R3BBunchedFiberCal2Hit::FinishTask()
{

    fh_ToT_MA_Fib->Write();
    fh_ToT_MA_Fib_ac->Write();
    fh_ToT_MA_Fib_max->Write();
    fh_ToT_Single_Fib->Write();
    fh_ToT_Single_Fib_ac->Write();
    fh_dt_Fib->Write();
    fh_Fib_ToF->Write();
    fh_dt_Fib_ac->Write();
    fh_Fib_ToF_ac->Write();
    fh_ecorell->Write();
    fh_tcorell->Write();
    fh_hit1->Write();
    fh_hit2->Write();
    fh_hit1_ac->Write();
    fh_hit2_ac->Write();
    fh_tmapmt->Write();
    fh_tsapmt->Write();
    fh_tmapmt_ac->Write();
    fh_tsapmt_ac->Write();
    fh_dttrig_all->Write();
    fh_ToT_MA_Fib_raw->Write();
    fh_ToT_SA_Fib_raw->Write();
    fh_x_vs_y->Write();
    fh_ch_corr->Write();
    fh_ToT_ifib->Write();
    fh_tot_SA_ch->Write();
    fh_iFib_nHit->Write();
    fh_iFib_nHit_ac->Write();
    fh_ToT1_ToT2->Write();
    fh_ToT1_ToT3->Write();
    fh_ToT1_ToT4->Write();
    fh_ToT1_ToT2_ac->Write();
    fh_ToT1_ToT3_ac->Write();
    fh_ToT1_ToT4_ac->Write();
    fh_multi->Write();
    fh_lowMtot->Write();
    fh_Mtot_vs_NEvents->Write();
    fh_fibId_vs_NEvents->Write();
    fh_dt_dtot->Write();
    fh_t_ihit->Write();
    fh_tot_ihit->Write();
    for (Int_t i = 0; i < 4; i++)
    {
        fh_ToT_s_Fib[i]->Write();
    }

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
                //   R3BBunchedFiberHitModulePar* par = fCalPar->GetModuleParAt(i);
                Double_t gain;
                Double_t offs;

                // time offset
                R3BBunchedFiberHitModulePar* par1 = fCalPar->GetModuleParAt(i);
                TH1D* proj1 = fh_dt_Fib->ProjectionY("", i + 1, i + 1, 0);
                par1->SetOffset1(0.5 * proj1->GetBinCenter(proj1->GetMaximumBin()));
                par1->SetOffset2(-0.5 * proj1->GetBinCenter(proj1->GetMaximumBin()));

                // tsync
                R3BBunchedFiberHitModulePar* par2 = fCalPar->GetModuleParAt(i);
                TH1D* proj2 = fh_Fib_ToF->ProjectionY("", i + 1, i + 1, 0);
                Double_t tsync = proj2->GetBinCenter(proj2->GetMaximumBin());
                par2->SetSync(proj2->GetBinCenter(proj2->GetMaximumBin()));

                if (i < 257)
                    cout << fName << " Fiber: " << i << ", tsync" << proj2->GetBinCenter(proj2->GetMaximumBin())
                         << ", offset: " << 0.5 * proj1->GetBinCenter(proj1->GetMaximumBin()) << endl;
            }
        }
        if (fIsGain)
        {
            for (UInt_t i = 1; i <= max; i++)
            {
                if (!fIsTsync)
                {
                    R3BBunchedFiberHitModulePar* par4 = fCalPar->GetModuleParAt(i);
                    par4->SetSync(tsync_temp[i - 1]);
                    par4->SetOffset1(offset1_temp[i - 1]);
                    par4->SetOffset2(offset2_temp[i - 1]);
                }

                // gain MA
                TH1D* proj = fh_ToT_MA_Fib_raw->ProjectionY("", i + 1, i + 1, 0);
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

                        if (i < 257)
                            cout << fName << " Fiber: " << i << " gain MA: " << proj->GetBinCenter(j) << endl;
                        break;
                    }
                }
                // gain SA
                TH1D* proj3 = fh_ToT_SA_Fib_raw->ProjectionY("", i + 1, i + 1, 0);
                for (UInt_t j = proj3->GetNbinsX() - 2; j > 2; j--)
                {
                    if (j == 2)
                    {
                        // could not find maximum
                    }
                    if (proj3->GetBinContent(j) > proj3->GetMaximum() * 20. / 100.)
                    {
                        R3BBunchedFiberHitModulePar* par3 = fCalPar->GetModuleParAt(i);
                        par3->SetGainS(proj->GetBinCenter(j));

                        if (i < 257)
                            cout << fName << " Fiber: " << i << " gain SA: " << proj3->GetBinCenter(j) << endl;
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
    cout << "Set trigger map: "
         << "MAPMT trigger map length=" << c_length << ", sub#=" << fSubNum << " mapmt_per_sub=" << fChPerSub[0]
         << endl;
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
    cout << "Set trigger map: "
         << "SPMT trigger map length=" << c_length << ", sub#=" << fSubNum << " spmt_per_sub=" << fChPerSub[1] << endl;
    fSPMTTriggerMap = a_map;
}

ClassImp(R3BBunchedFiberCal2Hit)
