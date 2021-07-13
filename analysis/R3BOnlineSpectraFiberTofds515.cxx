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
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BFrsData.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
//#include "R3BOnlineSpectra.h"
#include "R3BOnlineSpectraFiberTofds515.h"

#include "R3BRoluCalData.h"
#include "R3BRoluMappedData.h"

#include "R3BSci8CalData.h"
#include "R3BSci8MappedData.h"

#include "R3BBeamMonitorMappedData.h"

#include "R3BTofdCalData.h"
#include "R3BTofdMappedData.h"

#include "R3BPaddleCalData.h"
#include "R3BPaddleTamexMappedData.h"

#include "R3BPspxCalData.h"
#include "R3BPspxHitData.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxPrecalData.h"

#include "R3BMusicHitData.h"

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"

#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberMappedData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define IS_NAN(x) TMath::IsNaN(x)

#include <boost/multi_array.hpp>
#include <cassert>
#include <cmath> // NAN

template <class T>
static void init_array(T& array, double init)
{
    // boost::multi_array prefers cascaded for loops over index sets
    // (e.g. python's dict().keys())
    // thus we can not really use a range based for loop
    // and instead wrangle pointers like it is 1980 again.
    for (double* e = array.origin(); e < array.origin() + array.num_elements(); ++e)
        *e = init;
}

using namespace std;

R3BOnlineSpectraFiberTofds515::R3BOnlineSpectraFiberTofds515()
    //	: R3BOnlineSpectraFiberTofds515("OnlineSpectraFiberTofds515", 1)
    : FairTask("OnlineSpectraFiberTofds515", 1)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(5.)
    , fNofPlanes(N_PLANE_MAX_TOFD)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFD)
    , fNEvents(0)
    , fToFoffset(0)
    , fToFmin(-5000)
    , fToFmax(5000)
    , fTof2InvV_p0(-7.8)
    , fTof2InvV_p1(0.0073)
    , fL2(137)
    , fPos_p0(-11)
    , fPos_p1(54.7)
    , fDispersionS2(7000)
    , fBrho0_S2toCC(9.458)
    , fP0(-2.12371e7)
    , fP1(4.9473e7)
    , fP2(-2.87635e7)
    , fZprimary(50.)
    , fZoffset(-1.3)
    , fPos2min(-200.)
    , fPos2max(200.)
    , fHitItemsMus(NULL)
    , fHitItemsFrs(NULL)
    , ZMUSIC_cut(50)
    , ZMUSIC_wcut(0.5)
    , AoQ_cut(2.64)
    , AoQ_wcut(0.004)

{
}

R3BOnlineSpectraFiberTofds515::R3BOnlineSpectraFiberTofds515(const char* name, Int_t iVerbose)

    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(5.)
    , fNofPlanes(N_PLANE_MAX_TOFD)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFD)
    //, fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
    //, FtCALsCI2(nULL)
    , fToFoffset(0)
    , fToFmin(-5000)
    , fToFmax(5000)
    , fTof2InvV_p0(-7.8)
    , fTof2InvV_p1(0.0073)
    , fL2(137)
    , fPos_p0(-11)
    , fPos_p1(54.7)
    , fDispersionS2(7000)
    , fBrho0_S2toCC(9.458)
    , fP0(-2.12371e7)
    , fP1(4.9473e7)
    , fP2(-2.87635e7)
    , fZprimary(50.)
    , fZoffset(-1.3)
    , fPos2min(-200.)
    , fPos2max(200.)
    , fHitItemsMus(NULL)
    , fHitItemsFrs(NULL)
    , ZMUSIC_cut(50)
    , ZMUSIC_wcut(0.5)
    , AoQ_cut(2.64)
    , AoQ_wcut(0.004)
{
}

R3BOnlineSpectraFiberTofds515::~R3BOnlineSpectraFiberTofds515()
{
    //	delete fhTpat ;
    //	delete fhTrigger;
    //	delete fh_SEETRAM;

    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        delete fh_channels_Fib[i];
        delete fh_fibers_Fib[i];
        delete fh_mult_Fib[i];
        delete fh_time_Fib[i];
        delete fh_multihit_m_Fib[i];
        delete fh_multihit_s_Fib[i];
        delete fh_ToT_m_Fib[i];
        delete fh_ToT_s_Fib[i];
        delete fh_ToT_single_Fib[i];
        delete fh_channels_single_Fib[i];
        delete fh_Fib10_vs_Fib13;
        delete fh_Fib11_vs_Fib12;
        delete fh_Fib11_vs_Fib12_cut;
        delete fh_channels_Fib11_cut;
    }
    if (fHitItemsMus)
        delete fHitItemsMus;
    if (fTcalSci2)
        delete fTcalSci2;
    if (fHitItemsFrs)
        delete fHitItemsFrs;
}

InitStatus R3BOnlineSpectraFiberTofds515::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    if (!header)
        header = (R3BEventHeader*)mgr->GetObject("EventHeader.");

    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("", this);

    // Get objects for detectors on all levels

    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    printf("Have %d fiber detectors.\n", NOF_FIB_DET);
    for (int det = 0; det < DET_MAX; det++)
    {
        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }
        fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCal", fDetectorNames[det])));
        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
    }

    // get access to hit data of the MUSIC
    fHitItemsMus = (TClonesArray*)mgr->GetObject("MusicHitData");
    if (!fHitItemsMus)
        LOG(WARNING) << "R3BOnlineSpectraLosVsSci2: MusicHitData not found";
    // get access to hit data of FRS
    fHitItemsFrs = (TClonesArray*)mgr->GetObject("FrsData");
    if (!fHitItemsFrs)
        LOG(WARNING) << "R3BOnlineSpectraLosVsSci2: FrsData not found";

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    // Trigger and Tpat
    TCanvas* cTrigg = new TCanvas("Trigg", "Triggers", 10, 10, 650, 350);
    fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fhTpat->GetXaxis()->SetTitle("Tpat value");

    fhTrigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fhTrigger->GetXaxis()->SetTitle("Trigger value");

    cTrigg->Divide(2, 1);
    cTrigg->cd(1);
    gPad->SetLogy();
    fhTrigger->Draw();
    cTrigg->cd(2);
    gPad->SetLogy();
    fhTpat->Draw();
    // cTrigg->cd(0);

    //------------------------------------------------------------------------
    // Rolu detector

    if (fMappedItems.at(DET_ROLU))
    {
        TCanvas* cRolu = new TCanvas("Rolu", "Rolu", 10, 10, 800, 400);

        fh_rolu_tot = new TH2F("Rolu_tot", "ROLU ToT", 10, 0, 10, 3000, 0, 300);
        fh_rolu_tot->GetXaxis()->SetTitle("Channel number");
        fh_rolu_tot->GetYaxis()->SetTitle("ToT / ns");

        fh_rolu_channels = new TH1F("Rolu_channels", "ROLU channels", 10, 0, 10);
        fh_rolu_channels->GetXaxis()->SetTitle("Channel number");
        fh_rolu_channels->GetYaxis()->SetTitle("Counts");

        cRolu->Divide(2, 1);
        cRolu->cd(1);
        gPad->SetLogy();
        fh_rolu_channels->Draw();
        cRolu->cd(2);
        gPad->SetLogz();
        fh_rolu_tot->Draw("colz");
        // cRolu->cd(0);
        run->AddObject(cRolu);

        run->GetHttpServer()->RegisterCommand("Reset_ROLU", Form("/Objects/%s/->Reset_ROLU_Histo()", GetName()));
    }
    //--3BOnlineSpectraFibe---------------------------------------------------
    // Sci8 detector

    if (fMappedItems.at(DET_SCI8))
    {
        TCanvas* cSci8 = new TCanvas("Sci8", "SCI8", 10, 10, 850, 850);

        fh_sci8_channels = new TH1F("sci8_channels", "SCI8 channels", 4, 0., 4.);
        fh_sci8_channels->GetXaxis()->SetTitle("Channel number");
        fh_sci8_multihit = new TH1F("sci8_multihit", "SCI8 multihit", 20, 0., 20.);
        fh_sci8_multihit->GetXaxis()->SetTitle("Multihit");
        fh_sci8_multihitVFTX = new TH2F("sci8_vftx_multihit", "SCI8 VFTX multihit", 4, 0., 4., 20, 0., 20.);
        fh_sci8_multihitVFTX->GetYaxis()->SetTitle("Multihit ");
        fh_sci8_multihitVFTX->GetXaxis()->SetTitle("Channel number");
        fh_sci8_multihitLEAD = new TH2F("sci8_lead_multihit", "SCI8 TAMEX Leading multihit", 4, 0., 4., 20, 0., 20.);
        fh_sci8_multihitLEAD->GetYaxis()->SetTitle("Multihit ");
        fh_sci8_multihitLEAD->GetXaxis()->SetTitle("Channel number");
        fh_sci8_multihitTRAI = new TH2F("sci8_trai_multihit", "SCI8 TAMEX Trailing multihit", 4, 0., 4., 20, 0., 20.);
        fh_sci8_multihitTRAI->GetYaxis()->SetTitle("Multihit ");
        fh_sci8_multihitTRAI->GetXaxis()->SetTitle("Channel number");
        fh_sci8_dt_hits = new TH1F("sci8_dt_hits", "SCI8 dt multihits", 40000, -2000, 2000);
        fh_sci8_dt_hits->GetXaxis()->SetTitle("dt_VFTX between two hits / ns");
        fh_sci8_dt_hits_l = new TH1F("sci8_dt_hits_l", "SCI8 dt multihits leading", 40000, -2000, 2000);
        fh_sci8_dt_hits_l->GetXaxis()->SetTitle("dt_LEAD between two hits / ns");
        fh_sci8_dt_hits_t = new TH1F("sci8_dt_hits_t", "SCI8 dt multihits trailing", 40000, -2000, 2000);
        fh_sci8_dt_hits_t->GetXaxis()->SetTitle("dt_TRAIL between two hits / ns");
        fh_sci8_tres_MCFD = new TH1F("sci8_time_res_MCFD", "SCI8 MCFD Time resolution - raw", 30000, 0., 300.);
        fh_sci8_tres_MCFD->GetXaxis()->SetTitle("Time MCFD / ns");
        fh_sci8_tres_TAMEX = new TH1F("sci8_time_res_TAMEX", "SCI8 TAMEX Time resolution -raw ", 8000, -4., 4.);
        fh_sci8_tres_TAMEX->GetXaxis()->SetTitle("Time TAMEX / ns");
        fh_sci8_tot = new TH2F("sci8_tot", "SCI8 ToT vs PM", 4, 0, 4, 3000, 0., 300.);
        fh_sci8_tot->GetXaxis()->SetTitle("PMT number");
        fh_sci8_tot->GetYaxis()->SetTitle("ToT / ns");
        fh_sci8_tot_mean = new TH1F("sci8_tot_mean", "SCI8 mean ToT", 30000, 0., 300.);
        fh_sci8_tot_mean->GetYaxis()->SetTitle("Counts");
        fh_sci8_tot_mean->GetXaxis()->SetTitle("ToT / ns");
        fh_tof_sci8 = new TH1F("sci8_tof", "SCI8-LOS ToF", 40000, -20, 20);
        fh_tof_sci8->GetXaxis()->SetTitle("ToF / ns");

        cSci8->Divide(3, 3);
        cSci8->cd(1);
        fh_sci8_channels->Draw();
        cSci8->cd(2);
        gPad->SetLogy();
        fh_sci8_multihit->Draw();
        cSci8->cd(3);
        gPad->SetLogz();
        fh_sci8_tot->Draw("colz");
        cSci8->cd(4);
        fh_sci8_tres_MCFD->Draw();
        cSci8->cd(5);
        fh_sci8_tres_TAMEX->Draw();
        cSci8->cd(6);
        fh_tof_sci8->Draw();
        cSci8->cd(7);
        fh_sci8_tot_mean->Draw();

        // cSci8->cd(0);
        run->AddObject(cSci8);

        run->GetHttpServer()->RegisterCommand("Reset_SCI8", Form("/Objects/%s/->Reset_SCI8_Histo()", GetName()));
    }

    if (fMappedItems.at(DET_BMON))
    {

        // get the theoretical calib factors for SEETRAM
        Double_t fexp = float(fsens_SEE + 9);
        Double_t fpow = float(pow(10., fexp));
        calib_SEE = 135641.7786 * fpow;
        cout << fsens_SEE << ", " << fexp << ", " << fpow << ", " << calib_SEE << endl;

        TCanvas* cbmon = new TCanvas("Beam_Monitor", "Beam Monitors", 820, 10, 900, 900);
        Int_t Nbin_bmon = reset_time / read_time;

        fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
        fhTpat->GetXaxis()->SetTitle("Tpat value");

        fhTrigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
        fhTrigger->GetXaxis()->SetTitle("Trigger value");

        fh_spill_length = new TH1F("spill_length", "Spill ", Nbin_bmon, 0, reset_time);
        fh_spill_length->GetXaxis()->SetTitle("time / sec");

        fh_IC = new TH1F("IC", "IC ", Nbin_bmon, 0, reset_time);
        fh_IC->GetXaxis()->SetTitle("time / sec");
        fh_IC->GetYaxis()->SetTitle("IC counts");

        fh_SEE = new TH1F("SEETRAM", "SEETRAM ", Nbin_bmon, 0, reset_time);
        fh_SEE->GetXaxis()->SetTitle("time / sec");
        fh_SEE->GetYaxis()->SetTitle("SEETRAM counts");

        fh_TOFDOR = new TH1F("TOFDOR", "TOFDOR ", Nbin_bmon, 0, reset_time);
        fh_TOFDOR->GetXaxis()->SetTitle("time / sec");
        fh_TOFDOR->GetYaxis()->SetTitle("TOFDOR counts");

        fh_IC_spill = new TH1F("IC_spill", "IC rate in kHz ", Nbin_bmon, 0, reset_time);
        fh_IC_spill->GetXaxis()->SetTitle("time / sec");
        fh_IC_spill->GetYaxis()->SetTitle("IC rate / kHz");

        fh_SEE_spill = new TH1F("SEE_spill", "SEE particle rate ", Nbin_bmon, 0, reset_time);
        fh_SEE_spill->GetXaxis()->SetTitle("time / sec");
        fh_SEE_spill->GetYaxis()->SetTitle("Particles / sec");

        fh_TOFDOR_spill = new TH1F("TOFDOR_spill", "TOFDOR rate in kHz ", Nbin_bmon, 0, reset_time);
        fh_TOFDOR_spill->GetXaxis()->SetTitle("time / sec");
        fh_TOFDOR_spill->GetYaxis()->SetTitle("TOFDOR rate / kHz");

        cbmon->Divide(3, 3);
        cbmon->cd(1);
        gPad->SetLogy();
        fhTrigger->Draw();
        cbmon->cd(2);
        gPad->SetLogy();
        fhTpat->Draw();
        cbmon->cd(3);
        fh_spill_length->Draw();
        cbmon->cd(4);
        fh_IC->Draw("hist");
        //        fh_TimePreviousEvent->Draw("hist");
        cbmon->cd(5);
        fh_SEE->Draw("hist");
        cbmon->cd(6);
        fh_TOFDOR->Draw("hist");
        cbmon->cd(7);
        fh_IC_spill->Draw("hist");
        cbmon->cd(8);
        fh_SEE_spill->Draw("hist");
        cbmon->cd(9);
        fh_TOFDOR_spill->Draw("hist");
        cbmon->cd(0);

        run->AddObject(cbmon);

        run->GetHttpServer()->RegisterCommand("Reset_BMON", Form("/Objects/%s/->Reset_BMON_Histo()", GetName()));
    }

    //------------------------------------------------------------------------
    // Los detector
    /*
        if (fMappedItems.at(DET_LOS))
        {
            TCanvas* cLos = new TCanvas("Los_", "LOS", 10, 10, 910, 810);

            fh_los_channels = new TH1F("los_channels_", "LOS channels", 10, 0., 10.);
            fh_los_channels->GetXaxis()->SetTitle("Channel number");
            fh_los_multihit = new TH1F("los_multihitv", "LOS multihit && all 8 PMs", 10, 0., 10.);
            fh_los_multihit->GetXaxis()->SetTitle("Multihit");

            fh_los_pos_MCFD = new TH2F("los_pos_MCFD_", "LOS MCFD Position ", 1000, -5., 5., 1000, -5., 5.);
            fh_los_pos_MCFD->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_MCFD->GetYaxis()->SetTitle("Y position / cm");
            fh_los_pos_TAMEX = new TH2F("los_pos_TAMEX_", "LOS TAMEX Position ", 1000, -5., 5., 1000, -5., 5.);
            fh_los_pos_TAMEX->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_TAMEX->GetYaxis()->SetTitle("Y position / cm");
            fh_los_pos_ToT = new TH2F("los_pos_ToT_", "LOS ToT Position ", 1000, -5., 5., 1000, -5., 5.);
            fh_los_pos_ToT->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_ToT->GetYaxis()->SetTitle("Y position / cm");
            fh_los_dt_hits_ToT =
                new TH2F("los_dt_ToT_", "LOS ToT dt ", 6000, 0, 3000, 2000, 0, 200.); // 6000,0,3000,2000,0,200.
            fh_los_dt_hits_ToT->GetXaxis()->SetTitle("dt between two hits / usec");  // dt between two hits / ns
            fh_los_dt_hits_ToT->GetYaxis()->SetTitle("ToT / ns");                    // ToT / ns
            fh_los_dt_first_ToT = new TH2F("los_dt_events_ToT_", "LOS ToT dtevents ", 5000, 0, 50000, 500, 0, 200.);
            fh_los_dt_first_ToT->GetXaxis()->SetTitle("dt between two events / usec");
            fh_los_dt_first_ToT->GetYaxis()->SetTitle("ToT / ns");

            fh_los_dt_hits = new TH1F("los_dt_vhits_", "LOS dt hits", 100000, 0, 10000);
            fh_los_dt_hits->GetXaxis()->SetTitle("dt between two hits / micros");
            fh_los_dt_hits->GetYaxis()->SetTitle("Counts");

            fh_los_tres_MCFD = new TH1F("los_time_res_MCFD_", "LOS MCFD Time resolution - raw", 4000, -4., 4.);
            fh_los_tres_MCFD->GetXaxis()->SetTitle("Time MCFD / ns");
            fh_los_tres_TAMEX = new TH1F("los_time_res_TAMEX_", "LOS TAMEX Time resolution -raw ", 4000, -4., 4.);
            fh_los_tres_TAMEX->GetXaxis()->SetTitle("Time TAMEX / ns");
            fh_los_tres_MTDC = new TH1F("los_time_res_MTDC_", "LOS MTDC Time resolution - raw", 4000, -4., 4.);
            fh_los_tres_MTDC->GetXaxis()->SetTitle("Time MTDC / ns");
            fh_los_tot = new TH2F("los_tot", "LOS ToT vs PMTv", 10, 0, 10, 3000, 0., 300.);
            fh_los_tot->GetXaxis()->SetTitle("PMT number");
            fh_los_tot->GetYaxis()->SetTitle("ToT / ns");
            fh_los_tot_mean = new TH1F("los_tot_mean_", "LOS mean ToT", 1500, 0., 300.);
            fh_los_tot_mean->GetYaxis()->SetTitle("Counts");
            fh_los_tot_mean->GetXaxis()->SetTitle("ToT / ns");
            fh_los_ihit_ToT = new TH2F("los_tot_ihit_", "LOS ToT vs ihit", 10, 0, 10, 3100, -10., 300.);
            fh_los_ihit_ToT->GetXaxis()->SetTitle("iHit");
            fh_los_ihit_ToT->GetYaxis()->SetTitle("ToT / ns");

            cLos->Divide(4, 3);
            cLos->cd(1);
            fh_los_channels->Draw();
            cLos->cd(2);
            gPad->SetLogy();
            fh_los_multihit->Draw();
            cLos->cd(3);
            gPad->SetLogz();
            fh_los_tot->Draw("colz");
            cLos->cd(4);
            gPad->SetLogy();
            fh_los_tot_mean->Draw();
            cLos->cd(5);
            gPad->SetLogz();
            fh_los_ihit_ToT->Draw("colz");
            cLos->cd(6);
            gPad->SetLogz();
            fh_los_dt_hits_ToT->Draw("colz");
            cLos->cd(7);
            gPad->SetLogy();
            fh_los_tres_MTDC->Draw();
            cLos->cd(8);
            gPad->SetLogy();
            fh_los_tres_MCFD->Draw();
            cLos->cd(9);
            gPad->SetLogy();
            fh_los_tres_TAMEX->Draw();
            cLos->cd(10);
            gPad->SetLogz();
            fh_los_pos_ToT->Draw("colz");
            cLos->cd(11);
            gPad->SetLogz();
            fh_los_pos_MCFD->Draw("colz");
            cLos->cd(12);
            gPad->SetLogz();
            fh_los_pos_TAMEX->Draw("colz");
            cLos->cd(0);
            run->AddObject(cLos);

            run->GetHttpServer()->RegisterCommand("Reset_LOS", Form("/Objects/%s/->Reset_LOS_Histo()", GetName()));
        }
    */
    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    TCanvas* FibCanvas[NOF_FIB_DET];

    // Fiber vs Fiber channels
    fh_Fib10_vs_Fib13 = new TH2F("Fib10_vs_Fib13", "Fib10_vs_Fib13", 520, 0, 520, 520, 0, 520);
    fh_Fib11_vs_Fib12 = new TH2F("Fib11_vs_Fib12", "Fib11_vs_Fib12", 520, 0, 520, 520, 0, 520);
    fh_Fib11_vs_Fib12_cut =
        new TH2F("Fib11_vs_Fib12_ZMUSIC_cut", "Fib11_vs_Fib12_ZMUSIC_cut", 520, 0, 520, 520, 0, 520);
    fh_channels_Fib11_cut = new TH1F("Fib11_channels_ZMUSIC_cut", "Fib11_channels_ZMUSIC_cut", 520, 0, 520);
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {

        std::stringstream fibhitobj;
        std::stringstream fibmapobj;
        std::stringstream FiName;
        std::string temp;

        if (fMappedItems.at(DET_FI_FIRST + ifibcount))
        {
            const char* chistName;
            const char* chistTitle;
            const char* detName;
            std::string tempTitle;
            std::string tempName;
            std::stringstream tempFibName;
            std::string tempFibNames;
            std::stringstream tempCanvName;

            detName = fDetectorNames[DET_FI_FIRST + ifibcount];

            FibCanvas[ifibcount] = new TCanvas(detName, detName, 10, 10, 910, 910);

            // Channels:
            fh_channels_Fib[ifibcount] =
                new TH1F(Form("%s_channels", detName), Form("%s channels", detName), 520, 0., 520.);
            fh_channels_Fib[ifibcount]->GetXaxis()->SetTitle("Channel number");
            fh_channels_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Channels:
            fh_channels_single_Fib[ifibcount] =
                new TH1F(Form("%s_channels_single", detName), Form("%s channels of single PMTs", detName), 10, 0., 10.);
            fh_channels_single_Fib[ifibcount]->GetXaxis()->SetTitle("Channel number");
            fh_channels_single_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Fibers:
            fh_fibers_Fib[ifibcount] =
                new TH1F(Form("%s_fibers", detName), Form("%s fibers", detName), N_FIBER_PLOT, 0., N_FIBER_PLOT);
            fh_fibers_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_fibers_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multiplicity (number of hit fibers):
            fh_mult_Fib[ifibcount] = new TH1F(Form("%s_mult", detName), Form("%s mult", detName), 100, 0., 100.);
            fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multihit MAPMT:
            fh_multihit_m_Fib[ifibcount] = new TH2F(
                Form("%s_multihit_m", detName), Form("%s multihits MAPMT", detName), 520, 0., 520., 20, 0., 20.);
            fh_multihit_m_Fib[ifibcount]->GetXaxis()->SetTitle("MAPMT channel");
            fh_multihit_m_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // Multihit SAPMT:
            fh_multihit_s_Fib[ifibcount] = new TH2F(
                Form("%s_multihit_s", detName), Form("%s multihits single PMT", detName), 16, 0., 16., 20, 0., 20.);
            fh_multihit_s_Fib[ifibcount]->GetXaxis()->SetTitle("Single PMT channel");
            fh_multihit_s_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // ToT MAPMT:
            fh_ToT_m_Fib[ifibcount] = new TH2F(Form("%s_tot_m", detName),
                                               Form("%s ToT of MAPMT", detName),
                                               N_FIBER_PLOT,
                                               0.,
                                               N_FIBER_PLOT,
                                               200,
                                               0.,
                                               20.);
            fh_ToT_m_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_m_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToT SAPMT:
            fh_ToT_s_Fib[ifibcount] = new TH2F(Form("%s_tot_s", detName),
                                               Form("%s ToT of single PMT", detName),
                                               N_FIBER_PLOT,
                                               0.,
                                               N_FIBER_PLOT,
                                               400,
                                               0.,
                                               40.);
            fh_ToT_s_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_s_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // Time of fiber:
            fh_time_Fib[ifibcount] = new TH2F(Form("%s_TimevsFiber", detName),
                                              Form("%s Time vs Fiber", detName),
                                              N_FIBER_PLOT,
                                              0.,
                                              N_FIBER_PLOT,
                                              2048,
                                              -1024.,
                                              1024.);
            fh_time_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_time_Fib[ifibcount]->GetYaxis()->SetTitle("tMAPMT-tSPMT");

            // ToF LOS -> Fiber:
            fh_Fib_ToF[ifibcount] = new TH2F(Form("%s_tof", detName),
                                             Form("%s ToF LOS to Fiber", detName),
                                             N_FIBER_PLOT,
                                             0.,
                                             N_FIBER_PLOT,
                                             10000,
                                             -5000.,
                                             5000.);
            fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("Fiber number");

            // Not-calibrated position:
            fh_Fib_pos[ifibcount] =
                new TH1F(Form("%s_pos", detName), Form("%s Not-calibrated position", detName), 2100, -1050., 1050.);
            fh_Fib_pos[ifibcount]->GetXaxis()->SetTitle("Position");
            fh_Fib_pos[ifibcount]->GetYaxis()->SetTitle("Counts");

            // hit fiber number vs. event number:
            fh_Fib_vs_Events[ifibcount] = new TH2F(Form("%s_fib_vs_event", detName),
                                                   Form("%s Fiber # vs. Event #", detName),
                                                   10000,
                                                   0,
                                                   5e6,
                                                   1100,
                                                   0.,
                                                   1100.);
            fh_Fib_vs_Events[ifibcount]->GetYaxis()->SetTitle("Fiber number");
            fh_Fib_vs_Events[ifibcount]->GetXaxis()->SetTitle("Event number");

            // ToT SAPMT:
            fh_ToT_single_Fib[ifibcount] = new TH2F(
                Form("%s_tot_single", detName), Form("%s ToT of single PMTs", detName), 10, 0., 10., 400, 0., 40.);
            fh_ToT_single_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_single_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            FibCanvas[ifibcount]->Divide(3, 4);
            FibCanvas[ifibcount]->cd(1);
            fh_channels_Fib[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(2);
            gPad->SetLogz();
            fh_multihit_m_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(3);
            gPad->SetLogz();
            fh_multihit_s_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(4);
            fh_fibers_Fib[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(5);
            fh_mult_Fib[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(6);
            gPad->SetLogz();
            fh_ToT_m_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(7);
            gPad->SetLogz();
            fh_ToT_s_Fib[ifibcount]->Draw("colz");
            // FibCanvas[ifibcount]->cd(8);gPad->SetLogz();
            // fh_time_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(8);
            gPad->SetLogz();
            fh_Fib_ToF[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(9);
            fh_Fib_pos[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(10);
            fh_Fib_vs_Events[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(11);
            fh_ToT_single_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(12);
            fh_channels_single_Fib[ifibcount]->Draw();

            FibCanvas[ifibcount]->cd(0);
            run->AddObject(FibCanvas[ifibcount]);

        } // end if(Mapped)

    } // end for(ifibcount)
    TCanvas* Fib_corr = new TCanvas("Fib_corr", "Fib_coor", 10, 10, 910, 910);
    Fib_corr->Divide(2, 2);
    Fib_corr->cd(1);
    gPad->SetLogz();
    fh_Fib10_vs_Fib13->Draw("colz");
    Fib_corr->cd(2);
    gPad->SetLogz();
    fh_Fib11_vs_Fib12->Draw("colz");
    Fib_corr->cd(3);
    gPad->SetLogz();
    fh_Fib11_vs_Fib12_cut->Draw("colz");
    Fib_corr->cd(4);
    fh_channels_Fib11_cut->SetLineColor(kRed);
    fh_channels_Fib11_cut->Draw();
    // fh_channels_Fib[2]->Draw("same");

    if (NOF_FIB_DET > 0)
        run->GetHttpServer()->RegisterCommand("Reset_Fibers", Form("/Objects/%s/->Reset_FIBERS_Histo()", GetName()));

    //---------------------------------------------------------------------------------------
    // Ptof detector

    if (fCalItems.at(DET_PTOF))
    {

        TCanvas* cPtof_plane = new TCanvas("Ptof_plane", "Ptof plane", 10, 10, 510, 510);
        cPtof_plane->Divide(1, 2);

        fh_ptof_channels = new TH1F("Ptof_channels", "Ptof channels", 65, 0., 65.);
        cPtof_plane->cd(1);
        fh_ptof_channels->Draw();

        fh_ptof_channels_cut = new TH1F("Ptof_channels_cut", "Ptof channels with cut", 65, 0., 65.);
        cPtof_plane->cd(2);
        fh_ptof_channels_cut->Draw();
        run->AddObject(cPtof_plane);

        TCanvas* cPtof_test = new TCanvas("Ptof_test", "Ptof test", 10, 10, 510, 510);
        cPtof_test->Divide(1, 1);

        fh_ptof_test1 = new TH1F("Ptof_test1", "Ptof test1", 1000, 0., 100.);
        fh_ptof_test2 = new TH1F("Ptof_test2", "Ptof test2", 1000, 0., 100.);
        cPtof_test->cd(1);
        fh_ptof_test1->Draw();
        //		cPtof_test->cd(2);
        //		fh_ptof_test2->Draw();

        run->AddObject(cPtof_test);

        TCanvas* cPtof1;
        cPtof1 = new TCanvas("Ptof1", "Ptof1", 10, 10, 500, 500);
        cPtof1->Divide(3, 3);
        for (Int_t j = 1; j < 10; j++)
        {
            char strName[255];
            sprintf(strName, "ToT_Bar_%d_PM_1", j);
            fh_ptof_TotPm1[j] = new TH1F(strName, "", 1000, 0., 100.);
            cPtof1->cd(j);
            fh_ptof_TotPm1[j]->Draw();
        }
        run->AddObject(cPtof1);

        TCanvas* cPtof2;
        cPtof2 = new TCanvas("Ptof2", "Ptof2", 10, 10, 500, 500);
        cPtof2->Divide(3, 3);
        for (Int_t j = 1; j < 10; j++)
        {
            char strName[255];
            sprintf(strName, "ToT_Bar_%d_PM_2", j);
            fh_ptof_TotPm2[j] = new TH1F(strName, "", 1000, 0., 100.);
            cPtof2->cd(j);
            fh_ptof_TotPm2[j]->Draw();
        }
        run->AddObject(cPtof2);
    }

    //---------------------------------------------------------------------------------------------------
    // TofD detector

    if (fMappedItems.at(DET_TOFD))
    {
        TCanvas* cTofd_planes = new TCanvas("TOFD_planes", "TOFD planes", 10, 10, 1100, 1000);
        cTofd_planes->Divide(5, 4);

        TCanvas* cTofd_corr = new TCanvas("Tofd_corr_raw", "Tofd_corr_raw", 10, 10, 1100, 1100);
        cTofd_corr->Divide(2, 2);

        fh_TimePreviousEvent = new TH1F("TimePreviousEvent", "Time between 2 particles ", 300000, -3000, 3000);
        fh_TimePreviousEvent->GetXaxis()->SetTitle("time / µsec");
        fh_TimePreviousEvent->GetYaxis()->SetTitle("counts");

        for (Int_t j = 0; j < 4; j++)
        {
            char strName1[255];
            sprintf(strName1, "tofd_channels_plane_%d", j + 1);
            char strName2[255];
            sprintf(strName2, "Tofd channels plane %d", j + 1);
            fh_tofd_channels[j] = new TH1F(strName1, strName2, 90, -45., 45.);
            fh_tofd_channels[j]->GetXaxis()->SetTitle("Channel");
            fh_tofd_channels[j]->GetYaxis()->SetTitle("Counts");

            char strName3[255];
            sprintf(strName3, "tofd_ToT_plane_%d", j + 1);
            char strName4[255];
            sprintf(strName4, "Tofd ToT plane %d", j + 1);
            fh_tofd_TotPm[j] = new TH2F(strName3, strName4, 90, -45, 45, 3000, 0., 300.);
            fh_tofd_TotPm[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_TotPm[j]->GetYaxis()->SetTitle("ToT / ns");

            char strName7[255];
            sprintf(strName7, "tofd_ToF_plane_%d", j + 1);
            char strName8[255];
            sprintf(strName8, "TOFD-LOS ToF plane %d", j + 1);
            fh_tofd_ToF[j] = new TH2F(strName7, strName8, 15, 0, 15, 1000, 0, 100);
            fh_tofd_ToF[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_ToF[j]->GetYaxis()->SetTitle("ToF / ns");

            char strName9[255];
            sprintf(strName9, "tofd_multihit_plane_%d", j + 1);
            char strName10[255];
            sprintf(strName10, "Tofd multihit plane %d", j + 1);
            fh_tofd_multihit[j] = new TH2F(strName9, strName10, 45, 0., 45., 10, 0, 10);
            fh_tofd_multihit[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_multihit[j]->GetYaxis()->SetTitle("Multihit");

            char strName11[255];
            sprintf(strName11, "tofd_side1_vs_side2_plane_%d", j + 1);
            tofd_side1_vs_side2[j] = new TH2F(strName11, strName11, 44, 0., 44, 44, 0., 44);
            tofd_side1_vs_side2[j]->GetXaxis()->SetTitle("Bar side1");
            tofd_side1_vs_side2[j]->GetYaxis()->SetTitle("Bar side2");

            char strName12[255];
            sprintf(strName12, "tofd_eloss_plane_%d", j + 1);
            fh_tofd_eloss[j] = new TH2F(strName12, strName12, 44, 0., 44, 1600, 0, 400);
            fh_tofd_eloss[j]->GetXaxis()->SetTitle("Bar");
            fh_tofd_eloss[j]->GetYaxis()->SetTitle("Eloss");

            if (j < 3)
            {
                int jk = j + 1;
                char strName5[255];
                sprintf(strName5, "tofd_ToF_plane_%d_%d", jk, jk + 1);
                char strName6[255];
                sprintf(strName6, "Tofd ToF plane %d and %d ", jk, jk + 1);
                fh_tofd_dt[j] = new TH2F(strName5, strName6, 50, 0, 50, 800, -40., 40);
                fh_tofd_dt[j]->GetXaxis()->SetTitle("Bar number");
                fh_tofd_dt[j]->GetYaxis()->SetTitle("dt / ns");
            }
        }

        cTofd_corr->cd(1);
        tofd_side1_vs_side2[0]->Draw("colz");
        cTofd_corr->cd(2);
        tofd_side1_vs_side2[1]->Draw("colz");
        cTofd_corr->cd(3);
        tofd_side1_vs_side2[2]->Draw("colz");
        cTofd_corr->cd(4);
        tofd_side1_vs_side2[3]->Draw("colz");

        cTofd_planes->cd(1);
        fh_tofd_channels[0]->Draw();
        gPad->SetLogy();
        cTofd_planes->cd(2);
        gPad->SetLogz();
        fh_tofd_TotPm[0]->Draw("colz");
        cTofd_planes->cd(3);
        gPad->SetLogz();
        // fh_tofd_ToF[0]->Draw("colz");
        fh_tofd_eloss[0]->Draw("colz");
        cTofd_planes->cd(4);
        gPad->SetLogz();
        fh_tofd_multihit[0]->Draw("colz");

        cTofd_planes->cd(6);
        fh_tofd_channels[1]->Draw();
        gPad->SetLogy();
        cTofd_planes->cd(7);
        gPad->SetLogz();
        fh_tofd_TotPm[1]->Draw("colz");
        cTofd_planes->cd(8);
        gPad->SetLogz();
        // fh_tofd_ToF[1]->Draw("colz");
        fh_tofd_eloss[1]->Draw("colz");
        cTofd_planes->cd(9);
        gPad->SetLogz();
        fh_tofd_multihit[1]->Draw("colz");
        cTofd_planes->cd(10);
        gPad->SetLogz();
        fh_tofd_dt[0]->Draw("colz");

        cTofd_planes->cd(11);
        fh_tofd_channels[2]->Draw();
        gPad->SetLogy();
        cTofd_planes->cd(12);
        gPad->SetLogz();
        fh_tofd_TotPm[2]->Draw("colz");
        cTofd_planes->cd(13);
        gPad->SetLogz();
        // fh_tofd_ToF[2]->Draw("colz");
        fh_tofd_eloss[2]->Draw("colz");
        cTofd_planes->cd(14);
        gPad->SetLogz();
        fh_tofd_multihit[2]->Draw("colz");
        cTofd_planes->cd(15);
        gPad->SetLogz();
        fh_tofd_dt[1]->Draw("colz");

        cTofd_planes->cd(16);
        fh_tofd_channels[3]->Draw();
        gPad->SetLogy();
        cTofd_planes->cd(17);
        gPad->SetLogz();
        fh_tofd_TotPm[3]->Draw("colz");
        cTofd_planes->cd(18);
        gPad->SetLogz();
        // fh_tofd_ToF[3]->Draw("colz");
        fh_tofd_eloss[3]->Draw("colz");
        cTofd_planes->cd(19);
        gPad->SetLogz();
        fh_tofd_multihit[3]->Draw("colz");
        cTofd_planes->cd(20);
        gPad->SetLogz();
        fh_tofd_dt[2]->Draw("colz");

        cTofd_planes->cd(0);
        run->AddObject(cTofd_planes);

        run->GetHttpServer()->RegisterCommand("Reset_TOFD", Form("/Objects/%s/->Reset_TOFD_Histo()", GetName()));
    }

    // -------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // PSPX detector
    // get access to data

    if (fMappedItems.at(DET_PSPX) || fCalItems.at(DET_PSPX) || fHitItems.at(DET_PSPX))
    {
        TCanvas* cPspx_comp = new TCanvas("Pspx_comp", "Pspx Comparison", 10, 10, 1100, 1000);
        cPspx_comp->Divide(N_PSPX, 3);

        Int_t Emax = 1000000;

        if (fMappedItems.at(DET_PSPX))
        {
            // LOG(INFO) << "Init MappedPspx";

            for (UInt_t i = 0; i < N_PSPX; i++)
            {
                if (i % 2 == 0)
                { // even numbers = read out with energy filter
                    fh_pspx_channel_x[i] =
                        new TH1F(Form("pspx_%d_energyreadout_channel_x", i / 2),
                                 Form("Pspx %d Energy Readout: x Channel;x Position / Channel;Counts", i / 2 + 1),
                                 2 * N_STRIPS_PSPX,
                                 1,
                                 2 * N_STRIPS_PSPX + 1);
                    fh_pspx_channel_y[i] =
                        new TH1F(Form("pspx_%d_energyreadout_channel_y", i / 2),
                                 Form("Pspx %d Energy Readout: y Channel;y Position / Channel;Counts", i / 2 + 1),
                                 2 * N_STRIPS_PSPX,
                                 2 * N_STRIPS_PSPX + 1,
                                 4 * N_STRIPS_PSPX + 1);

                    fh_pspx_multiplicity_x[i] =
                        new TH1F(Form("pspx_%d_energyreadout_multiplicity_x", i / 2),
                                 Form("Pspx %d Energy Readout: x Multiplicity;Multiplicity;Counts", i / 2 + 1),
                                 10,
                                 0,
                                 10);
                    fh_pspx_multiplicity_y[i] =
                        new TH1F(Form("pspx_%d_energyreadout_multiplicity_y", i / 2),
                                 Form("Pspx %d Energy Readout: y Multiplicity;Multiplicity;Counts", i / 2 + 1),
                                 10,
                                 0,
                                 10);
                    fh_pspx_strips_position[i] = new TH2F(
                        Form("pspx_%d_energyreadout_strips", i),
                        Form("Pspx %d Energy Readout Position;x Position / Strips; y Position / Strips", i / 2 + 1),
                        N_STRIPS_PSPX,
                        1,
                        N_STRIPS_PSPX + 1,
                        N_STRIPS_PSPX,
                        N_STRIPS_PSPX + 1,
                        2 * N_STRIPS_PSPX + 1);
                }
                else
                { // odd numbers = read out with position filter
                    fh_pspx_channel_x[i] =
                        new TH1F(Form("pspx_%d_positionreadout_channel_x", i / 2),
                                 Form("Pspx %d Position Readout: x Channel;x Position / Channel;Counts", i / 2 + 1),
                                 2 * N_STRIPS_PSPX,
                                 1,
                                 2 * N_STRIPS_PSPX + 1);
                    fh_pspx_channel_y[i] =
                        new TH1F(Form("pspx_%d_positionreadout_channel_y", i / 2),
                                 Form("Pspx %d Position Readout: y Channel;y Position / Channel;Counts", i / 2 + 1),
                                 2 * N_STRIPS_PSPX,
                                 2 * N_STRIPS_PSPX + 1,
                                 4 * N_STRIPS_PSPX + 1);

                    fh_pspx_multiplicity_x[i] =
                        new TH1F(Form("pspx_%d_positionreadout_multiplicity_x", i / 2),
                                 Form("Pspx %d Position Readout: x Multiplicity;Multiplicity;Counts", i / 2 + 1),
                                 10,
                                 0,
                                 10);
                    fh_pspx_multiplicity_y[i] =
                        new TH1F(Form("pspx_%d_positionreadout_multiplicity_y", i / 2),
                                 Form("Pspx %d Position Readout: y Multiplicity;Multiplicity;Counts", i / 2 + 1),
                                 10,
                                 0,
                                 10);
                    fh_pspx_strips_position[i] = new TH2F(
                        Form("pspx_%d_positionreadout_strips", i),
                        Form("Pspx %d Position Readout Position;x Position / Strips; y Position / Strips", i / 2 + 1),
                        N_STRIPS_PSPX,
                        1,
                        N_STRIPS_PSPX + 1,
                        N_STRIPS_PSPX,
                        N_STRIPS_PSPX + 1,
                        2 * N_STRIPS_PSPX + 1);
                }
            }

            TCanvas* cPspx_strips = new TCanvas("Pspx_strips", "Pspx Strips", 10, 10, 1100, 1000);
            cPspx_strips->Divide(N_PSPX, 2);

            for (UInt_t i = 0; i < N_PSPX; i++)
            {
                cPspx_strips->cd(i + 1);
                fh_pspx_channel_x[i]->Draw();

                cPspx_strips->cd(i + 1 + N_PSPX);
                fh_pspx_channel_y[i]->Draw();
            }

            run->AddObject(cPspx_strips);

            TCanvas* cPspx_multiplicity = new TCanvas("Pspx_multiplicity", "Pspx Multiplicity", 10, 10, 1100, 1000);
            cPspx_multiplicity->Divide(N_PSPX, 2);

            for (UInt_t i = 0; i < N_PSPX; i++)
            {
                cPspx_multiplicity->cd(i + 1);
                fh_pspx_multiplicity_x[i]->Draw();

                cPspx_multiplicity->cd(i + 1 + N_PSPX);
                fh_pspx_multiplicity_y[i]->Draw();
            }

            run->AddObject(cPspx_multiplicity);

            // Fill cPspx_comp with Mapped level data
            for (UInt_t i = 0; i < N_PSPX; i++)
            {
                cPspx_comp->cd(i + 1);
                fh_pspx_strips_position[i]->Draw("colz");

                // Remove the current axis
                fh_pspx_strips_position[i]->GetYaxis()->SetLabelOffset(999);
                fh_pspx_strips_position[i]->GetYaxis()->SetTickLength(0);

                // Redraw the new inverted axis
                gPad->Update();
                TGaxis* newaxis = new TGaxis(gPad->GetUxmin(),
                                             gPad->GetUymax(),
                                             gPad->GetUxmin() - 0.001,
                                             gPad->GetUymin(),
                                             fh_pspx_strips_position[i]->GetYaxis()->GetXmin(),
                                             fh_pspx_strips_position[i]->GetYaxis()->GetXmax(),
                                             510,
                                             "+");
                newaxis->SetLabelOffset(0.003);
                newaxis->SetLabelSize(0.03);
                newaxis->SetTickLength(0.025);
                newaxis->Draw();
            }
        }

        if (fCalItems.at(DET_PSPX))
        {
            UInt_t nbins = 200;

            for (UInt_t i = 0; i < N_PSPX; i++)
            {

                if (i % 2 == 0)
                {
                    fh_pspx_cal_energy_frontback[i] = new TH2F(
                        Form("pspx_%d_energy_frontback", i / 2),
                        Form("Pspx %d Energy Back vs Front;Front Energy / arb.u.;Back Energy / arb.u.", i / 2 + 1),
                        nbins,
                        0,
                        Emax,
                        nbins,
                        -Emax,
                        0);
                }
                else
                {
                    fh_pspx_cal_energy_frontback[i] = new TH2F(
                        Form("pspx_%d_position_frontback", i / 2),
                        Form("Pspx %d Energy Back vs Front;Front Energy / arb.u.;Back Energy / arb.u.", i / 2 + 1),
                        nbins,
                        0,
                        Emax,
                        nbins,
                        -Emax,
                        0);
                }
            }

            // Fill cPspx_comp with Cal level data
            for (UInt_t i = 0; i < N_PSPX; i++)
            {

                cPspx_comp->cd(i + 1 + N_PSPX); // i*2
                fh_pspx_cal_energy_frontback[i]->Draw("colz");
            }
        }
        if (fHitItems.at(DET_PSPX))
        {
            UInt_t nbins = 100;
            Double_t length = 10; // detector length, range of histogram
            for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
            {
                fh_pspx_hit_position[i] = new TH2F(Form("pspx_%d_position_cm", i),
                                                   Form("Pspx %d Position;x Position / cm; y Position / cm", i + 1),
                                                   nbins * 10,
                                                   -length / 2.,
                                                   length / 2.,
                                                   nbins * 10,
                                                   -length / 2.,
                                                   length / 2.);

                fh_pspx_hit_energy[i] = new TH1F(Form("pspx_%d_energy", i),
                                                 Form("Pspx %d Energy;Energy / arb.u.; Counts", i + 1),
                                                 nbins * 10,
                                                 0,
                                                 2 * Emax);

                fh_pspx_hit_multi[i] = new TH2F(
                    Form("pspx_%d_multi", i), Form("Pspx %d Multi;x Multi ; y Multi", i + 1), 20, 0, 20, 20, 0, 20);
            }

            TCanvas* cPspx_hit = new TCanvas("Pspx_hit", "Pspx Hit", 10, 10, 1100, 1000);
            cPspx_hit->Divide((N_PSPX + 1) / 2, 3);

            for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
            {
                cPspx_hit->cd(i + 1);
                gPad->SetLogz();
                fh_pspx_hit_position[i]->Draw("colz");

                cPspx_hit->cd(i + 1 + (N_PSPX + 1) / 2);
                gPad->SetLogy();
                fh_pspx_hit_energy[i]->Draw();

                cPspx_hit->cd(i + 1 + (N_PSPX + 1) / 2 + 3);
                gPad->SetLogz();
                fh_pspx_hit_multi[i]->Draw("colz");
            }

            run->AddObject(cPspx_hit);

            // Fill cPspx_comp with Hit level data-((channel_y[i][0] + 1) / 2) + 3 * N_STRIPS_PSPX + 1)
            for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
            {
                cPspx_comp->cd(i * 2 + 2 * N_PSPX + 1); // supposed to be +2 if
                                                        // energy and position
                                                        // readout is used
                fh_pspx_hit_position[i]->Draw("colz");
            }
        }

        run->AddObject(cPspx_comp);

        run->GetHttpServer()->RegisterCommand("Reset_PSPX", Form("/Objects/%s/->Reset_PSPX_Histo()", GetName()));

        // -------------------------------------------------------------------------
    }
    return kSUCCESS;
}
/*
void R3BOnlineSpectraFiberTofds515::Reset_LOS_Histo()
{
    fh_los_channels->Reset();
    fh_los_tres_MCFD->Reset();
    fh_los_tres_TAMEX->Reset();
    fh_los_pos_ToT->Reset();
    fh_los_tot->Reset();
    fh_los_tot_mean->Reset();
    fh_los_pos_MCFD->Reset();
    fh_los_pos_TAMEX->Reset();
    fh_los_dt_hits_ToT->Reset();
    fh_los_multihit->Reset();
    fh_los_ihit_ToT->Reset();
    fh_los_dt_first_ToT->Reset();
}
*/
void R3BOnlineSpectraFiberTofds515::Reset_ROLU_Histo()
{
    fh_rolu_channels->Reset();
    fh_rolu_tot->Reset();
}

void R3BOnlineSpectraFiberTofds515::Reset_BMON_Histo()
{
    fhTrigger->Reset();
    fhTpat->Reset();
    fh_spill_length->Reset();
    fh_IC->Reset();
    fh_IC_spill->Reset();
    fh_SEE->Reset();
    fh_SEE_spill->Reset();
    fh_TOFDOR->Reset();
    fh_TOFDOR_spill->Reset();
}

void R3BOnlineSpectraFiberTofds515::Reset_SCI8_Histo()
{
    fh_sci8_channels->Reset();
    fh_sci8_tres_MCFD->Reset();
    fh_sci8_tres_TAMEX->Reset();
    fh_sci8_tot->Reset();
    fh_sci8_tot_mean->Reset();
    fh_sci8_dt_hits->Reset();
    fh_sci8_dt_hits_l->Reset();
    fh_sci8_dt_hits_t->Reset();
    fh_sci8_multihit->Reset();
    fh_sci8_multihitVFTX->Reset();
    fh_sci8_multihitLEAD->Reset();
    fh_sci8_multihitTRAI->Reset();
    fh_tof_sci8->Reset();
}
void R3BOnlineSpectraFiberTofds515::Reset_TOFD_Histo()
{
    for (int i = 0; i < N_PLANE_MAX_TOFD; i++)
    {
        fh_tofd_channels[i]->Reset();
        fh_tofd_multihit[i]->Reset();
        fh_tofd_ToF[i]->Reset();
        fh_tofd_TotPm[i]->Reset();
        tofd_side1_vs_side2[i]->Reset();
        fh_tofd_eloss[i]->Reset();
    }
    fh_tofd_dt[0]->Reset();
    fh_tofd_dt[1]->Reset();
    fh_tofd_dt[2]->Reset();
}
void R3BOnlineSpectraFiberTofds515::Reset_FIBERS_Histo()
{
    if (fMappedItems.at(DET_PSPX))
    {
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            fh_pspx_channel_x[i]->Reset();
            fh_pspx_channel_y[i]->Reset();
            fh_pspx_multiplicity_x[i]->Reset();
            fh_pspx_multiplicity_y[i]->Reset();
            fh_pspx_strips_position[i]->Reset();
        }
    }
    if (fCalItems.at(DET_PSPX))
    {
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            fh_pspx_cal_energy_frontback[i]->Reset();
        }
    }
    if (fHitItems.at(DET_PSPX))
    {
        for (UInt_t i = 0; i < N_PSPX / 2; i++)
        {
            fh_pspx_hit_position[i]->Reset();
            fh_pspx_hit_energy[i]->Reset();
        }
    }
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fMappedItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_channels_Fib[ifibcount]->Reset();
            fh_multihit_m_Fib[ifibcount]->Reset();
            fh_multihit_s_Fib[ifibcount]->Reset();
            fh_fibers_Fib[ifibcount]->Reset();
            fh_mult_Fib[ifibcount]->Reset();
            fh_ToT_m_Fib[ifibcount]->Reset();
            fh_ToT_s_Fib[ifibcount]->Reset();
            fh_ToT_single_Fib[ifibcount]->Reset();
            fh_Fib_ToF[ifibcount]->Reset();
            fh_Fib_pos[ifibcount]->Reset();
            fh_Fib_vs_Events[ifibcount]->Reset();
            fh_channels_single_Fib[ifibcount]->Reset();
            fh_Fib10_vs_Fib13->Reset();
            fh_Fib11_vs_Fib12->Reset();
            fh_Fib11_vs_Fib12_cut->Reset();
            fh_channels_Fib11_cut->Reset();
        }
    }
}

void R3BOnlineSpectraFiberTofds515::Exec(Option_t* option)
{
    //  cout << "fNEvents " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(ERROR) << "FairRootManager not found";
        return;
    }

    time = header->GetTimeStamp();

    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }

    time = header->GetTimeStamp();
    // time = 0;

    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }

    if (header->GetTrigger() == 12)
        time_spill_start = time; // header->GetTimeStamp();    // spill start in nsec
    if (header->GetTrigger() == 13)
        time_spill_end = time; // header->GetTimeStamp();    // spill end  in nsec

    if (header->GetTrigger() == 12)
        cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
    if (header->GetTrigger() == 13)
        cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec" << endl;

    fhTrigger->Fill(header->GetTrigger());

    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    Int_t tpatbin;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0)
            fhTpat->Fill(i + 1);
    }

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit = fTpat - 1;
    Int_t itpat;
    Int_t tpatvalue;
    if (fTpat_bit >= 0)
    {
        itpat = header->GetTpat();
        tpatvalue = (itpat && (1 << fTpat_bit)) >> fTpat_bit;
        if ((tpatvalue == 0))
            return;
    }
    /*
        if (fMappedItems.at(DET_BMON))
        {

            Bool_t spectra_clear = false;
            Double_t xtime = double(time - time_start) / 1.e9;
            // for reseting spectra
            Int_t icount = iclear_count * reset_time; // reset after reset_time (sec)
            if (time_clear < 0. && int(xtime) % icount == 0 && xtime > 1.)
            {
                time_clear = xtime;
                spectra_clear = true;
            }
            unsigned long IC;
            unsigned long SEETRAM;
            unsigned long TOFDOR;
            Bool_t bmon_read = false;

            auto det = fMappedItems.at(DET_BMON);
            Int_t nHitsbm = det->GetEntriesFast();

            for (Int_t ihit = 0; ihit < nHitsbm; ihit++)
            {
                R3BBeamMonitorMappedData* hit = (R3BBeamMonitorMappedData*)det->At(ihit);
                if (!hit)
                    continue;

                IC = hit->GetIC();           // negative values if offset not high enough
                SEETRAM = hit->GetSEETRAM(); // negative values if offset not high enough
                TOFDOR = hit->GetTOFDOR();   // only positive values possible

                //     unsigned long long time = header->GetTimeStamp();

                if (fNEvents == fNEvents_start)
                {
                    see_mem = SEETRAM;
                    ic_mem = IC;
                    tofdor_mem = TOFDOR;
                    time_mem = time_start;
                    see_start = SEETRAM;
                    ic_start = IC;
                    tofdor_start = TOFDOR;
                    time_prev_read = time_start;
                }

                if (time > 0)
                {

                    fh_spill_length->Fill((time - time_mem) / 1e9);

                    // Spectra below are filled every read_time (secs)
                    if (time_to_read == 0 && (time - time_prev_read) >= read_time * 1000000000)
                    {
                        time_to_read = time;
                        bmon_read = true;
                    }

                    if (bmon_read)
                    {

                        tdiff = double(time - time_mem) / 1.e9;
                        fNorm = 1.e-3 / (double(time - time_prev_read) / 1.e9); // kHz

                        // IC:
                        Int_t yIC = IC - ic_start;
                        fh_IC->Fill(tdiff, yIC);
                        fh_IC_spill->Fill(tdiff, (IC - ic_mem) * fNorm);
                        ic_mem = IC;

                        // SEETRAM:
                        Int_t ySEE = SEETRAM - see_start;
                        fh_SEE->Fill(tdiff, ySEE);
                        Double_t ySEE_part = (SEETRAM - see_mem) * fNorm * 1.e+3 - see_offset * calib_SEE;
                        fh_SEE_spill->Fill(tdiff, ySEE_part);
                        see_mem = SEETRAM;

                        // TOFDOR:
                        Int_t yTOFDOR = TOFDOR - tofdor_start;
                        fh_TOFDOR->Fill(tdiff, yTOFDOR);
                        fh_TOFDOR_spill->Fill(tdiff, (TOFDOR - tofdor_mem) * fNorm);
                        tofdor_mem = TOFDOR;

                        time_to_read = 0;
                        time_prev_read = time;
                        bmon_read = false;
                    }

                    if (spectra_clear)
                    {
                        fh_spill_length->Reset();
                        fh_IC_spill->Reset("ICESM");
                        fh_SEE_spill->Reset("ICESM");
                        fh_TOFDOR_spill->Reset("ICESM");
                        fh_IC->Reset("ICESM");
                        fh_SEE->Reset("ICESM");
                        fh_TOFDOR->Reset("ICESM");
                        time_mem = time;
                        time_clear = -1.;
                        iclear_count = iclear_count + 1;
                        spectra_clear = false;
                        see_start = SEETRAM;
                        ic_start = IC;
                        tofdor_start = TOFDOR;
                    }
                }
            }
        }
    */
    // **************   ROLU *************************************

    if (fMappedItems.at(DET_ROLU))
    {
        auto det = fMappedItems.at(DET_ROLU);
        Int_t nHits = det->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRoluMappedData* hit = (R3BRoluMappedData*)det->At(ihit);
            if (!hit)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hit->GetDetector(); // 1..
            Int_t iCha = hit->GetChannel();  // 1..

            if (iDet < 2)
                fh_rolu_channels->Fill(iCha); // ROLU 1
            if (iDet > 1)
                fh_rolu_channels->Fill(iCha + 4); // ROLU 2
        }
    }

    Int_t nParts;
    if (fCalItems.at(DET_ROLU))
    {

        auto det = fCalItems.at(DET_ROLU);
        nParts = det->GetEntriesFast();

        if (nParts < 1)
            return;

        Int_t iDet = 0;
        /*
         * Note: double x[nParts][2][4]={NAN};
         * will initialize everything other than x[0][0][0] to 0.0.
         * (and also not compile with our ancient gcc 4.8.5.)
         */
        using A = boost::multi_array<double, 3>;
        auto dims = boost::extents[nParts][2][4];
        A timeRolu_L(dims);
        init_array(timeRolu_L, NAN);
        A timeRolu_T(dims);
        init_array(timeRolu_T, NAN);
        A totRolu(dims);
        init_array(totRolu, NAN);

        for (Int_t iPart = 0; iPart < nParts; iPart++)
        {
            /*
             * nParts is the number of particle passing through detector in one event
             */
            R3BRoluCalData* calData = (R3BRoluCalData*)det->At(iPart);
            iDet = calData->GetDetector();

            for (Int_t iCha = 0; iCha < 4; iCha++)
            {

                if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                { // TAMEX leading
                    timeRolu_L[iPart][iDet - 1][iCha] = calData->GetTimeL_ns(iCha);
                }
                if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                { // TAMEX trailing
                    timeRolu_T[iPart][iDet - 1][iCha] = calData->GetTimeT_ns(iCha);
                }

                if (timeRolu_T[iPart][iDet - 1][iCha] > 0. && timeRolu_L[iPart][iDet - 1][iCha] > 0. &&
                    !(IS_NAN(timeRolu_T[iPart][iDet - 1][iCha])) && !(IS_NAN(timeRolu_L[iPart][iDet - 1][iCha])))
                {
                    while (timeRolu_T[iPart][iDet - 1][iCha] - timeRolu_L[iPart][iDet - 1][iCha] <= 0.)
                    {
                        timeRolu_T[iPart][iDet - 1][iCha] = timeRolu_T[iPart][iDet - 1][iCha] + 2048. * fClockFreq;
                    }

                    totRolu[iPart][iDet - 1][iCha] =
                        timeRolu_T[iPart][iDet - 1][iCha] - timeRolu_L[iPart][iDet - 1][iCha];
                }

                if (iDet < 2)
                    fh_rolu_tot->Fill(iCha + 1, totRolu[iPart][iDet - 1][iCha]);
                if (iDet > 1)
                    fh_rolu_tot->Fill(iCha + 5, totRolu[iPart][iDet - 1][iCha]);
            }

            if (!calData)
            {
                cout << "Rolu !calData" << endl;
                continue; // can this happen?
            }
        }
    }

    //----------------------------------------------------------------------
    // LOS detector
    //----------------------------------------------------------------------
    // Time nomenclature for LOS:
    // lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7
    //   t / \lt
    //   rt| |l
    //   r | |lb
    //   rb\ /b

    /*

        Int_t Multip;

        if (fMappedItems.at(DET_LOS))
        {
            auto det = fMappedItems.at(DET_LOS);
            Int_t nHits = det->GetEntriesFast();

            Multip = nHits;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BLosMappedData* hit = (R3BLosMappedData*)det->At(ihit);
                if (!hit)
                    continue;

                // channel numbers are stored 1-based (1..n)
                Int_t iDet = hit->GetDetector(); // 1..
                Int_t iCha = hit->GetChannel();  // 1..

                //fh_los_channels->Fill(iCha);
            }
        }

        Int_t nPart;

        if (fCalItems.at(DET_LOS))
        {
            Bool_t LOSID = false;

            auto det = fCalItems.at(DET_LOS);
            nPart = det->GetEntriesFast();

            Int_t iDet = 0;

            for (Int_t iPart = 0; iPart < nPart; iPart++)
            {

                 // nPart is the number of particle passing through LOS detector in one event

                R3BLosCalData* calData = (R3BLosCalData*)det->At(iPart);
                iDet = calData->GetDetector();

                for (Int_t iCha = 0; iCha < 8; iCha++)
                {

                    time_V[iPart][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V[iPart][iCha] = calData->GetTimeV_ns(iCha);
                    }

                    time_L[iPart][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[iPart][iCha] = calData->GetTimeL_ns(iCha);
                    }

                    time_T[iPart][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[iPart][iCha] = calData->GetTimeT_ns(iCha);
                    }

                    time_M[iPart][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeM_ns(iCha))))
                    { // MTDC
                        time_M[iPart][iCha] = calData->GetTimeM_ns(iCha);
                    }
                }

                if (!calData)
                {
                    cout << "LOS !calData" << endl;
                    continue; // can this happen?
                }
            }

            // Sorting VFTX data:

            std::qsort(time_V, nPart, sizeof(*time_V), [](const void* arg1, const void* arg2) -> int {
                double const* lhs = static_cast<double const*>(arg1);
                double const* rhs = static_cast<double const*>(arg2);

                return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
            });
            // End sorting

            signed long long time_first = -1;

            for (Int_t iPart = 0; iPart < nPart; iPart++)
            {

                Bool_t iLOSTypeMCFD = false;
                Bool_t iLOSTypeTAMEX = false;
                Bool_t iLOSType = false;

                if (time_V[iPart][0] > 0. && !(IS_NAN(time_V[iPart][0])) && time_V[iPart][1] > 0. &&
                    !(IS_NAN(time_V[iPart][1])) && time_V[iPart][2] > 0. && !(IS_NAN(time_V[iPart][2])) &&
                    time_V[iPart][3] > 0. && !(IS_NAN(time_V[iPart][3])) && time_V[iPart][4] > 0. &&
                    !(IS_NAN(time_V[iPart][4])) && time_V[iPart][5] > 0. && !(IS_NAN(time_V[iPart][5])) &&
                    time_V[iPart][6] > 0. && !(IS_NAN(time_V[iPart][6])) && time_V[iPart][7] > 0. &&
                    !(IS_NAN(time_V[iPart][7])))
                {
                    iLOSTypeMCFD = true; // all 8 MCFD times
                }

                if (time_L[iPart][0] > 0. && !(IS_NAN(time_L[iPart][0])) && time_L[iPart][1] > 0. &&
                    !(IS_NAN(time_L[iPart][1])) && time_L[iPart][2] > 0. && !(IS_NAN(time_L[iPart][2])) &&
                    time_L[iPart][3] > 0. && !(IS_NAN(time_L[iPart][3])) && time_L[iPart][4] > 0. &&
                    !(IS_NAN(time_L[iPart][4])) && time_L[iPart][5] > 0. && !(IS_NAN(time_L[iPart][5])) &&
                    time_L[iPart][6] > 0. && !(IS_NAN(time_L[iPart][6])) && time_L[iPart][7] > 0. &&
                    !(IS_NAN(time_L[iPart][7])) &&

                    time_T[iPart][0] > 0. && !(IS_NAN(time_T[iPart][0])) && time_T[iPart][1] > 0. &&
                    !(IS_NAN(time_T[iPart][1])) && time_T[iPart][2] > 0. && !(IS_NAN(time_T[iPart][2])) &&
                    time_T[iPart][3] > 0. && !(IS_NAN(time_T[iPart][3])) && time_T[iPart][4] > 0. &&
                    !(IS_NAN(time_T[iPart][4])) && time_T[iPart][5] > 0. && !(IS_NAN(time_T[iPart][5])) &&
                    time_T[iPart][6] > 0. && !(IS_NAN(time_T[iPart][6])) && time_T[iPart][7] > 0. &&
                    !(IS_NAN(time_T[iPart][7])))
                {
                    iLOSTypeTAMEX = true; // all 8 leading and trailing times
                }

                // We will consider only events in which booth MCFD and TAMEX see same number of channels:
                if (iLOSTypeTAMEX && iLOSTypeMCFD)
                    iLOSType = true;

                if (iDet == 1)
                {

                    if (1 == 1) //(iLOSType)
                    {

                        int nPMT = 0;
                        int nPMV = 0;

                        for (int ipm = 0; ipm < 8; ipm++)
                        {

                            tot[iPart][ipm] = 0. / 0.;

                            if (time_T[iPart][ipm] > 0. && time_L[iPart][ipm] > 0. && !(IS_NAN(time_T[iPart][ipm])) &&
                                !(IS_NAN(time_L[iPart][ipm])))
                            {
                                while (time_T[iPart][ipm] - time_L[iPart][ipm] <= 0.)
                                {
                                    time_T[iPart][ipm] = time_T[iPart][ipm] + 2048. * fClockFreq;
                                }

                                nPMT = nPMT + 1;
                                tot[iPart][ipm] = time_T[iPart][ipm] - time_L[iPart][ipm];
                            }

                            if (tot[iPart][ipm] != 0. && !(IS_NAN(tot[iPart][ipm])))
                                totsum[iPart] += tot[iPart][ipm];

                            if (time_L[iPart][ipm] > 0. && !(IS_NAN(time_L[iPart][ipm])))
                                timeLosT[iPart] += time_L[iPart][ipm];

                            if (time_V[iPart][ipm] > 0. && !(IS_NAN(time_V[iPart][ipm])))
                            {
                                timeLosV[iPart] += time_V[iPart][ipm];
                                nPMV = nPMV + 1;
                            }
                        }

                        totsum[iPart] = totsum[iPart] / nPMT;

                        timeLosV[iPart] = timeLosV[iPart] / nPMV;

                        timeLosT[iPart] = timeLosT[iPart] / nPMT;

                        timeLos[iPart] = timeLosV[iPart];

                        LosTresV[iPart] = ((time_V[iPart][0] + time_V[iPart][2] + time_V[iPart][4] + time_V[iPart][6]) -
                                           (time_V[iPart][1] + time_V[iPart][3] + time_V[iPart][5] + time_V[iPart][7]))
       / 4.;

                        LosTresT[iPart] = ((time_L[iPart][0] + time_L[iPart][2] + time_L[iPart][4] + time_L[iPart][6]) -
                                           (time_L[iPart][1] + time_L[iPart][3] + time_L[iPart][5] + time_L[iPart][7]))
       / 4.;

                        LosTresM[iPart] = ((time_M[iPart][0] + time_M[iPart][2] + time_M[iPart][4] + time_M[iPart][6]) -
                                           (time_M[iPart][1] + time_M[iPart][3] + time_M[iPart][5] + time_M[iPart][7]))
       / 4.;

                        // Position from tamex:
                        xT_cm[iPart] =
                            ((time_L[iPart][5] + time_L[iPart][6]) / 2. - (time_L[iPart][1] + time_L[iPart][2]) / 2.) *
                            (-1.);
                        yT_cm[iPart] =
                            ((time_L[iPart][7] + time_L[iPart][0]) / 2. - (time_L[iPart][3] + time_L[iPart][4]) / 2.) *
                            (-1.);
                        xT_cm[iPart] = (xT_cm[iPart] - flosOffsetXT) * flosVeffXT;
                        yT_cm[iPart] = (yT_cm[iPart] - flosOffsetYT) * flosVeffYT;

                        // Position from VFTX:
                        xV_cm[iPart] =
                            ((time_V[iPart][5] + time_V[iPart][6]) / 2. - (time_V[iPart][1] + time_V[iPart][2]) / 2.) *
                            (-1.);
                        yV_cm[iPart] =
                            ((time_V[iPart][7] + time_V[iPart][0]) / 2. - (time_V[iPart][3] + time_V[iPart][4]) / 2.) *
                            (-1.);
                        xV_cm[iPart] = (xV_cm[iPart] - flosOffsetXV) * flosVeffXV;
                        yV_cm[iPart] = (yV_cm[iPart] - flosOffsetYV) * flosVeffYV;

                        // Position from ToT:
                        xToT_cm[iPart] = (((tot[iPart][5] + tot[iPart][6]) / 2. - (tot[iPart][1] + tot[iPart][2]) / 2.)
       /
                                          ((tot[iPart][1] + tot[iPart][2] + tot[iPart][5] + tot[iPart][6]) / 4.));

                        yToT_cm[iPart] = (((tot[iPart][0] + tot[iPart][7]) / 2. - (tot[iPart][3] + tot[iPart][4]) / 2.)
       /
                                          ((tot[iPart][7] + tot[iPart][0] + tot[iPart][3] + tot[iPart][4]) / 4.));

                        xToT_cm[iPart] = (xToT_cm[iPart] - flosOffsetXQ) * flosVeffXQ;
                        yToT_cm[iPart] = (yToT_cm[iPart] - flosOffsetYQ) * flosVeffYQ;

                        if (1 == 1)
                        {

                            //fh_los_tot_mean->Fill(totsum[iPart]);

                            if (time_first < 0)
                                time_first = timeLosV[iPart];
                            Double_t timediff = time + (timeLosV[iPart] - time_first) - time_V_mem;
                            if (iPart < 1)
                                //fh_los_dt_first_ToT->Fill(timediff / 1.e3, totsum[iPart]);
                            if (iPart == nPart - 1)
                                time_V_mem = time + timeLosV[iPart] - time_first;
                            if (iPart > 0)
                                //fh_los_dt_hits_ToT->Fill((timeLosV[iPart] - timeLosV[iPart - 1]) / 1.e3,
       totsum[iPart]);
    */
    /*
            for (int ipm = 0; ipm < 8; ipm++)
            {
                fh_los_tot->Fill(ipm + 1, tot[iPart][ipm]);
            }

            fh_los_tres_MCFD->Fill(LosTresV[iPart]);
            fh_los_tres_TAMEX->Fill(LosTresT[iPart]);
            fh_los_tres_MTDC->Fill(LosTresM[iPart]);

            fh_los_pos_MCFD->Fill(xV_cm[iPart], yV_cm[iPart]);
            fh_los_pos_TAMEX->Fill(xT_cm[iPart], yT_cm[iPart]);
            fh_los_pos_ToT->Fill(xToT_cm[iPart], yToT_cm[iPart]);
            fh_los_ihit_ToT->Fill(iPart, totsum[iPart]);
            fh_los_multihit->Fill(iPart + 1);
*/
    /*
                }
                    }
                }
                else
                {
                    cout << "Wrong detector ID for LOS!" << endl;
                }
            }
        }

    */

    // NEW starts here
    /*

        Int_t multTcal[3];
        Double_t iRawTimeNs[3][64];
        UInt_t nlHits, iCh, cpt = 0;
        Double_t ToFraw = 0., PosRaw = -10000., ToFcal = 0., PosCal = -10000.;
        Double_t Velo = 0., Beta = 0., Gamma = 0., Brho = 0., AoQ = 0.;
        Double_t ToFraw_m1 = 0., PosRaw_m1 = -10000., ToFcal_m1 = 0., PosCal_m1 = -10000.;
        Double_t Velo_m1 = 0., Beta_m1 = 0., Gamma_m1 = 0., Brho_m1 = 0., AoQ_m1 = 0.;
        for (UShort_t pmt = 0; pmt < 3; pmt++)
        {
            multTcal[pmt] = 0;
            for (UShort_t m = 0; m < 64; m++)
                iRawTimeNs[pmt][m] = 0.;
        }
        Double_t timeTofd = 0;
        Double_t timeLosV[fNofLosDetectors][32]; // mean time from VFTX
        Double_t LosTresV[fNofLosDetectors][32];
        Double_t timeLosT[fNofLosDetectors][32];
        Double_t LosTresT[fNofLosDetectors][32];
        Double_t timeLos[fNofLosDetectors][32];
        Double_t totsum[fNofLosDetectors][32];
        Double_t xT_cm[fNofLosDetectors][32];
        Double_t yT_cm[fNofLosDetectors][32];
        Double_t xToT_cm[fNofLosDetectors][32];
        Double_t yToT_cm[fNofLosDetectors][32];
        Double_t xV_cm[fNofLosDetectors][32];
        Double_t yV_cm[fNofLosDetectors][32];

        Double_t time_V[fNofLosDetectors][32][8]; // [det][multihit][pm]
        Double_t time_L[fNofLosDetectors][32][8];
        Double_t time_T[fNofLosDetectors][32][8];
        Double_t tot[fNofLosDetectors][32][8];
        Double_t time_MTDC[32][8] = { 0. };
        Double_t LosTresMTDC[32];

        for (Int_t idet = 0; idet < fNofLosDetectors; idet++)
        {
            for (Int_t imult = 0; imult < 32; imult++)
            {
                timeLosV[idet][imult] = 0.0;
                LosTresV[idet][imult] = 0.0 / 0.0;
                timeLosT[idet][imult] = 0.0;
                LosTresT[idet][imult] = 0.0 / 0.0;
                timeLos[idet][imult] = 0.0;
                totsum[idet][imult] = 0.0;
                xT_cm[idet][imult] = 0.0 / 0.0;
                yT_cm[idet][imult] = 0.0 / 0.0;
                xToT_cm[idet][imult] = -100000.;
                yToT_cm[idet][imult] = -100000.;
                xV_cm[idet][imult] = 0.0 / 0.0;
                yV_cm[idet][imult] = 0.0 / 0.0;
                for (Int_t icha = 0; icha < 8; icha++)
                {
                    time_V[idet][imult][icha] = 0.0 / 0.0; // [det][multihit][pm]
                    time_L[idet][imult][icha] = 0.0 / 0.0;
                    time_T[idet][imult][icha] = 0.0 / 0.0;
                    tot[idet][imult][icha] = 0.0 / 0.0;
                }
            }
        }
        Int_t nPartLOS = 0;
        Int_t nPartc[fNofLosDetectors] = { 0 };
        Bool_t iLOSType[fNofLosDetectors][32];
        Bool_t iLOSPileUp[fNofLosDetectors][32];
        for (Int_t idet = 0; idet < fNofLosDetectors; idet++)
        {
            for (Int_t imult = 0; imult < 33; imult++)
            {
                iLOSType[idet][imult] = false;
                iLOSPileUp[idet][imult] = false;
            }
        }

        Int_t Multip;

        if (fMappedItems.at(DET_LOS))
        {
            auto det = fMappedItems.at(DET_LOS);
            nlHits = det->GetEntriesFast();
            if (nlHits > 0)
                nLosEvents += 1;
            Multip = nlHits;

            for (Int_t ihit = 0; ihit < nlHits; ihit++)
            {
                R3BLosMappedData* hit = (R3BLosMappedData*)det->At(ihit);
                if (!hit)
                    continue;

                Int_t iDet = hit->GetDetector(); // 1..
                Int_t iCha = hit->GetChannel();  // 1..
                Int_t iTyp = hit->GetType();     // 0,1,2,3

                fh_los_channels[iDet - 1]->Fill(iCha); // exclude MTDC data
            }
        }

        if (fCalItems.at(DET_LOS))
        {
            auto det = fCalItems.at(DET_LOS);
            nPartLOS = det->GetEntriesFast();

            Int_t iDet = 0;
            Double_t time_V_LOS1[32][8] = { 0. };
            Double_t time_V_LOS2[32][8] = { 0. };

            for (Int_t iPart = 0; iPart < nPartLOS; iPart++)
            {
                R3BLosCalData* calData = (R3BLosCalData*)det->At(iPart);
                iDet = calData->GetDetector();

                Double_t sumvtemp = 0, sumltemp = 0, sumttemp = 0;
                for (Int_t iCha = 0; iCha < 8; iCha++)
                {
                    sumvtemp += calData->GetTimeV_ns(iCha);
                    sumltemp += calData->GetTimeL_ns(iCha);
                    sumttemp += calData->GetTimeT_ns(iCha);
                }
                if (!(IS_NAN(sumvtemp)) && !(IS_NAN(sumltemp)) && !(IS_NAN(sumltemp)))
                {
                    nPartc[iDet - 1]++;
                }
                else
                {
                    continue;
                }

                for (Int_t iCha = 0; iCha < 8; iCha++)
                {
                    if (iDet == 1)
                    {
                        time_V_LOS1[nPartc[iDet - 1] - 1][iCha] = 0. / 0.;
                        if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                        { // VFTX
                            time_V_LOS1[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeV_ns(iCha);
                        }
                        if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                        { // TAMEX leading
                            time_L[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeL_ns(iCha);
                        }

                        if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                        { // TAMEX trailing
                            time_T[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeT_ns(iCha);
                        }
                        time_MTDC[nPartc[iDet - 1]][iCha] = 0. / 0.;
                        if (!(IS_NAN(calData->GetTimeM_ns(iCha))))
                        { // MTDC
                            time_MTDC[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeM_ns(iCha);
                        }
                    }
                    if (iDet == 2)
                    {
                        time_V_LOS2[nPartc[iDet - 1] - 1][iCha] = 0. / 0.;
                        if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                        { // VFTX
                            time_V_LOS2[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeV_ns(iCha);
                        }
                        if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                        { // TAMEX leading
                            time_L[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeL_ns(iCha);
                        }
                        if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                        { // TAMEX trailing
                            time_T[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeT_ns(iCha);
                        }
                    }
                }

                if (!calData)
                {
                    cout << "LOS !calData" << endl;
                    continue; // can this happen?
                }
            }

            // Sorting VFTX data:
            //
            //         // detector 1
            //

            if (nPartc[0] > 0)
            {
                std::qsort(time_V_LOS1, nPartc[0], sizeof(*time_V_LOS1), [](const void* arg1, const void* arg2) -> int {
                    double const* lhs = static_cast<double const*>(arg1);
                    double const* rhs = static_cast<double const*>(arg2);

                    return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
                });
                for (Int_t iPart = 0; iPart < nPartc[0]; iPart++)
                {
                    for (int ipm = 0; ipm < 8; ipm++)
                    {
                        time_V[0][iPart][ipm] = time_V_LOS1[iPart][ipm];
                    }
                }
            }

            // detector 2

            if (fNofLosDetectors > 1 && nPartc[1] > 0)
            {
                std::qsort(time_V_LOS2, nPartc[1], sizeof(*time_V_LOS2), [](const void* arg1, const void* arg2) -> int {
                    double const* lhs = static_cast<double const*>(arg1);
                    double const* rhs = static_cast<double const*>(arg2);

                    return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
                });
                for (Int_t iPart = 0; iPart < nPartc[1]; iPart++)
                {
                    for (int ipm = 0; ipm < 8; ipm++)
                    {
                        time_V[1][iPart][ipm] = time_V_LOS2[iPart][ipm];
                    }
                }
            }

            // End sorting

            std::vector<double_t> time_first(fNofLosDetectors, -1.);
            std::vector<double_t> time0(fNofLosDetectors, -1.);
            std::vector<double_t> time1(fNofLosDetectors, -1.);
            std::vector<double_t> time_abs(fNofLosDetectors, -1.);

            for (iDet = 1; iDet <= fNofLosDetectors; iDet++)
            {
                cpt = 0;
                for (Int_t iPart = 0; iPart < nPartc[iDet - 1]; iPart++)
                {
                    Bool_t iLOSTypeMCFD = false;
                    Bool_t iLOSTypeTAMEX = false;

                    if (time_V[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][0])) &&
                        time_V[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][1])) &&
                        time_V[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][2])) &&
                        time_V[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][3])) &&
                        time_V[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][4])) &&
                        time_V[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][5])) &&
                        time_V[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][6])) &&
                        time_V[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][7])))
                    {
                        iLOSTypeMCFD = true; // all 8 MCFD times
                    }

                    if (time_L[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][0])) &&
                        time_L[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][1])) &&
                        time_L[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][2])) &&
                        time_L[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][3])) &&
                        time_L[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][4])) &&
                        time_L[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][5])) &&
                        time_L[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][6])) &&
                        time_L[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][7])) &&

                        time_T[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][0])) &&
                        time_T[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][1])) &&
                        time_T[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][2])) &&
                        time_T[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][3])) &&
                        time_T[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][4])) &&
                        time_T[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][5])) &&
                        time_T[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][6])) &&
                        time_T[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][7])))
                    {
                        iLOSTypeTAMEX = true; // all 8 leading and trailing times
                    }

                    if (iLOSTypeTAMEX && iLOSTypeMCFD)
                        iLOSType[iDet - 1][iPart] = true;
                    if (iLOSType[iDet - 1][iPart])
                    {
                        int nPMT = 0;
                        int nPMV = 0;

                        for (int ipm = 0; ipm < 8; ipm++)
                        {

                            if (time_T[iDet - 1][iPart][ipm] > 0. && time_L[iDet - 1][iPart][ipm] > 0. &&
                                !(IS_NAN(time_T[iDet - 1][iPart][ipm])) && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                            {
                                while (time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm] <= 0.)
                                {
                                    time_T[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] + 2048. * fClockFreq;
                                }

                                nPMT = nPMT + 1;
                                tot[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm];

                                if (tot[iDet - 1][iPart][ipm] > fEpileup)
                                    iLOSPileUp[iDet - 1][iPart] = true;
                            }

                            if (tot[iDet - 1][iPart][ipm] != 0. && !(IS_NAN(tot[iDet - 1][iPart][ipm])))
                                totsum[iDet - 1][iPart] += tot[iDet - 1][iPart][ipm];

                            if (time_L[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                                timeLosT[iDet - 1][iPart] += time_L[iDet - 1][iPart][ipm];

                            if (time_V[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][ipm])))
                            {
                                timeLosV[iDet - 1][iPart] += time_V[iDet - 1][iPart][ipm];
                                nPMV = nPMV + 1;
                            }
                        }

                        totsum[iDet - 1][iPart] = totsum[iDet - 1][iPart] / nPMT;

                        timeLosV[iDet - 1][iPart] = timeLosV[iDet - 1][iPart] / nPMV;

                        timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] / nPMT;

                        timeLos[iDet - 1][iPart] = timeLosV[iDet - 1][iPart];

                        LosTresV[iDet - 1][iPart] = ((time_V[iDet - 1][iPart][0] + time_V[iDet - 1][iPart][2] +
                                                      time_V[iDet - 1][iPart][4] + time_V[iDet - 1][iPart][6]) -
                                                     (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][3] +
                                                      time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][7])) /
                                                    4.;

                        LosTresT[iDet - 1][iPart] = ((time_L[iDet - 1][iPart][0] + time_L[iDet - 1][iPart][2] +
                                                      time_L[iDet - 1][iPart][4] + time_L[iDet - 1][iPart][6]) -
                                                     (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][3] +
                                                      time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][7])) /
                                                    4.;

                        LosTresT[iDet - 1][iPart] = ((time_V[iDet - 1][iPart][0] * 0. + time_V[iDet - 1][iPart][2] +
                                                      time_V[iDet - 1][iPart][4] + time_V[iDet - 1][iPart][6]) /
                                                         3. -
                                                     (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][3] +
                                                      time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][7]) /
                                                         4.);

                        xT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][2]) / 2. -
                                                 (time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][6]) / 2.;
                        yT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][3] + time_L[iDet - 1][iPart][4]) / 2. -
                                                 (time_L[iDet - 1][iPart][7] + time_L[iDet - 1][iPart][0]) / 2.;
                        xT_cm[iDet - 1][iPart] = (xT_cm[iDet - 1][iPart] - flosOffsetXT[iDet - 1]) * flosVeffXT[iDet -
       1]; yT_cm[iDet - 1][iPart] = (yT_cm[iDet - 1][iPart] - flosOffsetYT[iDet - 1]) * flosVeffYT[iDet - 1];

                        xV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][2]) / 2. -
                                                 (time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][6]) / 2.;
                        yV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][3] + time_V[iDet - 1][iPart][4]) / 2. -
                                                 (time_V[iDet - 1][iPart][7] + time_V[iDet - 1][iPart][0]) / 2.;
                        xV_cm[iDet - 1][iPart] = (xV_cm[iDet - 1][iPart] - flosOffsetXV[iDet - 1]) * flosVeffXV[iDet -
       1]; yV_cm[iDet - 1][iPart] = (yV_cm[iDet - 1][iPart] - flosOffsetYV[iDet - 1]) * flosVeffYV[iDet - 1];

                        if (tot[iDet - 1][iPart][1] > 0. && tot[iDet - 1][iPart][2] > 0. && tot[iDet - 1][iPart][5] > 0.
       && tot[iDet - 1][iPart][6] > 0. && tot[iDet - 1][iPart][0] > 0. && tot[iDet - 1][iPart][3] > 0. && tot[iDet -
       1][iPart][4] > 0. && tot[iDet - 1][iPart][7] > 0.)
                        {
                            xToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) / 2. -
                                                         (tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2]) / 2.) /
                                                        ((tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2] +
                                                          tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) /
                                                         4.));

                            yToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7]) / 2. -
                                                         (tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) / 2.) /
                                                        ((tot[iDet - 1][iPart][7] + tot[iDet - 1][iPart][0] +
                                                          tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) /
                                                         4.));
                        }

                        xToT_cm[iDet - 1][iPart] =
                            (xToT_cm[iDet - 1][iPart] - flosOffsetXQ[iDet - 1]) * flosVeffXQ[iDet - 1];
                        yToT_cm[iDet - 1][iPart] =
                            (yToT_cm[iDet - 1][iPart] - flosOffsetYQ[iDet - 1]) * flosVeffYQ[iDet - 1];

                        if (timeLosV[iDet - 1][iPart] > 0. && timeLosV[iDet - 1][iPart] < 8192. * 5. &&
                            !(IS_NAN(timeLosV[iDet - 1][iPart])))
                        {
                            while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] < 2048. * 5. / 2.)
                            {
                                timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] + 2048. * 5.;
                            }
                            while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] > 2048. * 5. / 2.)
                            {
                                timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] - 2048. * 5.;
                            }

                            fh_los_vftx_tamex[iDet - 1]->Fill(timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart]);
                        }

                        fh_losMCFD_vs_Events[iDet - 1]->Fill(fNEvents, LosTresV[iDet - 1][iPart]);
                        fh_losTAMEX_vs_Events[iDet - 1]->Fill(fNEvents, LosTresT[iDet - 1][iPart]);
                        fh_losToT_vs_Events[iDet - 1]->Fill(fNEvents, totsum[iDet - 1][iPart]);

                        if (!(iLOSPileUp[iDet - 1][iPart])) // desregard pile-up events
                        {
                            for (int ipm = 0; ipm < 8; ipm++)
                            {
                                fh_los_tot[iDet - 1]->Fill(ipm + 1, tot[iDet - 1][iPart][ipm]);
                            }
                            fh_los_dt_hits_ToT[iDet - 1]->Fill(LosTresV[iDet - 1][iPart], totsum[iDet - 1][iPart]);
                            fh_los_tot_mean[iDet - 1]->Fill(totsum[iDet - 1][iPart]);
                            fh_los_tres_MCFD[iDet - 1]->Fill(LosTresV[iDet - 1][iPart]);
                            fh_los_tres_TAMEX[iDet - 1]->Fill(LosTresT[iDet - 1][iPart]);
                            fh_los_pos_MCFD[iDet - 1]->Fill(xV_cm[iDet - 1][iPart], yV_cm[iDet - 1][iPart]);
                            fh_los_pos_TAMEX[iDet - 1]->Fill(xT_cm[iDet - 1][iPart], yT_cm[iDet - 1][iPart]);
                            fh_los_pos_ToT[iDet - 1]->Fill(xToT_cm[iDet - 1][iPart], yToT_cm[iDet - 1][iPart]);
                            fh_los_ihit_ToT[iDet - 1]->Fill(iPart + 1, totsum[iDet - 1][iPart]);
                            fh_los_multihit[iDet - 1]->Fill(iPart + 1);
                        }
                    } // if iLosType
                    if (iDet == 1)
                    {
                        for (Int_t multR = 0; multR < 64; multR++)
                        {
                            for (Int_t multL = 0; multL < 64; multL++)
                            {
                                ToFraw = timeLosV[0][iPart] - 0.5 * (iRawTimeNs[0][multR] + iRawTimeNs[1][multL]);
                                if (fToFmin <= ToFraw && ToFraw <= fToFmax)
                                {
                                    cpt++;
                                    Velo = 1. / (fTof2InvV_p0 + fTof2InvV_p1 * (fToFoffset + ToFraw)); // [m/ns]
                                    Beta = Velo / 0.299792458;
                                    Gamma = 1. / (TMath::Sqrt(1. - TMath::Power(Beta, 2)));
                                    PosRaw = iRawTimeNs[0][multR] - iRawTimeNs[1][multL]; // [ns]
                                    PosCal = fPos_p0 + fPos_p1 * PosRaw;                  // [mm] at S2
                                    Brho = fBrho0_S2toCC * (1. - PosCal / fDispersionS2);
                                    AoQ = Brho / (3.10716 * Beta * Gamma);
                                }
                            }
                        }
                    }
                } // for iPart

            } // for iDet

        } // if fCallItems

        fNEvents += 1;

        */
    //----------------------------------------------------------------------
    // Fiber detectors
    //----------------------------------------------------------------------
    Double_t dtime = 0.0 / 0.0;

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {

        Int_t iFib = 0;

        auto detMapped = fMappedItems.at(DET_FI_FIRST + ifibcount);
        auto detHit = fHitItems.at(DET_FI_FIRST + ifibcount);

        if (detMapped)
        {
            Int_t nHits = detMapped->GetEntriesFast();
            std::vector<UInt_t> mapmt_num(512);
            std::vector<UInt_t> spmt_num(16);
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BBunchedFiberMappedData* hit = (R3BBunchedFiberMappedData*)detMapped->At(ihit);
                if (!hit)
                    continue;

                // channel numbers are stored 1-based (1..n)
                Int_t iCha = hit->GetChannel(); // 1..

                if (hit->IsMAPMT() && hit->IsLeading())
                {
                    fh_channels_Fib[ifibcount]->Fill(iCha); // Fill which clockTDC channel has events
                    ++mapmt_num.at(hit->GetChannel() - 1);  // multihit of a given clockTDC channel

                    int pls = 0;

                    if (ifibcount == 12)
                        pls = 3;
                    if (ifibcount == 13)
                        pls = 1;
                    if (pls > 0)
                    {
                        auto det1Mapped = fMappedItems.at(DET_FI_FIRST + ifibcount + pls);
                        auto det1Hit = fHitItems.at(DET_FI_FIRST + ifibcount + pls);
                        if (detMapped)
                        {
                            Int_t n1Hits = det1Mapped->GetEntriesFast();
                            for (Int_t jhit = 0; jhit < n1Hits; jhit++)
                            {
                                R3BBunchedFiberMappedData* hit_pls = (R3BBunchedFiberMappedData*)det1Mapped->At(jhit);
                                if (!hit_pls)
                                    continue;
                                Int_t jCha = hit_pls->GetChannel();
                                if (hit_pls->IsMAPMT() && hit_pls->IsLeading())
                                {
                                    if (pls == 3)
                                        fh_Fib10_vs_Fib13->Fill(iCha, jCha);
                                    if (pls == 1)
                                    {
                                        fh_Fib11_vs_Fib12->Fill(iCha, jCha);

                                        double Zmusic = 0.;
                                        double AoverQ = 0.;
                                        if (fHitItemsFrs && fHitItemsFrs->GetEntriesFast() > 0)
                                        {
                                            Int_t nFRSHits = fHitItemsFrs->GetEntriesFast();
                                            for (Int_t muhit = 0; muhit < nFRSHits; muhit++)
                                            {
                                                R3BFrsData* FRShit = (R3BFrsData*)fHitItemsFrs->At(muhit);
                                                if (!FRShit)
                                                    continue;
                                                Zmusic = FRShit->GetZ();
                                                AoverQ = FRShit->GetAq();
                                                if (abs(Zmusic - ZMUSIC_cut) < ZMUSIC_wcut &&
                                                    abs(AoverQ - AoQ_cut) < AoQ_wcut)
                                                {
                                                    fh_Fib11_vs_Fib12_cut->Fill(iCha, jCha);
                                                    fh_channels_Fib11_cut->Fill(iCha);
                                                }
                                            }
                                        }

                                        /*
                                        if (fCalItems.at(DET_TOFD))
                                        {
                                            auto det = fCalItems.at(DET_TOFD);
                                            Int_t nCals = det->GetEntriesFast();
                                            float tofd3tl=0;
                                            float tofd3tt=0;
                                            float tofd3tot=0;
                                            float tofd4tl=0;
                                            float tofd4tt=0;
                                            float tofd4tot=0;
                                            for (Int_t ical = 0; ical < nCals; ical++)
                                            {
                                                auto cal = (R3BTofdCalData const*)det->At(ical);
                                                if (!cal) continue; // should not happen
                                                Int_t const iPlane = cal->GetDetectorId(); // 1..n
                                                if (!IS_NAN(cal->GetTimeLeading_ns()))
                                                {
                                                    if (iPlane==3) tofd3tl = cal->GetTimeLeading_ns();
                                                    if (iPlane==4) tofd4tl = cal->GetTimeLeading_ns();
                                                }
                                                if (!IS_NAN(cal->GetTimeTrailing_ns()))
                                                {
                                                    if (iPlane==3) tofd3tt = cal->GetTimeTrailing_ns();
                                                    if (iPlane==4) tofd4tt = cal->GetTimeTrailing_ns();
                                                }
                                                if (abs(tofd4tt-tofd4tl-tofd3tt+tofd3tl)<50)
                                                {
                                                    tofd3tot=tofd3tt-tofd3tl;
                                                    tofd4tot=tofd4tt-tofd4tl;
                                                }
                                            }
                                            if (tofd3tot<160&&tofd4tot>140)
                                            {
                                                fh_Fib11_vs_Fib12_cut->Fill(iCha,jCha);
                                                fh_channels_Fib11_cut->Fill(iCha);
                                            }
                                        }
                                        */
                                    }
                                }
                            }
                        }
                    }
                }

                if (!hit->IsMAPMT() && hit->IsLeading())
                {
                    fh_channels_single_Fib[ifibcount]->Fill(iCha); // Fill which single PMT channel has events
                    ++spmt_num.at(hit->GetChannel() - 1);          // multihit of a given PADI channel
                }
            }

            for (int i = 0; i < 512; ++i)
            {
                auto m = mapmt_num.at(i);
                if (m > 0)
                    fh_multihit_m_Fib[ifibcount]->Fill(i + 1, m); // multihit of a given clockTDC channel
            }

            for (int i = 0; i < 16; ++i)
            {
                auto s = spmt_num.at(i);
                if (s > 0)
                    fh_multihit_s_Fib[ifibcount]->Fill(i + 1, s); // multihit of a given PADI channel
            }
        }

        if (detHit)
        {
            Int_t nHits = detHit->GetEntriesFast();

            // cout << "\nGot" << nHits << "hits";

            Double_t posfib = 0. / 0.;
            Double_t totMax = 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
            Double_t randx;
            Int_t iFibmax = 0;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                Double_t tMAPMT = 0. / 0.;
                Double_t tSPMT = 0. / 0.;

                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)detHit->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..

                // times
                tMAPMT = hit->GetMAPMTTime_ns();
                tSPMT = hit->GetSPMTTime_ns();

                // cout << "T(MA)=" << tMAPMT << " T(S)=" << tSPMT;

                dtime = tMAPMT - tSPMT;

                // "Push" two times in the same clock cycle:
                while (dtime < -1024)
                {
                    tSPMT = tSPMT - 2048.;
                    dtime = tMAPMT - tSPMT;
                }
                while (dtime > 1024.)
                {
                    tSPMT = tSPMT + 2048.;
                    dtime = tMAPMT - tSPMT;
                }

                // "Push" the Fib times in the same cycle with LOS:
                /*
                if (timeLos[0] > 0. && !(IS_NAN(timeLos[0])))
                        {
                            while (tMAPMT - timeLos[0] < 4096.)
                            {
                                tMAPMT = tMAPMT + 2048. * 4.;
                                tSPMT = tSPMT + 2048. * 4.;
                            }
                            while (tMAPMT - timeLos[0] > 4096.)
                            {
                                tMAPMT = tMAPMT - 2048. * 4.;
                                tSPMT = tSPMT - 2048. * 4.;
                            }
                        }
                */
                // cout << "\n ToT (single)..." << hit->GetSPMTToT_ns();
                // cout << "\n ToT (ma)..." << hit->GetMAPMTToT_ns();

                // Not-calibrated ToF:
                // tfib = (tMAPMT + tSPMT) / 2.;
                tfib = tSPMT;
                // if (tfib > 0. && !(IS_NAN(tfib)) && timeLos[0] > 0. && !(IS_NAN(timeLos[0])))
                //   tof_fib = tfib - timeLos[0];

                // Not-calibrated position:
                randx = (std::rand() / (float)RAND_MAX);
                if (iFib > 0)
                    posfib = (-n_fiber[ifibcount] / 2. + iFib + (0.5 - randx));

                // if (hit->GetSPMTToT_ns() > 0)
                //{

                //	cout << "\nFilling ToT...";

                fh_fibers_Fib[ifibcount]->Fill(iFib);
                fh_ToT_s_Fib[ifibcount]->Fill(iFib, hit->GetSPMTToT_ns());
                fh_ToT_m_Fib[ifibcount]->Fill(iFib, hit->GetMAPMTToT_ns());
                fh_time_Fib[ifibcount]->Fill(iFib, tMAPMT - tSPMT);
                fh_Fib_ToF[ifibcount]->Fill(iFib, tof_fib);
                fh_Fib_pos[ifibcount]->Fill(posfib);
                fh_Fib_vs_Events[ifibcount]->Fill(fNEvents, iFib);
                if (ifibcount == 12 || ifibcount == 13)
                {
                    fh_ToT_single_Fib[ifibcount]->Fill((iFib - 1) % 2 + 1 + 2 * ((iFib - 1) / 512),
                                                       hit->GetSPMTToT_ns());
                    //                    cout<<"Test: "<<ifibcount<<" ifib: "<<iFib<<" single PMT: "<<
                    //                    (iFib)%2+1+2*((iFib-1)/512)<<endl;
                }

                if (ifibcount == 14 || ifibcount == 15)
                {
                    fh_ToT_single_Fib[ifibcount]->Fill((iFib - 1) % 2 + 1 + 2 * ((iFib - 1) / 512),
                                                       hit->GetSPMTToT_ns());
                    //                    cout<<"Test: "<<ifibcount<<" ifib: "<<iFib<<" single PMT: "<<
                    //                    (iFib-1)%2+1+2*((iFib-1)/512)<<endl;
                }

                if (ifibcount == 9 || ifibcount == 10)
                {
                    fh_ToT_single_Fib[ifibcount]->Fill((iFib - 1) / 256 + 1, hit->GetSPMTToT_ns());
                }

                if (ifibcount == 4 || ifibcount == 5)
                {

                    fh_ToT_single_Fib[ifibcount]->Fill((iFib - 1) % 2 + 1, hit->GetSPMTToT_ns());
                }
                //}
            } // end for(ihit)

            if (nHits > 0)
                fh_mult_Fib[ifibcount]->Fill(nHits);

        } // end if(aHit[ifibcount])
    }     // end for(ifibcount)

    //----------------------------------------------------------------------
    // TOFD
    //----------------------------------------------------------------------

    Int_t NumPaddles[4] = { 0 };
    if (fMappedItems.at(DET_TOFD))
    {
        auto det = fMappedItems.at(DET_TOFD);
        Int_t nMapped = det->GetEntriesFast();
        Int_t iPlaneMem = 1, iBarMem = 0;
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BTofdMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen

            Int_t const iPlane = mapped->GetDetectorId(); // 1..n
            Int_t const iBar = mapped->GetBarId();        // 1..n
            Int_t const iSide = mapped->GetSideId();      // 1..n
            Int_t const iEdge = mapped->GetEdgeId();
            // Float_t const tc = mapped->GetTimeCoarse();
            // Float_t const tf = mapped->GetTimeFine();

            for (Int_t jmapped = 0; jmapped < nMapped; jmapped++)
            {
                auto mapped0 = (R3BTofdMappedData const*)det->At(jmapped);
                if (!mapped0)
                    continue;

                Int_t const jPlane = mapped0->GetDetectorId(); // 1..n
                Int_t const jBar = mapped0->GetBarId();        // 1..n
                Int_t const jSide = mapped0->GetSideId();      // 1..n
                Int_t const jEdge = mapped0->GetEdgeId();

                if (iPlane == jPlane && iEdge == 1 && jEdge == 1 && iSide == 1 && jSide == 2)
                {

                    tofd_side1_vs_side2[iPlane - 1]->Fill(iBar, jBar);
                }
            }

            if (iPlaneMem != iPlane)
                iBarMem = 0;
            if (iPlane == 1 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[0] += 1;
            if (iPlane == 2 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[1] += 1;
            if (iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[2] += 1;
            if (iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[3] += 1;

            //  cout<<imapped<<", "<<iPlane<<"; "<<iPlaneMem<<"; "<<iBar<<"; "<<iBarMem<<",
            //  "<<NumPaddles[iPlane-1]<<endl;

            iPlaneMem = iPlane;
            iBarMem = iBar;

            if (iPlane <= fNofPlanes)
            {
                if (iSide == 1)
                    fh_tofd_channels[iPlane - 1]->Fill(-iBar - 1);
                if (iSide == 2)
                    fh_tofd_channels[iPlane - 1]->Fill(iBar);
            }
        }
    }

    if (fCalItems.at(DET_TOFD))
    {
        auto det = fCalItems.at(DET_TOFD);
        Int_t nCals = det->GetEntriesFast();

        Double_t tot1[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t tot2[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t_paddle[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t1l[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t2l[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t1t[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t2t[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t ToF[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };

        Bool_t Bar_present[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { false };

        Int_t iBarMem = 0;
        Int_t jmult[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0 };

        unsigned long long time0 = header->GetTimeStamp();
        double_t time1 = -1.;

        for (Int_t ical = 0; ical < nCals; ical++)
        {
            auto cal = (R3BTofdCalData const*)det->At(ical);
            if (!cal)
                continue; // should not happen

            Int_t const iPlane = cal->GetDetectorId(); // 1..n
            Int_t const iBar = cal->GetBarId();        // 1..n

            //      std::cout << iPlane << ' ' << iBar <<
            //          ',' << cal->GetTimeLeading_ns() <<
            //          ' ' << cal->GetTimeTrailing_ns() <<
            //          ' ' << cal->GetTimeLeading_ns() <<
            //          ' ' << cal->GetTimeTrailing_ns() << std::endl;

            // get all times of one bar
            /*
                         cout<<"TOFD Online: "<<fNEvents<<", "<<nCals<<"; "<<ical<<"; "<<iPlane<<", "<<iBar<<","
                 <<iBarMem<<", "<<", "<< cal->fTime1L_ns<<", "<<cal->fTime1T_ns<<", "<< cal->fTime2L_ns<<", "
                 <<cal->fTime2T_ns<<endl;
            */
            /*
                  if(iBar != iBarMem )
                  {
                jmult[iPlane-1][iBar-1] = 0;
                if (!IS_NAN(cal->GetTimeLeading_ns())&&cal->GetSideId()==1) t1l[0][iPlane-1][iBar-1] =
               cal->GetTimeLeading_ns(); if (!IS_NAN(cal->GetTimeTrailing_ns())&&cal->GetSideId()==1)
               t1t[0][iPlane-1][iBar-1] = cal->GetTimeTrailing_ns(); if
               (!IS_NAN(cal->GetTimeLeading_ns())&&cal->GetSideId()==2) t2l[0][iPlane-1][iBar-1] =
               cal->GetTimeLeading_ns(); if (!IS_NAN(cal->GetTimeTrailing_ns())&&cal->GetSideId()==2)
               t2t[0][iPlane-1][iBar-1] = cal->GetTimeTrailing_ns(); Bar_present[0][iPlane-1][iBar-1] = true;
                  }
                  else
                  {
                jmult[iPlane-1][iBar-1] = jmult[iPlane-1][iBar-1] + 1;
                Int_t jm = jmult[iPlane-1][iBar-1];
                if (!IS_NAN(cal->GetTimeLeading_ns())&&cal->GetSideId()==1) t1l[jm][iPlane-1][iBar-1] =
               cal->GetTimeLeading_ns(); if (!IS_NAN(cal->GetTimeTrailing_ns())&&cal->GetSideId()==1)
               t1t[jm][iPlane-1][iBar-1] = cal->GetTimeTrailing_ns(); if
               (!IS_NAN(cal->GetTimeLeading_ns())&&cal->GetSideId()==2) t2l[jm][iPlane-1][iBar-1] =
               cal->GetTimeLeading_ns(); if (!IS_NAN(cal->GetTimeTrailing_ns())&&cal->GetSideId()==2)
               t2t[jm][iPlane-1][iBar-1] = cal->GetTimeTrailing_ns(); Bar_present[jm][iPlane-1][iBar-1] = true;
                  }
                  iBarMem = iBar;
            */

            Int_t jm = jmult[iPlane - 1][iBar - 1];

            if (!IS_NAN(cal->GetTimeLeading_ns()) && cal->GetSideId() == 1)
                t1l[jm][iPlane - 1][iBar - 1] = cal->GetTimeLeading_ns();
            if (!IS_NAN(cal->GetTimeTrailing_ns()) && cal->GetSideId() == 1)
                t1t[jm][iPlane - 1][iBar - 1] = cal->GetTimeTrailing_ns();
            if (!IS_NAN(cal->GetTimeLeading_ns()) && cal->GetSideId() == 2)
                t2l[jm][iPlane - 1][iBar - 1] = cal->GetTimeLeading_ns();
            if (!IS_NAN(cal->GetTimeTrailing_ns()) && cal->GetSideId() == 2)
                t2t[jm][iPlane - 1][iBar - 1] = cal->GetTimeTrailing_ns();

            Bar_present[jm][iPlane - 1][iBar - 1] = true;
            jmult[iPlane - 1][iBar - 1] = jmult[iPlane - 1][iBar - 1] + 1;

            double_t time2 = 0.;
            if (time1 < 0 && iPlane == 2)
                time1 = (t1l[jm][iPlane - 1][iBar - 1] + t2l[jm][iPlane - 1][iBar - 1]) / 2.;
            if (iPlane == 2)
                time2 = (t1l[jm][iPlane - 1][iBar - 1] + t2l[jm][iPlane - 1][iBar - 1]) / 2.;
            if (time1 > 0. && time2 > 0. && time2 > time1)
            {
                // cout<<"Time Test "<<time0<<"  "<<time1<< "   "<< time2 <<"  " <<time_previous_event <<endl;
                fh_TimePreviousEvent->Fill(time2 - time1);
                time2 = time1;
            }
        }

        /*
                for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
                for(Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++)
                {
                fh_tofd_multihit[ipl]->Fill(ibr+1,jmult[ipl][ibr]+1);
                }
         */
        for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
            for (Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++)
                for (Int_t jm = 0; jm < jmult[ipl][ibr]; jm++)
                {

                    if (Bar_present[jm][ipl][ibr])
                    {
                        Int_t iPlane = ipl + 1; // 1..n
                        Int_t iBar = ibr + 1;   // 1..n

                        fh_tofd_multihit[ipl]->Fill(ibr + 1, jmult[ipl][ibr]);

                        // calculate time over threshold and check if clock counter went out of range

                        while (t1t[jm][iPlane - 1][iBar - 1] < t1l[jm][iPlane - 1][iBar - 1])
                        {
                            t1t[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                        }

                        while (t2t[jm][iPlane - 1][iBar - 1] < t2l[jm][iPlane - 1][iBar - 1])
                        {
                            t2t[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                        }

                        int ilc = jm;
                        /*
                                    if (!(IS_NAN(timeLos[ilc])) && timeLos[ilc] > 0.)
                                    {

                                        while (t1l[jm][iPlane - 1][iBar - 1] < timeLos[ilc])
                                        {
                                            t1t[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                                            t1l[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                                        }
                                        while (t2l[jm][iPlane - 1][iBar - 1] < timeLos[ilc])
                                        {
                                            t2t[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                                            t2l[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                                        }
                                    }
                        */
                        // ToF
                        t_paddle[jm][iPlane - 1][iBar - 1] =
                            (t1l[jm][iPlane - 1][iBar - 1] + t2l[jm][iPlane - 1][iBar - 1]) / 2.;

                        //  between 2 bars in 2 planes
                        if (ipl > 0)
                        {
                            fh_tofd_dt[ipl - 1]->Fill(iBar,
                                                      t_paddle[jm][ipl][iBar - 1] - t_paddle[jm][ipl - 1][iBar - 1]);
                        }

                        /*
                                    if (!(IS_NAN(timeLos[ilc])) && timeLos[ilc] > 0.)
                                    {
                                        // between LOS and paddle
                                        ToF[jm][iPlane - 1][iBar - 1] =
                                            fmod(t_paddle[jm][iPlane - 1][iBar - 1] - timeLos[ilc] + 5 * 8192, 5 *
                        2048); fh_tofd_ToF[iPlane - 1]->Fill(iBar, ToF[jm][iPlane - 1][iBar - 1]);
                        }
                        */
                        // ToT
                        //

                        tot1[jm][iPlane - 1][iBar - 1] = t1t[jm][iPlane - 1][iBar - 1] - t1l[jm][iPlane - 1][iBar - 1];
                        if (tot1[jm][iPlane - 1][iBar - 1] < 0)
                        {
                            cout << "Negative ToT " << tot1[jm][iPlane - 1] << ", for hit= " << jm << endl;
                            cout << "Times1: " << t1t[jm][iPlane - 1][iBar - 1] << " " << t1l[jm][iPlane - 1][iBar - 1]
                                 << endl;
                        }

                        tot2[jm][iPlane - 1][iBar - 1] = t2t[jm][iPlane - 1][iBar - 1] - t2l[jm][iPlane - 1][iBar - 1];
                        if (tot2[jm][iPlane - 1][iBar - 1] < 0)
                        {
                            cout << "Negative ToT " << tot2[jm][iPlane - 1][iBar - 1] << ", for hit= " << jm << endl;
                            cout << "Times2: " << t2t[jm][iPlane - 1][iBar - 1] << " " << t2l[jm][iPlane - 1][iBar - 1]
                                 << endl;
                        }

                        fh_tofd_TotPm[iPlane - 1]->Fill(iBar, tot2[jm][iPlane - 1][iBar - 1]);
                        fh_tofd_TotPm[iPlane - 1]->Fill(-iBar - 1, tot1[jm][iPlane - 1][iBar - 1]);
                    }
                }
    }

    //----------------------------------------------------------------------
    // PTOF
    //----------------------------------------------------------------------
    /*
        if (fCalItems.at(DET_PTOF))
        {
            auto det = fCalItems.at(DET_PTOF);

            Double_t tot1 = 0.;
            Double_t tot2 = 0.;
            Double_t t1l = 0.;
            Double_t t2l = 0.;
            Double_t t1t = 0.;
            Double_t t2t = 0.;
            Bool_t bar_quer1 = false;
            Bool_t bar_quer2 = false;

            Int_t nHits = det->GetEntriesFast();
            LOG(DEBUG) << "nHits: " << nHits;
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BPaddleCalData* hit = (R3BPaddleCalData*)det->At(ihit);

                if (!hit)
                    continue; // should not happen

                Int_t iBar = hit->GetBar(); // 1..n
                // get all times of one bar
                t1l = hit->fTime1L_ns;
                t2l = hit->fTime2L_ns;
                t1t = hit->fTime1T_ns;
                t2t = hit->fTime2T_ns;

                if (!(t1l > 0 && t2l > 0 && t1t > 0 && t2t > 0))
                    continue;

                fh_ptof_channels->Fill(iBar);
                LOG(DEBUG) << "Bar: " << iBar;
                LOG(DEBUG) << "times PM1: " << t1l << "  " << t1t << "  " << t1t - t1l;
                LOG(DEBUG) << "times PM2: " << t2l << "  " << t2t << "  " << t2t - t2l;
                if (iBar == 7)
                    bar_quer1 = true;
                if (iBar == 8)
                    bar_quer2 = true;

                // calculate time over threshold and check if clock counter went out of range
                while (t1t - t1l < 0.)
                {
                    t1t = t1t + 2048. * fClockFreq;
                }

                while (t2t - t2l < 0.)
                {
                    t2t = t2t + 2048. * fClockFreq;
                }

                while (t1l - timeLos[0] < 0.)
                {
                    t1t = t1t + 2048. * fClockFreq;
                    t1l = t1l + 2048. * fClockFreq;
                    t2t = t2t + 2048. * fClockFreq;
                    t2l = t2l + 2048. * fClockFreq;
                }

                tot1 = t1t - t1l;
                if (tot1 < 0)
                {
                    LOG(WARNING) << "Negative ToT " << tot1;
                    LOG(WARNING) << "times1: " << t1t << " " << t1l;
                }

                tot2 = t2t - t2l;
                if (tot2 < 0)
                {
                    LOG(WARNING) << "Negative ToT " << tot2;
                    LOG(WARNING) << "times2: " << t2t << " " << t2l;
                }

                fh_ptof_TotPm1[iBar]->Fill(tot1);
                fh_ptof_TotPm2[iBar]->Fill(tot2);
                if (iBar == 2)
                    fh_ptof_test1->Fill(sqrt(tot1 * tot1));
            }

            // once again

            nHits = det->GetEntriesFast();
            //		LOG(DEBUG) << "nHits: " << nHits;
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BPaddleCalData* hit = (R3BPaddleCalData*)det->At(ihit);

                if (!hit)
                    continue; // should not happen

                Int_t iBar = hit->GetBar(); // 1..n
                // get all times of one bar
                t1l = hit->fTime1L_ns;
                t2l = hit->fTime2L_ns;
                t1t = hit->fTime1T_ns;
                t2t = hit->fTime2T_ns;

                if (!(t1l > 0 && t2l > 0 && t1t > 0 && t2t > 0))
                    continue;

                // calculate time over threshold and check if clock counter went out of range
                while (t1t - t1l < 0.)
                {
                    t1t = t1t + 2048. * fClockFreq;
                }

                while (t2t - t2l < 0.)
                {
                    t2t = t2t + 2048. * fClockFreq;
                }
                while (t1l - timeLos[0] < 0.)
                {
                    t1t = t1t + 2048. * fClockFreq;
                    t1l = t1l + 2048. * fClockFreq;
                    t2t = t2t + 2048. * fClockFreq;
                    t2l = t2l + 2048. * fClockFreq;
                }

                tot1 = t1t - t1l;
                if (tot1 < 0)
                {
                    LOG(WARNING) << "Negative ToT " << tot1;
                    LOG(WARNING) << "times1: " << t1t << " " << t1l;
                }

                tot2 = t2t - t2l;
                if (tot2 < 0)
                {
                    LOG(WARNING) << "Negative ToT " << tot2;
                    LOG(WARNING) << "times2: " << t2t << " " << t2l;
                }

                if (bar_quer1 && bar_quer2)
                {
                    //				fh_ptof_TotPm1[iBar]->Fill(tot1);
                    //				fh_ptof_TotPm2[iBar]->Fill(tot2);
                    //				if(iBar==2) fh_ptof_test2->Fill(sqrt(tot1*tot2));
                    fh_ptof_channels_cut->Fill(iBar);
                }
            }
        }
    */
    //----------------------------------------------------------------------
    // PSPX
    //----------------------------------------------------------------------
    /* if (fMappedItems.at(DET_PSPX))
    {
        UInt_t mult_x[N_PSPX];
        UInt_t mult_y[N_PSPX];
        UInt_t channel_x[N_PSPX][N_STRIPS_PSPX * 2];
        UInt_t channel_y[N_PSPX][N_STRIPS_PSPX * 2];
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            mult_x[i] = 0;
            mult_y[i] = 0;
            for (UInt_t j = 0; j < N_STRIPS_PSPX * 2; j++)
            {
                channel_x[i][j] = 0;
                channel_y[i][j] = 0;
            }
        }
        Int_t nHits = fMappedItems.at(DET_PSPX)->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BPspxMappedData* mappedData = (R3BPspxMappedData*)fMappedItems.at(DET_PSPX)->At(ihit);
            UInt_t i = mappedData->GetDetector() - 1;
            if (mappedData->GetChannel() > N_STRIPS_PSPX * 2 && mappedData->GetChannel() < N_STRIPS_PSPX * 4 + 1)
            {
                // LOG(INFO) << "Test1 " << i << " " << mappedData->GetDetector() << " " <<
                // mappedData->GetChannel();
                channel_y[i][mult_y[i]] = mappedData->GetChannel();
                mult_y[i]++;
            }
            else if (mappedData->GetChannel() > 0 && mappedData->GetChannel() < N_STRIPS_PSPX * 2 + 1)
            {
                // LOG(INFO) << "Test2 " << i << " " << mappedData->GetDetector() << " " <<
                // mappedData->GetChannel();
                channel_x[i][mult_x[i]] = mappedData->GetChannel();
                mult_x[i]++;
            }
        }
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            // LOG(INFO) << "Test3 " << i << " " << mult_x[i] << " " << mult_y[i];
            fh_pspx_multiplicity_x[i]->Fill(mult_x[i]);
            fh_pspx_multiplicity_y[i]->Fill(mult_y[i]);
            // LOG(INFO) << "Test4 " << fh_pspx_multiplicity_x[i]->GetBinContent(1);
            std::vector<int> v_ch_x, v_ch_y;
            for (Int_t j = 0; j < mult_x[i]; j++)
            {
                if ((channel_x[i][j] + 1) / 2 == (channel_x[i][j + 1] + 1) / 2)
                {
                    v_ch_x.push_back((channel_x[i][j] + 1) / 2);
                }
            }
            for (Int_t j = 0; j < mult_y[i]; j++)
            {
                if ((channel_y[i][j] + 1) / 2 == (channel_y[i][j + 1] + 1) / 2)
                {
                    v_ch_y.push_back((channel_y[i][j] + 1) / 2);
                }
            }
            for (auto it_x = v_ch_x.begin(); v_ch_x.end() != it_x; ++it_x)
            {
                for (auto it_y = v_ch_y.begin(); v_ch_y.end() != it_y; ++it_y)
                {
                    fh_pspx_strips_position[i]->Fill(
                        *it_x,
                        -*it_y + 3 * N_STRIPS_PSPX + 1); // with inverted axis to account for orientation
                    // fh_pspx_strips_position[i]->Fill((channel_x[i][0] + 1) / 2, ((channel_y[i][0] + 1) / 2));
                    // //without inverted axis => wrong orientation y axis
                }
            }
            // std::cout << "x: " << mult_x[i] << ", " << (channel_x[i][0]+1)/2 << endl;
            // std::cout << "y: " << mult_y[i] << ", " << -((int)channel_y[i][0]+1)/2 + 3 * N_STRIPS_PSPX + 1<< endl;
            for (Int_t j = 0; j < mult_x[i]; j++)
            {
                fh_pspx_channel_x[i]->Fill(channel_x[i][j]);
            }
            for (Int_t j = 0; j < mult_y[i]; j++)
            {
                fh_pspx_channel_y[i]->Fill(channel_y[i][j]);
            }
        }
    }
    if (fCalItems.at(DET_PSPX))
    {
        Int_t energy_front[N_PSPX];
        Int_t energy_back[N_PSPX];
        Int_t mult_front[N_PSPX];
        Int_t mult_back[N_PSPX];
        Int_t num_strip[50][N_PSPX];
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            energy_front[i] = 0;
            energy_back[i] = 0;
            mult_front[i] = 0;
            mult_back[i] = 0;
        }
        Int_t nHits = fCalItems.at(DET_PSPX)->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            for (UInt_t i = 0; i < N_PSPX; i++)
            {
                R3BPspxCalData* calData = (R3BPspxCalData*)fCalItems.at(DET_PSPX)->At(ihit);
                num_strip[ihit][i] = calData->GetStrip();
                if (calData->GetDetector() == i + 1 && calData->GetStrip() > N_STRIPS_PSPX &&
                    calData->GetStrip() < N_STRIPS_PSPX * 2 + 1)
                {
                    energy_back[i] += (calData->GetEnergy1() + calData->GetEnergy2());
                    mult_back[i]++;
                }
                else if (calData->GetDetector() == i + 1 && calData->GetStrip() > 0 &&
                         calData->GetStrip() < N_STRIPS_PSPX + 1)
                {
                    energy_front[i] += (calData->GetEnergy1() + calData->GetEnergy2());
                    mult_front[i]++;
                }
                //     cout<<"Num strips: "<<nHits<<", "<<ihit<<", "<<i<<", "<<calData->GetStrip()<<",
                //     "<<energy_back[i]<<endl;
            }
        }
        // if(mult_front[1] > 0) cout<<"PSPx mult: "<<num_strip[0]<<"; "<<num_strip[1]<<endl;
        // Interstrip position of PSP1
        if (((mult_back[0] == 2 && mult_front[0] == 1) && abs(energy_back[0]) < 380000) ||
            ((mult_back[0] == 1 && mult_front[0] == 2) && abs(energy_front[0]) < 380000))
        {
        }
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            if (i != 2 && i != 3)
            {
                fh_pspx_cal_energy_frontback[i]->Fill(energy_front[i], energy_back[i]);
            }
            else
            {
                fh_pspx_cal_energy_frontback[i]->Fill(-energy_front[i], -energy_back[i]);
            }
        }
    }
    Int_t etrue[3];
    if (fHitItems.at(DET_PSPX))
    {
        Int_t nHits = fHitItems.at(DET_PSPX)->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
            {
                R3BPspxHitData* hitData = (R3BPspxHitData*)fHitItems.at(DET_PSPX)->At(ihit);
                if (hitData->GetDetector() == i * 2 + 1)
                {
                    etrue[i] = 0;
                    if (i == 0 && hitData->GetEnergy() > 250000 && hitData->GetEnergy() < 271000)
                        etrue[0] = 1;
                    if (i == 1 && hitData->GetEnergy() > 380000 && hitData->GetEnergy() < 405000)
                        etrue[1] = 1;
                    if (i == 2 && hitData->GetEnergy() > 380000 && hitData->GetEnergy() < 405000)
                        etrue[2] = 1;
                    etrue[i] = 1;
                    if (etrue[i] == 1)
                    {
                        fh_pspx_hit_multi[i]->Fill(hitData->GetXMultiplicity(), hitData->GetYMultiplicity());
                        fh_pspx_hit_energy[i]->Fill(hitData->GetEnergy());
                        if ((hitData->GetXMultiplicity() == 4 && hitData->GetYMultiplicity() == 2) ||
                            (hitData->GetXMultiplicity() == 2 && hitData->GetYMultiplicity() == 4))
                        {
                            fh_pspx_hit_position[i]->Fill(hitData->GetX(), hitData->GetY());
                            if(hitData->GetDetector() == 1 ){//&& hitData->GetX() > -5 && hitData->GetX() < 5){
                             fh_los_pos_MCFD->Fill(xV_cm[0],yV_cm[0]);
                             fh_los_pos_TAMEX->Fill(xT_cm[0],yT_cm[0]);
                             fh_los_pos_ToT->Fill(xToT_cm[0],yToT_cm[0]);
                            }
                        }
                    }
                }
            }
        }
    }
   }
*/
    fNEvents += 1;
}

void R3BOnlineSpectraFiberTofds515::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {
        if (fMappedItems.at(det))
        {
            fMappedItems.at(det)->Clear();
        }
        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
        if (fHitItems.at(det))
        {
            fHitItems.at(det)->Clear();
        }
    }

    if (fHitItemsMus)
    {
        fHitItemsMus->Clear();
    }

    if (fHitItemsFrs)
    {
        fHitItemsFrs->Clear();
    }
}

void R3BOnlineSpectraFiberTofds515::FinishTask()
{

    if (fMappedItems.at(DET_ROLU))
    {
        fh_rolu_channels->Write();
        fh_rolu_tot->Write();
    }
    /*
        if (fMappedItems.at(DET_LOS))
        {
            fh_los_channels->Write();
            fh_los_tot->Write();
            fh_los_dt_hits_ToT->Write();
            fh_los_ihit_ToT->Write();
            fh_los_dt_hits->Write();
        }
    */
    if (fMappedItems.at(DET_BMON))
    {
        fhTpat->Write();
        fh_spill_length->Write();
        fhTrigger->Write();
        fh_IC->Write();
        fh_SEE->Write();
        fh_TOFDOR->Write();
        fh_IC_spill->Write();
        fh_SEE_spill->Write();
        fh_TOFDOR_spill->Write();
    }

    if (fCalItems.at(DET_TOFD))
    {
        fh_TimePreviousEvent->Write();

        for (Int_t i = 0; i < 4; i++)
        {
            fh_tofd_TotPm[i]->Write();
        }
    }
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fMappedItems.at(ifibcount + DET_FI_FIRST))
        {
            fh_channels_Fib[ifibcount]->Write();
            fh_fibers_Fib[ifibcount]->Write();
            fh_mult_Fib[ifibcount]->Write();
            fh_time_Fib[ifibcount]->Write();
            fh_multihit_m_Fib[ifibcount]->Write();
            fh_multihit_s_Fib[ifibcount]->Write();
            fh_ToT_m_Fib[ifibcount]->Write();
            fh_ToT_s_Fib[ifibcount]->Write();
            fh_ToT_single_Fib[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_Fib_ToF[ifibcount]->Write();
            fh_channels_single_Fib[ifibcount]->Write();
            fh_Fib10_vs_Fib13->Write();
            fh_Fib11_vs_Fib12->Write();
            fh_Fib11_vs_Fib12_cut->Write();
        }
    }

    if (fMappedItems.at(DET_PSPX))
    {
        // LOG(INFO) << "Finish MappedPspx";

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            fh_pspx_channel_x[i]->Write();
            fh_pspx_channel_y[i]->Write();
            fh_pspx_multiplicity_x[i]->Write();
            fh_pspx_multiplicity_y[i]->Write();
            fh_pspx_strips_position[i]->Write();
        }
    }
    if (fCalItems.at(DET_PSPX))
    {
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            fh_pspx_cal_energy_frontback[i]->Write();
        }
    }
    if (fHitItems.at(DET_PSPX))
    {
        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            fh_pspx_hit_energy[i]->Write();
            fh_pspx_hit_position[i]->Write();
        }
    }
}
