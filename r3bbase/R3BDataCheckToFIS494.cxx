// ------------------------------------------------------------
// -----                  R3BDataCheckToFIS494       -----
// -----          Created April 13th 2016 by M.Heil       -----
// -----          Modify 2021 by A.Kelic-Heil             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BDataCheckToFIS494.h"


#include "R3BTofiCalData.h"
#include "R3BTofiHitData.h"
#include "R3BTofiMappedData.h"

#include "R3BOnlineSpectraToFI_S494.h"
#include "R3BOnlineSpectraToFD_S494.h"


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

#include <TGraph.h>

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

#include <TNtuple.h>
#include <TTree.h>
#include "mapping_datacheck_ToFI.hh"

extern unsigned gg_tofi_trig_map[2][24];



//void tofi_trig_map_setup();

#define GRAPHWRITING 0
#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 20; // nanoseconds.
} // namespace
R3BDataCheckToFIS494::R3BDataCheckToFIS494()
    : R3BDataCheckToFIS494("DataCheckS494", 1)
{
}

R3BDataCheckToFIS494::R3BDataCheckToFIS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fCalTriggerItems(NULL)
    , fNofPlanes(N_PLANE_MAX_TOFI)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFI)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BDataCheckToFIS494::~R3BDataCheckToFIS494() {}

InitStatus R3BDataCheckToFIS494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BDataCheckToFIS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    //run->GetHttpServer()->Register("/Tasks", this);

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
        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
    }

    
	ttofi_trig_map_setup();

	

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
       
        // TofI detector

    if (fMappedItems.at(DET_TOFI))
    {
        for (Int_t j = 0; j < N_PLANE_MAX_TOFI; j++)
        {
			char strName1[255]; char strName2[255];
            
            
            sprintf(strName1, "tofi_channels_check plane_%d", j + 1);
            sprintf(strName2, "Tofi channels check plane %d", j + 1);
            
            fh_tofi_check_channels[j] = new TH1F(strName1, strName2, 60, -30., 30.);
            
            fh_tofi_check_channels[j]->GetXaxis()->SetTitle("Channel");
            fh_tofi_check_channels[j]->GetYaxis()->SetTitle("Counts");
            
            sprintf(strName1, "tofi_channels_check plane_%d_sum", j + 1);
            sprintf(strName2, "Tofi channels check plane %d_sum", j + 1);
            
            fh_tofi_check_channels_sum[j] = new TH1F(strName1, strName2, 60, -30., 30.);
            
            fh_tofi_check_channels_sum[j]->GetXaxis()->SetTitle("Channel");
            fh_tofi_check_channels_sum[j]->GetYaxis()->SetTitle("Counts");
            
            sprintf(strName1, "tofi_ToT channels_check plane_%d", j + 1);
            sprintf(strName2, "Tofi ToT_channels check plane_%d", j + 1);
            
            fh_tofi_check_ToT_channels[j] = new TH2F(strName1, strName2, 60, -30., 30., 500, 0, 500);
                        
            fh_tofi_check_ToT_channels[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofi_check_ToT_channels[j]->GetYaxis()->SetTitle("ToT / ns");
            
            sprintf(strName1, "tofi_ToT channels_check plane_%d_sum", j + 1);
            sprintf(strName2, "Tofi ToT_channels check plane_%d_sum", j + 1);
            
            fh_tofi_check_ToT_channels_sum[j] = new TH2F(strName1, strName2, 60, -30., 30., 500, 0, 500);
                        
            fh_tofi_check_ToT_channels_sum[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofi_check_ToT_channels_sum[j]->GetYaxis()->SetTitle("ToT / ns");
			          
        }
    
    }
    
    
    
            for (Int_t j = 0; j < N_PLANE_MAX_TOFI; j++)
        {
			TL_ToT_channels[j] = new TList();
        
			for (int n = 0; n < 60; n++){
				
				TL_ToT_channels[j]->Add(new TGraph());
				
				((TGraph*)TL_ToT_channels[j]->At(n))->SetName(Form("TofI_ToT_event_plane_%d_ch_%d", j+1, n-30) );
				((TGraph*)TL_ToT_channels[j]->At(n))->SetLineColor(0);
				((TGraph*)TL_ToT_channels[j]->At(n))->SetMarkerStyle(1);
				((TGraph*)TL_ToT_channels[j]->At(n))->SetMarkerSize(1);
				((TGraph*)TL_ToT_channels[j]->At(n))->SetMarkerColor(1);
				
				
				
				}
        }





		spillstarts = new TGraph();
		spillstops = new TGraph();

		spillstarts_time = new TGraph();
		spillstops_time = new TGraph();

		spillstarts_eventno = new TGraph();
		spillstops_eventno = new TGraph();


		spillstarts->SetName("spillstarts");
		spillstops->SetName("spillstops");

		spillstarts_time->SetName("spillstarts_time");
		spillstops_time->SetName("spillstops_time");

		spillstarts_eventno->SetName("spillstarts_eventno");
		spillstops_eventno->SetName("spillstops_eventno");
		
		spillstarts->SetLineColor(0); 
		spillstops->SetLineColor(0); 

		spillstarts_time->SetLineColor(0); 
		spillstops_time->SetLineColor(0); 

		spillstarts_eventno->SetLineColor(0); 
		spillstops_eventno->SetLineColor(0); 
    
       
    // -------------------------------------------------------------------------
	printf("Success Init.\n");
    return kSUCCESS;
}

void R3BDataCheckToFIS494::Reset_TOFI_Histo()
{	
	printf("RESET TOFI....\n");
}

namespace
{
    uint64_t n1, n2;
};

void R3BDataCheckToFIS494::Exec(Option_t* option)
{
    //  cout << "fNEvents " << fNEvents << endl;
	//printf("Exec items....\n");
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
        gDirectory->mkdir("TofI");
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
           {
			   spill_number += 1;
			   cout << "Spill Nr. " << spill_number << " start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
			   spill_time = (Double_t) (double(time_spill_start - time_start) / 1.e9);
				
			   
			   spill_eventno = header->GetEventno();
			   spill_time = (Double_t) (double(time_spill_start - time_start) / 1.e9);
			   
			   
			   spillstarts->SetPoint(spillstarts->GetN(), spill_eventno, spill_time);
			   spillstarts_time->SetPoint(spillstarts_time->GetN(), spill_number, spill_time);
			   spillstarts_eventno->SetPoint(spillstarts_eventno->GetN(), spill_number, spill_eventno);


			   if (spill_number < 2){ 
				   
					for (int i=0; i< N_PLANE_MAX_TOFI; i++)
						{ 				
							fh_tofi_check_channels[i]->Reset();
							fh_tofi_check_ToT_channels[i]->Reset();
						}			   
				}
			   
			   }
       if (header->GetTrigger() == 13)
          { cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec" << endl;
			  spill_time = (Double_t) (double(time_spill_end - time_start) / 1.e9);
			  
			  
			  spill_eventno = header->GetEventno();
				spill_time = (Double_t) (double(time_spill_end - time_start) / 1.e9);		   

				spillstops->SetPoint(spillstops->GetN(), spill_eventno, spill_time);
				spillstops_time->SetPoint(spillstops_time->GetN(), spill_number, spill_time);
				spillstops_eventno->SetPoint(spillstops_eventno->GetN(), spill_number, spill_eventno);
			    
			    gDirectory->cd("TofI");
			 	   
						
					for (int i=0; i< N_PLANE_MAX_TOFI; i++)
				     	{ 	fh_tofi_check_channels[i]->SetName(Form("%s_spill_%04d", fh_tofi_check_channels[i]->GetTitle(), spill_number));
							fh_tofi_check_channels[i]->Write();
							
							fh_tofi_check_ToT_channels[i]->SetName(Form("%s_spill_%04d", fh_tofi_check_ToT_channels[i]->GetTitle(), spill_number));
							fh_tofi_check_ToT_channels[i]->Write();
						}	
						
				gDirectory->cd("..");
			  }

    //   check for requested trigger (Todo: should be done globablly / somewhere else)
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
    // TOFI
    //----------------------------------------------------------------------

if (fMappedItems.at(DET_TOFI))
    {
        Int_t nsum_lead = 0;
        Int_t nsum_trail = 0;
        Int_t nsum_top = 0;
        Int_t nsum_bot = 0;
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

            if (iEdge == 1)
                nsum_lead += 1;
            if (iEdge == 2)
                nsum_trail += 1;
            if (iSide == 1 && iEdge == 1 && iPlane == 1)
                nsum_bot += 1;
            if (iSide == 2 && iEdge == 1 && iPlane == 1)
                nsum_top += 1;

            // if (iBarMem != iBar && iSide == 1 && iEdge == 1)    NumPaddles += 1;

            iBarMem = iBar;
            if (iPlane <= fNofPlanes)
            {
                if (iSide == 1 && iEdge == 1 && iPlane == 1) // only leading edges iEdge == 1
                    {fh_tofi_check_channels[iPlane - 1]->Fill(-iBar - 1);
                    fh_tofi_check_channels_sum[iPlane - 1]->Fill(-iBar - 1);}
                
                if (iSide == 2 && iEdge == 1 && iPlane == 1)
                    {fh_tofi_check_channels[iPlane - 1]->Fill(iBar);
                    fh_tofi_check_channels_sum[iPlane - 1]->Fill(iBar);}
            }
            //Int_t maxentry = fh_tofi_channels[0]->GetBinContent(fh_tofi_channels[0]->GetMaximumBin());
        }
        
    }

if (fCalItems.at(DET_TOFI)) // without coincidances top/bottom
    {

        UInt_t vmultihits[N_PLANE_MAX_TOFI][N_PADDLE_MAX_TOFI];
        UInt_t vmultihits_top[N_PLANE_MAX_TOFI][N_PADDLE_MAX_TOFI];
        UInt_t vmultihits_bot[N_PLANE_MAX_TOFI][N_PADDLE_MAX_TOFI];
        Double_t time_bar[N_PLANE_MAX_TOFI][N_PADDLE_MAX_TOFI][32];
        Double_t tot_bar[N_PLANE_MAX_TOFI][N_PADDLE_MAX_TOFI][32];
        auto time0_top = -1;
        auto time0_bot = -1;
        auto time_abs = -1;
        auto time1 = -1;
        for (Int_t i = 0; i < fNofPlanes; i++)
        {
            for (Int_t j = 0; j < N_PADDLE_MAX_TOFI; j++)
            {
                vmultihits[i][j] = 0;
                vmultihits_top[i][j] = 0;
                vmultihits_bot[i][j] = 0;
                for (Int_t k = 0; k < 32; k++)
                {
                    tot_bar[i][j][k] = 0;
                    time_bar[i][j][k] = 0;
                }
            }
        }

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

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {

            auto* hit = (R3BTofiCalData*)det->At(ihit);
            size_t idx = hit->GetDetectorId() * fPaddlesPerPlane * hit->GetBarId();

            auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));
            auto& vec = 1 == hit->GetSideId() ? ret.first->second.bot : ret.first->second.top;
            vec.push_back(hit);

            if (hit->GetDetectorId() == 1 && hit->GetSideId() == 1 && hit->GetTimeLeading_ns() > 0)
                nHitsEvent += 1;
        }

        //     if(NumPaddles > 0)cout <<"nHits cal level: "<<nHitsEvent<<", "<<NumPaddles<<endl;

        // Build trigger map.
        std::vector<R3BTofiCalData*> trig_map;
        for (int i = 0; i < fCalTriggerItems->GetEntries(); ++i)
        {
            auto trig = (R3BTofiCalData*)fCalTriggerItems->At(i);
            if (trig_map.size() < trig->GetBarId())
            {
                trig_map.resize(trig->GetBarId());
            }
            trig_map.at(trig->GetBarId() - 1) = trig;
        }

        // ******************* Without coincidences ****************************************************************
        bool s_was_trig_missing = false;

        for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
        {
            auto const& top_vec = it->second.top;
            auto const& bot_vec = it->second.bot;
            size_t top_i = 0;
            size_t bot_i = 0;

            // TOP PMTS
            for (; top_i < top_vec.size();)
            {
                auto top = top_vec.at(top_i);
                auto top_trig_i = gg_tofi_trig_map[top->GetSideId() - 1][top->GetBarId() - 1];
                Double_t top_trig_ns = 0;
                if (top_trig_i < trig_map.size() && trig_map.at(top_trig_i))
                {
                    auto top_trig = trig_map.at(top_trig_i);
                    top_trig_ns = top_trig->GetTimeLeading_ns();
                }
                else
                {
                    if (!s_was_trig_missing)
                    {
                        LOG(ERROR) << "R3BOnlineSpectraToFI::Exec() : Missing trigger information! TOP";
                        s_was_trig_missing = true;
                    }
                }

                // Shift the cyclic difference window by half a window-length and move it back,
                // this way the trigger time will be at 0.
                auto top_ns = fmod(top->GetTimeLeading_ns() - top_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) -
                              c_range_ns / 2;

                auto top_raw = fmod(top->GetTimeLeading_ns() + c_range_ns, c_range_ns);

                //      cout<<"Time: "<<top_ns<<"; "<<bot_ns<<endl;
                Int_t iPlane = top->GetDetectorId(); // 1..n
                Int_t iBar = top->GetBarId();        // 1..n

                auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);

                //fh_tofi_TotPm[iPlane - 1]->Fill(iBar, top_tot);
                fh_tofi_check_ToT_channels[iPlane - 1]->Fill(iBar, top_tot);
                fh_tofi_check_ToT_channels_sum[iPlane - 1]->Fill(iBar, top_tot);
                
                if(GRAPHWRITING) ((TGraph*)TL_ToT_channels[iPlane - 1]->At( iBar+30 ) )->SetPoint( ((TGraph*)TL_ToT_channels[iPlane - 1]->At(iBar+30) )->GetN(), header->GetEventno(), top_tot);
                
                
                
                //fh_tofi_timePm[iPlane - 1]->Fill(iBar, top_ns);
                //if (nHitsEvent == 1)                     fh_test->Fill(iBar, top_tot);
                // register multi hits
                vmultihits_top[iPlane - 1][iBar - 1] += 1;

                ++top_i;
            }

            time_abs = -1;
            time1 = -1;

            // BOTTOM PMTS
            for (; bot_i < bot_vec.size();)
            {
                auto bot = bot_vec.at(bot_i);
                auto bot_trig_i = gg_tofi_trig_map[bot->GetSideId() - 1][bot->GetBarId() - 1];
                Double_t bot_trig_ns = 0;
                if (bot_trig_i < trig_map.size() && trig_map.at(bot_trig_i))
                {
                    auto bot_trig = trig_map.at(bot_trig_i);
                    bot_trig_ns = bot_trig->GetTimeLeading_ns();
                }
                else
                {
                    if (!s_was_trig_missing)
                    {
                        LOG(ERROR) << "R3BOnlineSpectraToFI::Exec() : Missing trigger information! BOT";
                        s_was_trig_missing = true;
                    }
                }

                // Shift the cyclic difference window by half a window-length and move it back,
                // this way the trigger time will be at 0.
                auto bot_ns = fmod(bot->GetTimeLeading_ns() - bot_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) -
                              c_range_ns / 2;

                auto bot_raw = fmod(bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);

                //      cout<<"Time: "<<top_ns<<"; "<<bot_ns<<endl;
                Int_t iPlane = bot->GetDetectorId(); // 1..n
                Int_t iBar = bot->GetBarId();        // 1..n
                // auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                auto bot_tot = bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns();

				fh_tofi_check_ToT_channels[iPlane - 1]->Fill(-iBar - 1, bot_tot);
                fh_tofi_check_ToT_channels_sum[iPlane - 1]->Fill(-iBar - 1, bot_tot);
                
                if(GRAPHWRITING) ((TGraph*)TL_ToT_channels[iPlane - 1]->At( -iBar-1+30 ) )->SetPoint( ((TGraph*)TL_ToT_channels[iPlane - 1]->At(-iBar-1+30) )->GetN(), header->GetEventno(), bot_tot);
                

                //fh_tofi_TotPm[iPlane - 1]->Fill(-iBar - 1, bot_tot);
                //fh_tofi_timePm[iPlane - 1]->Fill(-iBar - 1, bot_ns);
                //if (nHitsEvent == 1)  fh_test->Fill(-iBar - 1, bot_tot);
                // register multi hits
                vmultihits_bot[iPlane - 1][iBar - 1] += 1;

                ++bot_i;
            }
        }
        
        

    } // endi if fCalItems



    fNEvents += 1;

}

void R3BDataCheckToFIS494::FinishEvent()
{
	//printf("Finish event....\n");
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
}

void R3BDataCheckToFIS494::FinishTask()
{	printf("finish task....\n");
      
      
    gDirectory->cd("TofI");  
      
      
	spillstarts->Write();
	spillstops->Write();

	spillstarts_time->Write();
	spillstops_time->Write();

	spillstarts_eventno->Write();
	spillstops_eventno->Write();

	for (int i=0; i< N_PLANE_MAX_TOFI; i++)
    { 
		fh_tofi_check_channels_sum[i]->Write();
		fh_tofi_check_ToT_channels_sum[i]->Write();
	
	}


	gDirectory->mkdir("ToT_PMTs");
	gDirectory->cd("ToT_PMTs");
	for (int i=0; i< N_PLANE_MAX_TOFI; i++)
    { 
		TL_ToT_channels[i]->Write(); //("TL_ToT_channels", TObject::kSingleKey);
}

	gDirectory->cd("..");
	gDirectory->cd("..");
    
}

ClassImp(R3BDataCheckToFIS494)
