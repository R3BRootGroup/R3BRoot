// ------------------------------------------------------------
// -----                  R3BOnlineSpectraToFI_S494                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraToFI_S494.h"

#include "R3BTofiCalData.h"
#include "R3BTofiMappedData.h"

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
#include "mapping_tofi_trig.hh"
extern unsigned g_tofi_trig_map[2][24];
void tofi_trig_map_setup();

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 20; // nanoseconds.
} // namespace

R3BOnlineSpectraToFI_S494::R3BOnlineSpectraToFI_S494()
    : R3BOnlineSpectraToFI_S494("OnlineSpectraToFI_S494", 1)
{
}

R3BOnlineSpectraToFI_S494::R3BOnlineSpectraToFI_S494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fCalTriggerItems(NULL)
    , fNofPlanes(N_PLANE_MAX_TOFI)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFI)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraToFI_S494::~R3BOnlineSpectraToFI_S494() {}

InitStatus R3BOnlineSpectraToFI_S494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraToFI_S494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("/Tasks", this);
  
    fCalTriggerItems = (TClonesArray*)mgr->GetObject("TofiTriggerCal");
    if (NULL == fCalTriggerItems)
        printf("Branch TofiTriggerCal not found.\n");
        
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
    
    tofi_trig_map_setup();
    
    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
    // TofI detector

    if (fMappedItems.at(DET_TOFI))
    {
        TCanvas* ctofi_planes = new TCanvas("TOFI_planes", "TOFI planes", 10, 10, 1100, 1000);
        ctofi_planes->Divide(4,1);

        for (Int_t j = 0; j < N_PLANE_MAX_TOFI; j++)
        {
            char strName1[255];
            sprintf(strName1, "tofi_channels_plane_%d", j + 1);
            char strName2[255];
            sprintf(strName2, "Tofi channels plane %d", j + 1);
            fh_tofi_channels[j] = new TH1F(strName1, strName2, 60, -30, 30);
            fh_tofi_channels[j]->GetXaxis()->SetTitle("Channel");
            fh_tofi_channels[j]->GetYaxis()->SetTitle("Counts");

            char strName3[255];
            sprintf(strName3, "tofi_ToT_plane_%d", j + 1);
            char strName4[255];
            sprintf(strName4, "Tofi ToT plane %d", j + 1);
            fh_tofi_TotPm[j] = new TH2F(strName3, strName4, 60, -30, 30, 3000, 0., 300.);
            fh_tofi_TotPm[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofi_TotPm[j]->GetYaxis()->SetTitle("ToT / ns");

            char strName9[255];
            sprintf(strName9, "tofi_multihit_plane_%d", j + 1);
            char strName10[255];
            sprintf(strName10, "Tofi multihit plane %d", j + 1);
            fh_tofi_multihit[j] = new TH2F(strName9, strName10, 30, 0., 30., 10, 0, 10);
            fh_tofi_multihit[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofi_multihit[j]->GetYaxis()->SetTitle("Multihit");
        }
 
         fh_tofi_dt = new TH2F("tofi_dt", "Tofi dt", 25, 0, 25, 2000, -100., 100);
         fh_tofi_dt->GetXaxis()->SetTitle("Bar number");
         fh_tofi_dt->GetYaxis()->SetTitle("dt / ns");
           
        ctofi_planes->cd(1);
        fh_tofi_channels[0]->Draw();
        ctofi_planes->cd(2);
        gPad->SetLogz();
        fh_tofi_TotPm[0]->Draw("colz");
        ctofi_planes->cd(3);
        gPad->SetLogz();
        fh_tofi_multihit[0]->Draw("colz");
        ctofi_planes->cd(4);
        gPad->SetLogz();
        fh_tofi_dt->Draw("colz");



        ctofi_planes->cd(0);
        run->AddObject(ctofi_planes);

        run->GetHttpServer()->RegisterCommand("Reset_TOFI", Form("/Tasks/%s/->Reset_TOFI_Histo()", GetName()));
    }

    // -------------------------------------------------------------------------

    return kSUCCESS;
}

void R3BOnlineSpectraToFI_S494::Reset_TOFI_Histo()
{
    for (int i = 0; i < N_PLANE_MAX_TOFI; i++)
    {
        fh_tofi_channels[i]->Reset();
        fh_tofi_multihit[i]->Reset();
        fh_tofi_TotPm[i]->Reset();
    }
    fh_tofi_dt->Reset();
    
}

namespace
{
    uint64_t n1, n2;
};

void R3BOnlineSpectraToFI_S494::Exec(Option_t* option)
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
    // TOFI
    //----------------------------------------------------------------------

    Int_t NumPaddles =  0;
    if (fMappedItems.at(DET_TOFI))
    {
        auto det = fMappedItems.at(DET_TOFI);
        Int_t nMapped = det->GetEntriesFast();
        Int_t iPlaneMem = 1, iBarMem = 0;
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BTofiMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen

            Int_t const iPlane = mapped->GetDetectorId(); // 1..n
            Int_t const iBar = mapped->GetBarId();        // 1..n
            Int_t const iSide = mapped->GetSideId();      // 1..n
            Int_t const iEdge = mapped->GetEdgeId();

            if (iPlaneMem != iPlane)
                iBarMem = 0;
            if (iPlane == 1 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles += 1;
 
            //  cout<<imapped<<", "<<iPlane<<"; "<<iPlaneMem<<"; "<<iBar<<"; "<<iBarMem<<",
            //  "<<NumPaddles[iPlane-1]<<endl;

            iPlaneMem = iPlane;
            iBarMem = iBar;

            if (iPlane <= fNofPlanes)
            {
                if (iSide == 1)
                    fh_tofi_channels[iPlane - 1]->Fill(-iBar - 1);
                if (iSide == 2)
                    fh_tofi_channels[iPlane - 1]->Fill(iBar);
            }
        }
    }

    if (fCalItems.at(DET_TOFI))
    {

    UInt_t vmultihits[N_PLANE_MAX_TOFI + 1][N_PADDLE_MAX_TOFI + 1];
    Double_t time_bar[N_PLANE_MAX_TOFI + 1][N_PADDLE_MAX_TOFI + 1];
    for (Int_t i = 0; i <= fNofPlanes; i++)
    {
        for (Int_t j = 0; j <= N_PADDLE_MAX_TOFI; j++)
        {
            vmultihits[i][j] = 0;
            time_bar[i][j] = 0;
        }
    }

    //    std::cout<<"new event!*************************************\n";
    auto det = fCalItems.at(DET_TOFI);
    Int_t nHits = det->GetEntries();
   

    Int_t nHitsEvent = 0;
    // Organize cals into bars.
    struct Entry
    {
        std::vector<R3BTofiCalData*> top;
        std::vector<R3BTofiCalData*> bot;
    };
    std::map<size_t, Entry> bar_map;
    // puts("Event");
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        auto* hit = (R3BTofiCalData*)det->At(ihit);
        size_t idx = hit->GetDetectorId() * fPaddlesPerPlane * hit->GetBarId();

        auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));
        auto& vec = 1 == hit->GetSideId() ? ret.first->second.top : ret.first->second.bot;  // if side=1 -> top, if side=2 -> bottom
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
            auto top_trig_i = g_tofi_trig_map[top->GetSideId() - 1][top->GetBarId() - 1];
            auto bot_trig_i = g_tofi_trig_map[bot->GetSideId() - 1][bot->GetBarId() - 1];
            Double_t top_trig_ns = 0, bot_trig_ns = 0;
            if (top_trig_i < trig_num && bot_trig_i < trig_num)
            {
                auto top_trig = (R3BTofiCalData const*)fCalTriggerItems->At(top_trig_i);
                auto bot_trig = (R3BTofiCalData const*)fCalTriggerItems->At(bot_trig_i);
                top_trig_ns = top_trig->GetTimeLeading_ns();
                bot_trig_ns = bot_trig->GetTimeLeading_ns();
                ++n1;
            }
            else
            {
                if (!s_was_trig_missing)
                {
                    LOG(ERROR) << "R3BOnlineSpectraToFI::Exec() : Missing trigger information!";
                    s_was_trig_missing = true;
                }
                ++n2;
            }

            // Shift the cyclic difference window by half a window-length and move it back,
            // this way the trigger time will be at 0.
            auto top_ns =
                fmod(top->GetTimeLeading_ns() - top_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;
            auto bot_ns =
                fmod(bot->GetTimeLeading_ns() - bot_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;
            /*
                        if(top_ns>2000 || bot_ns>2000){
                            std::cout << top->GetTimeLeading_ns() << ' ' << top_trig_ns << ' ' << top_ns << std::endl;
                            std::cout << bot->GetTimeLeading_ns() << ' ' << bot_trig_ns << ' ' << bot_ns << std::endl;
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
            fh_tofi_dt->Fill(top->GetBarId(),dt_mod);
            
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
                    LOG(ERROR) << "R3BTOnlineSpectraToFI::Exec() : more detectors than expected! Det: " << iPlane
                               << " allowed are 1.." << fNofPlanes;
                    continue;
                }
                if (iBar > fPaddlesPerPlane) // same here
                {
                    LOG(ERROR) << "R3BTOnlineSpectraToFI::Exec() : more bars then expected! Det: " << iBar
                               << " allowed are 1.." << fPaddlesPerPlane;
                    continue;
                }

                auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                
                fh_tofi_TotPm[iPlane - 1]->Fill(iBar, bot_tot);
                fh_tofi_TotPm[iPlane - 1]->Fill(-iBar - 1, top_tot);
                
                 
                // std::cout<<"ToT: "<<top_tot << " "<<bot_tot<<"\n";

                // register multi hits
                vmultihits[iPlane][iBar] += 1;
                time_bar[iPlane][iBar]  = (top_ns + bot_ns)/2.;
                                
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
		for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFI; ipl++)
        {    
            for (Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFI; ibr++)
            {
				fh_tofi_multihit[ipl]->Fill(ibr + 1, vmultihits[ipl][ibr]);
			
			}
		} 		   
	
   }// endi if fCalItems 

    fNEvents += 1;
}

void R3BOnlineSpectraToFI_S494::FinishEvent()
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

void R3BOnlineSpectraToFI_S494::FinishTask()
{
    if (fCalItems.at(DET_TOFI))
    {
        for (Int_t i = 0; i < N_PLANE_MAX_TOFI; i++)
        {
            fh_tofi_TotPm[i]->Write();
            fh_tofi_channels[i]->Write();       
            fh_tofi_multihit[i]->Write();       
            }
            fh_tofi_dt->Write();
    }
}

ClassImp(R3BOnlineSpectraToFI_S494)
