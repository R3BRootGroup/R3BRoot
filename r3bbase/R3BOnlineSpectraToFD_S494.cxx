// ------------------------------------------------------------
// -----                  R3BOnlineSpectraToFD_S494                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraToFD_S494.h"

#include "R3BTofdCalData.h"
#include "R3BTofdMappedData.h"

#include "R3BPaddleCalData.h"
#include "R3BPaddleTamexMappedData.h"

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"

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
#include "mapping_tofd_trig.hh"
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
extern unsigned g_tofd_trig_map[4][2][48];
void tofd_trig_map_setup();

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 20; // nanoseconds.
} // namespace

R3BOnlineSpectraToFD_S494::R3BOnlineSpectraToFD_S494()
    : R3BOnlineSpectraToFD_S494("OnlineSpectraToFD_S494", 1)
{
}

R3BOnlineSpectraToFD_S494::R3BOnlineSpectraToFD_S494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fCalTriggerItems(NULL)
    , fNofPlanes(N_PLANE_MAX_TOFD)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFD)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraToFD_S494::~R3BOnlineSpectraToFD_S494() {}

InitStatus R3BOnlineSpectraToFD_S494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraToFD_S494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("/Tasks", this);

    fCalTriggerItems = (TClonesArray*)mgr->GetObject("TofdTriggerCal");
    if (NULL == fCalTriggerItems)
        printf("Branch TofdTriggerCal not found.\n");

    // Get objects for detectors on all levels
    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    for (int det = 0; det < DET_MAX; det++)
    {
        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }
        fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCal", fDetectorNames[det])));
    }

    tofd_trig_map_setup();

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
    // TofD detector

    if (fMappedItems.at(DET_TOFD))
    {
        TCanvas* cTofd_planes = new TCanvas("TOFD_planes_CalLevel", "TOFD planes CAL Level", 10, 10, 1100, 1000);
        cTofd_planes->Divide(5, 4);

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

            char strName9[255];
            sprintf(strName9, "tofd_multihit_plane_%d", j + 1);
            char strName10[255];
            sprintf(strName10, "Tofd multihit plane %d", j + 1);
            fh_tofd_multihit[j] = new TH2F(strName9, strName10, 45, 0., 45., 10, 0, 10);
            fh_tofd_multihit[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_multihit[j]->GetYaxis()->SetTitle("Multihit");

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

        cTofd_planes->cd(1);
        fh_tofd_channels[0]->Draw();
        cTofd_planes->cd(2);
        gPad->SetLogz();
        fh_tofd_TotPm[0]->Draw("colz");
        cTofd_planes->cd(3);
        gPad->SetLogz();
        fh_tofd_multihit[0]->Draw("colz");

        cTofd_planes->cd(6);
        fh_tofd_channels[1]->Draw();
        cTofd_planes->cd(7);
        gPad->SetLogz();
        fh_tofd_TotPm[1]->Draw("colz");
        cTofd_planes->cd(8);
        gPad->SetLogz();
        fh_tofd_multihit[1]->Draw("colz");
        cTofd_planes->cd(9);
        gPad->SetLogz();
        fh_tofd_dt[0]->Draw("colz");

        cTofd_planes->cd(11);
        fh_tofd_channels[2]->Draw();
        cTofd_planes->cd(12);
        gPad->SetLogz();
        fh_tofd_TotPm[2]->Draw("colz");
        cTofd_planes->cd(13);
        gPad->SetLogz();
        fh_tofd_multihit[2]->Draw("colz");
        cTofd_planes->cd(14);
        gPad->SetLogz();
        fh_tofd_dt[1]->Draw("colz");

        cTofd_planes->cd(16);
        fh_tofd_channels[3]->Draw();
        cTofd_planes->cd(17);
        gPad->SetLogz();
        fh_tofd_TotPm[3]->Draw("colz");
        cTofd_planes->cd(18);
        gPad->SetLogz();
        fh_tofd_multihit[3]->Draw("colz");
        cTofd_planes->cd(19);
        gPad->SetLogz();
        fh_tofd_dt[2]->Draw("colz");

        cTofd_planes->cd(0);
        run->AddObject(cTofd_planes);

        run->GetHttpServer()->RegisterCommand("Reset_TOFD", Form("/Tasks/%s/->Reset_TOFD_Histo()", GetName()));
    }

    // -------------------------------------------------------------------------

    return kSUCCESS;
}

void R3BOnlineSpectraToFD_S494::Reset_TOFD_Histo()
{
    for (int i = 0; i < N_PLANE_MAX_TOFD; i++)
    {
        fh_tofd_channels[i]->Reset();
        fh_tofd_multihit[i]->Reset();
        fh_tofd_TotPm[i]->Reset();
    }
    fh_tofd_dt[0]->Reset();
    fh_tofd_dt[1]->Reset();
    fh_tofd_dt[2]->Reset();
}

namespace
{
    uint64_t n1, n2;
};

void R3BOnlineSpectraToFD_S494::Exec(Option_t* option)
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
        time_spill_start = header->GetTimeStamp(); // spill start in nsec
    if (header->GetTrigger() == 13)
        time_spill_end = header->GetTimeStamp(); // spill end  in nsec

    if (header->GetTrigger() == 12)
        cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
    if (header->GetTrigger() == 13)
        cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec" << endl;

    //   check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit = fTpat - 1;
    Int_t itpat;
    Int_t tpatvalue;
    if (fTpat_bit >= 0)
    {
        itpat = header->GetTpat();
        tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
        if ((tpatvalue == 0))
            return;
    }

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

        UInt_t vmultihits[N_PLANE_MAX_TOFD + 1][N_PADDLE_MAX_TOFD + 1];
        Double_t time_bar[N_PLANE_MAX_TOFD + 1][N_PADDLE_MAX_TOFD + 1];
        for (Int_t i = 0; i <= fNofPlanes; i++)
        {
            for (Int_t j = 0; j <= N_PADDLE_MAX_TOFD; j++)
            {
                vmultihits[i][j] = 0;
                time_bar[i][j] = 0;
            }
        }

        //    std::cout<<"new event!*************************************\n";
        auto det = fCalItems.at(DET_TOFD);
        Int_t nHits = det->GetEntries();

        Int_t nHitsEvent = 0;
        // Organize cals into bars.
        struct Entry
        {
            std::vector<R3BTofdCalData*> top;
            std::vector<R3BTofdCalData*> bot;
        };

        std::map<size_t, Entry> bar_map;
        //   puts("Event");
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = (R3BTofdCalData*)det->At(ihit);
            size_t idx = hit->GetDetectorId() * fPaddlesPerPlane * hit->GetBarId();

            auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));

            // hit->GetSideId() gives 1 for bottom and 2 for top
            auto& vec = 1 == hit->GetSideId() ? ret.first->second.bot : ret.first->second.top;
            vec.push_back(hit);
        }

        static bool s_was_trig_missing = false;
        auto trig_num = fCalTriggerItems->GetEntries();
        for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
        {
            auto const& top_vec = it->second.top;
            auto const& bot_vec = it->second.bot;
            size_t top_i = 0;
            size_t bot_i = 0;
            for (; top_i < top_vec.size() && bot_i < bot_vec.size();)
            {
                auto top = top_vec.at(top_i);
                auto bot = bot_vec.at(bot_i);
                auto top_trig_i = g_tofd_trig_map[top->GetDetectorId() - 1][top->GetSideId() - 1][top->GetBarId() - 1];
                auto bot_trig_i = g_tofd_trig_map[bot->GetDetectorId() - 1][bot->GetSideId() - 1][bot->GetBarId() - 1];

                Double_t top_trig_ns = 0, bot_trig_ns = 0;
                if (top_trig_i < trig_num && bot_trig_i < trig_num)
                {
                    auto top_trig = (R3BTofdCalData const*)fCalTriggerItems->At(top_trig_i);
                    auto bot_trig = (R3BTofdCalData const*)fCalTriggerItems->At(bot_trig_i);
                    top_trig_ns = top_trig->GetTimeLeading_ns();
                    bot_trig_ns = bot_trig->GetTimeLeading_ns();
                    ++n1;
                }
                else
                {
                    if (!s_was_trig_missing)
                    {
                        LOG(ERROR) << "R3BOnlineSpectraToFD::Exec() : Missing trigger information!";
                        s_was_trig_missing = true;
                    }
                    ++n2;
                }

                // Shift the cyclic difference window by half a window-length and move it back,
                // this way the trigger time will be at 0.
                auto top_ns = fmod(top->GetTimeLeading_ns() - top_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) -
                              c_range_ns / 2;
                auto bot_ns = fmod(bot->GetTimeLeading_ns() - bot_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) -
                              c_range_ns / 2;
                /*
                            if(top_ns>2000 || bot_ns>2000){
                                std::cout << top->GetTimeLeading_ns() << ' ' << top_trig_ns << ' ' << top_ns <<
                   std::endl; std::cout << bot->GetTimeLeading_ns() << ' ' << bot_trig_ns << ' ' << bot_ns << std::endl;
                            }
                */
                auto dt = top_ns - bot_ns;
                // Handle wrap-around.
                auto dt_mod = fmod(dt + c_range_ns, c_range_ns);
                if (dt < 0)
                {
                    // We're only interested in the short time-differences, so we
                    // want to move the upper part of the coarse counter range close
                    // to the lower range, i.e. we cut the middle of the range and
                    // glue zero and the largest values together.
                    dt_mod -= c_range_ns;
                }
                // std::cout << top_i << ' ' << bot_i << ": " << top_ns << ' ' << bot_ns << " = " << dt << ' ' <<
                // std::abs(dt_mod) << '\n';
                if (std::abs(dt_mod) < c_bar_coincidence_ns)
                {
                    // Hit!
                    // std::cout << "Hit!\n";
                    Int_t iPlane = top->GetDetectorId(); // 1..n
                    Int_t iBar = top->GetBarId();        // 1..n
                    if (iPlane > fNofPlanes)             // this also errors for iDetector==0
                    {
                        LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more detectors than expected! Det: " << iPlane
                                   << " allowed are 1.." << fNofPlanes;
                        continue;
                    }
                    if (iBar > fPaddlesPerPlane) // same here
                    {
                        LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more bars then expected! Det: " << iBar
                                   << " allowed are 1.." << fPaddlesPerPlane;
                        continue;
                    }

                    auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                    auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);

                    fh_tofd_TotPm[iPlane - 1]->Fill(iBar, bot_tot);
                    fh_tofd_TotPm[iPlane - 1]->Fill(-iBar - 1, top_tot);

                    // std::cout<<"ToT: "<<top_tot << " "<<bot_tot<<"\n";

                    // register multi hits
                    vmultihits[iPlane - 1][iBar - 1] += 1;
                    time_bar[iPlane - 1][iBar - 1] = (top_ns + bot_ns) / 2.;

                    ++top_i;
                    ++bot_i;
                }
                else if (dt < 0 && dt > -c_range_ns / 2)
                {
                    ++top_i;
                }
                else
                {
                    ++bot_i;
                }
            }
        }

        for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
        {
            for (Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++)
            {
                fh_tofd_multihit[ipl]->Fill(ibr + 1, vmultihits[ipl][ibr]);
                if (ipl > 0)
                {
                    Double_t tof_plane = fmod(time_bar[ipl][ibr] - time_bar[ipl - 1][ibr] + c_range_ns, c_range_ns);
                    fh_tofd_dt[ipl - 1]->Fill(ibr, tof_plane);
                }
            }
        }

    } // endi if fCalItems

    fNEvents += 1;
}

void R3BOnlineSpectraToFD_S494::FinishEvent()
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
    }
}

void R3BOnlineSpectraToFD_S494::FinishTask()
{
    if (fCalItems.at(DET_TOFD))
    {
        for (Int_t i = 0; i < 4; i++)
        {
            fh_tofd_TotPm[i]->Write();
            fh_tofd_channels[i]->Write();
        }
        for (Int_t i = 0; i < 3; i++)
        {
            fh_tofd_dt[i]->Write();
        }
    }
}

ClassImp(R3BOnlineSpectraToFD_S494)
