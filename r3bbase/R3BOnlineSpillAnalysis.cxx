// ------------------------------------------------------------
// -----                  R3BOnlineSpillAnalysis                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpillAnalysis.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"

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

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"
//#include "R3BTCalEngine.h"

#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberMappedData.h"

#include "R3BSamplerMappedData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TGaxis.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THistPainter.h"
#include "TLegend.h"
#include "TStyle.h"

#include "THttpServer.h"

#include "TVirtualFFT.h"

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
using namespace std;

R3BOnlineSpillAnalysis::R3BOnlineSpillAnalysis()
    : R3BOnlineSpillAnalysis("OnlineSpillAnalysis", 1)
{
}

R3BOnlineSpillAnalysis::R3BOnlineSpillAnalysis(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fNofPlanes(N_PLANE_MAX_TOFD)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFD)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpillAnalysis::~R3BOnlineSpillAnalysis()
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
    }
}

InitStatus R3BOnlineSpillAnalysis::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpillAnalysis::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("/Tasks", this);

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
    puts("1111111111111111111111111111111111111111111111111111111111");
    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Los detector
    TCanvas* cLos[fNofLosDetectors];

    if (fMappedItems.at(DET_LOS))
    {

        if (fNofLosDetectors == 2)
        {
            fh_los_ToT_vs_ToT = new TH2F("los_tot_vs_tot", "LOS ToT vs ToT", 3000, 0., 300., 3000, 0., 300.);
            fh_los_ToT_vs_ToT->GetXaxis()->SetTitle("ToT LOS1 / ns");
            fh_los_ToT_vs_ToT->GetYaxis()->SetTitle("ToT LOS2 / ns");
        }

        // for (Int_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        for (Int_t iloscount = 0; iloscount < 1; iloscount++)
        {

            char detName[255];
            sprintf(detName, "LOS%d", iloscount + 1);

            cLos[iloscount] = new TCanvas(detName, detName, 10, 10, 910, 810);

            fh_los_channels[iloscount] =
                new TH1F(Form("%s_channels", detName), Form("%s channels", detName), 10, 0., 10.);
            fh_los_channels[iloscount]->GetXaxis()->SetTitle("Channel number");

            fh_los_multihit[iloscount] =
                new TH1F(Form("%s_multihit", detName), Form("%s multihit && all 8 PMs", detName), 30, 0., 30.);
            fh_los_multihit[iloscount]->GetXaxis()->SetTitle("Multihit");

            fh_los_pos_MCFD[iloscount] = new TH2F(
                Form("%s_pos_MCFD", detName), Form("%s MCFD Position ", detName), 1000, -5., 5., 1000, -5., 5.);
            fh_los_pos_MCFD[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_MCFD[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_TAMEX[iloscount] = new TH2F(
                Form("%s_pos_TAMEX", detName), Form("%s TAMEX Position ", detName), 1000, -5., 5., 1000, -5., 5.);
            fh_los_pos_TAMEX[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_TAMEX[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_ToT[iloscount] =
                new TH2F(Form("%s_pos_ToT", detName), Form("%s ToT Position ", detName), 1000, -5., 5., 1000, -5., 5.);
            fh_los_pos_ToT[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_ToT[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_dt_hits_ToT[iloscount] =
                new TH2F(Form("%s_dt_ToT", detName), Form("%s ToT dt ", detName), 100000, 0, 500, 500, 0, 200.);
            fh_los_dt_hits_ToT[iloscount]->GetXaxis()->SetTitle(
                "dt between two hits / micros");                             // dt between two hits / ns
            fh_los_dt_hits_ToT[iloscount]->GetYaxis()->SetTitle("ToT / ns"); // ToT / ns

            fh_los_tres_MCFD[iloscount] = new TH1F(
                Form("%s_time_res_MCFD", detName), Form("%s MCFD Time resolution - raw", detName), 4000, -4., 4.);
            fh_los_tres_MCFD[iloscount]->GetXaxis()->SetTitle("Time MCFD / ns");

            fh_los_tres_TAMEX[iloscount] = new TH1F(
                Form("%s_time_res_TAMEX", detName), Form("%s TAMEX Time resolution -raw ", detName), 4000, -4., 4.);
            fh_los_tres_TAMEX[iloscount]->GetXaxis()->SetTitle("Time TAMEX / ns");

            fh_los_tres_MTDC = new TH1F("los_time_res_MTDC", "LOS MTDC Time resolution - raw", 4000, -4, 4);
            fh_los_tres_MTDC->GetXaxis()->SetTitle("Time MTDC / ns");

            fh_los_tot[iloscount] =
                new TH2F(Form("%s_tot", detName), Form("%s ToT vs PMT", detName), 10, 0, 10, 3000, 0., 300.);
            fh_los_tot[iloscount]->GetXaxis()->SetTitle("PMT number");
            fh_los_tot[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            fh_los_tot_mean[iloscount] =
                new TH1F(Form("%s_tot_mean", detName), Form("%s mean ToT", detName), 1500, 0., 300.);
            fh_los_tot_mean[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_tot_mean[iloscount]->GetXaxis()->SetTitle("ToT / ns");

            fh_los_ihit_ToT[iloscount] =
                new TH2F(Form("%s_tot_ihit", detName), Form("%s ToT vs ihit", detName), 15, 0, 15, 9100, -10., 900.);
            fh_los_ihit_ToT[iloscount]->GetXaxis()->SetTitle("iHit");
            fh_los_ihit_ToT[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            fh_los_dt_hits[iloscount] =
                new TH1F(Form("%s_los_dt_hits", detName), Form("%s los dt hits", detName), 200000, 0, 1000.);
            fh_los_dt_hits[iloscount]->GetXaxis()->SetTitle("dt between two hits / micros");
            fh_los_dt_hits[iloscount]->GetYaxis()->SetTitle("Counts");

            fh_dt_hits[iloscount] = new TH1F(Form("%s_dt_hits", detName), Form("%s dt hits", detName), 2000, 0, 500);
            fh_dt_hits[iloscount]->GetXaxis()->SetTitle("dt between two hits / micros");
            fh_dt_hits[iloscount]->GetYaxis()->SetTitle("Counts");

            fh_los_dt_first_ToT[iloscount] = new TH2F(
                Form("%s_dt_events_ToT", detName), Form("%s ToT vs deltat", detName), 5000, 0, 150000, 500, 0, 200.);
            fh_los_dt_first_ToT[iloscount]->GetXaxis()->SetTitle("dt between two events / ns");
            fh_los_dt_first_ToT[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            cLos[iloscount]->Divide(4, 3);
            cLos[iloscount]->cd(1);
            fh_los_channels[iloscount]->Draw();
            cLos[iloscount]->cd(2);
            gPad->SetLogy();
            fh_los_multihit[iloscount]->Draw();
            cLos[iloscount]->cd(3);
            gPad->SetLogz();
            fh_los_tot[iloscount]->Draw("colz");
            cLos[iloscount]->cd(4);
            gPad->SetLogy();
            fh_los_tot_mean[iloscount]->Draw();
            cLos[iloscount]->cd(5);
            gPad->SetLogz();
            fh_los_ihit_ToT[iloscount]->Draw("colz");
            // cLos[iloscount]->cd(6);gPad->SetLogz();
            // fh_los_dt_first_ToT[iloscount]->Draw("colz");
            cLos[iloscount]->cd(6);
            gPad->SetLogz();
            fh_los_dt_hits_ToT[iloscount]->Draw("colz");
            cLos[iloscount]->cd(7);
            gPad->SetLogz();

            cLos[iloscount]->cd(8);
            gPad->SetLogy();
            fh_los_tres_MCFD[iloscount]->Draw();
            cLos[iloscount]->cd(9);
            gPad->SetLogz();
            fh_los_ToT_vs_ToT->Draw("colz");
            cLos[iloscount]->cd(10);
            gPad->SetLogz();
            fh_los_pos_ToT[iloscount]->Draw("colz");
            cLos[iloscount]->cd(11);
            gPad->SetLogz();
            fh_los_pos_MCFD[iloscount]->Draw("colz");
            cLos[iloscount]->cd(12);
            gPad->SetLogz();
            fh_los_pos_TAMEX[iloscount]->Draw("colz");
            cLos[iloscount]->cd(0);
            run->AddObject(cLos[iloscount]);
        }

        run->GetHttpServer()->RegisterCommand("Reset_LOS", Form("/Tasks/%s/->Reset_LOS_Histo()", GetName()));
    }

    //-----------------------------------------------------------------------
    // FIB Spill
    TCanvas* cSpill_FIB = new TCanvas("Spill_FIB", "Spill_FIB", 10, 10, 910, 810);
    fh_spill_times_fib = new TH1F("CountsPerSpillFiber", "counts per spill ", 2500, 0., 5.); // s, 2ms bin
    fh_spill_times_fib_fine =
        new TH1F("CountsPerSpillFiber_fine", "counts per spill fine", 500000, 0., 5.); // s, 10µs bin ///?
    fh_spill_times_fib_Coarse =
        new TH1F("CountsPerSpillFiber_coarse", "counts per spill coarse", 500, 0., 5.);            // s, 10ms bin ///?
    fh_spill_hits_vs_time = new TH1F("hits_vs_timediff", "hits_vs_timediff", 200000, 0., 200000.); // ns, 1ns bin
    fh_FIB_count_t = new TH1F("Hit_in_TrggW", "Hits within Trigger window FIB", 18000, -9., 9.);   //µs, 1ns bin
    fh_FIB_TrgW_dt = new TH1F("dt_TrggW_Fib", "Time-difference within Trigger window", 10000, 0., 10.);   //µs, 1ns bin
    fh_LOS_TrgW_dt = new TH1F("dt_TrggW_LOS", "Time-difference within Trigger window", 10000, 0., 10.);   //µs, 1ns bin
    fh_SAMP_TrgW_dt = new TH1F("dt_TrggW_SAMP", "Time-difference within Trigger window", 10000, 0., 10.); //µs, 1ns bin
    fh_FIB_TrgW_dt_coarse =
        new TH1F("dt_TrggW_Fib_coarse", "Time-difference within Trigger window", 1000, 0., 10.); //µs, 10ns bin
    fh_LOS_TrgW_dt_coarse =
        new TH1F("dt_TrggW_LOS_coarse", "Time-difference within Trigger window", 1000, 0., 10.); //µs, 10ns bin
    fh_SAMP_TrgW_dt_coarse =
        new TH1F("dt_TrggW_SAMP_coarse", "Time-difference within Trigger window", 1000, 0., 10.); //µs, 10ns bin
    fh_FIB_Freq = new TH1F("Fib_freq", "Fiber Freqency", 5000, 0., 5000.);                        // kHz

    cSpill_FIB->Divide(2, 3);
    cSpill_FIB->cd(1);
    gPad->SetLogy();
    fh_spill_hits_vs_time->GetXaxis()->SetTitle("Time-difference [ns]");
    fh_spill_hits_vs_time->GetYaxis()->SetTitle("Counts");
    fh_spill_hits_vs_time->Draw();

    cSpill_FIB->cd(2);
    fh_spill_times_fib->GetXaxis()->SetTitle("Time [s]");
    fh_spill_times_fib->GetYaxis()->SetTitle("Counts");
    fh_spill_times_fib->Draw();

    cSpill_FIB->cd(3);
    gPad->SetLogy();
    fh_FIB_count_t->GetXaxis()->SetTitle("HitTime [us]");
    fh_FIB_count_t->GetYaxis()->SetTitle("Counts");
    fh_FIB_count_t->Draw();
    run->AddObject(cSpill_FIB);

    cSpill_FIB->cd(4);
    gPad->SetLogy();
    fh_SAMP_TrgW_dt->Draw("hist");
    fh_FIB_TrgW_dt->Draw("same");
    fh_LOS_TrgW_dt->Draw("same");
    TLegend* leg_TrggW = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg_TrggW->AddEntry(fh_SAMP_TrgW_dt, "Sampler", "l");
    leg_TrggW->AddEntry(fh_FIB_TrgW_dt, "Fiber1", "l");
    leg_TrggW->AddEntry(fh_LOS_TrgW_dt, "LOS", "l");
    leg_TrggW->Draw("same");
    cSpill_FIB->cd(5);
    gPad->SetLogy();
    fh_SAMP_TrgW_dt_coarse->Draw("hist");
    fh_FIB_TrgW_dt_coarse->Draw("same");
    fh_LOS_TrgW_dt_coarse->Draw("same");
    TLegend* leg_TrggW_coarse = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg_TrggW_coarse->AddEntry(fh_SAMP_TrgW_dt_coarse, "Sampler", "l");
    leg_TrggW_coarse->AddEntry(fh_FIB_TrgW_dt_coarse, "Fiber1", "l");
    leg_TrggW_coarse->AddEntry(fh_LOS_TrgW_dt_coarse, "LOS", "l");
    leg_TrggW_coarse->Draw("same");

    cSpill_FIB->cd(6);
    gPad->SetLogy();
    fh_FIB_Freq->GetXaxis()->SetTitle("Frequency [kHz]");
    fh_FIB_Freq->GetYaxis()->SetTitle("Counts");
    fh_FIB_Freq->Draw();

    TCanvas* cTrggW = new TCanvas("TrggW", "Trigger Window", 10, 10, 910, 810);
    fh_SAMP_TrgW_dt->GetXaxis()->SetTitle("Time-difference [us]");
    fh_SAMP_TrgW_dt->GetYaxis()->SetTitle("Counts");
    fh_SAMP_TrgW_dt->SetLineColor(kBlack);
    fh_FIB_TrgW_dt->GetXaxis()->SetTitle("Time-difference [us]");
    fh_FIB_TrgW_dt->GetYaxis()->SetTitle("Counts");
    fh_FIB_TrgW_dt->SetLineColor(kBlue);
    fh_LOS_TrgW_dt->GetXaxis()->SetTitle("Time-difference [us]");
    fh_LOS_TrgW_dt->GetYaxis()->SetTitle("Counts");
    fh_LOS_TrgW_dt->SetLineColor(kRed);
    fh_SAMP_TrgW_dt_coarse->GetXaxis()->SetTitle("Time-difference [us]");
    fh_SAMP_TrgW_dt_coarse->GetYaxis()->SetTitle("Counts");
    fh_SAMP_TrgW_dt_coarse->SetLineColor(kBlack);
    fh_FIB_TrgW_dt_coarse->GetXaxis()->SetTitle("Time-difference [us]");
    fh_FIB_TrgW_dt_coarse->GetYaxis()->SetTitle("Counts");
    fh_FIB_TrgW_dt_coarse->SetLineColor(kBlue);
    fh_LOS_TrgW_dt_coarse->GetXaxis()->SetTitle("Time-difference [us]");
    fh_LOS_TrgW_dt_coarse->GetYaxis()->SetTitle("Counts");
    fh_LOS_TrgW_dt_coarse->SetLineColor(kRed);

    cTrggW->cd(1);
    gPad->SetLogy();
    gPad->SetLogx();
    fh_SAMP_TrgW_dt->Draw("hist");
    fh_FIB_TrgW_dt->Draw("same");
    fh_LOS_TrgW_dt->Draw("same");
    TLegend* leg_TrggW2 = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg_TrggW2->AddEntry(fh_SAMP_TrgW_dt, "Sampler", "l");
    leg_TrggW2->AddEntry(fh_FIB_TrgW_dt, "Fiber1", "l");
    leg_TrggW2->AddEntry(fh_LOS_TrgW_dt, "LOS", "l");
    leg_TrggW2->Draw("same");

    // LOS Spill
    TCanvas* cSpill_LOS = new TCanvas("Spill_SAMP", "Spill_SAMP", 10, 10, 910, 810);
    fh_spill_times_LOS =
        new TH1F("CountsPerSpillLOSRaw", "counts per spill RAW [100us bin]", 120000, 0., 12.); // bin 100µs
    fh_spill_times_LOS_Fine =
        new TH1F("CountsPerSpillLOSfine", "counts per spill fine [10us bin] ", 1000000, 0., 10.); // 10µs bin
    fh_spill_times_LOS_Fine_adj =
        new TH1F("CountsPerSpillLOSfineadj", "counts per spill fine [10us bin] ", 1000000, 0., 10.); // 10µs bin
    fh_spill_times_LOSFIB_Fine = new TH1F(
        "CountsPerSpillLOSFIBfine", "counts per spill fine Sampler + FIB [10us bin] ", 1000000, 0., 10.); // 10µs bin
    fh_spill_times_LOSFIB_Fine_adj = new TH1F(
        "CountsPerSpillLOSFIBfineadj", "counts per spill fine Sampler + FIB [10us bin] ", 1000000, 0., 10.); // 10µs bin
    fh_spill_times_LOS_Coarse =
        new TH1F("CountsPerSpillLOScoarse", "counts per spill coarse [10ms bin] ", 1200, 0., 12.); // 10ms bin
    fh_SAMPFIB_tDiff =
        new TH1F("Time_difference_SAMPFIB", "Time-difference Sampler + Fiber", 200000, 0., 1.); // 1ms, 5ns bin
    fh_LOS_tDiff = new TH1F("Time_difference_SAMP", "Time-difference Sampler", 200000, 0., 1.); // 1ms, 5ns bin
    fh_LOS_freq = new TH1F("SAMP_Frequencies", "Sampler Frequencies", 6000., 0., 6000.);
    fh_LOS_tDiff_pois = new TH1F("Random_distributed_tDIFF", "Random_tDiff", 2000000, 0., 10.); // 0-1 ms, 5ns bin
    fh_spill_times_LOS_pois = new TH1F(
        "Random_distributed_times", "Random_tDiff", 10000000 * fSpillLength, 0., fSpillLength); // 0-5s, 100ns bin
    fh_FFT = new TH1F("FFT", "FFT", 1000000, 0., 10.);
    fh_FFT_adj = new TH1F("adj_FFT", "adj FFT", 1000000, 0., 10.);
    fh_FFT_adj_SAMPFIB = new TH1F("adj_FFT_SAMPFIB", "adj FFT", 1000000, 0., 10.);
    fh_FFT_add = new TH1F("added_FFT", "added FFT", 1000000, 0., 10.);
    fh_FFT_add_SAMPFIB = new TH1F("added_FFT_SAMPFIB", "added FFT", 1000000, 0., 10.);
    fh_hans_sum = new TH1F("Spillquality_sum", "Spill quality HT", 600, -300., 300.);
    fh_hans_sum_byMax = new TH1F("Spillquality_sum_byMax", "Spill quality HT", 600, -300., 300.);
    fh_EL_sum = new TH1F("QualityFactor_sum", "Spill quality EL", 200, -100., 100.);
    fh_MtA_sum = new TH1F("MaxToAverage_sum", "Maximum to average", 1000, 0., 100.);
    fh_deadTime_adj = new TH1F("DeadTime_adj", "DeadTime adjustment", 3000, 0., 300.);
    fh_deadTime_mean = new TH1F("DeadTime_mean", "DeadTime mean", 3000, 0., 300.);
    fh_deadTime_sum = new TH1F("DeadTime_sum", "DeadTime sum", 3000, 0., 3000.); //µs bin

    fh_rate = new TH1F("rate", "accepted events relativ to poisson", 360, 0, 180);
    fh_rate->GetXaxis()->SetTitle("spill number");
    fh_rate->GetYaxis()->SetTitle("Percent");
    fh_rate_SAMPFIB = new TH1F("rate_SAMPFIB", "accepted events relativ to poisson", 360, 0, 180);
    fh_rate->GetXaxis()->SetTitle("spill number");
    fh_rate->GetYaxis()->SetTitle("Percent");
    fh_rate_adj = new TH1F("rate_adj", "accepted events relativ to poisson - adj deadtime", 360, 0, 180);
    fh_rate_adj->GetXaxis()->SetTitle("spill number");
    fh_rate_adj->GetYaxis()->SetTitle("Percent");
    fh_rate_set = new TH1F("rate_set", "accepted events relativ to poisson - set deadtime", 360, 0, 180);
    fh_rate_set->GetXaxis()->SetTitle("spill number");
    fh_rate_set->GetYaxis()->SetTitle("Percent");

    fh_particle = new TH1F("particle_per_time", "Particle per time", 100, -50., 50.);
    fh_particle->GetXaxis()->SetTitle("particle per time [100us]");
    fh_particle->GetYaxis()->SetTitle("counts");

    fh_particle_pois = new TH1F("particle_per_time_pois", "Particle per time poisson", 100, -50., 50.);
    fh_particle_pois->GetXaxis()->SetTitle("particle per time [100us]");
    fh_particle_pois->GetYaxis()->SetTitle("counts");
    fh_particle_pois->SetLineColor(kRed);

    fh_spill_times_daq = new TH1F("CountsPerSpill_daq", "counts per spill daq", 50000, 0., 5.);
    fh_spill_times_daq->GetXaxis()->SetTitle("time / sec");
    fh_spill_times_daq->GetYaxis()->SetTitle("counts");

    fh_spill_times_daq_first =
        new TH1F("CountsPerSpill_daq_first", "counts per spill daq first hit only", 50000, 0., 5.);
    fh_spill_times_daq_first->GetXaxis()->SetTitle("time / sec");
    fh_spill_times_daq_first->GetYaxis()->SetTitle("counts");

    cSpill_LOS->Divide(4, 3);

    cSpill_LOS->cd(1);
    fh_spill_times_LOS->GetXaxis()->SetTitle("time / sec");
    fh_spill_times_LOS->GetYaxis()->SetTitle("counts");
    fh_spill_times_LOS->Draw();

    cSpill_LOS->cd(2);
    fh_spill_times_LOS_Fine->GetXaxis()->SetTitle("time / sec");
    fh_spill_times_LOS_Fine->GetYaxis()->SetTitle("counts");
    fh_spill_times_LOS_Fine->Draw();

    cSpill_LOS->cd(3);
    fh_spill_times_LOS_Coarse->GetXaxis()->SetTitle("time / sec");
    fh_spill_times_LOS_Coarse->GetYaxis()->SetTitle("counts");
    fh_spill_times_LOSFIB_Fine->GetXaxis()->SetTitle("time / sec");
    fh_spill_times_LOSFIB_Fine->GetYaxis()->SetTitle("counts");
    fh_spill_times_LOSFIB_Fine->Draw();

    cSpill_LOS->cd(6);
    fh_LOS_tDiff->GetXaxis()->SetTitle("dt/ms");
    fh_LOS_tDiff->GetYaxis()->SetTitle("counts");
    cSpill_LOS->cd(6);
    gPad->SetLogx();
    gPad->SetLogy();
    fh_LOS_tDiff->Draw();
    cSpill_LOS->cd(7);
    fh_SAMPFIB_tDiff->GetXaxis()->SetTitle("dt/ms");
    fh_SAMPFIB_tDiff->GetYaxis()->SetTitle("counts");
    cSpill_LOS->cd(7);
    gPad->SetLogx();
    gPad->SetLogy();
    fh_SAMPFIB_tDiff->Draw();
    cSpill_LOS->cd(5);
    fh_LOS_tDiff_pois->GetXaxis()->SetTitle("dt/ms");
    fh_LOS_tDiff_pois->GetYaxis()->SetTitle("counts");
    cSpill_LOS->cd(5);
    gPad->SetLogx();
    gPad->SetLogy();
    fh_LOS_tDiff_pois->SetStats(1);
    fh_LOS_tDiff_pois->Draw("hist");
    cSpill_LOS->cd(8);
    cSpill_LOS->cd(8);
    gPad->SetLogy();
    fh_FFT_adj->GetXaxis()->SetTitle("Frequency [10kHz]");
    fh_FFT_adj->GetYaxis()->SetTitle("Magnitude arb. units");
    fh_FFT_adj->Draw();
    fh_FFT_add->GetXaxis()->SetTitle("Frequency [10kHz]");
    fh_FFT_add->GetYaxis()->SetTitle("Magnitude arb. units");

    cSpill_LOS->cd(9);
    gPad->SetLogy();
    fh_LOS_freq->GetXaxis()->SetTitle("kHz");
    fh_LOS_freq->GetYaxis()->SetTitle("counts");
    fh_LOS_freq->Draw();

    cSpill_LOS->cd(4);
    gPad->SetLogy();
    fh_particle->Draw("hist");
    fh_particle_pois->Draw("same");
    TLegend* leg_particle = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg_particle->AddEntry(fh_particle, "Measured", "l");
    leg_particle->AddEntry(fh_particle_pois, "Poisson", "l");
    leg_particle->Draw("same");

    cSpill_LOS->cd(10);
    // cSpill_LOS->cd(9);gPad->SetLogy();
    fh_rate->SetFillColor(3);
    fh_rate->GetYaxis()->SetRange(0, 110);
    fh_rate->Draw("B");

    cSpill_LOS->cd(11);
    cSpill_LOS->cd(11);
    gPad->SetLogx();
    fh_los_dt_hits[0]->Draw(); // For LOS1

    cSpill_LOS->cd(12);
    gPad->SetLogy();
    fh_deadTime_sum->GetXaxis()->SetTitle("DeadTime [us]");
    fh_deadTime_sum->GetYaxis()->SetTitle("Counts");
    fh_deadTime_sum->Draw();
    run->AddObject(cSpill_LOS);

    run->GetHttpServer()->RegisterCommand("Reset_SPILL_LOS", Form("/Tasks/%s/->Reset_SPILLMON_Histo()", GetName()));

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    TCanvas* FibCanvas[NOF_FIB_DET];

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

            cout << "I am creating canvas " << detName << endl;

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
            fh_fibers_Fib[ifibcount] = new TH1F(Form("%s_fibers", detName), Form("%s fibers", detName), 512, 0., 512.);
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
            // fh_ToT_m_Fib[ifibcount] = new TH2F(Form("%s_tot_m",detName), Form("%s ToT of MAPMT",detName),
            // N_FIBER_PLOT, 0., N_FIBER_PLOT, 100, 0., 100.);
            fh_ToT_m_Fib[ifibcount] =
                new TH2F(Form("%s_tot_m", detName), Form("%s ToT of MAPMT", detName), 512, 0., 512, 300, 0., 100.);
            fh_ToT_m_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_m_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToT SAPMT:
            fh_ToT_s_Fib[ifibcount] =
                new TH2F(Form("%s_tot_s", detName), Form("%s ToT of single PMT", detName), 512, 0., 512, 400, 0., 40.);
            fh_ToT_s_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_s_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // Time of fiber:
            fh_time_Fib[ifibcount] = new TH2F(
                Form("%s_TimevsFiber", detName), Form("%s Time vs Fiber", detName), 512, 0., 512, 2048, -1024., 1024.);
            fh_time_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_time_Fib[ifibcount]->GetYaxis()->SetTitle("tMAPMT-tSPMT");

            // ToF LOS -> Fiber:
            fh_Fib_ToF[ifibcount] = new TH2F(
                Form("%s_tof", detName), Form("%s ToF LOS to Fiber", detName), 512, 0., 512, 1000, -50000., 50000.);
            fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("Fiber number");

            // Not-calibrated position:
            fh_Fib_pos_xy[ifibcount] =
                new TH2F(Form("%s_pos", detName), Form("%s xy position", detName), 500, 0., 500, 1000, 0., 200.);
            fh_Fib_pos_xy[ifibcount]->GetXaxis()->SetTitle("x position (fiber)");
            fh_Fib_pos_xy[ifibcount]->GetYaxis()->SetTitle("y position (dt)");

            // hit fiber number vs. event number:
            fh_Fib_vs_Events[ifibcount] = new TH2F(Form("%s_fib_vs_event", detName),
                                                   Form("%s Fiber # vs. Event #", detName),
                                                   1000,
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
            fh_Fib_pos_xy[ifibcount]->Draw("colz");
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
    if (NOF_FIB_DET > 0)
        run->GetHttpServer()->RegisterCommand("Reset_Fibers", Form("/Tasks/%s/->Reset_FIBERS_Histo()", GetName()));

    //----------------------------------------
    // Poisson
    TCanvas* cPoisson = new TCanvas("Poisson_Sampler", "Poisson_Sampler", 10, 10, 910, 810);
    fh_r_pois_fib = new TH1F("R_Pois_fib", "Relative to poisson per spill FIB", 200, 0., 1.);
    fh_r_pois_LOS = new TH1F("R_Pois_LOS", "Relative to poisson per spill LOS", 200, 0., 2.);
    fh_spill_hans = new TH1F("Spill_quality", "Spill Quality", 360., 0., 180.);
    fh_spill_hans_byMax = new TH1F("Spill_quality_HT_byMax", "Spill Quality", 360., 0., 180.);
    fh_spill_Q_E_real = new TH1F("QualityFactorReal", "Quality Factor", 360., 0., 180.);
    fh_spill_Q_E_pois = new TH1F("QualityFactorPois", "Quality Factor Pois", 360., 0., 180.);
    fh_spill_frac = new TH1F("frac_under_border", "frac under border", 100., 0., 50.);
    fh_DutyFactor_LOS = new TH1F("DutyFactor", "Duty Factor", 1200., 0., 12. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_DutyFactor_MtA =
        new TH1F("DutyFactor_MtA", "Duty Factor MtA", 1200., 0., 12. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_DutyFactor_LOS_pois =
        new TH1F("DutyFactor_pois", "Duty Factor", 1200., 0., 12. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_DutyFactor_MaxToAvg =
        new TH1F("MaxToAvg", "Duty Factor - Maximum to Average", 360., 0., 180. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_DutyFactor_MaxRun =
        new TH1F("DFMaxRun", "Duty Factor - Maximum over run", 360., 0., 180. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_DutyFactor_AvgRun =
        new TH1F("DFAvgRun", "Duty Factor - Average over run", 360., 0., 180. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_rate_sum = new TH1F("rate_sum", "Rate accepted relative to poisson", 200, -100., 100.);
    fh_rate_sum_DTmean = new TH1F("rate_sum_DTmean", "Rate accepted relative to poisson", 200, -100., 100.);
    fh_spillQuality_ppt = new TH1F("SpillQuality_ppt", "Spill Quality ppt", 360, 0., 180.);
    fh_spillQuality_ppt_stat = new TH1F("SpillQuality_ppt_stat", "Spill Quality ppt", 300, 0., 300.);
    fh_DutyFactor_SAMP_clean = new TH1F("DutyFactor_SAMPFIB_clean",
                                        "Duty Factor meassured/poisson",
                                        1200.,
                                        0.,
                                        12. /*, 2000., 0., 2.*/); // timebin in 10ms

    cPoisson->Divide(3, 3);

    cPoisson->cd(1);

    fh_DutyFactor_LOS->GetXaxis()->SetTitle("time [s]");
    fh_DutyFactor_LOS->GetYaxis()->SetTitle("Duty Factor");
    fh_DutyFactor_LOS->SetFillColor(kBlue);
    fh_DutyFactor_LOS->Draw("HIST");
    fh_DutyFactor_LOS_pois->SetMarkerStyle(20);
    fh_DutyFactor_LOS_pois->SetMarkerColor(kRed);
    fh_DutyFactor_LOS_pois->SetLineColor(kRed);
    fh_DutyFactor_LOS_pois->Draw("SAME P C");
    cPoisson->cd(2);
    fh_DutyFactor_MaxToAvg->GetXaxis()->SetTitle("Spillnumber");
    fh_DutyFactor_MaxToAvg->SetFillColor(kGreen);
    fh_DutyFactor_MtA->GetXaxis()->SetTitle("time [s]");
    fh_DutyFactor_MtA->GetYaxis()->SetTitle("Duty Factor");
    fh_DutyFactor_MtA->SetFillColor(kBlue);
    fh_DutyFactor_SAMP_clean->GetXaxis()->SetTitle("time [s]");
    fh_DutyFactor_SAMP_clean->GetYaxis()->SetTitle("Duty Factor");
    fh_DutyFactor_SAMP_clean->SetFillColor(kGreen);
    fh_DutyFactor_SAMP_clean->Draw("Hist");

    cPoisson->cd(3);
    fh_r_pois_LOS->GetXaxis()->SetTitle("Ratio to poisson");
    fh_r_pois_LOS->GetYaxis()->SetTitle("counts");
    fh_r_pois_LOS->Draw();

    cPoisson->cd(4);
    fh_spill_hans->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_hans->GetXaxis()->SetTitle("Spill number");
    fh_spill_hans->SetFillColor(3);
    fh_spill_hans->GetYaxis()->SetRange(-250, 250);
    fh_spill_hans->Draw("B");

    cPoisson->cd(5);
    fh_spill_Q_E_real->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_Q_E_real->GetXaxis()->SetTitle("Spill number");
    fh_spill_Q_E_real->SetFillColor(4);
    fh_spill_Q_E_real->GetYaxis()->SetRange(-100, 100);
    fh_spill_Q_E_pois->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_Q_E_pois->GetXaxis()->SetTitle("Spill number");
    fh_spill_Q_E_pois->SetFillColor(2);
    fh_spill_Q_E_pois->GetYaxis()->SetRange(-100, 100);
    fh_spill_Q_E_real->Draw("B");
    // fh_spill_Q_E_pois->Draw("SAME B");

    cPoisson->cd(6);
    // fh_rate_sum->Draw();
    fh_rate->Draw();

    cPoisson->cd(7);
    fh_spillQuality_ppt->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spillQuality_ppt->GetXaxis()->SetTitle("Spill number");
    fh_spillQuality_ppt->SetFillColor(5);
    fh_spillQuality_ppt->SetLineColor(5);
    // fh_spillQuality_ppt->Draw("B");
    fh_spill_hans_byMax->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_hans_byMax->GetXaxis()->SetTitle("Spill number");
    fh_spill_hans_byMax->SetFillColor(3);
    fh_spill_hans_byMax->GetYaxis()->SetRange(-250, 250);
    fh_spill_hans_byMax->Draw("B");

    cPoisson->cd(8);
    fh_DutyFactor_MaxRun->GetYaxis()->SetTitle("Spill quality in percent");
    fh_DutyFactor_MaxRun->GetXaxis()->SetTitle("Spill number");
    fh_DutyFactor_AvgRun->GetYaxis()->SetTitle("Spill quality in percent");
    fh_DutyFactor_AvgRun->GetXaxis()->SetTitle("Spill number");
    fh_DutyFactor_MaxRun->SetFillColor(2);
    fh_DutyFactor_AvgRun->SetFillColor(4);
    fh_DutyFactor_MaxRun->Draw("B");
    fh_DutyFactor_AvgRun->Draw("SAME B");

    cPoisson->cd(3);
    gPad->SetLogy();
    fh_FFT_add->GetXaxis()->SetTitle("Frequency [10kHz]");
    fh_FFT_add->GetYaxis()->SetTitle("Magnitude arb. units");
    fh_FFT_add->Draw();

    cPoisson->cd(9);
    fh_rate_adj->SetFillColor(4);
    fh_rate_adj->Draw("bar");

    run->AddObject(cPoisson);

    // Poisson_SAMPFIB
    TCanvas* cPoisson_SAMPFIB = new TCanvas("Poisson_SAMPFIB", "Poisson_SAMPFIB", 10, 10, 910, 810);
    fh_spill_hans_SAMPFIB = new TH1F("Spill_quality_SAMPFIB", "Spill Quality", 360., 0., 180.);
    fh_spill_hans_SAMPFIB_byMean = new TH1F("Spill_quality_SAMPFIB_byMax", "Spill Quality", 360., 0., 180.);
    fh_spill_Q_E_real_SAMPFIB = new TH1F("QualityFactorReal_SAMPFIB", "Quality Factor Real", 360., 0., 180.);
    fh_spill_Q_E_pois_SAMPFIB = new TH1F("QualityFactorPois_SAMPFIB", "Quality Factor Pois", 360., 0., 180.);
    fh_spill_frac_SAMPFIB = new TH1F("frac_under_border_SAMPFIB", "frac under border", 100., 0., 50.);
    fh_DutyFactor_SAMPFIB =
        new TH1F("DutyFactor_SAMPFIB", "Duty Factor", 1200., 0., 12. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_DutyFactor_SAMPFIB_pois =
        new TH1F("DutyFactor_SAMPFIB_pois", "Duty Factor", 1200., 0., 12. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_rate_sum_SAMPFIB = new TH1F("rate_sum_SAMPFIB", "Rate accepted relative to poisson", 200, -100., 100.);
    fh_rate_sum_DTmean_SAMPFIB =
        new TH1F("rate_sum_SAMPFIB_DTmean", "Rate accepted relative to poisson", 200, 0., 200.);
    fh_spillQuality_ppt_SAMPFIB = new TH1F("SpillQuality_ppt_SAMPFIB", "Spill Quality ppt", 360, 0., 180.);
    fh_spillQuality_ppt_stat_SAMPFIB = new TH1F("SpillQuality_ppt_stat_SAMPFIB", "Spill Quality ppt", 300, 0., 300.);
    fh_DutyFactor_SAMPFIB_clean = new TH1F("DutyFactor_SAMPFIB_clean",
                                           "Duty Factor meassured/poisson",
                                           1200.,
                                           0.,
                                           12. /*, 2000., 0., 2.*/); // timebin in 10ms
    fh_rate_adj_SAMPFIB = new TH1F("rate_adj", "accepted events relativ to poisson - adj deadtime", 360, 0, 180);
    fh_rate_adj_SAMPFIB->GetXaxis()->SetTitle("spill number");
    fh_rate_adj_SAMPFIB->GetYaxis()->SetTitle("Percent");
    fh_rate_set_SAMPFIB = new TH1F("rate_set", "accepted events relativ to poisson - set deadtime", 360, 0, 180);
    fh_rate_set_SAMPFIB->GetXaxis()->SetTitle("spill number");
    fh_rate_set_SAMPFIB->GetYaxis()->SetTitle("Percent");

    cPoisson_SAMPFIB->Divide(3, 3);

    cPoisson_SAMPFIB->cd(1);
    fh_DutyFactor_SAMPFIB->GetXaxis()->SetTitle("time [s]");
    fh_DutyFactor_SAMPFIB->GetYaxis()->SetTitle("Duty Factor");
    fh_DutyFactor_SAMPFIB->SetFillColor(kBlue);
    fh_DutyFactor_SAMPFIB->Draw("HIST");
    fh_DutyFactor_SAMPFIB_pois->SetMarkerStyle(20);
    fh_DutyFactor_SAMPFIB_pois->SetMarkerColor(kRed);
    fh_DutyFactor_SAMPFIB_pois->SetLineColor(kRed);
    fh_DutyFactor_SAMPFIB_pois->Draw("SAME P C");
    cPoisson_SAMPFIB->cd(2);
    fh_DutyFactor_MaxToAvg->GetXaxis()->SetTitle("Spillnumber");
    fh_DutyFactor_MaxToAvg->SetFillColor(kGreen);
    fh_DutyFactor_MtA->GetXaxis()->SetTitle("time [s]");
    fh_DutyFactor_MtA->GetYaxis()->SetTitle("Duty Factor");
    fh_DutyFactor_MtA->SetFillColor(kBlue);
    // fh_DutyFactor_MtA->Draw("HIST");

    cPoisson_SAMPFIB->cd(2);
    fh_r_pois_LOS->GetXaxis()->SetTitle("Ratio to poisson");
    fh_r_pois_LOS->GetYaxis()->SetTitle("counts");
    // fh_r_pois_LOS->Draw();
    fh_DutyFactor_SAMPFIB_clean->GetXaxis()->SetTitle("time [s]");
    fh_DutyFactor_SAMPFIB_clean->GetYaxis()->SetTitle("Duty Factor");
    fh_DutyFactor_SAMPFIB_clean->SetFillColor(kGreen);
    fh_DutyFactor_SAMPFIB_clean->Draw("Hist");

    cPoisson_SAMPFIB->cd(4);
    fh_spill_hans_SAMPFIB->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_hans_SAMPFIB->GetXaxis()->SetTitle("Spill number");
    fh_spill_hans_SAMPFIB->SetFillColor(3);
    fh_spill_hans_SAMPFIB->GetYaxis()->SetRange(-250, 250);
    fh_spill_hans_SAMPFIB->Draw("B");

    cPoisson_SAMPFIB->cd(5);
    fh_spill_Q_E_real_SAMPFIB->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_Q_E_real_SAMPFIB->GetXaxis()->SetTitle("Spill number");
    fh_spill_Q_E_real_SAMPFIB->SetFillColor(4);
    fh_spill_Q_E_real_SAMPFIB->GetYaxis()->SetRange(-100, 100);
    fh_spill_Q_E_pois_SAMPFIB->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_Q_E_pois_SAMPFIB->GetXaxis()->SetTitle("Spill number");
    fh_spill_Q_E_pois_SAMPFIB->SetFillColor(2);
    fh_spill_Q_E_pois_SAMPFIB->GetYaxis()->SetRange(-100, 100);
    fh_spill_Q_E_real_SAMPFIB->Draw("B");
    // fh_spill_Q_E_pois_SAMPFIB->Draw("SAME B");

    cPoisson_SAMPFIB->cd(6);
    // fh_rate_sum->Draw();
    fh_rate_SAMPFIB->SetFillColor(4);
    fh_rate_SAMPFIB->Draw();

    cPoisson_SAMPFIB->cd(7);
    fh_spillQuality_ppt_SAMPFIB->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spillQuality_ppt_SAMPFIB->GetXaxis()->SetTitle("Spill number");
    fh_spillQuality_ppt_SAMPFIB->SetFillColor(5);
    fh_spillQuality_ppt_SAMPFIB->SetLineColor(5);
    // fh_spillQuality_ppt_SAMPFIB->Draw("B");
    fh_spill_hans_SAMPFIB_byMean->GetYaxis()->SetTitle("Spill quality in percent");
    fh_spill_hans_SAMPFIB_byMean->GetXaxis()->SetTitle("Spill number");
    fh_spill_hans_SAMPFIB_byMean->SetFillColor(3);
    fh_spill_hans_SAMPFIB_byMean->GetYaxis()->SetRange(-250, 250);
    fh_spill_hans_SAMPFIB_byMean->Draw("B");
    cPoisson_SAMPFIB->cd(8);
    fh_DutyFactor_MaxRun->GetYaxis()->SetTitle("Spill quality in percent");
    fh_DutyFactor_MaxRun->GetXaxis()->SetTitle("Spill number");
    fh_DutyFactor_AvgRun->GetYaxis()->SetTitle("Spill quality in percent");
    fh_DutyFactor_AvgRun->GetXaxis()->SetTitle("Spill number");
    fh_DutyFactor_MaxRun->SetFillColor(2);
    fh_DutyFactor_AvgRun->SetFillColor(4);
    // fh_DutyFactor_MaxRun->Draw("B");
    // fh_DutyFactor_AvgRun->Draw("SAME B");

    cPoisson_SAMPFIB->cd(3);
    gPad->SetLogy();
    fh_FFT_add_SAMPFIB->GetXaxis()->SetTitle("Frequency [10kHz]");
    fh_FFT_add_SAMPFIB->GetYaxis()->SetTitle("Magnitude arb. units");
    fh_FFT_add_SAMPFIB->Draw();

    cPoisson_SAMPFIB->cd(9);
    fh_rate_adj_SAMPFIB->SetFillColor(4);
    fh_rate_adj_SAMPFIB->Draw("bar");

    run->AddObject(cPoisson_SAMPFIB);
    //---------------SAMPFIB sum histos----------------------------------------------------

    fh_hans_SAMPFIB_sum = new TH1F("Spillquality_SAMPFIB_sum", "Spill quality HT", 600, -300., 300.);
    fh_hans_SAMPFIB_sum_byMean = new TH1F("Spillquality_SAMPFIB_sum_byMean", "Spill quality HT", 600, -300., 300.);
    fh_EL_SAMPFIB_sum = new TH1F("QualityFactor_SAMPFIB_sum", "Spill quality EL", 200, -100., 100.);
    fh_DutyFactor_Max_SAMPFIB = new TH1F("DutyFactor_SAMPFIB_Max", "DutyFactor Max", 100, 0., 1.);
    fh_DutyFactor_Max_SAMPFIB_clean = new TH1F("DutyFactor_SAMPFIB_Max_clean", "DutyFactor Max", 200, -100., 100.);
    fh_DutyFactor_Avg_SAMPFIB = new TH1F("DutyFactor_SAMPFIB_Avg", "DutyFactor Avg", 100, 0., 1.);
    fh_DutyFactor_Avg_SAMPFIB_clean = new TH1F("DutyFactor_SAMPFIB_Avg_clean", "DutyFactor Avg", 200, -100., 100.);

    //---------Duty Factor-----------------------------------------------------------------
    TCanvas* cDutyF = new TCanvas("DutyFactor", "DutyFactor", 10, 10, 910, 810);
    fh_DutyFactor_Max = new TH1F("DutyFactor_Max", "DutyFactor Max", 100, 0., 1.);
    fh_DutyFactor_Max_clean = new TH1F("DutyFactor_Max_clean", "DutyFactor Max", 100, 0., 1.);
    fh_DutyFactor_Avg = new TH1F("DutyFactor_Avg", "DutyFactor Avg", 100, 0., 1.);
    fh_DutyFactor_Avg_clean = new TH1F("DutyFactor_Avg_clean", "DutyFactor Avg", 200, -100., 100.);
    fh_DutyFactor_PLD = new TH1F("DutyFactor_PLD", "DutyFactor Poisson-Limit-Difference", 100, 0., 50.);
    cDutyF->Divide(2, 2);

    cDutyF->cd(1);
    fh_DutyFactor_LOS->GetXaxis()->SetTitle("time [s]");
    fh_DutyFactor_LOS->GetYaxis()->SetTitle("Duty Factor");
    fh_DutyFactor_LOS->SetFillColor(kBlue);
    fh_DutyFactor_LOS->Draw("HIST");
    fh_DutyFactor_LOS_pois->SetMarkerStyle(20);
    fh_DutyFactor_LOS_pois->SetMarkerColor(kRed);
    fh_DutyFactor_LOS_pois->SetLineColor(kRed);
    fh_DutyFactor_LOS_pois->Draw("SAME P C");
    cDutyF->cd(2);
    // fh_DutyFactor_MaxToAvg->SetMarkerStyle(20);
    fh_DutyFactor_MaxToAvg->GetXaxis()->SetTitle("Spillnumber");
    fh_DutyFactor_MaxToAvg->SetFillColor(kGreen);
    // fh_DutyFactor_MaxToAvg->SetLineColor(kGreen);
    fh_DutyFactor_MaxToAvg->Draw("Bar");

    cDutyF->cd(3);
    // cPoisson->cd(3);
    fh_DutyFactor_Avg->GetXaxis()->SetTitle("Average Duty Factor");
    fh_DutyFactor_Avg->GetYaxis()->SetTitle("Counts");
    fh_DutyFactor_Avg->Draw();
    fh_DutyFactor_Max->GetXaxis()->SetTitle("Maximum Duty Factor");
    fh_DutyFactor_Max->GetYaxis()->SetTitle("Counts");
    fh_DutyFactor_Max->Draw("same");
    fh_DutyFactor_Max->SetLineColor(kRed);
    TLegend* leg_DutyFactor = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg_DutyFactor->AddEntry(fh_DutyFactor_Avg, "Average", "l");
    leg_DutyFactor->AddEntry(fh_DutyFactor_Max, "Maximum", "l");
    leg_DutyFactor->Draw("same");

    cDutyF->cd(4);
    fh_DutyFactor_PLD->GetXaxis()->SetTitle("Spill number");
    fh_DutyFactor_PLD->GetYaxis()->SetTitle("Difference Avg to Pois-Limit");
    fh_DutyFactor_PLD->SetFillColor(kGreen);
    fh_DutyFactor_PLD->Draw("bar");

    cDutyF->cd(3);
    gPad->SetLogy();
    fh_FFT_add->Draw();

    run->AddObject(cDutyF);
    // gPad->SetLogy(); gStyle->SetOptFit(1111);
    // run->GetHttpServer()->RegisterCommand("Reset_POIS", Form("/Tasks/%s/->Reset_SPILLMON_Histo()", GetName()));
    puts("9999999999999999999999999999999999999999999999999999999");
    return kSUCCESS;
}

void R3BOnlineSpillAnalysis::Reset_LOS_Histo()
{
    if (fMappedItems.at(DET_LOS))
    {
        for (Int_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        {
            fh_los_channels[iloscount]->Reset();
            fh_los_tres_MCFD[iloscount]->Reset();
            fh_los_tres_TAMEX[iloscount]->Reset();
            fh_los_pos_ToT[iloscount]->Reset();
            fh_los_tot[iloscount]->Reset();
            fh_los_tot_mean[iloscount]->Reset();
            fh_los_pos_MCFD[iloscount]->Reset();
            fh_los_pos_TAMEX[iloscount]->Reset();
            fh_los_multihit[iloscount]->Reset();
            fh_los_ihit_ToT[iloscount]->Reset();
        }
        if (fNofLosDetectors == 2)
            fh_los_ToT_vs_ToT->Reset();
    }
}

void R3BOnlineSpillAnalysis::Reset_SPILLMON_Histo()
{
    fh_spill_hits_vs_time->Reset();
    fh_spill_hits_vs_time_fine->Reset();
    fh_spill_times_fib->Reset();
    fh_spill_times_LOS->Reset();
    fh_spill_times_LOS_Fine->Reset();
    fh_spill_times_LOSFIB_Fine->Reset();
    fh_spill_times_LOS_Coarse->Reset();
    fh_r_pois_fib->Reset();
    fh_r_pois_LOS->Reset();
    fh_LOS_tDiff->Reset();
    fh_SAMPFIB_tDiff->Reset();
    fh_LOS_freq->Reset();
    fh_LOS_tDiff_pois->Reset();
    fh_DutyFactor_LOS->Reset();
    fh_DutyFactor_LOS_pois->Reset();
    fh_DutyFactor_MtA->Reset();
    fh_DutyFactor_MaxToAvg->Reset();
    fh_DutyFactor_PLD->Reset();
    fh_DutyFactor_Max->Reset();
    fh_DutyFactor_Avg->Reset();
    fh_spill_times_daq->Reset();
    fh_spill_times_daq_first->Reset();
    fh_rate->Reset();
    fh_rate_set->Reset();
    fh_rate_adj->Reset();
    fh_rate_sum->Reset();
    fh_rate_sum_DTmean->Reset();
    fh_particle->Reset();
    fh_particle_pois->Reset();
    fh_spill_Q_E_real->Reset();
    fh_spill_Q_E_pois->Reset();
    fh_spill_frac->Reset();
    fh_spill_hans->Reset();
    fh_FFT->Reset();
    fh_FFT_adj->Reset();
    fh_FFT_adj_SAMPFIB->Reset();
    fh_FFT_add->Reset();
    fh_FFT_add_SAMPFIB->Reset();
    fh_hans_sum->Reset();
    fh_hans_sum_byMax->Reset();
    fh_EL_sum->Reset();
    fh_MtA_sum->Reset();
    fh_deadTime_adj->Reset();
    fh_deadTime_mean->Reset();
    fh_deadTime_sum->Reset();
    fh_los_dt_hits[0]->Reset();
    fh_spillQuality_ppt->Reset();
    fh_spillQuality_ppt_stat->Reset();
    fh_DutyFactor_MaxRun->Reset();
    fh_DutyFactor_AvgRun->Reset();
    fh_FIB_count_t->Reset();
    fh_FIB_TrgW_dt->Reset();
    fh_LOS_TrgW_dt->Reset();
    fh_SAMP_TrgW_dt->Reset();
    fh_FIB_TrgW_dt_coarse->Reset();
    fh_LOS_TrgW_dt_coarse->Reset();
    fh_SAMP_TrgW_dt_coarse->Reset();
    fh_FIB_Freq->Reset();
}

void R3BOnlineSpillAnalysis::Reset_FIBERS_Histo()
{

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
            fh_Fib_pos_xy[ifibcount]->Reset();
            fh_Fib_vs_Events[ifibcount]->Reset();
            fh_channels_single_Fib[ifibcount]->Reset();
        }
    }
}

void R3BOnlineSpillAnalysis::Exec(Option_t* option)
{

    cout << "fNEvents " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(ERROR) << "FairRootManager not found";
        return;
    }

    time = header->GetTimeStamp() / 1.6; // EVENT-TIME?
    /**Read comments further down (Fiber loop) regarding vulom -> dividing 1.6 */
    // cout<<"time: "<<time<<endl;
    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }
    Bool_t spectra_clear = false;
    Double_t xtime = double(time - time_start) / 1.e9;
    // for reseting spectra
    Int_t icount = iclear_count * reset_time; // reset after reset_time (sec)
    if (time_clear < 0. && int(xtime) % icount == 0 && xtime > 1.)
    {
        time_clear = xtime;
        spectra_clear = true;
    }

    if (header->GetTrigger() == 12)
    {
        time_spill_start = header->GetTimeStamp() / 1.6; // spill start in nsec
        spill_on = true;
        spill_on_sampler = true;
    }
    else
        spill_on = false;

    if (spill_on && time > 0)
    {
        time_spill_start = time; // spill start in nsec
        cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " " << endl;
        spillCounter++;
        dt_LOS = 0.;
        tt_prev = 0.;
        spill_ctr_LOS = 0;
        fh_spill_times_LOS_Fine->Reset();
        fh_spill_times_LOSFIB_Fine->Reset();
        fh_spill_times_LOS_Coarse->Reset();
        fh_LOS_tDiff->Reset();
        fh_SAMPFIB_tDiff->Reset();
        fh_los_dt_hits[0]->Reset();
        skip_spill = false;
        spillon_fib = true;
        fh_spill_times_fib->Reset();
        fh_spill_hits_vs_time->Reset();

        fh_FIB_TrgW_dt_coarse->Reset();
        fh_LOS_TrgW_dt_coarse->Reset();
        fh_SAMP_TrgW_dt_coarse->Reset();
    }

    if (fMappedItems.at(DET_SAMPLER))
    {
        // cout << "Samples:\n";

        auto det = fMappedItems.at(DET_SAMPLER);
        Int_t nHitsSamp = det->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHitsSamp; ihit++)
        {
            auto hit = (R3BSamplerMappedData*)det->At(ihit);
            // time is in steps of 10 ns
            // is is a 34 bit number, so max 1073741823
            samplerCurr = hit->GetTime();

            // cout<<"Test: " << samplerCurr<<endl;
            if (spill_on_sampler && ihit == 0)
            {
                samplerSpill = samplerCurr;
                spill_on_sampler = false;
                // cout<<"sampSpill: "<<samplerSpill<<" samplerCurr: "<<samplerCurr<<"
                // "<<samplerSpill-samplerCurr<<endl;
            }

            Int_t SE_ctr = 0;
            spill_off_calc = false;
            Double_t SL_var = fSpillLength;
            long samp = (samplerCurr - samplerSpill); // time in 10 ns
            if (samp < 0)
                samp += 1073741823;
            // cout<<"samp: "<<samp<<endl;
            if ((double)samp / (1e8 * 1.6) > fSpillLength && spill_ctr_LOS > 0)
            {
                SE_ctr = 1;
                MissedSpillEnd = true;
                SL_var = (double)samp / (1e8 * 1.6) - SL_var;
            }
            else if (MissedSpillEnd && SE_ctr == 0)
            {
                cout << "Missed spill end!" << endl;

                MissedSpillEnd = false;
            }

            Double_t dt = 0.;
            if (spill_ctr_LOS > 0)
            {
                dt = ((double)(samplerCurr - samplerPrev));
                if (dt < 0)
                    dt += 1073741823.;
                /**Only for the LaserRUN!!!!!!*/ // if(dt==11.){ ///Only for the Laser Run! There seems to be some
                                                 // Reflection-effect for the Laser, so that
                ///														Very repeatedly a Hit occurs after 110ns. Filter
                /// out these hits.

                // continue;
                //}
            }

            if (spill_ctr_LOS == 0) // first trigger in the spill
                dt_LOS_first = (double)samp;
            Double_t LOSDIFF = (double)samp - dt_LOS_first;
            if (LOSDIFF < 0.)
                LOSDIFF += 1073741823;
            fh_spill_times_LOS->Fill(LOSDIFF / (1.e8));
            fh_spill_times_LOS_Fine->Fill(LOSDIFF / (1.e8));
            fh_spill_times_LOSFIB_Fine->Fill(LOSDIFF / (1.e8));
            fh_spill_times_LOS_Coarse->Fill(LOSDIFF / (1.e8));

            // cout<<"samp: "<<samp<<"  "<<"samp first: "<<dt_LOS_first<<"  LOSDIFF: "<< LOSDIFF<<endl;

            // cout << "sampler times: " << samplerCurr << "  " << samplerPrev << "  " << dt << endl;
            if (spill_ctr_LOS > 0)
            {
                if (dt > 0.)
                {
                    fh_LOS_tDiff->Fill(dt / (/*1.6**/ 100000.));     /// samp in 10 ns => dt in 1ms w\ 10µs bin
                    fh_SAMPFIB_tDiff->Fill(dt / (/*1.6**/ 100000.)); /// samp in 10 ns => dt in 1ms w\ 10µs bin
                    fh_LOS_freq->Fill((/*1.6**/ 100.) / dt * 1.E3);  // kHz
                    dt_prev = dt;
                    if (dt < 1000. /*=10µs*/)
                    {
                        fh_SAMP_TrgW_dt->Fill(dt / 100.);
                        fh_SAMP_TrgW_dt_coarse->Fill(dt / 100.);
                    }
                }
            }
            spill_ctr_LOS++;

            samplerPrev = samplerCurr;
        }
    }

    if (header->GetTrigger() == 13)
    {
        time_spill_end = header->GetTimeStamp() / 1.6; // spill end  in nsec
        spill_off = true;
    }
    else if (spill_off_calc)
        spill_off = true;

    else
        spill_off = false;

    if (spill_off == true)
    {
        fh_spill_times_LOS_pois->Reset();
        fh_particle->Reset();
        fh_particle_pois->Reset();
        fh_DutyFactor_LOS->Reset();
        fh_DutyFactor_SAMPFIB->Reset();
        fh_DutyFactor_SAMPFIB_clean->Reset();
        fh_DutyFactor_SAMP_clean->Reset();
        fh_DutyFactor_LOS_pois->Reset();
        fh_DutyFactor_SAMPFIB_pois->Reset();
        fh_DutyFactor_MtA->Reset();
        fh_LOS_tDiff_pois->Reset();
        fh_spill_times_LOS_Fine_adj->Reset();
        fh_spill_times_LOSFIB_Fine_adj->Reset();
        time_spill_end = time; // spill end  in nsec
        cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " " << endl;

        deadTime = fDAQ_dead_time;

        // this doesn't quite work yet...
        /// if(dt_dead_mean > 0.) deadTime = dt_dead_mean / dead_ctr;
        /// cout<<"DEADTIME MEAN: "<<deadTime<<"µs DAQ deadTime: "<<fDAQ_dead_time<<"µs"<<endl;
        /// fh_deadTime_mean->Fill(deadTime);
        /// dt_dead_mean = 0.;
        /// dead_ctr = 0;
        // fSpillLength = (time_spill_end - time_spill_start)/1.e9;
        // cout<<"SpillLength: "<<fSpillLength<<endl;

        Int_t pps_LOS =
            fh_spill_times_LOS->Integral(1, fh_spill_times_LOS->GetSize() - 2); /// pps_fib = parts per spill?
        Int_t pps_LOSDIFF = fh_LOS_tDiff->Integral(1, fh_LOS_tDiff->GetSize() - 2);
        Int_t pps_daq_LOS = fh_spill_times_daq->Integral(1, fh_spill_times_daq->GetSize() - 2);
        Int_t pps_daq_first_LOS = fh_spill_times_daq_first->Integral(1, fh_spill_times_daq_first->GetSize() - 2);
        cout << "pps_LOS:  " << pps_LOS << " spill_ctr: " << spill_ctr << " spill_ctr_LOS: " << spill_ctr_LOS << endl;
        // cout<<"time_spill_start: "<<time_spill_start<<endl;

        /**Adjust pps_LOS & Histo(tDiff) with Fiber dt<=100ns */
        Int_t pps_fiber_adj = fh_FIB_TrgW_dt_coarse->Integral(0, 10) - fh_SAMP_TrgW_dt_coarse->Integral(0, 10);
        // Want to combine the two domains, need to remove double counting.
        Int_t pps_SAMPFIB = pps_LOS + pps_fiber_adj;
        /**------------------------------------------------------*/

        if (time_spill_start > 0 && pps_LOS > 0 && spill_ctr_LOS > 0)
        {

            // cout << "pps_fib: " << pps_fib << "  " << pps_daq << "  " << pps_daq_first << endl;

            Double_t norm_LOS = (double)pps_LOS * 100.E-6 / fSpillLength; // expected particle per channel

            /// DeadTime adjustment TEST:
            /**The choice of how to calculate the deadtime is not yet completed. Most accurate way appears to be
             * DeadTime by calibration->Deadtime by Maximum gets closest. */

            Double_t deadTime_calc = fSpillLength * 1e6 * (1. / (double)pps_daq_first_LOS - 1. / (double)pps_LOS);

            Double_t DeadTime_sum = fh_deadTime_sum->GetBinCenter(fh_deadTime_sum->GetMaximumBin());

            /// Double_t DeadTime_set = 97.9556;
            Double_t DeadTime_set =
                fDAQ_dead_time; // Set a deadtime, possibility if no calculation should be done -> not dynamic

            fh_deadTime_adj->Fill(deadTime);

            // deadTime = deadTime_calc;
            deadTime = DeadTime_sum; // Maximum DeadTime
            cout << "DEADTIME MAX: " << DeadTime_sum << endl;
            Double_t DeadAdj = fh_deadTime_sum->GetMean(); //µs
            cout << "DEADTIME MEAN: " << DeadAdj << endl;

            Double_t TotZeit_geeicht =
                TotZeitEichung(fSpillLength, (double)pps_LOS); // Calibrate DeadTime via function. Got parameters
                                                               // through calibration with known poisson dist.
            cout << "Geeichte Totzeit: " << TotZeit_geeicht << endl;

            deadTime = TotZeit_geeicht;

            // calculate the accepted trigger relative to Poisson (Michael Heil)
            Double_t a = pps_LOS * deadTime / (fSpillLength * 1e6);
            cout << "a_los: " << a << " pps_LOS: " << pps_LOS << endl;
            Double_t R_Poisson_LOS = (1 - exp(-a)) / a; // ratio of accepted trigger relativ to incoming particle

            Double_t accepted_LOS =
                ((double)pps_LOS / fSpillLength) /
                (1 + ((double)pps_LOS / fSpillLength) * deadTime * 1.e-6); // accepted rate! Calibrated DeadTime
            /**
            Double_t accepted_LOS_set = ((double)pps_LOS / fSpillLength) / (1 + ((double)pps_LOS / fSpillLength) *
            DeadTime_set * 1.e-6);//accepted rate! Fix DeadTime
            ///Double_t accepted_LOS_adj = ((double)pps_LOS / fSpillLength) / (1 + ((double)pps_LOS / fSpillLength) *
            DeadAdj * 1.e-6);//accepted rate! Mean DeadTime Double_t accepted_LOS_adj = ((double)pps_LOS / fSpillLength)
            / (1 + ((double)pps_LOS / fSpillLength) * TotZeit_geeicht * 1.e-6);//accepted rate! calibrated DeadTime
            */
            Double_t accepted_SAMPFIB = ((double)pps_SAMPFIB / fSpillLength) /
                                        (1 + ((double)pps_SAMPFIB / fSpillLength) * deadTime * 1.e-6); // accepted rate!
            /**
            Double_t accepted_SAMPFIB_set = ((double)pps_SAMPFIB / fSpillLength) / (1 + ((double)pps_SAMPFIB /
            fSpillLength) * DeadTime_set * 1.e-6);//accepted rate! Double_t accepted_SAMPFIB_adj = ((double)pps_SAMPFIB
            / fSpillLength) / (1 + ((double)pps_SAMPFIB / fSpillLength) * TotZeit_geeicht * 1.e-6);//accepted rate!
            */
            cout << "Ratio pois LOS: " << R_Poisson_LOS << "  "
                 << " Accepted LOS trigger: " << accepted_LOS / (double)pps_LOS * fSpillLength << "  "
                 << // accepted rate relative to full rate
                /*(double)pps_daq_first / (double)pps_fib << "  " <<
                (double)pps_daq / (double)pps_fib<<*/
                endl;

            fh_r_pois_LOS->Fill(R_Poisson_LOS);

            // now make histogram for random distribution
            for (Int_t i = 0; i < pps_LOS; i++)
            {
                Double_t num = (std::rand() / (float)RAND_MAX * fSpillLength);

                fh_spill_times_LOS_pois->Fill(num);
            }

            // cout<<"value PPSdf:  "<< (double)pps_daq_first_LOS<<" accepted: "<<accepted_LOS<<" t_spill:
            // "<<fSpillLength<<endl;
            /**Calibrated DeadTime */ fh_rate->SetBinContent(
                2 * spillCounter - 1,
                (double)pps_daq_first_LOS / accepted_LOS / fSpillLength * 100. -
                    100.); // Number of event-trigger / (accepted rate * t_spill)
            /**Calibrated DeadTime */ fh_rate_SAMPFIB->SetBinContent(
                2 * spillCounter - 1,
                (double)pps_daq_first_LOS / accepted_SAMPFIB / fSpillLength * 100. -
                    100.); // Number of event-trigger / (accepted rate * t_spill)

            /**Fixed DeadTime*/ // fh_rate_set->SetBinContent(2*spillCounter-1,(double)pps_daq_first_LOS/accepted_LOS_set/fSpillLength*100.
                                // - 100.); //Number of event-trigger / (accepted rate * t_spill)
            /**Fixed DeadTime*/ // fh_rate_set_SAMPFIB->SetBinContent(2*spillCounter-1,(double)pps_daq_first_LOS/accepted_SAMPFIB_set/fSpillLength*100.
                                // - 100.); //Number of event-trigger / (accepted rate * t_spill)
            /**Mean DeadTime*/ // fh_rate_adj->SetBinContent(2*spillCounter-1,(double)pps_daq_first_LOS/accepted_LOS_adj/fSpillLength*100.
                               // - 100.); //Number of event-trigger / (accepted rate * t_spill)
            /**Mean DeadTime*/ // fh_rate_adj_SAMPFIB->SetBinContent(2*spillCounter-1,(double)pps_daq_first_LOS/accepted_SAMPFIB_adj/fSpillLength*100.
                               // - 100.); //Number of event-trigger / (accepted rate * t_spill)
                               // => #event-trigger / #accepted hits = ratio
            /** With the given dead-time, accepted_LOS gives us the rate of hits that are (theoretically) expected to be
             * accepted. By dividing the number of daq_first hits (these are the hits, that trigger the readout window),
             * we get the number of events / expected accepted hits. For a periodic (and/or low intesity) spill, the
             * number of recorded events is equal to the number of real events.*/

            fh_rate_sum->Fill((double)pps_daq_first_LOS / accepted_LOS / fSpillLength * 100. - 100.);
            fh_rate_sum_SAMPFIB->Fill((double)pps_daq_first_LOS / accepted_SAMPFIB / fSpillLength * 100. - 100.);
            // fh_rate_sum_DTmean->Fill((double)pps_daq_first_LOS/accepted_LOS_adj/fSpillLength*100. - 100.);

            Int_t Size = fh_spill_times_LOS->GetSize() - 2;
            Int_t Size_pois = fh_spill_times_LOS_pois->GetSize() - 2;
            Int_t pps_LOS_pois = fh_spill_times_LOS_pois->Integral(1, fh_spill_times_LOS_pois->GetSize() - 2);
            // cout<<"Size: "<<Size<<" SizePois: "<<Size_pois<<" pps_LOS: "<<pps_LOS<<" pps_LOS_pois:
            // "<<pps_LOS_pois<<endl;
            Int_t real_count = 0;
            Int_t pois_count = 0;
            Int_t kL = fh_spill_times_LOS_pois->FindLastBinAbove(0, 1);
            for (Int_t i = 1; i < Size; i++)
            {
                Int_t counts = fh_spill_times_LOS->GetBinContent(i);
                Int_t counts_pois = fh_spill_times_LOS_pois->Integral(((i - 1) * 1000 + 1), i * 1000);

                if (fh_spill_times_LOS->GetBinCenter(i) < fSpillLength && counts > 0)
                {
                    fh_particle->Fill((double)counts - norm_LOS);
                    real_count++;
                }
                if (fh_spill_times_LOS_pois->GetBinCenter(i) < fSpillLength && counts_pois > 0)
                {
                    fh_particle_pois->Fill((double)counts_pois - norm_LOS);
                    pois_count++;
                }

                if (fh_spill_times_LOS_pois->GetBinCenter(i) > fSpillLength)
                {
                    cout << "last point: " << i << "Lastbin: " << kL << endl;
                    break;
                }
            }
            // cout<<"real count: "<<real_count<<" pois count: "<<pois_count<<endl;
        }

        /* This is an early version.*/
        /// Set SpillQuality via particle per time. Compare std_pois/std_meassured
        Double_t std_pois = fh_particle_pois->GetStdDev();
        Double_t std_meassured = fh_particle->GetStdDev();
        // cout<<"PPT Ana: std_pois: "<<std_pois<<" std_meassured: "<<std_meassured<<endl;
        Double_t Quality_ppt = /*100.*/ (std_pois * std_pois) / (std_meassured * std_meassured);
        /// is equal to 100% for pois spill, lower for worse spill, higher for better spill
        if (!IS_NAN(Quality_ppt))
        {
            fh_spillQuality_ppt->SetBinContent(2 * spillCounter - 1, 100. * sqrt(Quality_ppt));
            fh_spillQuality_ppt_stat->Fill(100. * sqrt(Quality_ppt));
        }

        /* Get Duty-Factor. Get the mean value over 10ms of 1000 10 µs bin of spill times (Rahul Singh)*/
        for (int k1 = 0; k1 < 2; k1++)
        { // Loop over "only sampler"(k1=0) & "sampler & fiber1" (k1=1)
            int Xmax = 0;
            if (k1 == 0)
                Xmax = fh_spill_times_LOS_Fine->FindLastBinAbove(0, 1);
            if (k1 == 1)
                Xmax = fh_spill_times_LOSFIB_Fine->FindLastBinAbove(0, 1);

            Int_t N_count = 0;
            Int_t N_MtA = 0;
            Int_t Np = 0;
            Int_t Nps = 0;
            Int_t NCTR = 0;
            Double_t FD = 0.;
            Double_t DutyAvg = 0.;
            Double_t DutyAvg_clean = 0.;
            Double_t PoisAvg = 0.;
            Int_t AvgCtr = 0;
            /*Get DutyFactor like RS */

            for (int i = 0; i <= Xmax; i++)
            {

                if (k1 == 0)
                    N_count = fh_spill_times_LOS_Fine->GetBinContent(i);
                if (k1 == 1)
                    N_count = fh_spill_times_LOSFIB_Fine->GetBinContent(i);
                if (N_MtA < N_count)
                    N_MtA = N_count;
                Np += N_count;
                Nps += N_count * N_count;
                NCTR++;
                if ((((i + 1) % 1000 == 0) || i == Xmax) && (Nps > 0))
                {
                    if (i < Xmax)
                    {
                        F_duty = (double)(Np * Np) / (double)Nps * 1. / 1000.; /* F_Duty = MEAN(Np)² / MEAN(Nps) */
                        FD = Np / 1000.;
                    }
                    else
                    {
                        F_duty = (double)(Np * Np) / (double)Nps * 1. / (double)((i) % 1000);
                        FD = Np / (double)((i) % 1000);
                    }

                    Np = 0;
                    Nps = 0;
                    Double_t Filler =
                        ((double)(i)) / 1000.; /// Not exactly sure why, but need to reduce i by 1, otherwise there
                                               /// occurs a problem in the binning and some bins arent getting filled,
                                               /// while the ones before are filled twice.

                    if (Filler < 0.)
                        Filler += fSpillLength;
                    if (F_duty > -1000. && F_duty < 1000.)
                    {
                        if (k1 == 0)
                            fh_DutyFactor_LOS->SetBinContent(Filler, F_duty);
                        if (k1 == 1)
                            fh_DutyFactor_SAMPFIB->SetBinContent(Filler, F_duty);
                    }
                    if (FD / (FD + 1.) > -1000. && FD / (FD + 1.) < 1000.)
                    {
                        if (k1 == 0)
                            fh_DutyFactor_LOS_pois->SetBinContent(
                                Filler,
                                FD / (FD +
                                      1.)); /// Get the Poisson-Limit for a given N (=FD). This is the highest possible
                                            /// value. If DutyFactor is lower, then there is a problem with a device.
                        if (k1 == 1)
                            fh_DutyFactor_SAMPFIB_pois->SetBinContent(
                                Filler,
                                FD / (FD +
                                      1.)); /// Get the Poisson-Limit for a given N (=FD). This is the highest possible
                                            /// value. If DutyFactor is lower, then there is a problem with a device.
                    }
                    if (k1 == 0)
                        fh_DutyFactor_SAMP_clean->SetBinContent(Filler,
                                                                (F_duty - FD / (FD + 1.)) / (FD / (FD + 1.)) * 100.);
                    if (k1 == 1)
                        fh_DutyFactor_SAMPFIB_clean->SetBinContent(Filler,
                                                                   (F_duty - FD / (FD + 1.)) / (FD / (FD + 1.)) * 100.);
                    if (!IS_NAN((F_duty - FD / (FD + 1.)) / (FD / (FD + 1.))))
                    {
                        DutyAvg_clean += (F_duty - FD / (FD + 1.)) / (FD / (FD + 1.));
                        DutyAvg += F_duty;
                        PoisAvg += FD / (FD + 1.);
                        AvgCtr++;
                    }
                    // cout<<"N_MtA: "<<N_MtA<<" pps: "<<pps_LOS<<endl;
                    if (k1 == 0)
                        fh_DutyFactor_MtA->SetBinContent(Filler, (double)N_MtA / (double)pps_LOS);
                    N_MtA = 0;
                }
            }
            if (k1 == 0)
            {
                DutyAvg = DutyAvg / (double)AvgCtr;
                PoisAvg = PoisAvg / (double)AvgCtr;
                Double_t DutyMax = fh_DutyFactor_LOS->GetBinContent(fh_DutyFactor_LOS->GetMaximumBin());
                Double_t DutyMax_clean = fh_DutyFactor_LOS->GetBinContent(fh_DutyFactor_LOS->GetMaximumBin());
                if (DutyMax / DutyAvg > -1000. && DutyMax / DutyAvg < 1000.)
                {
                    fh_DutyFactor_Avg->Fill(DutyAvg);
                    fh_DutyFactor_Max->Fill(DutyMax);
                    fh_DutyFactor_MaxToAvg->SetBinContent(2 * spillCounter - 1, DutyMax / DutyAvg);
                    if (DutyMax > -1000. && DutyMax < 1000.)
                        fh_DutyFactor_MaxRun->SetBinContent(2 * spillCounter - 1, 100. * DutyMax);
                    if (DutyAvg > -1000. && DutyAvg < 1000.)
                        fh_DutyFactor_AvgRun->SetBinContent(2 * spillCounter, 100. * DutyAvg);
                    if (DutyAvg < 1000. && DutyAvg > -1000. && PoisAvg < 1000. && PoisAvg > -1000.)
                        fh_DutyFactor_PLD->SetBinContent(2. * spillCounter - 1, DutyAvg - PoisAvg);
                    fh_MtA_sum->Fill(DutyMax / DutyAvg);
                }

                // cout<<"Max. DUTYFACTOR: "<<DutyMax<<" Avg Duty: "<<DutyAvg<<" PoisAvg: "<<PoisAvg<<endl;
            }
            if (k1 == 1)
            {
                DutyAvg = DutyAvg / (double)AvgCtr;
                DutyAvg_clean = DutyAvg_clean / (double)AvgCtr;
                PoisAvg = PoisAvg / (double)AvgCtr;
                Double_t DutyMax = fh_DutyFactor_SAMPFIB->GetBinContent(fh_DutyFactor_SAMPFIB->GetMaximumBin());
                Double_t DutyMax_clean =
                    fh_DutyFactor_SAMPFIB_clean->GetBinContent(fh_DutyFactor_SAMPFIB->GetMaximumBin());
                if (DutyMax / DutyAvg > -1000. && DutyMax / DutyAvg < 1000. && !IS_NAN(DutyAvg_clean))
                {
                    fh_DutyFactor_Avg_SAMPFIB->Fill(DutyAvg);
                    fh_DutyFactor_Max_SAMPFIB->Fill(DutyMax);
                    fh_DutyFactor_Avg_SAMPFIB_clean->Fill(100. * DutyAvg_clean);
                    fh_DutyFactor_Max_SAMPFIB_clean->Fill(100. * DutyMax_clean);
                }
            }
        }
#define DO_FFT
#ifdef DO_FFT
        for (int ft = 0; ft < 2; ft++)
        {
            /*Get FFT of fh_spill_times_LOS_Fine */
            if (spillCounter > 1)
                fh_FFT->Reset();
            if (ft == 0)
                fh_spill_times_LOS_Fine_adj->Reset();
            if (ft == 1)
                fh_spill_times_LOSFIB_Fine_adj->Reset();
            if (ft == 0)
                fh_FFT_adj->Reset();
            if (ft == 1)
                fh_FFT_adj_SAMPFIB->Reset();

            Double_t mean_of_spill = 0;
            Int_t Last_of_spill = 0;
            Int_t First_of_spill = 0;
            Int_t Int_mean = 0;
            if (ft == 0)
                Last_of_spill = fh_spill_times_LOS_Fine->FindLastBinAbove(0, 1);
            if (ft == 1)
                Last_of_spill = fh_spill_times_LOSFIB_Fine->FindLastBinAbove(0, 1);
            if (ft == 0)
                First_of_spill = fh_spill_times_LOS_Fine->FindFirstBinAbove(0, 1);
            if (ft == 1)
                First_of_spill = fh_spill_times_LOSFIB_Fine->FindFirstBinAbove(0, 1);
            if (ft == 0)
                Int_mean = fh_spill_times_LOS_Fine->Integral(First_of_spill, Last_of_spill);
            if (ft == 1)
                Int_mean = fh_spill_times_LOSFIB_Fine->Integral(First_of_spill, Last_of_spill);
            Int_t mean_count = 0;
            for (int k1 = First_of_spill; k1 < Last_of_spill + 1; k1++)
            {
                if (ft == 0)
                    mean_of_spill += fh_spill_times_LOS_Fine->GetBinContent(k1);
                if (ft == 1)
                    mean_of_spill += fh_spill_times_LOSFIB_Fine->GetBinContent(k1);
                mean_count++;
            }

            /// Substract the mean (here calculated via integral) from the spill to eliminate the lowest freq. (0th bin
            /// of FFT)
            for (int i = First_of_spill; i < Last_of_spill + 1; i++)
            {
                if (ft == 0)
                    fh_spill_times_LOS_Fine_adj->SetBinContent(
                        i, fh_spill_times_LOS_Fine->GetBinContent(i) - (double)mean_of_spill / (double)mean_count);
                if (ft == 1)
                    fh_spill_times_LOSFIB_Fine_adj->SetBinContent(
                        i, fh_spill_times_LOSFIB_Fine->GetBinContent(i) - (double)mean_of_spill / (double)mean_count);
            }

            TH1* fft = 0;
            // actual FFT
            TVirtualFFT::SetTransform(0);
            if (ft == 0)
                fh_FFT_adj = fh_spill_times_LOS_Fine_adj->FFT(fh_FFT_adj, "MAG");
            if (ft == 1)
                fh_FFT_adj_SAMPFIB = fh_spill_times_LOSFIB_Fine_adj->FFT(fh_FFT_adj, "MAG");
            /// Add FFTs
            if (ft == 0)
                fh_FFT_add->Add(fh_FFT_adj);
            if (ft == 1)
                fh_FFT_add_SAMPFIB->Add(fh_FFT_adj_SAMPFIB);

#endif
        }

        /*Get the TimeDifferences of the randomized poisson Spill */
        Double_t lambda = 1. / (fh_LOS_tDiff->GetMean());
        cout << "lambda: " << lambda << endl;
        Double_t nPPS = 0.;
        if (pps_LOS < 100000.)
            nPPS = pps_LOS * 1000.;
        if (pps_LOS >= 100000.)
            nPPS = pps_LOS * 10.;

        for (int i = 0; i < nPPS; i++)
        {
            /* Make a poisson distribution with same lambda value as timedifferences*/
            Double_t num = ran_expo(lambda);
            fh_LOS_tDiff_pois->Fill(num);
        }

/// Test how StdDev behaves, when we periodically remove entries in fh_LOS_tDiff_pois
//#define Test_pois_tDiff
#ifdef Test_pois_tDiff
        Int_t maxbin = fh_LOS_tDiff_pois->FindLastBinAbove(0, 1);
        cout << "Maximum bin: " << maxbin << endl;
        for (int i = 1; i < maxbin + 1; i++)
        {
            if (rand() % 3 == 0)
            {
                Int_t log = fh_LOS_tDiff_pois->GetBinContent(i);
                if (log > 0)
                    fh_LOS_tDiff_pois->SetBinContent(i, rand() % log);
            }
        }
#endif

        // Calculate amount of low timedifferences relative to poisson (Hans Törnqvist)
        for (int k2 = 0; k2 < 2; k2++)
        { // Loop over "only sampler"(k2=0) & "sampler & fiber1" (k2=1)
            // Now get the Integral of the real, and the poisson dt. Get the maximum of pois. Ratio of Area  until the
            // Max is quality Int_t max_pois = fh_LOS_tDiff_pois->GetMaximumBin(); Int_t max_real =
            // fh_LOS_tDiff->GetMaximumBin();
            Int_t max_pois = (int)(fh_LOS_tDiff_pois->GetMean() * 1.e6 / 5.);
            Int_t max_real = (int)(fh_LOS_tDiff->GetMean() * 1.e6 / 5.); // in ns
            cout << "max_real: " << max_real << " max_pois: " << max_pois << endl;
            Double_t int_pois = 0.;
            Double_t int_real = 0.;
            Double_t int_pois_byMax = 0.;
            Double_t int_real_byMax = 0.;
            if (k2 == 0)
                int_pois = fh_LOS_tDiff_pois->Integral(1, 200); // Get Integral of tDiffs equal and below 1µs
            if (k2 == 1)
                int_pois = fh_LOS_tDiff_pois->Integral(1, 200); // Get Integral of tDiffs equal and below 1µs
            if (k2 == 0)
                int_real = fh_LOS_tDiff->Integral(1, 200); // with 5ns bin from 0th to 200th bin
            if (k2 == 1)
                int_real = fh_SAMPFIB_tDiff->Integral(1, 200); // with 5ns bin from 0th to 200th bin
            if (k2 == 0)
                int_pois_byMax = fh_LOS_tDiff_pois->Integral(1, max_pois); // Get Integral of tDiffs equal and below 1µs
            if (k2 == 1)
                int_pois_byMax = fh_LOS_tDiff_pois->Integral(1, max_pois); // Get Integral of tDiffs equal and below 1µs
            if (k2 == 0)
                int_real_byMax = fh_LOS_tDiff->Integral(1, max_real); // with 5ns bin from 0th to 200th bin
            if (k2 == 1)
                int_real_byMax = fh_SAMPFIB_tDiff->Integral(1, max_real); // with 5ns bin from 0th to 200th bin
            Double_t int_real_LOS = fh_los_dt_hits[0]->Integral(1, 200);  // with 5ns bin from 0th to 200th bin
            /**One can either use 1µs tDiff as Integration limit or the mean -> both show the same course, but the
             * absolute values are a bit different. Both seem valid and independent of particle rate, because we compare
             * it to a poisson distribution with the same mean value => the same particle rate. */

            cout << "int_pois: " << int_pois << " int_real: " << int_real << endl;
            Double_t frac_int_real = 0;
            if (k2 == 0)
                frac_int_real = int_real / fh_LOS_tDiff->Integral(1, fh_LOS_tDiff->GetSize() - 2);
            if (k2 == 1)
                frac_int_real = int_real / fh_SAMPFIB_tDiff->Integral(1, fh_SAMPFIB_tDiff->GetSize() - 2);
            Double_t frac_int_real_LOS = int_real_LOS / fh_los_dt_hits[0]->Integral(1, 100000);
            Double_t frac_int_pois = int_pois / fh_LOS_tDiff_pois->Integral(1, fh_LOS_tDiff_pois->GetSize() - 2);
            Double_t frac_int_real_byMax = 0;
            if (k2 == 0)
                frac_int_real_byMax = int_real_byMax / fh_LOS_tDiff->Integral(1, fh_LOS_tDiff->GetSize() - 2);
            if (k2 == 1)
                frac_int_real_byMax = int_real_byMax / fh_SAMPFIB_tDiff->Integral(1, fh_SAMPFIB_tDiff->GetSize() - 2);
            Double_t frac_int_pois_byMax =
                int_pois_byMax / fh_LOS_tDiff_pois->Integral(1, fh_LOS_tDiff_pois->GetSize() - 2);

            // cout<<"frac_int_real: "<<frac_int_real<<endl;
            // cout<<"frac_int_pois: "<<frac_int_pois<<endl;
            // cout<<"frac_int_real_byMax: "<<frac_int_real_byMax<<endl;
            // cout<<"frac_int_pois_byMax: "<<frac_int_pois_byMax<<endl;

            /* Since the exponential distribution has many entries (high statistics) compare the fractions of dt's under
             * 1µs */
            Double_t Hans2 = (frac_int_pois - frac_int_real) / frac_int_pois; // >0 = good; <0 = bad
            Double_t Hans = (frac_int_pois_byMax - frac_int_real_byMax) / frac_int_pois_byMax;

            // Double_t Hans = (int_pois - int_real)/int_pois;// >0 = good; <0 = bad
            // Double_t Hans3 = (frac_int_pois - frac_int_real_LOS)/frac_int_pois;

            cout << "HT: " << Hans2 << endl;
            cout << "HT_byMax: " << Hans << endl;
            if (!IS_NAN(Hans) && k2 == 0)
            {
                Hans_mean += Hans;
                hans_ctr++;
            }
            if (!IS_NAN(Hans2))
            {
                if (k2 == 0)
                    fh_spill_hans->SetBinContent(2 * spillCounter - 1, 100. * Hans2);
                if (k2 == 1)
                    fh_spill_hans_SAMPFIB->SetBinContent(2 * spillCounter - 1, 100. * Hans2);
                if (k2 == 0)
                    fh_hans_sum->Fill(100. * Hans2);
                if (k2 == 1)
                    fh_hans_SAMPFIB_sum->Fill(100. * Hans2);
            }
            if (!IS_NAN(Hans))
            {
                if (k2 == 0)
                    fh_spill_hans_byMax->SetBinContent(2 * spillCounter - 1, 100. * Hans);
                if (k2 == 1)
                    fh_spill_hans_SAMPFIB_byMean->SetBinContent(2 * spillCounter - 1, 100. * Hans);
                if (k2 == 0)
                    fh_hans_sum_byMax->Fill(100. * Hans2);
                if (k2 == 1)
                    fh_hans_SAMPFIB_sum_byMean->Fill(100. * Hans2);
            }
        }
        /**Test if time diffence distribution is a exponential distribution (therefore particles are poisson
         * distributed) with F = 2 * µ²/(µ² + sigma²) of tDiffs (Enis Lorenz)*/
        /**If spill is poisson distr. then F = 100%, for better (sharper around mean) F>100%, for higher stddev (worse)
         * F<100% */
        for (int k3 = 0; k3 < 2; k3++)
        {
            Double_t mean_pois = 0.;
            Double_t mean_real = 0.;
            Double_t stdDev_pois = 0.;
            Double_t stdDev_real = 0.;

            if (k3 == 0)
                mean_pois = fh_LOS_tDiff_pois->GetMean();
            if (k3 == 1)
                mean_pois = fh_LOS_tDiff_pois->GetMean();
            if (k3 == 0)
                mean_real = fh_LOS_tDiff->GetMean();
            if (k3 == 1)
                mean_real = fh_SAMPFIB_tDiff->GetMean();
            if (k3 == 0)
                stdDev_pois = fh_LOS_tDiff_pois->GetStdDev();
            if (k3 == 1)
                stdDev_pois = fh_LOS_tDiff_pois->GetStdDev();
            if (k3 == 0)
                stdDev_real = fh_LOS_tDiff->GetStdDev();
            if (k3 == 1)
                stdDev_real = fh_SAMPFIB_tDiff->GetStdDev();

            Double_t FQ_pois =
                (mean_pois * mean_pois) / (mean_pois * mean_pois + stdDev_pois * stdDev_pois); // Qualityfactor

            Double_t FQ_real =
                1. / (1. + (stdDev_real * stdDev_real) /
                               (mean_real * mean_real)); // Qualityfactor of poisson distribution -> must be 100%

            if (!IS_NAN(FQ_real))
            {
                if (k3 == 0)
                    fh_spill_Q_E_real->SetBinContent(2 * spillCounter - 1, 200 * FQ_real - 100.);
                if (k3 == 1)
                    fh_spill_Q_E_real_SAMPFIB->SetBinContent(2 * spillCounter - 1, 200 * FQ_real - 100.);
                if (k3 == 0)
                    fh_EL_sum->Fill(200. * FQ_real - 100.);
                if (k3 == 1)
                    fh_EL_SAMPFIB_sum->Fill(200. * FQ_real - 100.);
            }
            if (!IS_NAN(FQ_pois))
            {
                if (k3 == 0)
                    fh_spill_Q_E_pois->SetBinContent(2 * spillCounter, 200 * FQ_pois - 100.);
                if (k3 == 1)
                    fh_spill_Q_E_pois_SAMPFIB->SetBinContent(2 * spillCounter, 200 * FQ_pois - 100.);
            }
            // When FQ is Filled subtract 100 to normalize it to other methods of analysis => 0 = poisson, lower than 0
            // = worse, higher than 0 = better
        }

        fh_spill_times_LOS_Fine->Reset();
        fh_spill_times_LOSFIB_Fine->Reset();
        fh_spill_times_LOS_Coarse->Reset();
        fh_spill_times_LOS->Reset();
        fh_spill_times_daq->Reset();
        fh_spill_times_daq_first->Reset();
        fh_deadTime_sum->Reset();
    } // end spill_off

    //----------------------------------------------------------------------
    // LOS detector
    //----------------------------------------------------------------------
    // Time nomenclature for LOS:
    // lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7
    //   t / \lt
    //   rt| |l
    //   r | |lb
    //   rb\ /b

    Double_t timeTofd = 0;
    Double_t time_V[fNofLosDetectors][32][8]; // [det][multihit][pm]
    Double_t time_L[fNofLosDetectors][32][8];
    Double_t time_T[fNofLosDetectors][32][8];

    Double_t time_MTDC[32][8] = { 0.0 / 0.0 };
    Double_t LosTresMTDC[32] = { 0.0 / 0.0 };

    Double_t timeLosV[fNofLosDetectors][32];
    Double_t LosTresV[fNofLosDetectors][32];
    Double_t timeLosT[fNofLosDetectors][32];
    Double_t LosTresT[fNofLosDetectors][32];
    Double_t timeLos[fNofLosDetectors][32];
    Double_t totsum[fNofLosDetectors][32];
    Double_t tot[fNofLosDetectors][32][8];
    Double_t xT_cm[fNofLosDetectors][32];
    Double_t yT_cm[fNofLosDetectors][32];
    Double_t xToT_cm[fNofLosDetectors][32];
    Double_t yToT_cm[fNofLosDetectors][32];
    Double_t xV_cm[fNofLosDetectors][32];
    Double_t yV_cm[fNofLosDetectors][32];
    Double_t time_V_temp[32][8] = { 0. / 0. };

    for (int i = 0; i < fNofLosDetectors; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            timeLosV[i][j] = 0.;
            LosTresV[i][j] = 0. / 0.;
            timeLosT[i][j] = 0.;
            LosTresT[i][j] = 0. / 0.;
            timeLos[i][j] = 0.;
            totsum[i][j] = 0.;
            xT_cm[i][j] = 0. / 0.;
            yT_cm[i][j] = 0. / 0.;
            xToT_cm[i][j] = 0. / 0.;
            yToT_cm[i][j] = 0. / 0.;
            xV_cm[i][j] = 0. / 0.;
            yV_cm[i][j] = 0. / 0.;
            for (int k = 0; k < 8; k++)
            {
                time_V[i][j][k] = 0. / 0.;
                time_L[i][j][k] = 0. / 0.;
                time_T[i][j][k] = 0. / 0.;
                tot[i][j][k] = 0. / 0.;
            }
        }
    }

    Int_t Multip;

    if (fMappedItems.at(DET_LOS))
    {
        auto det = fMappedItems.at(DET_LOS);
        Int_t nHits = det->GetEntriesFast();

        Multip = nHits;
        // cout<<"LOS: nHits"<<nHits<<endl;

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BLosMappedData* hit = (R3BLosMappedData*)det->At(ihit);
            if (!hit)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hit->GetDetector(); // 1..
            Int_t iCha = hit->GetChannel();  // 1..
            Int_t iTyp = hit->GetType();     // 0,1,2,3

            // cout<<"LOS mapped: "<<ihit<<"; "<<iDet<<"; "<<iCha<<", "<<iTyp<<endl;

            if (iTyp < 3)
                fh_los_channels[iDet - 1]->Fill(iCha); // exclude MTDC data
        }
    }

    Int_t nPart;
    Int_t nPartLos[fNofLosDetectors];
    for (int i = 0; i < fNofLosDetectors; i++)
    {
        nPartLos[i] = 0;
    }
    // Bool_t foptcond = true;

    if (fCalItems.at(DET_LOS))
    {
        Bool_t LOSID = false;

        auto det = fCalItems.at(DET_LOS);
        nPart = det->GetEntriesFast();

        Int_t iDet = 0;
        Double_t time_V_LOS1[32][8] = { 0. };
        Double_t time_V_LOS2[32][8] = { 0. };

        for (Int_t iPart = 0; iPart < nPart; iPart++)
        {
            /*
             * nPart is the number of particle passing through LOS detector in one event
             */
            R3BLosCalData* calData = (R3BLosCalData*)det->At(iPart);
            iDet = calData->GetDetector();
            Int_t nVftx = 0;
            Int_t nTamexL = 0;
            Int_t nTamexT = 0;

            for (Int_t iCha = 0; iCha < 8; iCha++)
            {
                if (iDet == 1)
                {
                    time_V_LOS1[nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V_LOS1[nPartLos[iDet - 1]][iCha] = calData->GetTimeV_ns(iCha);
                        nVftx++;
                    }
                    // cout<<"time_V_LOS1: "<<calData->GetTimeV_ns(iCha)<<endl;
                    time_L[iDet - 1][nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[iDet - 1][nPartLos[iDet - 1]][iCha] = calData->GetTimeL_ns(iCha);
                        nTamexL++;
                    }

                    time_T[iDet - 1][nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[iDet - 1][nPartLos[iDet - 1]][iCha] = calData->GetTimeT_ns(iCha);
                        nTamexT++;
                    }
                    // cout<<"time Tamex trailing LOS1: "<<calData->GetTimeT_ns(iCha)<<endl;

                    time_MTDC[nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeM_ns(iCha))))
                    { // MTDC
                        time_MTDC[nPartLos[iDet - 1]][iCha] = calData->GetTimeM_ns(iCha);
                    }
                }
                if (iDet == 2)
                {
                    time_V_LOS2[nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V_LOS2[nPartLos[iDet - 1]][iCha] = calData->GetTimeV_ns(iCha);
                        nVftx++;
                    }
                    // cout<<"time_V_LOS2: "<<calData->GetTimeV_ns(iCha)<<endl;
                    time_L[iDet - 1][nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[iDet - 1][nPartLos[iDet - 1]][iCha] = calData->GetTimeL_ns(iCha);
                        nTamexL++;
                    }

                    time_T[iDet - 1][nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[iDet - 1][nPartLos[iDet - 1]][iCha] = calData->GetTimeT_ns(iCha);
                        nTamexT++;
                    }

                    time_MTDC[nPartLos[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeM_ns(iCha))))
                    { // MTDC
                        time_MTDC[nPartLos[iDet - 1]][iCha] = calData->GetTimeM_ns(iCha);
                    }
                }
            }
            // cout<<"n: "<<nVftx<<"  "<<nTamexL<<"  "<<nTamexT<<endl;
            // if (nVftx == 8 && nTamexL == 8 && nTamexT == 8){
            if (nVftx == 8 && nTamexL == nTamexT)
            {
                nPartLos[iDet - 1]++;
                // cout<<"nPartLos["<<iDet-1<<"]: "<< nPartLos[iDet - 1]<<endl;
            }

            if (!calData)
            {
                cout << "LOS !calData" << endl;
                continue; // can this happen?
            }
        }

        // Sorting VFTX data:

        // detector 1
        if (nPartLos[0] > 0)
        {
            std::qsort(time_V_LOS1, nPartLos[0], sizeof(*time_V_LOS1), [](const void* arg1, const void* arg2) -> int {
                double const* lhs = static_cast<double const*>(arg1);
                double const* rhs = static_cast<double const*>(arg2);

                return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
            });
            for (Int_t iPart = 0; iPart < nPartLos[0]; iPart++)
            {
                for (int ipm = 0; ipm < 8; ipm++)
                {
                    time_V[0][iPart][ipm] = time_V_LOS1[iPart][ipm];
                    // cout<<"Test "<<ipm <<"  "<<iPart<<"  "<< time_V_LOS1[iPart][ipm]<<endl;
                }
            }
        }

        // detector 2
        if (nPartLos[1] > 0)
        {
            std::qsort(time_V_LOS2, nPartLos[1], sizeof(*time_V_LOS2), [](const void* arg1, const void* arg2) -> int {
                double const* lhs = static_cast<double const*>(arg1);
                double const* rhs = static_cast<double const*>(arg2);

                return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
            });
            for (Int_t iPart = 0; iPart < nPartLos[1]; iPart++)
            {
                for (int ipm = 0; ipm < 8; ipm++)
                {
                    time_V[1][iPart][ipm] = time_V_LOS2[iPart][ipm];
                    // cout<<"Test "<<ipm <<"  "<<iPart<<"  "<< time_V_LOS2[iPart][ipm]<<endl;
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
            for (Int_t iPart = 0; iPart < nPartLos[iDet - 1]; iPart++)
            {
                Bool_t iLOSTypeMCFD = false;
                Bool_t iLOSTypeTAMEX = false;
                Bool_t iLOSType = false;

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

                // We will consider only events in which booth MCFD and TAMEX see same number of channels:
                if (iLOSTypeTAMEX && iLOSTypeMCFD)
                    iLOSType = true;

                if (1 == 1)
                {

                    if (1 == 1) //(iLOSType)
                    {
                        int nPMT = 0;
                        int nPMV = 0;

                        for (int ipm = 0; ipm < 8; ipm++)
                        {

                            tot[iDet - 1][iPart][ipm] = 0. / 0.;

                            if (time_T[iDet - 1][iPart][ipm] > 0. && time_L[iDet - 1][iPart][ipm] > 0. &&
                                !(IS_NAN(time_T[iDet - 1][iPart][ipm])) && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                            {
                                while (time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm] <= 0.)
                                {
                                    time_T[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] + 2048. * fClockFreq;
                                }

                                nPMT = nPMT + 1;
                                tot[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm];
                            }

                            if (tot[iDet - 1][iPart][ipm] != 0. && !(IS_NAN(tot[iDet - 1][iPart][ipm])))
                                totsum[iDet - 1][iPart] += tot[iDet - 1][iPart][ipm];

                            if (time_L[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                                timeLosT[iDet - 1][iPart] += time_L[iDet - 1][iPart][ipm];

                            /*
                                                    if(iDet == 2){
                                                        // Push VFTX times of LOS2
                                                        if( timeLos[iDet-2][iPart] > 0. &&
                               !(IS_NAN(timeLos[iDet-2][iPart])) > 0. && time_V[iDet-1][iPart][ipm] > 0. &&
                               !(IS_NAN(time_V[iDet-1][iPart][ipm]))){

                                                            while(time_V[iDet-1][iPart][ipm]-timeLos[iDet-2][iPart] <
                               8192.*5.){ time_V[iDet-1][iPart][ipm] =   time_V[iDet-1][iPart][ipm] + 8192.*5.;
                                                            }
                                                        }
                                                    }
                            */

                            // Calculate detector time
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

                        // cout<<"LOS: "<<iDet<<", "<<iPart<<"; "<<timeLos[iDet-1][iPart]<<endl;
                        // if(iDet == 1) cout<<"Time - Los1: "<<iPart<<"  "<<time - (int)timeLos[iDet-1][iPart]<<endl;

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

                        //   LosTresMTDC[iPart] =
                        //   ((time_MTDC[iPart][0]+time_MTDC[iPart][2]+time_MTDC[iPart][4]+time_MTDC[iPart][6]) -
                        //      (time_MTDC[iPart][1]+time_MTDC[iPart][3]+time_MTDC[iPart][5]+time_MTDC[iPart][7]))/4.;

                        LosTresMTDC[iPart] = ((time_MTDC[iPart][1] + time_MTDC[iPart][3]) -
                                              (time_MTDC[iPart][5] + time_MTDC[iPart][7])) /
                                             4.;
                        // LosTresMTDC[iPart] = (time_MTDC[iPart][1] + time_MTDC[iPart][7])/2.;

                        // Position from tamex:
                        xT_cm[iDet - 1][iPart] = ((time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][6]) / 2. -
                                                  (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][2]) / 2.) *
                                                 (-1.);
                        yT_cm[iDet - 1][iPart] = ((time_L[iDet - 1][iPart][7] + time_L[iDet - 1][iPart][0]) / 2. -
                                                  (time_L[iDet - 1][iPart][3] + time_L[iDet - 1][iPart][4]) / 2.) *
                                                 (-1.);
                        xT_cm[iDet - 1][iPart] =
                            (xT_cm[iDet - 1][iPart] - flosOffsetXT[iDet - 1]) * flosVeffXT[iDet - 1];
                        yT_cm[iDet - 1][iPart] =
                            (yT_cm[iDet - 1][iPart] - flosOffsetYT[iDet - 1]) * flosVeffYT[iDet - 1];

                        // Position from VFTX:
                        xV_cm[iDet - 1][iPart] = ((time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][6]) / 2. -
                                                  (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][2]) / 2.) *
                                                 (-1.);
                        yV_cm[iDet - 1][iPart] = ((time_V[iDet - 1][iPart][7] + time_V[iDet - 1][iPart][0]) / 2. -
                                                  (time_V[iDet - 1][iPart][3] + time_V[iDet - 1][iPart][4]) / 2.) *
                                                 (-1.);
                        xV_cm[iDet - 1][iPart] =
                            (xV_cm[iDet - 1][iPart] - flosOffsetXV[iDet - 1]) * flosVeffXV[iDet - 1];
                        yV_cm[iDet - 1][iPart] =
                            (yV_cm[iDet - 1][iPart] - flosOffsetYV[iDet - 1]) * flosVeffYV[iDet - 1];

                        // Position from ToT:
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

                        xToT_cm[iDet - 1][iPart] =
                            (xToT_cm[iDet - 1][iPart] - flosOffsetXQ[iDet - 1]) * flosVeffXQ[iDet - 1];
                        yToT_cm[iDet - 1][iPart] =
                            (yToT_cm[iDet - 1][iPart] - flosOffsetYQ[iDet - 1]) * flosVeffYQ[iDet - 1];

                        // cout<<"iDet "<<iDet<<" iPart "<<iPart<<"  "<<nPMV<<endl;
                        if (nPMV == 8)
                        {

                            // time difference between two hits
                            if (time0[iDet - 1] < 0)
                            {
                                time0[iDet - 1] = timeLosV[iDet - 1][iPart];
                                Double_t tt = (time0[iDet - 1] + time - time_spill_start) / 1.e9;
                                if (iDet == 1)
                                    ;
                                fh_spill_times_daq_first->Fill(tt);
                            }
                            time1[iDet - 1] = timeLosV[iDet - 1][iPart];
                            time_abs[iDet - 1] =
                                time1[iDet - 1] - time0[iDet - 1] + (double)(time - time_prev[iDet - 1]);
                            // cout<<"idet: "<<iDet<<"  "<<time_abs[iDet-1]<<endl;
                            // cout<<"Time: "<<time<<" time_prev: "<<time_prev<<" dt:
                            // "<<(double)(time-time_prev[iDet-1])<<endl; cout<<"time0: "<<time0[iDet-1]<<" time1:
                            // "<<time1[iDet-1]<<" dt: "<<time1[iDet-1] - time0[iDet-1]<<endl;
                            if (time_abs[iDet - 1] > 0 && time_abs[iDet - 1] < 5.E8)
                            {
                                fh_los_dt_hits_ToT[iDet - 1]->Fill(time_abs[iDet - 1] / 1000., totsum[iDet - 1][iPart]);
                                // cout<<"draw "<<time_abs[iDet-1]/1000.<<"  " <<totsum[iDet-1][iPart]<<endl;
                                fh_los_dt_hits[iDet - 1]->Fill(time_abs[iDet - 1] / 1000.);
                                if (time_abs[iDet - 1] / 1000. < 10.)
                                {
                                    fh_LOS_TrgW_dt->Fill(time_abs[iDet - 1] / 1000.);
                                    fh_LOS_TrgW_dt_coarse->Fill(time_abs[iDet - 1] / 1000.);
                                }
                                ///-------------------------------------------------------------------------------------------
                                if (time_abs[iDet - 1] / 1000. > 15. /*µs*/)
                                {
                                    dt_dead_mean += time_abs[iDet - 1] / 1000.;
                                    dead_ctr++;
                                    fh_deadTime_sum->Fill(time_abs[iDet - 1] / 1000.);
                                    /**Define every Hit with dt higher than 15µs as new TriggerWindow -> Deadtime */
                                }
                            }
                            time_prev[iDet - 1] = time;
                            time0[iDet - 1] = time1[iDet - 1];

                            // Plot Events over one spill
                            Double_t tt =
                                (time1[iDet - 1] - time0[iDet - 1] + (double)(time - time_spill_start)) / 1.e9;
                            fh_spill_times_daq->Fill(tt);
                            // cout<<"tt: "<<tt<<" tt_prev: "<<tt_prev<<endl;

                            // MH
                            /*
                                        // time difference between two events in one read-out window
                                        time2[iDet-1]=0.;
                                        if (time0[iDet-1] < 0 ) {
                                            time0[iDet-1] = timeLosV[iDet-1][iPart];
                                            Double_t tt = (time0[iDet-1] + time - time_spill_start)/1.e9;
                                            fh_spill_times_daq_first->Fill(tt);
                                        }
                                        if (time1[iDet-1] < 0 ) time1[iDet-1] = timeLosV[iDet-1][iPart];

                                        time2[iDet-1] = timeLosV[iDet-1][iPart] ;


                                        double_t time_abs[iDet-1] = time2[iDet-1] - time1[iDet-1] +
                            (double)(time-time_prev); if(time_abs[iDet-1] > 0 && time_abs[iDet-1] < 5.E8) {
                                            //fh_los_dt_hits->Fill(time_abs[iDet-1] / 1000.);
                                            fh_tDiff_daq->Fill(time_abs[iDet-1] / 1000.);
                            //                fh_frequencies->Fill(1./time_abs*1.E6);
                                            //if(1./time_abs*1.E6<30.)
                                            //cout << "Time Test " << time1 << "  " << time2 << "  " << time2 - time1 <<
                            "  " << time << "  " << time_prev << "  " << time_abs<<"  "<<1./time_abs*1.E6<<endl;
                                        }

                                        time_prev[iDet-1] = time;
                                        time1[iDet-1]=time2[iDet-1];


                                        // Plot Events over one spill
                                        Double_t tt = (time2[iDet-1] - time0[iDet-1] + time - time_spill_start)/1.e9;
                                        fh_spill_times_daq->Fill(tt);
                            */
                            // end MH
                            // cout<<"foptcond "<<foptcond<<endl;
                            if (foptcond == 0)
                            {
                                fh_los_tot_mean[iDet - 1]->Fill(totsum[iDet - 1][iPart]);
                                for (int ipm = 0; ipm < 8; ipm++)
                                {
                                    fh_los_tot[iDet - 1]->Fill(ipm + 1, tot[iDet - 1][iPart][ipm]);
                                }

                                fh_los_tres_MCFD[iDet - 1]->Fill(LosTresV[iDet - 1][iPart]);
                                fh_los_tres_TAMEX[iDet - 1]->Fill(LosTresT[iDet - 1][iPart]);
                                if (iDet == 1)
                                    fh_los_tres_MTDC->Fill(LosTresMTDC[iPart]);

                                fh_los_pos_MCFD[iDet - 1]->Fill(xV_cm[iDet - 1][iPart], yV_cm[iDet - 1][iPart]);
                                fh_los_pos_TAMEX[iDet - 1]->Fill(xT_cm[iDet - 1][iPart], yT_cm[iDet - 1][iPart]);
                                fh_los_pos_ToT[iDet - 1]->Fill(xToT_cm[iDet - 1][iPart], yToT_cm[iDet - 1][iPart]);
                            }

                            fh_los_ihit_ToT[iDet - 1]->Fill(iPart + 1, totsum[iDet - 1][iPart]);
                            fh_los_multihit[iDet - 1]->Fill(iPart + 1);
                        }
                    }
                }
                else
                {
                    cout << "Wrong detector ID for LOS!" << endl;
                }
            }
        }
    }
    if (fCalItems.at(DET_LOS) && fNofLosDetectors == 2)
    {
        for (Int_t iPart = 0; iPart < nPart; iPart++)
        {
            //	 fh_los_ToT_vs_ToT->Fill(totsum[0][iPart],totsum[1][iPart]);
        }
    }

    //----------------------------------------------------------------------
    // Fiber detectors
    //----------------------------------------------------------------------
    Double_t dtime = 0.0 / 0.0;
    Bool_t fib1a_cut = false;
    Bool_t fib1b_cut = false;
    Int_t iLosHit = 0;

    Double_t ToTmax[2] = { 0. };
    Int_t iFib1Max[2] = { 0 };
    Double_t tMax[2] = { 0. };
    Int_t ihitMax[2] = { 0 };

    Double_t tLastOne = 0.0;

    int tDIFF_ctr = 0; // Count how many tDiff's have been calculated

    double* tHitFib1a = NULL;
    double* tHitFib1b = NULL;
    /* Two arrays for Hit Coincidence calculation.*/

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        // if(ifibcount==0) cout<<"1A"<<endl;
        // if(ifibcount==1) cout<<"1B"<<endl;

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
            pHits += nHits;

            Double_t xpos = 0. / 0.;
            Double_t ypos = 0. / 0.;
            Double_t totMax = 0.;
            Double_t totMax_MA = 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
            Double_t randx;
            Int_t iFibmax = -100;
            Int_t iFibMax_MA = -100;
            Double_t totMax_S = 0.;
            Double_t yposMax = -10000;
            Double_t tof_fibMax = -10000;

            Double_t timehit = 0.;

            Double_t MAPMT_t_check = -1.e9; // Set an impossible value.
            hit_counter += nHits;

            bool is_first = true; // Check to see, if it's the 1st particle in spill/window

            time_global = header->GetTimeStamp() / 1.6; ////EVENT-TIME? ns

            /**Vulom runs on 100MHz clock, which means that the timestamp increments every 10 ns!
             * The timestamp is shifted to the left by 4bits, => it's multiplied by 16
             * this means every 10ns it grows by 16, therefore we need to divide by 1.6!
             * At somepoint this will be adjusted... */

            iFib_tmp = -1;

            Double_t tPrev = 0.;
            Int_t FibPrev = -1;

            double* tHit = NULL;    // Array to collect all Hit-Times
            int* tHitFibID = NULL;  // Array to collect all Hit-Fiber-ID's
            double* tHitToT = NULL; // Array to collect all Hit-ToT's

            tHit = new double[nHits];    // Anfordern eines Datenfeld aus double-Werten
            tHitFibID = new int[nHits];  // Anfordern eines Datenfeld aus double-Werten
            tHitToT = new double[nHits]; // Anfordern eines Datenfeld aus double-Werten
            Int_t tHit_ctr = 0;
            /// Set Array values to -1.e9
            for (int i = 0; i < nHits; i++)
            {
                tHit[i] = -1.e9;
                tHitFibID[i] = -1.e9;
                tHitToT[i] = -1.e9;
            }

            for (int ihit = nHits - 1; ihit > -1; ihit--)
            { /// Try going backwards through nHits -> Time order should be conserved this way

                if (ihit < 0)
                    break; /// Failsafe, if nHits == 0, or other bug

                int Fiber_counter = -1; // count Fiber-ID

                Double_t tMAPMT = 0. / 0.;
                Double_t tSPMT = 0. / 0.;

                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)detHit->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 0=trig, 1...=fiber
                if (0 == iFib)
                    continue;

                // times
                tMAPMT = hit->GetMAPMTTime_ns();
                tSPMT = hit->GetSPMTTime_ns();

                if (tMAPMT == MAPMT_t_check)
                {
                    continue;
                    // If hit is doubly counted->skip
                }
                else
                    MAPMT_t_check = tMAPMT;
                /**Check for double counting of tMAPMT, while comparing tSPMT */

                dtime = tMAPMT - tSPMT;

#define USE_MAPMT
#ifdef USE_MAPMT
                timehit = tMAPMT; // Get time from MAPMT
#else
            timehit = tSPMT; // Get time from SPMT
#endif

                if (ifibcount < 2)
                { // only look at Fiber1a & Fiber1b
                    tHit[ihit] = hit->GetTime();
                    tHitFibID[ihit] = hit->GetFiberId();
                    tHitToT[ihit] = hit->GetMAPMTToT_ns();
                    // Collect HitTime, FiberID & ToT
                    // cout<<"ihit: "<<ihit<<" iFib: "<<iFib<<" tMA: "<<tMAPMT<<" tS: "<<tSPMT<<" tHit:
                    // "<<hit->GetTime()<<endl;//for Debugging
                }

                /// Loop over LOS-Hits to make Coincidence between LOS & Fiber1, not directly needed for micro spill
                /// structure
                // loop over all hits of LOS1:
                for (Int_t lhit = 0; lhit < nPartLos[0]; lhit++)
                { // cout<<"Loop check"<<endl;
                    if (IS_NAN(timeLos[1][lhit]))
                    {
                        // cout<<"timeLos[1][lhit] is NAN"<<endl;
                        continue;
                    }

                    // if not multiplicity 8 or pile-up, go to next event
                    Bool_t pileup = false;
                    Int_t nPMT = 0;
                    for (int ipm = 0; ipm < 8; ipm++)
                    {
                        if (tot[1][iLosHit][ipm] > 0. && !(IS_NAN(tot[0][iLosHit][ipm])))
                            nPMT++;
                        if (tot[1][iLosHit][ipm] > 100.)
                        {
                            pileup = true;
                            // cout<<"pileup"<<endl;
                        }
                    }
                    if (pileup || nPMT != 8)
                        ;
                    // continue;

                    // "Push" the Fib times in the same cycle with LOS:
                    // Los coarse counter has 8192 channels with 5 ns each
                    // SPMT coarse counter has 2048 channels with 5 ns each
                    if (timeLos[0][lhit] > 0. && timeLos[0][lhit] < 8192. * 5. && !(IS_NAN(timeLos[0][lhit])))
                    {
                        while (tSPMT - timeLos[0][lhit] < 2048. * 5. / 2.)
                        {
                            tSPMT = tSPMT + 2048. * 5.;
                        }
                        while (tSPMT - timeLos[0][lhit] > 2048. * 5. / 2.)
                        {
                            tSPMT = tSPMT - 2048. * 5.;
                        }
                    }

                    // "Push" two times in the same clock cycle:
                    // MAPMT coarse counter has 4096 channels with 1000/150=6.67 ns each
                    // SPMT coarse counter has 2048 channels with 5 ns each
                    dtime = tMAPMT - tSPMT;
                    while (dtime > 1024. / 2. * 1000. / 150.)
                    {
                        tMAPMT = tMAPMT - 1024. * 1000. / 150.;
                        dtime = tMAPMT - tSPMT;
                    }
                    while (dtime < -1024. / 2. * 1000. / 150.)
                    {
                        tMAPMT = tMAPMT + 1024. * 1000. / 150.;
                        dtime = tMAPMT - tSPMT;
                    }

                    // Not-calibrated ToF:
                    tfib = (tMAPMT + tSPMT) / 2.;
                    // tfib = tSPMT;
                    // tfib = tMAPMT;
                    if (tfib > 0. && !(IS_NAN(tfib)) && timeLos[0][lhit] > 0. && !(IS_NAN(timeLos[0][lhit])))
                        tof_fib = tfib - timeLos[0][lhit];

                    //					cout<<"Test "<<tof_fib<<"  "<<tfib<<"  "<<hit->GetMAPMTToT_ns()<<"
                    //"<<hit->GetSPMTToT_ns()<< 						  "  "<<lhit<<"  "<<timeLos[1][lhit]<< endl;

                    // if ToF between LOS1 and fiber is not okay, go to next
                    if (tof_fib < tof_fiber[ifibcount] - 10. || tof_fib > tof_fiber[ifibcount] + 10.)
                    {
                        // cout<<"TOF between LOS1 and fiber is not okay"<<endl;
                        // continue;
                    }

                    // Not-calibrated ToF:
                    // tfib = (tMAPMT + tSPMT) / 2.;
                    tfib = tSPMT;
                    // tfib = tMAPMT;

                    if (tfib > 0. && !(IS_NAN(tfib)) && timeLos[0][lhit] > 0. && !(IS_NAN(timeLos[0][lhit])))
                        tof_fib = tfib - timeLos[0][lhit];

                    // Not-calibrated position:
                    randx = (std::rand() / (float)RAND_MAX);
                    //                if(iFib > 0) xpos = ((-n_fiber[ifibcount]/2.+iFib+(0.5-randx)))*0.21;
                    xpos = iFib;
                    ypos = dtime;

                    // if not resonable y-position, go to next
                    if (ypos < 70. || ypos > 100.)
                    {
                        //	cout<<"Y not reasonable"<<endl;
                        continue;
                    }

                    // find fiber with most light which has resonable ToF and y-position
                    Double_t ToT_MA = hit->GetMAPMTToT_ns();
                    // cout<<"ToT_MA: "<<ToT_MA<<" totMax_MA: "<<totMax_MA<<endl;
                    if (ToT_MA > totMax_MA)
                    { // cout<<"Here I am"<<endl;
                        totMax_MA = ToT_MA;
                        iFibMax_MA = iFib;
                        totMax_S = hit->GetSPMTToT_ns();
                        yposMax = ypos;
                        tof_fibMax = tof_fib;
                        iLosHit = lhit;
                        // cout<<"new imax "<<iFibMax_MA<<" max "<<totMax_MA<<endl;
                    }

                } // end for (lhit)
                  /// End Loop over LOS & Fiber1

                fh_fibers_Fib[ifibcount]->Fill(iFibMax_MA);
                fh_ToT_s_Fib[ifibcount]->Fill(iFibMax_MA, totMax_S);
                fh_ToT_m_Fib[ifibcount]->Fill(iFibMax_MA, totMax_MA);
                fh_time_Fib[ifibcount]->Fill(iFibMax_MA, yposMax);
                fh_Fib_ToF[ifibcount]->Fill(iFibMax_MA, tof_fibMax);
                fh_Fib_pos_xy[ifibcount]->Fill(xpos, ypos);
                fh_Fib_vs_Events[ifibcount]->Fill(fNEvents, iFibMax_MA);
                fh_ToT_single_Fib[ifibcount]->Fill(1, totMax_S);

                // cout<<"iFibMax: "<<iFibMax_MA<<endl;
                if (ifibcount == 0 && iFibMax_MA > 120 && iFibMax_MA < 136)
                {
                    // cout<<"Cut-case"<<endl;
                    // exit(0);
                    fib1a_cut = true;
                }
                if (ifibcount == 1 && iFibMax_MA > 120 && iFibMax_MA < 136)
                {
                    fib1b_cut = true;
                    // cout<<"Cut true "<< iFibMax_MA << endl;
                }
                if (nHits > 0)
                    fh_mult_Fib[ifibcount]->Fill(nHits);
                // cout<<"Hits "<<nHits<<endl;

            } // end for(ihit), End collecting HitTimes

            /*Now sort these entries
             * Hits are read in by Fibernumbers, so we need to sort these*/
            Int_t nHits_corrected = 0;
            if (sort_array(&nHits, tHit, tHitFibID, tHitToT))
            { // Sorting function in header -> if(tHit[i] == -1.e9) -> need to sort these out. Let following loop start
              // at nHits_corrected
                for (int i = 0; i < nHits; i++)
                {
                    if (tHit[i] == -1.e9)
                        nHits_corrected++;
                    else
                        break;
                }
            }

            /* For Debugging, check if sorted correctly*/
            // cout<<"after sorting: "<<endl;
            // for(int i=0; i<nHits; i++){
            //	cout<<"tHit["<<i<<"]: "<<tHit[i]<<endl;
            //}
            // cout<<"___________________________"<<endl;

            // Begin tDiff-Calculation
            // Make HitTimeArrays for Fiber1a & Fiber1b
            if (ifibcount == 0)
                tHitFib1a = new double[nHits];
            if (ifibcount == 1)
                tHitFib1b = new double[nHits];
            for (int i = 0; i < nHits; i++)
            {
                if (ifibcount == 0)
                    tHitFib1a[i] = 0. / 0.;
                if (ifibcount == 1)
                    tHitFib1b[i] = 0. / 0.;
            }

            Int_t numHits = 0;
            Int_t point = 0;

            /**Now check hits for crosstalk, merge if necessary and fill them into tHitFib1a/b to be able to make
             * coincidence
             * --Checking Hits within 5ns interval, since MAPMT channels have delays upto 5ns. Hence it's possible for
             * CT within 5ns */
            for (int k = nHits_corrected; k < nHits; k++)
            {
                Int_t merge_counter = 0;
                if (tHit[k] > -9000. && tHit[k] < 9000.)
                {
                    if (tHit[k + 1] > -9000. && tHit[k + 1] < 9000.)
                    {
                        for (int i = k; i < nHits - 1; i++)
                        {
                            if (tHit[i + 1] - tHit[k] > 5.)
                            {
                                break;
                            }
                            if (tHit[i + 1] - tHit[k] < 5.)
                            {
                                bool no_ACT = true;
                                bool no_FCT = true;

                                if (check_anode_CT(tHitFibID[i + 1], tHitFibID[k]))
                                    no_ACT = false;
                                if (check_fiber_CT(tHitFibID[i + 1], tHitFibID[k]))
                                    no_FCT = false;
                                // if(!no_ACT) cout<<"Found Anode CT"<<endl;//For Debugging
                                // if(!no_FCT) cout<<"Found Fiber CT"<<endl;//For Debugging

                                if (no_ACT && no_FCT)
                                    break;
                            }
                            // std::cout<<"arr["<<k<<"]: "<<tHit[k]<<" arr["<<i+1<<"]: "<<tHit[i+1]<<std::endl;//For
                            // Debugging
                            merge_counter++;
                        }
                        if (merge_counter == 0)
                        {
                            if (ifibcount == 0)
                                tHitFib1a[point] = tHit[k];
                            if (ifibcount == 1)
                                tHitFib1b[point] = tHit[k];
                            numHits++;
                        }
                        else
                        {
                            // cout<<"Merging CT Hits"<<endl;
                            if (ifibcount == 0)
                                tHitFib1a[point] = 0;
                            if (ifibcount == 1)
                                tHitFib1b[point] = 0;
                            //	cout<<"Merge Counter: "<<merge_counter<<endl;
                            for (int j = 0; j < merge_counter + 1; j++)
                            {
                                if (ifibcount == 0)
                                    tHitFib1a[point] += tHit[k + j];
                                if (ifibcount == 1)
                                    tHitFib1b[point] += tHit[k + j];
                            }
                            if (ifibcount == 0)
                            {
                                //		cout<<"Merged Hit to tHitFib["<<point<<"]:
                                //"<<tHitFib1a[point]/(double)(merge_counter+1)<<endl;//For Debugging
                                tHitFib1a[point] = tHitFib1a[point] / (double)(merge_counter + 1);
                            }
                            if (ifibcount == 1)
                            {
                                //	cout<<"Merged Hit to tHitFib["<<point<<"]:
                                //"<<tHitFib1b[point]/(double)(merge_counter+1)<<endl;//For Debugging
                                tHitFib1b[point] = tHitFib1b[point] / (double)(merge_counter + 1);
                            }
                            //	cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;//For
                            // Debugging
                            numHits++;
                        }
                        point++;
                        k += merge_counter;
                    }
                }
            }

            // Int_t controlcounter=0;
            // Int_t adj_ctr = -1;
            // Bool_t adj_times = false;
            // Bool_t adjusted = false;
            ////Int_t numHits = 0;
            // Double_t tHitDiffcheck = 1.e9;

            // cout<<"___________________________"<<endl;

            // cout<<"-------------------------------"<<endl;
            if (ifibcount == 0)
                numHits1a = numHits;
            if (ifibcount == 1)
                numHits1b = numHits;
            /** Delete tHit so no memory leak on the next Window*/
            delete[] tHitFibID; // Das Freigeben des Speichers funktioniert wie für einzelne Werte
            tHitFibID = NULL;   // Pointer definiert auf Null zeigen lassen
            delete[] tHit;      // Das Freigeben des Speichers funktioniert wie für einzelne Werte
            tHit = NULL;        // Pointer definiert auf Null zeigen lassen
            delete[] tHitToT;   // Das Freigeben des Speichers funktioniert wie für einzelne Werte
            tHitToT = NULL;     // Pointer definiert auf Null zeigen lassen

        } // end if(aHit[ifibcount])

    } // end for(ifibcount)

    /* Since it's possible for Fib1a & Fib1b a different amount of hits, we need to adjust the length of the arrays to
     * compare them. Take the length of the longer array.*/
    Int_t numctr = 0;
    if (numHits1a < numHits1b)
        numctr = numHits1a;
    else
        numctr = numHits1b;
    Int_t numctr2 = 0;
    if (numHits1a > numHits1b)
        numctr2 = numHits1a;
    else
        numctr2 = numHits1b;

    Double_t* tHitCoince;
    /** Re Alloc arrays*/
    if (numHits1a < numHits1b)
    {
        Double_t* tmp;
        tmp = new double[numHits1a];
        for (int t = 0; t < numHits1a; t++)
        {
            tmp[t] = tHitFib1a[t];
        }
        delete[] tHitFib1a;
        tHitFib1a = NULL;
        tHitFib1a = new double[numHits1b];
        tHitCoince = new double[numHits1b];
        for (int t = 0; t < numHits1b; t++)
        {
            if (t < numHits1a)
                tHitFib1a[t] = tmp[t];
            else
                tHitFib1a[t] = 0. / 0.;
            tHitCoince[t] = 0. / 0.;
        }
        delete[] tmp;
        tmp = NULL;
    }
    else
    {
        Double_t* tmp;
        tmp = new double[numHits1b];
        for (int t = 0; t < numHits1b; t++)
        {
            tmp[t] = tHitFib1b[t];
        }
        delete[] tHitFib1b;
        tHitFib1b = NULL;
        tHitFib1b = new double[numHits1a];
        tHitCoince = new double[numHits1a];
        for (int t = 0; t < numHits1a; t++)
        {
            if (t < numHits1b)
                tHitFib1b[t] = tmp[t];
            else
                tHitFib1b[t] = 0. / 0.;
            tHitCoince[t] = 0. / 0.;
        }
        delete[] tmp;
        tmp = NULL;
    }

    /** Create Hits, by coincidence through Fib1a & Fib1b*/
    // For Debugging
    // if(!(numctr == 0 && numctr2 == 0))
    // cout<<"numctr: "<<numctr<<" numctr2: "<<numctr2<<endl;
    // for(int h = 0; h<numctr2; h++){
    //	cout<<"FibHit1a["<<h<<"]: "<<tHitFib1a[h]<<"   FibHit1b["<<h<<"]: "<<tHitFib1b[h]<<endl;
    //}

#define Calc_tDiff
#ifdef Calc_tDiff

//#define DO_COINCE
#ifdef DO_COINCE
    /** Do a Coincidence between Fib1a & Fib1b. If Both saw a hit within 2ns -> Fill the Hit into tHitCoince*/
    Int_t r_check = -1;
    for (int p = 0; p < numctr2; p++)
    {
        if (tHitFib1a[p] > 0.)
        {
            for (int r = r_check + 1; r < numctr2; r++)
            {
                if (tHitFib1b[r] > 0.)
                {
                    if ((tHitFib1a[p] - tHitFib1b[r] < 2.) || (tHitFib1b[r] - tHitFib1a[p] < 2.))
                    {
                        if (r == r_check)
                        {
                            r_check = r;
                            continue;
                        }
                        tHitCoince[p] = (tHitFib1a[p] + tHitFib1b[r]) / 2.;
                        r_check = r;
                    }
                }
            }
        }
    }
#endif

#ifndef DO_COINCE
    /**It is possible that Fib1a or Fib1b don't work properly. Fill tHitCoince with entries tHitFib1a or tHitFib1b*/
    bool use_fib1a = false;
    bool use_fib1b = true;
    for (int i = 0; i < numctr2; i++)
    {
        if (use_fib1a)
        {
            if (!IS_NAN(tHitFib1a[i]))
                tHitCoince[i] = tHitFib1a[i];
        }
        if (use_fib1b)
        {
            if (!IS_NAN(tHitFib1b[i]))
                tHitCoince[i] = tHitFib1b[i];
        }
    }

#endif

    Int_t FibHitCtr = 0;

    for (int h = 0; h < numctr2; h++)
    {
        // cout<<"tHitCoince["<<h<<"]: "<<tHitCoince[h]<<endl;//For Debugging
        if (!IS_NAN(tHitCoince[h]))
            FibHitCtr++;
    }
    /**Now make tDiff-Calculation */
    Bool_t is_first = true;
    Double_t time_previous = 0.;
    Double_t timedifference = 0.;
    for (int i = 0; i < FibHitCtr; i++)
    {
        if (IS_NAN(tHitCoince[i]))
        {
            FibHitCtr++;
            continue;
        }
        fh_FIB_count_t->Fill(tHitCoince[i] / 1000.); //µs
        if (is_first)
        {
            if (spillon_fib)
            {                        /// First in Spill?
                FIB_time_spill = 0.; /// Time of first Hit in Spill->Set to 0
                spill_ctr_FIB++;
                spillon_fib = false;
            }
            else
            {
                Double_t time_lastInPrevWindow_minus_firstInNewWindow =
                    tLastInWindow - tHitCoince[i]; /// Calculate tDiff between two TriggerWindows

                timedifference = time_global - (LastWindowTime + time_lastInPrevWindow_minus_firstInNewWindow);
            }
            is_first = false;
        }
        else
        {
            timedifference = tHitCoince[i] - time_previous;
        }
        time_previous = tHitCoince[i];

        fh_spill_hits_vs_time->Fill(timedifference); // fill timedifferences of hits in ns
        tLastInWindow = time_previous;
        LastWindowTime = time_global;
        if (spill_ctr_LOS > 0.)
        {
            FIB_time_spill += timedifference; // in ns,
            if (timedifference < 10000. /*=10µs*/)
            {
                fh_FIB_TrgW_dt->Fill(timedifference / 1000.);        //µs
                fh_FIB_TrgW_dt_coarse->Fill(timedifference / 1000.); //µs
            }
            fh_FIB_Freq->Fill(1.e6 / timedifference); // kHz
            fh_spill_times_fib->Fill(FIB_time_spill / 1.e9);
            if (timedifference < 100.)
            {
                fh_spill_times_LOSFIB_Fine->Fill(FIB_time_spill /
                                                 1.e9); // Fill hits, that are registered by Fiber, but not by Sampler
                fh_SAMPFIB_tDiff->Fill(timedifference / 1000000.); // ms
            }
        }
    } //
#endif

    /// Delete Arrays so no memory leak on next window
    delete[] tHitFib1a;
    delete[] tHitFib1b;
    delete[] tHitCoince;
    tHitFib1a = NULL;
    tHitFib1b = NULL;
    tHitCoince = NULL;
    /**At this point tDiffs and HitTimes over spill are calculated. For Spillquality analysis go to if(spill_off){...},
     * like for LOS/Sampler*/

    // cout<<"fibcut "<<fib1a_cut<<endl;

    if (foptcond == 1 && (fib1a_cut == true)) // && fib1b_cut == true))
    {                                         // exit(0);
        for (Int_t ilos = 0; ilos < fNofLosDetectors; ilos++)
        {
            if (iLosHit < nPartLos[ilos])
            {
                fh_los_pos_MCFD[ilos]->Fill(xV_cm[ilos][iLosHit], yV_cm[ilos][iLosHit]);
                fh_los_pos_TAMEX[ilos]->Fill(xT_cm[ilos][iLosHit], yT_cm[ilos][iLosHit]);
                fh_los_pos_ToT[ilos]->Fill(xToT_cm[ilos][iLosHit], yToT_cm[ilos][iLosHit]);
                fh_los_tres_MCFD[ilos]->Fill(LosTresV[ilos][iLosHit]);
                fh_los_tres_TAMEX[ilos]->Fill(LosTresT[ilos][iLosHit]);
                // fh_los_tres_MTDC->Fill(LosTresMTDC[iLosHit]);
                fh_los_tot_mean[ilos]->Fill(totsum[ilos][iLosHit]);
                for (int ipm = 0; ipm < 8; ipm++)
                {
                    fh_los_tot[ilos]->Fill(ipm + 1, tot[ilos][iLosHit][ipm]);
                }
                fh_los_ToT_vs_ToT->Fill(totsum[0][iLosHit], totsum[1][iLosHit]);
            }
        }
    }

    fNEvents += 1;
    cout << "fNEvents end: " << fNEvents << endl;
}

void R3BOnlineSpillAnalysis::FinishEvent()
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
    // cout<<"In 'FinishEvent'"<<endl;
}

void R3BOnlineSpillAnalysis::FinishTask()
{

    cout << "HT mean: " << fh_hans_sum->GetMean() << " std: " << fh_hans_sum->GetStdDev() << endl;
    cout << "HT_bymax mean: " << Hans_mean / (double)hans_ctr << endl;
    cout << "EL Mean: " << fh_EL_sum->GetMean() << " std: " << fh_EL_sum->GetStdDev() << endl;
    cout << "MH Mean: " << fh_rate_sum->GetMean() << " std: " << fh_rate_sum->GetStdDev() << endl;
    cout << "MH DTMEAN Mean: " << fh_rate_sum_DTmean->GetMean() << " std: " << fh_rate_sum_DTmean->GetStdDev() << endl;
    cout << "RS Mean: " << fh_DutyFactor_Avg->GetMean() << " std: " << fh_DutyFactor_Avg->GetStdDev()
         << " RS peak: " << fh_DutyFactor_Max->GetMean() << " std: " << fh_DutyFactor_Max->GetStdDev() << endl;
    cout << "RS Mean normalized: " << fh_DutyFactor_Avg_clean->GetMean()
         << " std: " << fh_DutyFactor_Avg_clean->GetStdDev()
         << " RS peak normalized: " << fh_DutyFactor_Max_clean->GetMean()
         << " std: " << fh_DutyFactor_Max_clean->GetStdDev() << endl;
    cout << "PPT Mean: " << fh_spillQuality_ppt_stat->GetMean() << " std: " << fh_spillQuality_ppt_stat->GetStdDev()
         << endl;
    cout << "DeadTimeAdj: " << fh_deadTime_adj->GetMean() << "µs with " << fh_deadTime_adj->GetStdDev() << "µs stdd"
         << endl;
    cout << "______________________________________________________________________________________________" << endl;
    cout << "With Fiber:" << endl;
    cout << "HT mean: " << fh_hans_SAMPFIB_sum->GetMean() << " std: " << fh_hans_SAMPFIB_sum->GetStdDev() << endl;
    cout << "EL Mean: " << fh_EL_SAMPFIB_sum->GetMean() << " std: " << fh_EL_SAMPFIB_sum->GetStdDev() << endl;
    cout << "MH Mean: " << fh_rate_sum_SAMPFIB->GetMean() << " std: " << fh_rate_sum_SAMPFIB->GetStdDev() << endl;
    //	cout<<"MH DTMEAN Mean: "<<fh_rate_sum_DTmean->GetMean()<<" std: "<<fh_rate_sum_DTmean->GetStdDev()<<endl;
    cout << "RS Mean: " << fh_DutyFactor_Avg_SAMPFIB->GetMean() << " std: " << fh_DutyFactor_Avg_SAMPFIB->GetStdDev()
         << " RS peak: " << fh_DutyFactor_Max_SAMPFIB->GetMean() << " std: " << fh_DutyFactor_Max_SAMPFIB->GetStdDev()
         << endl;
    cout << "RS Mean normalized: " << fh_DutyFactor_Avg_SAMPFIB_clean->GetMean()
         << " std: " << fh_DutyFactor_Avg_SAMPFIB_clean->GetStdDev()
         << " RS peak normalized: " << fh_DutyFactor_Max_SAMPFIB_clean->GetMean()
         << " std: " << fh_DutyFactor_Max_SAMPFIB_clean->GetStdDev() << endl;

    cout << "start finish task" << endl;
    cout << "Counted " << hit_counter << " Hits" << endl;
    cout << "Counted " << spillCounter << " Spills" << endl;

    if (fMappedItems.at(DET_LOS))
    {
        // for (Int_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        for (Int_t iloscount = 0; iloscount < 1; iloscount++)
        {

            fh_los_channels[iloscount]->Write();
            fh_los_tot[iloscount]->Write();
            fh_los_dt_hits_ToT[iloscount]->Write();
            fh_los_ihit_ToT[iloscount]->Write();
            fh_los_dt_hits[iloscount]->Write();
            fh_dt_hits[iloscount]->Write();
            fh_los_pos_MCFD[iloscount]->Write();
            fh_los_pos_TAMEX[iloscount]->Write();
            fh_los_pos_ToT[iloscount]->Write();
            fh_spill_times_LOS->Write();
            fh_spill_times_LOS_Fine->Write();
            fh_spill_times_LOSFIB_Fine->Write();
            fh_spill_times_LOS_Coarse->Write();
            fh_LOS_tDiff->Write();
            fh_SAMPFIB_tDiff->Write();
            fh_LOS_freq->Write();
            fh_LOS_tDiff_pois->Write();
            fh_spill_times_LOS_pois->Write();
            fh_DutyFactor_LOS->Write();
            fh_DutyFactor_SAMPFIB->Write();
            fh_DutyFactor_MtA->Write();
            fh_DutyFactor_LOS_pois->Write();
            fh_DutyFactor_SAMPFIB_pois->Write();
            fh_DutyFactor_MaxToAvg->Write();
            fh_DutyFactor_Avg->Write();
            fh_DutyFactor_Avg_SAMPFIB->Write();
            fh_DutyFactor_Max->Write();
            fh_DutyFactor_Max_SAMPFIB->Write();
            fh_DutyFactor_PLD->Write();
            // fh_r_pois_fib->Write();
            // fh_r_pois_LOS->Write();
            fh_spill_hans->Write();
            fh_spill_hans_SAMPFIB->Write();
            fh_spill_Q_E_real->Write();
            fh_spill_Q_E_real_SAMPFIB->Write();
            fh_spill_Q_E_pois->Write();
            fh_spill_Q_E_pois_SAMPFIB->Write();
            fh_FFT->Write();
            fh_FFT_adj->Write();
            fh_FFT_add->Write();
            fh_particle->Write();
            fh_particle_pois->Write();
            fh_hans_sum->Write();
            fh_hans_SAMPFIB_sum->Write();
            fh_EL_sum->Write();
            fh_EL_SAMPFIB_sum->Write();
            fh_MtA_sum->Write();
            fh_rate_sum->Write();
            fh_rate_sum_SAMPFIB->Write();
            fh_rate_sum_DTmean->Write();
            fh_rate->Write();
            fh_rate_set->Write();
            fh_rate_adj->Write();
            fh_deadTime_adj->Write();
            fh_deadTime_mean->Write();
            fh_deadTime_sum->Write();
            fh_los_dt_hits[0]->Write();
            fh_spillQuality_ppt->Write();
            fh_spillQuality_ppt_stat->Write();
            fh_DutyFactor_MaxRun->Write();
            fh_DutyFactor_AvgRun->Write();
        }
    }

#define NOTUSEREST

    bool already_saved = false;
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fMappedItems.at(ifibcount + DET_FI_FIRST))
        {
#if 0	
		  fh_channels_Fib[ifibcount]->Write();
		  fh_fibers_Fib[ifibcount]->Write();
		  fh_mult_Fib[ifibcount]->Write();
		  fh_time_Fib[ifibcount]->Write();
		  fh_multihit_m_Fib[ifibcount]->Write();   
		  fh_multihit_s_Fib[ifibcount]->Write();
		  fh_ToT_m_Fib[ifibcount]->Write();
		  fh_Fib_pos_xy[ifibcount]->Write();
		  fh_ToT_s_Fib[ifibcount]->Write();
		  fh_ToT_single_Fib[ifibcount]->Write();
		  fh_Fib_vs_Events[ifibcount]->Write();
		  fh_Fib_ToF[ifibcount]->Write();
		  fh_channels_single_Fib[ifibcount]->Write();
#endif
            if (!already_saved)
            {
                fh_spill_hits_vs_time->Write();
                fh_spill_times_fib->Write();
                fh_FIB_count_t->Write();
                fh_FIB_TrgW_dt->Write();
                fh_LOS_TrgW_dt->Write();
                fh_SAMP_TrgW_dt->Write();
                fh_FIB_TrgW_dt_coarse->Write();
                fh_LOS_TrgW_dt_coarse->Write();
                fh_SAMP_TrgW_dt_coarse->Write();
                fh_FIB_Freq->Write();
                already_saved = true;
            }
        }
    }
}

ClassImp(R3BOnlineSpillAnalysis)
