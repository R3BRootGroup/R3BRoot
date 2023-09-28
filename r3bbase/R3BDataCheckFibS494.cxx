// ------------------------------------------------------------
// -----                  R3BDataCheckFibS494       -----
// -----          Created April 13th 2016 by M.Heil       -----
// -----          Modify 2021 by A.Kelic-Heil             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BDataCheckFibS494.h"
#include "R3BEventHeader.h"
#include "R3BSamplerMappedData.h"
#include "R3BTCalEngine.h"


#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

/*
#include "mapping_fib23a_trig_hit.hh"
#include "mapping_fib23b_trig_hit.hh"
#include "mapping_fib30_trig.hh"
#include "mapping_fib31_trig.hh"
#include "mapping_fib32_trig.hh"
#include "mapping_fib33_trig.hh"
*/
#include "mapping_datacheck_fib.hh"

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
#include <list>

#include <TGraph.h>
#include <TTree.h>


extern unsigned gg_fib23a_trigg_map_hit[2][384];
extern unsigned gg_fib23b_trigg_map_hit[2][384];

extern unsigned gg_fib30_trigg_map[2][512];
extern unsigned gg_fib31_trigg_map[2][512];
extern unsigned gg_fib32_trigg_map[2][512];
extern unsigned gg_fib33_trigg_map[2][512];


#define GRAPHWRITING 0
#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;
namespace
{
    double c_period = 4096 * 1000. / 150;
    double c_fiber_coincidence_ns = 20; // nanoseconds.
    double c_tot_coincidence_ns = 100;  // nanoseconds
} // namespace

R3BDataCheckFibS494::R3BDataCheckFibS494()
    : R3BDataCheckFibS494("DataCheckS494", 1)
{
}

R3BDataCheckFibS494::R3BDataCheckFibS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
    , fChannelArray()
{
}

R3BDataCheckFibS494::~R3BDataCheckFibS494() {}

InitStatus R3BDataCheckFibS494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BDataCheckFibS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    //run->GetHttpServer()->Register("/Tasks", this);

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
                gfib30_trigg_map_setup();
                fTriggerMap[0] = gg_fib30_trigg_map[0];
                fTriggerMap[1] = gg_fib30_trigg_map[1];
            }
            if ((strcmp(name, "Fi31") == 0))
            {
                gfib31_trigg_map_setup();
                fTriggerMap[0] = gg_fib31_trigg_map[0];
                fTriggerMap[1] = gg_fib31_trigg_map[1];
            }

            if ((strcmp(name, "Fi32") == 0))
            {
                gfib32_trigg_map_setup();
                fTriggerMap[0] = gg_fib32_trigg_map[0];
                fTriggerMap[1] = gg_fib32_trigg_map[1];
            }
            if ((strcmp(name, "Fi33") == 0))
            {
                gfib33_trigg_map_setup();
                fTriggerMap[0] = gg_fib33_trigg_map[0];
                fTriggerMap[1] = gg_fib33_trigg_map[1];
            }
            if ((strcmp(name, "Fi23a") == 0))
            {
                gfib23a_trigg_map_hit_setup();
                fTriggerMap[0] = gg_fib23a_trigg_map_hit[0];
                fTriggerMap[1] = gg_fib23a_trigg_map_hit[1];
            }
            if ((strcmp(name, "Fi23b") == 0))
            {
                gfib23b_trigg_map_hit_setup();
                fTriggerMap[0] = gg_fib23b_trigg_map_hit[0];
                fTriggerMap[1] = gg_fib23b_trigg_map_hit[1];
            }
        }
    }




char canvName[255];
   // TCanvas* FibCanvas[NOF_FIB_DET];

	/*gROOT->cd()
	gDirectory->mkdir("Fib");
	gDirectory->mkdir("Fibsum");
	*/



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

            //FibCanvas[ifibcount] = new TCanvas(detName, detName, 10, 10, 910, 910);
            // Cal level


            // Channels:
            fh_check_channels_Fib[ifibcount] = new TH1F(Form("%s_Check_Mapped_channels_up", detName), Form("%sMapped channels up", detName), 520, 0., 520.);
            fh_check_channels_Fib[ifibcount]->GetXaxis()->SetTitle("PMT channel number");
            fh_check_channels_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Channels:
            fh_check_channels_single_Fib[ifibcount] = new TH1F(Form("%s_Check_Mapped_channels_down", detName), Form("%sMapped channels down", detName), 520, 0., 520.);
            fh_check_channels_single_Fib[ifibcount]->GetXaxis()->SetTitle("PMT channel number");
            fh_check_channels_single_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");


            // Cal level
            // Channels:
            fh_check_channels_Fib_sum[ifibcount] = new TH1F(Form("%s_Check_Mapped_channels_up_sum", detName), Form("%sMapped channels up", detName), 520, 0., 520.);
            fh_check_channels_Fib_sum[ifibcount]->GetXaxis()->SetTitle("PMT channel number");
            fh_check_channels_Fib_sum[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Channels:
            fh_check_channels_single_Fib_sum[ifibcount] = new TH1F(Form("%s_Check_Mapped_channels_down_sum", detName), Form("%sMapped channels down", detName), 520, 0., 520.);
            fh_check_channels_single_Fib_sum[ifibcount]->GetXaxis()->SetTitle("PMT channel number");
            fh_check_channels_single_Fib_sum[ifibcount]->GetYaxis()->SetTitle("Counts");



			//fh_check_ToT_Fib[ifibcount] =new TH2F(Form("%sCal__checktotraw_up", detName), Form("%sCal ToT raw up", detName), N_FIBER_PLOT_S494,   0., N_FIBER_PLOT_S494,  200,      0.,   100.);

			//
			// ToT Spectra:
			//


			// ToT SAPMT:
            fh_check_raw_tot_down[ifibcount] = new TH2F(Form("%scheck_Cal_totraw_down", detName), Form("%scheck_Cal ToT raw down", detName), N_FIBER_PLOT_S494, 0.,N_FIBER_PLOT_S494, 200, 0., 100.);
            fh_check_raw_tot_down[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_check_raw_tot_down[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

			// ToT raw up:
            fh_check_raw_tot_up[ifibcount] = new TH2F(Form("%scheck_Cal_totraw_up", detName), Form("%scheck_Cal ToT raw up", detName), N_FIBER_PLOT_S494, 0., N_FIBER_PLOT_S494, 200, 0., 100.);
            fh_check_raw_tot_up[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_check_raw_tot_up[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

			// ToT SAPMT sum:
            fh_check_raw_tot_down_sum[ifibcount] = new TH2F(Form("%scheck_Cal_totraw_down_sum", detName), Form("%scheck_Cal ToT raw down_sum", detName), N_FIBER_PLOT_S494, 0.,N_FIBER_PLOT_S494, 200, 0., 100.);
            fh_check_raw_tot_down_sum[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_check_raw_tot_down_sum[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

			// ToT raw up sum:
            fh_check_raw_tot_up_sum[ifibcount] = new TH2F(Form("%scheck_Cal_totraw_up_sum", detName), Form("%scheck_Cal ToT raw up_sum", detName), N_FIBER_PLOT_S494, 0., N_FIBER_PLOT_S494, 200, 0., 100.);
            fh_check_raw_tot_up_sum[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_check_raw_tot_up_sum[ifibcount]->GetYaxis()->SetTitle("ToT / ns");






/*
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
            fh_raw_tot_up[ifibcount] = new TH2F(Form("%sCal_totraw_up", detName), Form("%sCal ToT raw up", detName), N_FIBER_PLOT_S494,   0.,         N_FIBER_PLOT_S494,                       200,
                                                0.,
                                                100.);
            fh_raw_tot_up[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_raw_tot_up[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToT SAPMT:
            fh_raw_tot_down[ifibcount] = new TH2F(Form("%sCal_totraw_down", detName),
                                                  Form("%sCal ToT raw down", detName),
                                                  N_FIBER_PLOT_S494,
                                                  0.,
                                                  N_FIBER_PLOT_S494,
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
                                                 2 * N_FIBER_PLOT_S494,
                                                 -N_FIBER_PLOT_S494,
                                                 N_FIBER_PLOT_S494,
                                                 1700,
                                                 -1500.,
                                                 200.);
            fh_chan_dt_cal[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_chan_dt_cal[ifibcount]->GetYaxis()->SetTitle("time(up/down)-time_trigger / ns");

            // Hit level
            // Fibers:
            fh_fibers_Fib[ifibcount] = new TH1F(
                Form("%sHit_fibers", detName), Form("%sHit fibers", detName), N_FIBER_PLOT_S494, 0., N_FIBER_PLOT_S494);
            fh_fibers_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_fibers_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multiplicity (number of hit fibers):
            fh_mult_Fib[ifibcount] = new TH1F(Form("%sHit_mult", detName), Form("%sHit mult", detName), 520, 0., 520.);
            fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // ToT :
            fh_ToT_Fib[ifibcount] = new TH2F(Form("%sHit_ToT_iFib", detName),
                                             Form("%sHit ToT vs Fib", detName),
                                             N_FIBER_PLOT_S494,
                                             0.,
                                             N_FIBER_PLOT_S494,
                                             200,
                                             0.,
                                             100.);
            fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // Time of fiber:
            fh_time_Fib[ifibcount] = new TH2F(Form("%sHit_TimevsFiber", detName),
                                              Form("%sHit Time vs Fiber", detName),
                                              N_FIBER_PLOT_S494,
                                              0.,
                                              N_FIBER_PLOT_S494,
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
                                                   N_FIBER_PLOT_S494,
                                                   0.,
                                                   N_FIBER_PLOT_S494);
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

           */
        } // end if(Cal)

    } // end for(ifibcount)



	for (Int_t j = 0; j < NOF_FIB_DET; j++)
        {
			TL_ToT_channels_up[j] = new TList();
			const char* detName;
			detName = fDetectorNames[DET_FI_FIRST + j];
			for (int n = 0; n < 520; n++){

				TL_ToT_channels_up[j]->Add(new TGraph());

				((TGraph*)TL_ToT_channels_up[j]->At(n))->SetName(Form("%s_up_ToT_event_plane_%d_ch_%d", detName, j+1, n) );
				((TGraph*)TL_ToT_channels_up[j]->At(n))->SetLineColor(0);
				((TGraph*)TL_ToT_channels_up[j]->At(n))->SetMarkerStyle(1);
				((TGraph*)TL_ToT_channels_up[j]->At(n))->SetMarkerSize(1);
				((TGraph*)TL_ToT_channels_up[j]->At(n))->SetMarkerColor(1);

				}
        }

	for (Int_t j = 0; j < NOF_FIB_DET; j++)
        {
			TL_ToT_channels_down[j] = new TList();
			const char* detName;
			detName = fDetectorNames[DET_FI_FIRST + j];
			for (int n = 0; n < 520; n++){

				TL_ToT_channels_down[j]->Add(new TGraph());

				((TGraph*)TL_ToT_channels_down[j]->At(n))->SetName(Form("%s_down_ToT_event_plane_%d_ch_%d", detName, j+1, n) );
				((TGraph*)TL_ToT_channels_down[j]->At(n))->SetLineColor(0);
				((TGraph*)TL_ToT_channels_down[j]->At(n))->SetMarkerStyle(1);
				((TGraph*)TL_ToT_channels_down[j]->At(n))->SetMarkerSize(1);
				((TGraph*)TL_ToT_channels_down[j]->At(n))->SetMarkerColor(1);

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

namespace
{
    uint64_t n1, n2;
};

void R3BDataCheckFibS494::Exec(Option_t* option)
{
    //  cout << "fNEvents " << fNEvents << endl;
	//printf("Exec items....\n");
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(error) << "FairRootManager not found";
        return;
    }

    time = header->GetTimeStamp();
//printf("Fib_Pos1....\n");
    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
        gDirectory->mkdir("Fib");
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
			   cout << "FibSpill Nr. " << spill_number << " start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
			   spill_time = (Double_t) (double(time_spill_start - time_start) / 1.e9);

			   spill_eventno = header->GetEventno();
			   spill_time = (Double_t) (double(time_spill_start - time_start) / 1.e9);


			   spillstarts->SetPoint(spillstarts->GetN(), spill_eventno, spill_time);
			   spillstarts_time->SetPoint(spillstarts_time->GetN(), spill_number, spill_time);
			   spillstarts_eventno->SetPoint(spillstarts_eventno->GetN(), spill_number, spill_eventno);



			   if (spill_number < 2){
				   for (int i=0; i< NOF_FIB_DET; i++)
						{
							fh_check_channels_single_Fib[i]->Reset();
							fh_check_channels_Fib[i]->Reset();

							fh_check_raw_tot_down[i]->Reset();
							fh_check_raw_tot_up[i]->Reset();

							//fh_tofd_check_ToT_channels[i]->Reset();

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





				gDirectory->cd("Fib");
			 	   for (int i=0; i< NOF_FIB_DET; i++)
				     	{


							fh_check_channels_Fib[i]->SetName(Form("%s_spill_%04d", fh_check_channels_Fib[i]->GetTitle(), spill_number));
							fh_check_channels_Fib[i]->Write();

							fh_check_channels_single_Fib[i]->SetName(Form("%s_spill_%04d", fh_check_channels_single_Fib[i]->GetTitle(), spill_number));
							fh_check_channels_single_Fib[i]->Write();

							fh_check_raw_tot_down[i]->SetName(Form("%s_spill_%04d", fh_check_raw_tot_down[i]->GetTitle(), spill_number));
							fh_check_raw_tot_down[i]->Write();


							fh_check_raw_tot_up[i]->SetName(Form("%s_spill_%04d", fh_check_raw_tot_up[i]->GetTitle(), spill_number));
							fh_check_raw_tot_up[i]->Write();


							//fh_tofd_check_ToT_channels[i]->SetName(Form("%s_spill_%04d", fh_tofd_check_ToT_channels[i]->GetTitle(), spill_number));
							//fh_tofd_check_ToT_channels[i]->Write();
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


//printf("Fib_Pos2....\n");
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
                        fh_check_channels_Fib[ifibcount]->Fill(ch_i); // Fill which channel has events
                        fh_check_channels_Fib_sum[ifibcount]->Fill(ch_i); // Fill which channel has events
                    }

                    if (side_i == 0)
                    {
                        fh_check_channels_single_Fib[ifibcount]->Fill(ch_i); // Fill which channel has events
                        fh_check_channels_single_Fib_sum[ifibcount]->Fill(ch_i); // Fill which channel has events
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

/*
                    if (side_i == 0)
                        fh_chan_dt_cal[ifibcount]->Fill(-ch_i - 1, time_ns);
                    if (side_i == 1)
                        fh_chan_dt_cal[ifibcount]->Fill(ch_i + 1, time_ns);
                        * */

                    auto& channel = fChannelArray[side_i].at(ch_i);

                    channel.lead_list.push_back(cur_cal_lead); // make list with leading times
                }
            }

            for (int i = 0; i < fc; ++i)
            {

               /* if (vmultihits_top[i] > 0)
                    fh_multihit_m_Fib[ifibcount]->Fill(i + 1, vmultihits_top[i]); // multihit of a given up killom channel

                if (vmultihits_bot[i] > 0)
                    fh_multihit_s_Fib[ifibcount]->Fill(i + 1, vmultihits_bot[i]); // multihit of a given down killom channel
            */
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
                        {	//fh_check_ToT_Fib[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);
                            fh_check_raw_tot_up[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);
                            fh_check_raw_tot_up_sum[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);

                            if(GRAPHWRITING) ((TGraph*)TL_ToT_channels_up[ifibcount]->At( cur_cal_trail->GetChannel()) )->SetPoint( ((TGraph*)TL_ToT_channels_up[ifibcount]->At(cur_cal_trail->GetChannel()) )->GetN(), header->GetEventno(), tot_ns);
                         }
                        if (cur_cal_trail->GetSide() == 0)
						{	//fh_check_ToT_Fib[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);
                            fh_check_raw_tot_down[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);
                            fh_check_raw_tot_down_sum[ifibcount]->Fill(cur_cal_trail->GetChannel(), tot_ns);

                            if(GRAPHWRITING) ((TGraph*) TL_ToT_channels_down[ifibcount]->At( cur_cal_trail->GetChannel() ))->SetPoint( ((TGraph*)TL_ToT_channels_down[ifibcount]->At(cur_cal_trail->GetChannel()) )->GetN(), header->GetEventno(), tot_ns);
						}
                    }
                }
            }

            /*for (size_t j = 0; j < nCals; ++j)
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
                            { //printf("Fib_Pos4....\n");;
                                // von TH auskommentiert, gucken ob noch noetig...
                                //fh_chan_corell[ifibcount]->Fill(cur_cal_bot->GetChannel(), cur_cal_top->GetChannel());
							}
                        }
                    }
                }
            }  */

            fChannelArray[0].clear();
            fChannelArray[1].clear();
        } // if Cal


    }     // end for(ifibcount)



    fNEvents += 1;

}

void R3BDataCheckFibS494::FinishEvent()
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

void R3BDataCheckFibS494::FinishTask()
{	printf("finish task....\n");

    gDirectory->cd("Fib");

    spillstarts->Write();
	spillstops->Write();

	spillstarts_time->Write();
	spillstops_time->Write();

	spillstarts_eventno->Write();
	spillstops_eventno->Write();


    for (int i=0; i< NOF_FIB_DET; i++)
    {
		fh_check_channels_Fib_sum[i]->Write();
		fh_check_channels_single_Fib_sum[i]->Write();

		fh_check_raw_tot_down_sum[i]->Write();
		fh_check_raw_tot_up_sum[i]->Write();
	}


for (int i=0; i< NOF_FIB_DET; i++)
    {
		fh_check_channels_Fib_sum[i]->Print();
		fh_check_channels_single_Fib_sum[i]->Print();

		fh_check_raw_tot_down_sum[i]->Print();
		fh_check_raw_tot_up_sum[i]->Print();
	}


	gDirectory->mkdir("ToT_PMTs");
	gDirectory->cd("ToT_PMTs");

	for (int i=0; i< NOF_FIB_DET; i++)
    {
		TL_ToT_channels_up[i]->Write();
		TL_ToT_channels_down[i]->Write();
	}

	gDirectory->cd("..");
	gDirectory->cd("..");


}

ClassImp(R3BDataCheckFibS494)
