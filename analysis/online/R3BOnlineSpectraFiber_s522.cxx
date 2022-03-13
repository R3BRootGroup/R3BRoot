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
// -----            R3BOnlineSpectraFiber_s522            -----
// -----    Created 13/03/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill tracking online histograms          -----
// ------------------------------------------------------------

#include "R3BOnlineSpectraFiber_s522.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BSamplerMappedData.h"
#include "R3BTCalEngine.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

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
#include "mapping_fib30_trig.hh"
#include "mapping_fib31_trig.hh"
#include "mapping_fib32_trig.hh"
#include "mapping_fib33_trig.hh"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <algorithm>
#include <vector>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;
namespace
{
    double c_period = 4096 * 1000. / 150;
    double c_fiber_coincidence_ns = 20; // nanoseconds.
    double c_tot_coincidence_ns = 100;  // nanoseconds
} // namespace
R3BOnlineSpectraFiber_s522::R3BOnlineSpectraFiber_s522()
    : R3BOnlineSpectraFiber_s522("OnlineSpectraFiber_s522", 1)
{
}

R3BOnlineSpectraFiber_s522::R3BOnlineSpectraFiber_s522(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
    , fChannelArray()
{
}

R3BOnlineSpectraFiber_s522::~R3BOnlineSpectraFiber_s522()
{
    R3BLOG(DEBUG1, "Destructor");
    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        if (fh_channels_Fib[i])
            delete fh_channels_Fib[i];
        if (fh_fibers_Fib[i])
            delete fh_fibers_Fib[i];
        if (fh_mult_Fib[i])
            delete fh_mult_Fib[i];
        if (fh_time_Fib[i])
            delete fh_time_Fib[i];
        if (fh_multihit_m_Fib[i])
            delete fh_multihit_m_Fib[i];
        if (fh_multihit_s_Fib[i])
            delete fh_multihit_s_Fib[i];
        if (fh_ToT_Fib[i])
            delete fh_ToT_Fib[i];
        if (fh_channels_single_Fib[i])
            delete fh_channels_single_Fib[i];
        if (fh_Fib_pos[i])
            delete fh_Fib_pos[i];
        if (fh_Fib_vs_Events[i])
            delete fh_Fib_vs_Events[i];
        if (fh_ToTup_vs_ToTdown[i])
            delete fh_ToTup_vs_ToTdown[i];
        if (fh_chan_corell[i])
            delete fh_chan_corell[i];
        if (fh_raw_tot_up[i])
            delete fh_raw_tot_up[i];
        if (fh_raw_tot_down[i])
            delete fh_raw_tot_down[i];
    }
}

InitStatus R3BOnlineSpectraFiber_s522::Init()
{
    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    R3BLOG(INFO, "");

    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!header)
    {
        R3BLOG(WARNING, "EventHeader. not found");
        header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        R3BLOG(INFO, "EventHeader. found");

    // uncomment lines below when ucesb avaliable
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // Get objects for detectors on all levels
    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    printf("**** Have %d fiber detectors ==>.\n", NOF_FIB_DET);
    for (int det = 0; det < DET_MAX; det++)
    {
        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }
        fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCal", fDetectorNames[det])));
        if (NULL == fCalItems.at(det))
        {
            printf("Could not find Cal data for '%s'.\n", fDetectorNames[det]);
        }
        fCalTriggerItems.push_back((TClonesArray*)mgr->GetObject(Form("%sTriggerCal", fDetectorNames[det])));
        if (NULL == fCalTriggerItems.at(det))
        {
            printf("Could not find CalTrigger data for '%s'.\n", fDetectorNames[det]);
        }
        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
        if (NULL == fHitItems.at(det))
        {
            printf("Could not find hit data for '%s'.\n", fDetectorNames[det]);
        }
    }

    // Trigger mappin from included mapping files for each det.
    const char* name;
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fCalItems.at(DET_FI_FIRST + ifibcount))
        {
            name = fDetectorNames[DET_FI_FIRST + ifibcount];
            if ((strcmp(name, "Fi30") == 0))
            {
                fib30_trig_map_setup();
                fTriggerMap[0] = g_fib30_trig_map[0];
                fTriggerMap[1] = g_fib30_trig_map[1];
            }
            if ((strcmp(name, "Fi31") == 0))
            {
                fib31_trig_map_setup();
                fTriggerMap[0] = g_fib31_trig_map[0];
                fTriggerMap[1] = g_fib31_trig_map[1];
            }

            if ((strcmp(name, "Fi32") == 0))
            {
                fib32_trig_map_setup();
                fTriggerMap[0] = g_fib32_trig_map[0];
                fTriggerMap[1] = g_fib32_trig_map[1];
            }
            if ((strcmp(name, "Fi33") == 0))
            {
                fib33_trig_map_setup();
                fTriggerMap[0] = g_fib33_trig_map[0];
                fTriggerMap[1] = g_fib33_trig_map[1];
            }
        }
    }

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    TCanvas* FibCanvas[NOF_FIB_DET];

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {

        if (fCalItems.at(DET_FI_FIRST + ifibcount))
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
            // Cal level
            // Channels:
            fh_channels_Fib[ifibcount] =
                new TH1F(Form("%sMapped_channels_up", detName), Form("%sMapped channels up", detName), 520, 0., 520.);
            fh_channels_Fib[ifibcount]->GetXaxis()->SetTitle("PMT channel number");
            fh_channels_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Channels:
            fh_channels_single_Fib[ifibcount] = new TH1F(
                Form("%sMapped_channels_down", detName), Form("%sMapped channels down", detName), 520, 0., 520.);
            fh_channels_single_Fib[ifibcount]->GetXaxis()->SetTitle("PMT channel number");
            fh_channels_single_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multihit MAPMT:
            fh_multihit_m_Fib[ifibcount] = new TH2F(
                Form("%sCal_multihit_up", detName), Form("%sCal multihits up", detName), 520, 0., 520., 20, 0., 20.);
            fh_multihit_m_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_multihit_m_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // Multihit SAPMT:
            fh_multihit_s_Fib[ifibcount] = new TH2F(
                Form("%sCal_multihit_down", detName), Form("%sCal multihits down", detName), 520, 0., 520, 20, 0., 20.);
            fh_multihit_s_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_multihit_s_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // ToT raw up:
            fh_raw_tot_up[ifibcount] = new TH2F(Form("%sCal_totraw_up", detName),
                                                Form("%sCal ToT raw up", detName),
                                                N_FIBER_PLOT_S522,
                                                0.,
                                                N_FIBER_PLOT_S522,
                                                200,
                                                0.,
                                                100.);
            fh_raw_tot_up[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_raw_tot_up[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToT SAPMT:
            fh_raw_tot_down[ifibcount] = new TH2F(Form("%sCal_totraw_down", detName),
                                                  Form("%sCal ToT raw down", detName),
                                                  N_FIBER_PLOT_S522,
                                                  0.,
                                                  N_FIBER_PLOT_S522,
                                                  200,
                                                  0.,
                                                  100.);
            fh_raw_tot_down[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_raw_tot_down[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            fh_chan_corell[ifibcount] = new TH2F(Form("%sCal_chan_vs_chan", detName),
                                                 Form("%sCal ch_up vs ch_down", detName),
                                                 512,
                                                 0.,
                                                 512,
                                                 512,
                                                 0.,
                                                 512.);
            fh_chan_corell[ifibcount]->GetXaxis()->SetTitle("Fiber number down");
            fh_chan_corell[ifibcount]->GetYaxis()->SetTitle("Fiber number up");

            fh_chan_dt_cal[ifibcount] = new TH2F(Form("%sCal_TimevsChannel", detName),
                                                 Form("%sCal Time bottom and top vs fiber number", detName),
                                                 2 * N_FIBER_PLOT_S522,
                                                 -N_FIBER_PLOT_S522,
                                                 N_FIBER_PLOT_S522,
                                                 1700,
                                                 -1500.,
                                                 200.);
            fh_chan_dt_cal[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_chan_dt_cal[ifibcount]->GetYaxis()->SetTitle("time(up/down)-time_trigger / ns");

            // Hit level
            // Fibers:
            fh_fibers_Fib[ifibcount] = new TH1F(
                Form("%sHit_fibers", detName), Form("%sHit fibers", detName), N_FIBER_PLOT_S522, 0., N_FIBER_PLOT_S522);
            fh_fibers_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_fibers_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multiplicity (number of hit fibers):
            fh_mult_Fib[ifibcount] = new TH1F(Form("%sHit_mult", detName), Form("%sHit mult", detName), 520, 0., 520.);
            fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // ToT :
            fh_ToT_Fib[ifibcount] = new TH2F(Form("%sHit_ToT_iFib", detName),
                                             Form("%sHit ToT vs Fib", detName),
                                             N_FIBER_PLOT_S522,
                                             0.,
                                             N_FIBER_PLOT_S522,
                                             200,
                                             0.,
                                             100.);
            fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // Time of fiber:
            fh_time_Fib[ifibcount] = new TH2F(Form("%sHit_TimevsFiber", detName),
                                              Form("%sHit Time vs Fiber", detName),
                                              N_FIBER_PLOT_S522,
                                              0.,
                                              N_FIBER_PLOT_S522,
                                              4096,
                                              -2048.,
                                              2048.);
            fh_time_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_time_Fib[ifibcount]->GetYaxis()->SetTitle("(tUp+tDown)/2 / ns");

            // Not-calibrated position:
            fh_Fib_pos[ifibcount] = new TH2F(Form("%sHit_pos", detName),
                                             Form("%sHit Not-calibrated position", detName),
                                             300,
                                             -30,
                                             30,
                                             500,
                                             -500,
                                             500);
            fh_Fib_pos[ifibcount]->GetXaxis()->SetTitle("x position");
            fh_Fib_pos[ifibcount]->GetYaxis()->SetTitle("y position");

            // hit fiber number vs. event number:
            fh_Fib_vs_Events[ifibcount] = new TH2F(Form("%sHit_fib_vs_event", detName),
                                                   Form("%sHit Fiber # vs. Event #", detName),
                                                   100,
                                                   0,
                                                   5e6,
                                                   N_FIBER_PLOT_S522,
                                                   0.,
                                                   N_FIBER_PLOT_S522);
            fh_Fib_vs_Events[ifibcount]->GetYaxis()->SetTitle("Fiber number");
            fh_Fib_vs_Events[ifibcount]->GetXaxis()->SetTitle("Event number");

            fh_ToTup_vs_ToTdown[ifibcount] = new TH2F(Form("%sHit_time_vs_event", detName),
                                                      Form("%sHit time # vs. Event #", detName),
                                                      200,
                                                      0,
                                                      5e6,
                                                      2000,
                                                      -1000.,
                                                      1000.);
            fh_ToTup_vs_ToTdown[ifibcount]->GetXaxis()->SetTitle("Event number");
            fh_ToTup_vs_ToTdown[ifibcount]->GetYaxis()->SetTitle("Time  / ns");

            FibCanvas[ifibcount]->Divide(4, 3);

            FibCanvas[ifibcount]->cd(1);
            gPad->SetLogy();
            fh_channels_Fib[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(2);
            gPad->SetLogy();
            fh_channels_single_Fib[ifibcount]->Draw();

            if (fCalItems.at(DET_FI_FIRST + ifibcount))
            {

                FibCanvas[ifibcount]->cd(3);
                gPad->SetLogz();
                fh_multihit_m_Fib[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(4);
                gPad->SetLogz();
                fh_multihit_s_Fib[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(5);
                gPad->SetLogz();
                fh_chan_corell[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(6);
                gPad->SetLogz();
                fh_chan_dt_cal[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(7);
                gPad->SetLogz();
                fh_raw_tot_up[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(8);
                gPad->SetLogz();
                fh_raw_tot_down[ifibcount]->Draw("colz");
            }
            if (fHitItems.at(DET_FI_FIRST + ifibcount))
            {
                FibCanvas[ifibcount]->cd(9);
                gPad->SetLogy();
                fh_fibers_Fib[ifibcount]->Draw();
                FibCanvas[ifibcount]->cd(10);
                gPad->SetLogy();
                fh_mult_Fib[ifibcount]->Draw();
                FibCanvas[ifibcount]->cd(12);
                gPad->SetLogz();
                fh_ToT_Fib[ifibcount]->Draw("colz");

                FibCanvas[ifibcount]->cd(11);
                gPad->SetLogz();
                fh_time_Fib[ifibcount]->Draw("colz");
            }
            FibCanvas[ifibcount]->cd(0);
            // uncomment lines below when ucesb avaliable
            run->AddObject(FibCanvas[ifibcount]);
            run->GetHttpServer()->RegisterCommand("Reset_Fibers", Form("/Objects/%s/->Reset_Fiber_Histo()", GetName()));
        } // end if(Mapped)

    } // end for(ifibcount)

    return kSUCCESS;
}
void R3BOnlineSpectraFiber_s522::Reset_Fiber_Histo()
{
    R3BLOG(INFO, "Reset_Histo");
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fCalItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_channels_Fib[ifibcount]->Reset();
            fh_mult_Fib[ifibcount]->Reset();
            fh_multihit_m_Fib[ifibcount]->Reset();
            fh_multihit_s_Fib[ifibcount]->Reset();
            fh_chan_corell[ifibcount]->Reset();
            fh_channels_single_Fib[ifibcount]->Reset();
            fh_raw_tot_up[ifibcount]->Reset();
            fh_raw_tot_down[ifibcount]->Reset();
            fh_chan_dt_cal[ifibcount]->Reset();
        }
        if (fHitItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_time_Fib[ifibcount]->Reset();
            fh_fibers_Fib[ifibcount]->Reset();
            fh_ToT_Fib[ifibcount]->Reset();
            fh_Fib_vs_Events[ifibcount]->Reset();
            fh_Fib_pos[ifibcount]->Reset();
            fh_Fib_vs_Events[ifibcount]->Reset();
            fh_ToTup_vs_ToTdown[ifibcount]->Reset();
        }
    }
}
void R3BOnlineSpectraFiber_s522::Exec(Option_t* option)
{
    fNEvents += 1;
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        cout << "Events: " << fNEvents << flush << '\r';

    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit1 = fTpat1 - 1;
    Int_t fTpat_bit2 = fTpat2 - 1;
    Int_t tpatbin;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
        {
            return;
        }
    }

    //----------------------------------------------------------------------
    // Fiber detectors
    //----------------------------------------------------------------------
    Double_t xpos_global = 0. / 0.;
    Double_t ypos_global = 0. / 0.;
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        Int_t iFib = 0;
        Double_t dtime = 0.0 / 0.0, dtimeMax;
        Double_t yfib = -100000., xfib = -100000., tfibMax = 0. / 0.;
        Double_t rfib = 0.0 / 0.0;
        Int_t iFibMax;
        Double_t totMax;
        Double_t tofMax;
        Double_t tot_up = 0., tot_down = 0.;
        Int_t iCha = 0;
        Int_t iCha_up = 0, iCha_down = 0;
        Double_t tFib = 0. / 0.;
        Int_t iSide;
        Double_t c_period = 4096. * (1000. / fClockFreq);

        auto detMap = fMappedItems.at(DET_FI_FIRST + ifibcount);
        auto detCal = fCalItems.at(DET_FI_FIRST + ifibcount);
        auto detHit = fHitItems.at(DET_FI_FIRST + ifibcount);
        auto detTrig = fCalTriggerItems.at(DET_FI_FIRST + ifibcount);

        //  cout<<"DETECTOR: "<<fDetectorNames[DET_FI_FIRST + ifibcount]<<endl;

        if (detMap && detMap->GetEntriesFast() > 0)
        {
            auto nMapp = detMap->GetEntries();
            for (Int_t i = 0; i < nMapp; i++)
            {

                auto map_lead = (R3BFiberMAPMTMappedData const*)detMap->At(i);

                if (map_lead->IsLeading())
                {
                    auto side_i = map_lead->GetSide();
                    auto ch_i = map_lead->GetChannel() - 1;

                    if (side_i == 1)
                    {
                        fh_channels_Fib[ifibcount]->Fill(ch_i); // Fill which channel has events
                    }

                    if (side_i == 0)
                    {
                        fh_channels_single_Fib[ifibcount]->Fill(ch_i); // Fill which channel has events
                    }
                }
            }
        }

        if (detCal && detCal->GetEntriesFast() > 0)
        {
            Int_t fc = n_fiber[ifibcount];
            UInt_t vmultihits_top[fc], vmultihits_bot[fc];
            for (Int_t i = 0; i < fc; i++)
            {
                vmultihits_top[i] = 0;
                vmultihits_bot[i] = 0;
            }
            // Resize per-channel info arrays.
            for (auto i = 0; i < 2; ++i)
            {
                fChannelArray[i].resize(fc);
            }

            double trig_time[8] = { 0 };

            //------ Collecting cal trigger hits --------
            size_t cal_num = detTrig->GetEntriesFast();
            Double_t tl, tt; // lead and trail times of the trigger
            for (UInt_t j = 0; j < cal_num; ++j)
            {
                auto cur_cal = (R3BFiberMAPMTCalData const*)detTrig->At(j);
                auto ch = cur_cal->GetChannel() - 1;
                tl = cur_cal->GetTime_ns();
                trig_time[ch] = tl;
            }

            Int_t nCals = detCal->GetEntries();

            for (auto side_i = 0; side_i < 2; ++side_i)
            {
                // Clear local helper containers.
                auto& array = fChannelArray[side_i];
                for (auto it = array.begin(); array.end() != it; ++it)
                {
                    it->lead_list.clear();
                }
            }

            for (size_t j = 0; j < nCals; ++j)
            {
                auto cur_cal_lead = (R3BFiberMAPMTCalData const*)detCal->At(j);

                if (cur_cal_lead->IsLeading())
                {
                    auto side_i = cur_cal_lead->GetSide();
                    auto ch_i = cur_cal_lead->GetChannel() - 1;

                    if (side_i == 1)
                    {
                        //   fh_channels_Fib[ifibcount]->Fill(ch_i); // Fill which channel has events
                        vmultihits_top[ch_i] += 1; // multihit of a given up killom channel
                    }

                    if (side_i == 0)
                    {
                        //  fh_channels_single_Fib[ifibcount]->Fill(ch_i); // Fill which channel has events
                        vmultihits_bot[ch_i] += 1; // multihit of a given down killom channel
                    }

                    auto time_trig = trig_time[fTriggerMap[side_i][ch_i]];
                    auto time_ns =
                        fmod(cur_cal_lead->GetTime_ns() - time_trig + c_period + c_period / 2, c_period) - c_period / 2;

                    if (side_i == 0)
                        fh_chan_dt_cal[ifibcount]->Fill(-ch_i - 1, time_ns);
                    if (side_i == 1)
                        fh_chan_dt_cal[ifibcount]->Fill(ch_i + 1, time_ns);

                    auto& channel = fChannelArray[side_i].at(ch_i);

                    channel.lead_list.push_back(cur_cal_lead); // make list with leading times
                }
            }

            for (int i = 0; i < fc; ++i)
            {

                if (vmultihits_top[i] > 0)
                    fh_multihit_m_Fib[ifibcount]->Fill(i + 1,
                                                       vmultihits_top[i]); // multihit of a given up killom channel

                if (vmultihits_bot[i] > 0)
                    fh_multihit_s_Fib[ifibcount]->Fill(i + 1,
                                                       vmultihits_bot[i]); // multihit of a given down killom channel
            }

            for (size_t j = 0; j < nCals; ++j)
            {
                auto cur_cal_trail = (R3BFiberMAPMTCalData const*)detCal->At(j);

                if (cur_cal_trail->IsTrailing())
                {
                    auto side_i = cur_cal_trail->GetSide();
                    auto ch_i = cur_cal_trail->GetChannel() - 1;

                    auto& channel = fChannelArray[side_i].at(ch_i);
                    if (channel.lead_list.empty())
                    {
                        continue;
                    }
                    auto lead = channel.lead_list.front();
                    Double_t lead_raw = 0;
                    Double_t trail_raw = 0;

                    lead_raw = lead->GetTime_ns();
                    trail_raw = cur_cal_trail->GetTime_ns();

                    auto time_trig_lead = trig_time[fTriggerMap[lead->GetSide()][lead->GetChannel() - 1]];
                    auto tlead_ns =
                        fmod(lead->GetTime_ns() - time_trig_lead + c_period + c_period / 2, c_period) - c_period / 2;

                    auto time_trig_trail = trig_time[fTriggerMap[side_i][ch_i]];
                    auto ttrail_ns =
                        fmod(cur_cal_trail->GetTime_ns() - time_trig_trail + c_period + c_period / 2, c_period) -
                        c_period / 2;

                    auto tot_ns = fmod(ttrail_ns - tlead_ns + c_period + c_period / 2, c_period) - c_period / 2;

                    if (tot_ns < c_tot_coincidence_ns && tot_ns > 0.)
                    {
                        if (cur_cal_trail->GetSide() == 1)
                            fh_raw_tot_up[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);
                        if (cur_cal_trail->GetSide() == 0)
                            fh_raw_tot_down[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);
                    }
                }
            }

            for (size_t j = 0; j < nCals; ++j)
            {
                auto cur_cal_top = (R3BFiberMAPMTCalData const*)detCal->At(j);
                if (cur_cal_top->IsLeading() && cur_cal_top->GetSide() == 1) // choose leading top times
                {
                    auto cht_i = cur_cal_top->GetChannel() - 1;
                    auto time_trig_top = trig_time[fTriggerMap[cur_cal_top->GetSide()][cht_i]];
                    auto time_top =
                        fmod(cur_cal_top->GetTime_ns() - time_trig_top + c_period + c_period / 2, c_period) -
                        c_period / 2;
                    for (size_t k = 0; k < nCals; ++k)
                    {
                        auto cur_cal_bot = (R3BFiberMAPMTCalData const*)detCal->At(k);
                        if (cur_cal_bot->IsLeading() && cur_cal_bot->GetSide() == 0) // choose leading bottom times
                        {
                            auto chb_i = cur_cal_bot->GetChannel() - 1;
                            auto time_trig_bot = trig_time[fTriggerMap[cur_cal_bot->GetSide()][chb_i]];
                            auto time_bot =
                                fmod(cur_cal_bot->GetTime_ns() - time_trig_bot + c_period + c_period / 2, c_period) -
                                c_period / 2;
                            auto dt = time_top - time_bot;
                            auto dt_mod = fmod(dt + c_period, c_period);

                            if (dt < 0)
                            {
                                // We're only interested in the short time-differences, so we
                                // want to move the upper part of the coarse counter range close
                                // to the lower range, i.e. we cut the middle of the range and
                                // glue zero and the largest values together.
                                dt_mod -= c_period;
                            }
                            if (std::abs(dt_mod) < c_fiber_coincidence_ns)
                                fh_chan_corell[ifibcount]->Fill(cur_cal_bot->GetChannel(), cur_cal_top->GetChannel());
                        }
                    }
                }
            }

            fChannelArray[0].clear();
            fChannelArray[1].clear();
        } // if Cal

        if (detHit)
        {
            Int_t nHits = detHit->GetEntriesFast();
            Double_t xposMax = 0. / 0.;
            Double_t yposMax = 0. / 0.;
            Double_t tfib = 0. / 0.;
            Double_t totMax_MA = 0.;
            Double_t spmtMax;
            Double_t mapmtMax;
            Double_t totMax_S = 0.;
            iFibMax = -1000;
            totMax = 0.;
            dtimeMax = 0. / 0.;

            // if (nHits > 1)
            //  return;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                Double_t tDown = 0. / 0.;
                Double_t tUp = 0. / 0.;

                R3BFiberMAPMTHitData* hit = (R3BFiberMAPMTHitData*)detHit->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..

                // times
                tUp = hit->GetTopTime_ns();
                tDown = hit->GetBottomTime_ns();
                dtime = tUp - tDown;

                // (tUp+tDown)/2:
                tfib = hit->GetTime();

                // ToT from up and down MAPMT:
                Double_t ToT_up = hit->GetTopToT_ns();
                Double_t ToT_down = hit->GetBottomToT_ns();

                Double_t ToT = hit->GetEloss();
                Double_t xpos = hit->GetX();
                Double_t ypos = hit->GetY();

                fh_Fib_pos[ifibcount]->Fill(xpos, ypos);

                if (ToT > totMax)
                {
                    totMax = ToT;
                    iFibMax = iFib;
                    tfibMax = tfib;
                    dtimeMax = dtime;
                    xposMax = xpos;
                    yposMax = ypos;
                }

                fh_fibers_Fib[ifibcount]->Fill(iFib);
                fh_ToT_Fib[ifibcount]->Fill(iFib, ToT);

                fh_ToTup_vs_ToTdown[ifibcount]->Fill(fNEvents, tfib);

                fh_time_Fib[ifibcount]->Fill(iFib, tfib);
                fh_Fib_vs_Events[ifibcount]->Fill(fNEvents, iFib);

            } // end for(ihit)

            if (nHits > 0)
                fh_mult_Fib[ifibcount]->Fill(nHits);

        } // end if(aHit[ifibcount])
    }     // end for(ifibcount)
}

void R3BOnlineSpectraFiber_s522::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {

        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
        if (fHitItems.at(det))
        {
            fHitItems.at(det)->Clear();
        }
    }
}

void R3BOnlineSpectraFiber_s522::FinishTask()
{
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fCalItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_channels_Fib[ifibcount]->Write();
            fh_mult_Fib[ifibcount]->Write();
            fh_multihit_m_Fib[ifibcount]->Write();
            fh_multihit_s_Fib[ifibcount]->Write();
            fh_channels_single_Fib[ifibcount]->Write();
            fh_ToTup_vs_ToTdown[ifibcount]->Write();
        }
        if (fHitItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_time_Fib[ifibcount]->Write();
            fh_fibers_Fib[ifibcount]->Write();
            fh_ToT_Fib[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_Fib_pos[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_chan_corell[ifibcount]->Write();
            fh_raw_tot_up[ifibcount]->Write();
            fh_raw_tot_down[ifibcount]->Write();
        }
    }
}

ClassImp(R3BOnlineSpectraFiber_s522);
