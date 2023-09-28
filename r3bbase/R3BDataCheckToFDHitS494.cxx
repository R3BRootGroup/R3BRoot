// ------------------------------------------------------------
// -----                  R3BDataCheckToFDHitS494       -----
// -----          Created April 13th 2016 by M.Heil       -----
// -----          Modify 2021 by A.Kelic-Heil             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BDataCheckToFDHitS494.h"

#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"


#include "R3BOnlineSpectraToFD_S494.h"


/*
#include "mapping_fib23a_trig_hit.hh"
#include "mapping_fib23b_trig_hit.hh"
#include "mapping_fib30_trig.hh"
#include "mapping_fib31_trig.hh"
#include "mapping_fib32_trig.hh"
#include "mapping_fib33_trig.hh"
*/

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
#include <TGraph.h>
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
//#include "mapping_datacheck_ToFD.hh"



//extern unsigned gg_tofd_trig_map[4][2][44];


#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 15; // nanoseconds.
} // namespace

R3BDataCheckToFDHitS494::R3BDataCheckToFDHitS494()
    : R3BDataCheckToFDHitS494("DataCheckS494", 1)
{
}

R3BDataCheckToFDHitS494::R3BDataCheckToFDHitS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fCalTriggerItems(NULL)
    , fNofPlanes(N_PLANE_MAX_TOFD_S494)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFD_S494)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BDataCheckToFDHitS494::~R3BDataCheckToFDHitS494() {}

InitStatus R3BDataCheckToFDHitS494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BDataCheckToFDHitS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    //run->GetHttpServer()->Register("/Tasks", this);

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
        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
    }

   // ttofd_trig_map_setup();


    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
    
   
     if (fHitItems.at(DET_TOFD))
    {	printf("ToFD hit items. found.\n");
        
        for (Int_t j = 0; j < 4; j++)
        {
            char strName1[255]; char strName2[255];
            
            
            sprintf(strName1, "tofd_y_poscheck_plane_%d", j + 1);
            sprintf(strName2, "Tofd_y_poscheck_plane %d", j + 1);
            
            fh_yposcheck_channels[j] = new TH2F(strName1, strName2, 90, -45., 45., 200, -300, 300);
            
            sprintf(strName1, "tofd_y_poscheck_sum_plane_%d", j + 1);
            sprintf(strName2, "Tofd_y_poscheck_sum_plane %d", j + 1);
            
            fh_yposcheck_channels_sum[j] = new TH2F(strName1, strName2, 90, -45., 45., 200, -300, 300);
         }
        
        /*
        for (Int_t j = 0; j < 4; j++)
        {
			TL_ypos_channels[j] = new TList();
        
			for (int n = 0; n < 90; n++){
				
				TL_ypos_channels[j]->Add(new TGraph());
				
				((TGraph*)TL_ypos_channels[j]->At(n))->SetName(Form("TofD_ToT_event_plane_%d_ch_%d", j+1, n-45) );
				((TGraph*)TL_ypos_channels[j]->At(n))->SetLineColor(0);
				((TGraph*)TL_ypos_channels[j]->At(n))->SetMarkerStyle(1);
				((TGraph*)TL_ypos_channels[j]->At(n))->SetMarkerSize(1);
				((TGraph*)TL_ypos_channels[j]->At(n))->SetMarkerColor(1);
				
				
				
				}
        }
*/

        
        
        
     }
    
     
     
       
    // -------------------------------------------------------------------------
	printf("Success Init.\n");
    return kSUCCESS;
}

void R3BDataCheckToFDHitS494::Reset_TOFD_Histo()
{	
	printf("RESET TOFD....\n");
}

namespace
{
    uint64_t n1, n2;
};

void R3BDataCheckToFDHitS494::Exec(Option_t* option)
{
    //  cout << "fNEvents " << fNEvents << endl;
	printf("Exec items....\n");
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(ERROR) << "FairRootManager not found";
        return;
    }
	printf("Do it.\n");
    
    /*time = header->GetTimeStamp();

    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
        gDirectory->mkdir("TofD");
        		
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
			   cout << "Spill Nr. " << spill_number << " start: " << double(time_spill_start - time_start) / 1.e9 << " sec at event no " <<  header->GetEventno() << endl;
			   
			   spill_eventno = header->GetEventno();
			   spill_time = (Double_t) (double(time_spill_start - time_start) / 1.e9);
			   
			   
			   spillstarts->SetPoint(spillstarts->GetN(), spill_eventno, spill_time);
			   spillstarts_time->SetPoint(spillstarts_time->GetN(), spill_number, spill_time);
			   spillstarts_eventno->SetPoint(spillstarts_eventno->GetN(), spill_number, spill_eventno);
			   
			   
			 			   
			   
			   //ntuple_spillstart->Fill(spill_time);
			   if (spill_number < 2){ 
				   for (int i=0; i< 4; i++)
						{ 
							fh_tofd_check_channels[i]->Reset();
							fh_tofd_check_ToT_channels[i]->Reset();
							fh_yposcheck_channels[i]->Reset();
												
						}
						   
				}
			   
			   }
       if (header->GetTrigger() == 13)
          { cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec at event no " <<  header->GetEventno() << endl;
			  
			 	spill_eventno = header->GetEventno();
				spill_time = (Double_t) (double(time_spill_end - time_start) / 1.e9);		   

				spillstops->SetPoint(spillstops->GetN(), spill_eventno, spill_time);
				spillstops_time->SetPoint(spillstops_time->GetN(), spill_number, spill_time);
				spillstops_eventno->SetPoint(spillstops_eventno->GetN(), spill_number, spill_eventno);
			  		  
			  
			 	   for (int i=0; i< N_PLANE_MAX_TOFD_S494; i++)
				     	{ 	
							gDirectory->cd("TofD");
							fh_tofd_check_channels[i]->SetName(Form("%s_spill_%04d", fh_tofd_check_channels[i]->GetTitle(), spill_number));
							fh_tofd_check_channels[i]->Write();
							
							fh_tofd_check_ToT_channels[i]->SetName(Form("%s_spill_%04d", fh_tofd_check_ToT_channels[i]->GetTitle(), spill_number));
							fh_tofd_check_ToT_channels[i]->Write();
							
							fh_yposcheck_channels[i]->SetName(Form("%s_spill_%04d", fh_yposcheck_channels[i]->GetTitle(), spill_number));
							fh_yposcheck_channels[i]->Write();
							
							gDirectory->cd("..");
						}	
						
					
			  }
			  * 
			  * */
printf("Do it2.\n");
    //   check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;
printf("Do it3.\n");
    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit1 = fTpat1 - 1;
    Int_t fTpat_bit2 = fTpat2 - 1;
    Int_t tpatbin;
    /*for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
        {
            return;
        }
    }*/
  printf("Do it4.\n");
    //----------------------------------------------------------------------
    // TOFD
    //----------------------------------------------------------------------

    //Int_t NumPaddles[4] = { 0 };
    
    
    
    if (fHitItems.at(DET_TOFD))
    {   
		printf("Do it5.\n");
        auto detTofd = fHitItems.at(DET_TOFD);
        Int_t nHits = detTofd->GetEntriesFast();

        if (nHits > 100)
            return;

        Double_t x[N_PLANE_MAX_TOFD_S494][10], 
        y[N_PLANE_MAX_TOFD_S494][10], 
        t[N_PLANE_MAX_TOFD_S494][10],
        q[N_PLANE_MAX_TOFD_S494][10],
        bar[N_PLANE_MAX_TOFD_S494][10];
        
        for (Int_t i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
        {
            for (Int_t k = 0; k < 10; k++)
            {
                x[i][k] = -1000.;
                y[i][k] = -1000.;
                q[i][k] = -1000.;
                t[i][k] = 0. / 0.;
                bar[i][k] = 0;
            }
        }
        Int_t nMulti[N_PLANE_MAX_TOFD_S494] = { 0 }, iCounts[N_PLANE_MAX_TOFD_S494] = { 0 };
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BTofdHitData* hitTofd = (R3BTofdHitData*)detTofd->At(ihit);

            if (IS_NAN(hitTofd->GetTime()))
                continue;

            Int_t iPlane = hitTofd->GetDetId();
            Double_t randx = (std::rand() / (float)RAND_MAX) - 0.5;
            Int_t ictemp = iCounts[iPlane - 1];
            x[iPlane - 1][ictemp] = hitTofd->GetX() + 2.7 * randx;
            y[iPlane - 1][ictemp] = hitTofd->GetY();
            t[iPlane - 1][ictemp] = hitTofd->GetTime();
            q[iPlane - 1][ictemp] = hitTofd->GetEloss();
            bar[iPlane - 1][ictemp] = hitTofd->GetBarId();
            
            fh_yposcheck_channels[iPlane - 1]->Fill(hitTofd->GetBarId(), hitTofd->GetY());
            fh_yposcheck_channels_sum[iPlane - 1]->Fill(hitTofd->GetBarId(), hitTofd->GetY());
            
            /*fh_tofd_Tot_hit[iPlane - 1]->Fill(bar[iPlane - 1][ictemp], q[iPlane - 1][ictemp]);
            fh_tofd_time_hit[iPlane - 1]->Fill(bar[iPlane - 1][ictemp], t[iPlane - 1][ictemp]);
            fh_tofd_bars[iPlane - 1]->Fill(bar[iPlane - 1][ictemp]);
            iCounts[iPlane - 1] += 1;
            nMulti[iPlane - 1] += 1;*/
        }

        
    }

    fNEvents += 1;
fh_yposcheck_channels_sum[0]->Print();
}

void R3BDataCheckToFDHitS494::FinishEvent()
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


void R3BDataCheckToFDHitS494::FinishTask()
{	printf("finish task....\n");
      
   /* gDirectory->cd();
    tree_spillstart->Write();
	tree_spillstart->Show();
	tree_spillstart->Print();*/
      
      
    //gDirectory->cd("TofD");  
    
    
    
    
    
    for (int i=0; i< N_PLANE_MAX_TOFD_S494; i++)
    { 
		// Write infos for TofD
		//fh_tofd_check_channels[i]->Write();
		//fh_tofd_check_ToT_channels[i]->Write();

		//fh_tofd_check_channels_sum[i]->Write();
		//fh_tofd_check_ToT_channels_sum[i]->Write();
		
		fh_yposcheck_channels[i]->Write();
		fh_yposcheck_channels_sum[i]->Write();
		
		
		// some other things, can be deleted
		//fh_tofd_check_channels[i]->Print();
		
		fh_yposcheck_channels_sum[i]->Print();
		fh_yposcheck_channels_sum[i]->Draw("COLZ");
		//check_channels(fh_tofd_check_channels[i]);
		
	}
		
	
	
	/*gDirectory->mkdir("ToT_Time");
	gDirectory->cd("ToT_Time");
	for (int i=0; i< N_PLANE_MAX_TOFD_S494; i++)
    { 
		TL_ToTTime_channels[i]->Write(); //("TL_ToT_channels", TObject::kSingleKey);
} */

	 
	//ntuple_spillstop->Write();
    
}

ClassImp(R3BDataCheckToFDHitS494)
