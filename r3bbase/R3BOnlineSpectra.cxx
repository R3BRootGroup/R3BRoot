// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow  
 * to test the detectors online
 *  
 */


#include "R3BOnlineSpectra.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BPaddleCalData.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxCalData.h"
#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"
#include "R3BTCalEngine.h"

#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberMappedData.h"

#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

#include "TClonesArray.h"
#include <iostream>
#include <sstream>
using namespace std;


R3BOnlineSpectra::R3BOnlineSpectra()
    : FairTask("OnlineSpectra", 1)
    , fCalItemsLos(NULL)
    , fCalItemsTofd(NULL)
    , fCalItemsPspx(NULL)
    , fHitItemsFi1(NULL)
    , fHitItemsFi5(NULL)
    , fHitItemsFi6(NULL)
    , fCalItemsPtof(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsTofd(NULL)
    , fMappedItemsPspx(NULL)
    , fMappedItemsFi1(NULL)
    , fMappedItemsFi5(NULL)
    , fMappedItemsFi6(NULL)
    , fTrigger(-1)
    , fNofPlanes(4)  
    , fPaddlesPerPlane(6)    
    , flosVeffX(14.)
    , flosVeffY(14.)    
    , flosOffsetX(0.)
    , flosOffsetY(0.)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectra::R3BOnlineSpectra(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fCalItemsTofd(NULL)
    , fCalItemsPspx(NULL)
    , fHitItemsFi1(NULL)
    , fHitItemsFi5(NULL)
    , fHitItemsFi6(NULL)
    , fCalItemsPtof(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsTofd(NULL)
    , fMappedItemsPspx(NULL)
    , fMappedItemsFi1(NULL)
    , fMappedItemsFi5(NULL)
    , fMappedItemsFi6(NULL)
    , fTrigger(-1)
    , fNofPlanes(4)  
    , fPaddlesPerPlane(6)    
    , flosVeffX(14.)
    , flosVeffY(14.)    
    , flosOffsetX(0.)
    , flosOffsetY(0.)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectra::~R3BOnlineSpectra()
{
}

InitStatus R3BOnlineSpectra::Init()
{

    LOG(INFO) << "R3BOnlineSpectra::Init " << FairLogger::endl;
    


	// try to get a handle on the EventHeader. EventHeader may not be 
	// present though and hence may be null. Take care when using.
	
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline *run = FairRunOnline::Instance();

// Los detector
	// get access to Mapped data
    fMappedItemsLos = (TClonesArray*)mgr->GetObject("LosMapped");
	// get access to cal data
    fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");
    
    if(fCalItemsLos || fMappedItemsLos){
       TCanvas *cLos = new TCanvas("Los", "LOS", 10, 10, 500, 500);
       cLos->Divide(2, 2);
 
       fh_los_channels = new TH1F("los_channels", "LOS channels", 6, 0., 6.); 
       fh_los_tres = new TH1F("los_time_res", "LOS Time resolution", 4000, -2., 2.);
       fh_los_pos = new TH2F("los_position", "LOS xy position", 1000, -5., 5.,1000, -5., 5.);
       fh_los_pos->GetXaxis()->SetTitle("X position / cm");
       fh_los_pos->GetYaxis()->SetTitle("Y position / cm");  
        
       cLos->cd(1);
       fh_los_channels->Draw();
       cLos->cd(2);
       fh_los_tres->Draw();
       cLos->cd(3);
       fh_los_pos->Draw();
       cLos->cd(0);
       run->AddObject(cLos);
    }

//-----------------------------------------------------------------------
// Fiber 1 detector

	// get access to Mapped data
    fMappedItemsFi1 = (TClonesArray*)mgr->GetObject("Fi1Mapped");

	// get access to cal data
    fHitItemsFi1 = (TClonesArray*)mgr->GetObject("Fi1Hit");
    if(fMappedItemsFi1){ 
		fh_Fi1_channels = new TH1F("fiber1_channels", "Fiber1 channels", 300, 0., 300.);
		fh_Fi1_fibers = new TH1F("fiber1_number", "Fiber1 number", 300, 0., 300.);    
		fh_Fi1_multihit = new TH2F("fiber1_multihit", "Fiber1 multi hits", 300, 0., 300., 20, 0., 20.);
		fh_Fi1_ToT = new TH2F("fiber1_ToT", "Fiber1 ToT", 300, 0., 300., 400, 0., 200.);    
		fh_Fi1_ToTvsTime= new TH2F("fiber1_ToTvsTime", "Fiber1 ToT vs Time", 300, 0., 300., 1000, 0., 100000.);
		fh_Fi1_multihit_s = new TH2F("fiber1_multihit_s", "Fiber1 multi hits of single PMT", 16, 0., 16., 20, 0., 20.);
		fh_Fi1_ToT_s = new TH2F("fiber1_ToT_s", "Fiber1 ToT of single PMT", 300, 0., 300., 400, 0., 200.);    
		fh_Fi1_ToTvsTime_s= new TH2F("fiber1_ToTvsTime_s", "Fiber1 ToT vs Time of single PMT", 300, 0., 300., 1000, 0., 100000.);
		
		TCanvas *cFi1 = new TCanvas("Fiber1", "Fiber1", 10, 10, 500, 500);
		cFi1->Divide(3, 3);
		cFi1->cd(1);
		fh_Fi1_channels->Draw();
		cFi1->cd(2);
		fh_Fi1_fibers->Draw();
		cFi1->cd(3);
		cFi1->cd(4);
		fh_Fi1_multihit->Draw("colz");
		cFi1->cd(5);
		fh_Fi1_ToT->Draw("colz"); 
		cFi1->cd(6);
		fh_Fi1_ToTvsTime->Draw("colz"); 
		cFi1->cd(7);
		fh_Fi1_multihit_s->Draw("colz");
		cFi1->cd(8);
		fh_Fi1_ToT_s->Draw("colz"); 
		cFi1->cd(9);
		fh_Fi1_ToTvsTime_s->Draw("colz"); 

		cFi1->cd(0);
		run->AddObject(cFi1);
    }
    
    

//-----------------------------------------------------------------------
// Fiber 5 detector

	// get access to Mapped data
    fMappedItemsFi5 = (TClonesArray*)mgr->GetObject("Fi5Mapped");

	// get access to cal data
    fHitItemsFi5 = (TClonesArray*)mgr->GetObject("Fi5Hit");

    if(fMappedItemsFi5){  
		fh_Fi5_channels = new TH1F("fiber5_channels", "Fiber5 channels", 300, 0., 300.);
		fh_Fi5_fibers = new TH1F("fiber5_number", "Fiber5 number", 1200, 0., 1200.);    
		fh_Fi5_multihit = new TH2F("fiber5_multihit", "Fiber5 multi hits", 300, 0., 300., 20, 0., 20.);
		fh_Fi5_ToT = new TH2F("fiber5_ToT", "Fiber5 ToT", 1200, 0., 1200., 400, 0., 200.);    
		fh_Fi5_ToTvsTime= new TH2F("fiber5_ToTvsTime", "Fiber5 ToT vs Time", 1200, 0., 1200., 1000, 0., 10000000.);
		fh_Fi5_multihit_s = new TH2F("fiber5_multihit_s", "Fiber5 multi hits of single PMT", 16, 0., 16., 20, 0., 20.);
		fh_Fi5_ToT_s = new TH2F("fiber5_ToT_s", "Fiber5 ToT of single PMT", 1200, 0., 1200., 400, 0., 200.);    
		fh_Fi5_ToTvsTime_s= new TH2F("fiber5_ToTvsTime_s", "Fiber5 ToT vs Time of single PMT", 1200, 0., 1200., 1000, 0., 10000000.);
			
		TCanvas *cFi5 = new TCanvas("Fiber5", "Fiber5", 10, 10, 500, 500);
		cFi5->Divide(3, 3);
		cFi5->cd(1);
		fh_Fi5_channels->Draw();
		cFi5->cd(2);
		fh_Fi5_fibers->Draw();
		cFi5->cd(3);
		cFi5->cd(4);
		fh_Fi5_multihit->Draw("colz");
		cFi5->cd(5);
		fh_Fi5_ToT->Draw("colz"); 
		cFi5->cd(6);
		fh_Fi5_ToTvsTime->Draw("colz"); 
		cFi5->cd(7);
		fh_Fi5_multihit_s->Draw("colz");
		cFi5->cd(8);
		fh_Fi5_ToT_s->Draw("colz"); 
		cFi5->cd(9);
		fh_Fi5_ToTvsTime_s->Draw("colz"); 
		cFi5->cd(0);
		run->AddObject(cFi5);
    }

//-----------------------------------------------------------------------
// Fiber 6 detector

	// get access to Mapped data
    fMappedItemsFi6 = (TClonesArray*)mgr->GetObject("Fi6Mapped");
	// get access to cal data
    fHitItemsFi6 = (TClonesArray*)mgr->GetObject("Fi6Hit");

    if(fMappedItemsFi6){  
		fh_Fi6_channels = new TH1F("fiber6_channels", "Fiber6 channels", 300, 0., 300.);
		fh_Fi6_fibers = new TH1F("fiber6_number", "Fiber6 number", 600, 0., 600.);    
		fh_Fi6_multihit = new TH2F("fiber6_multihit", "Fiber6 multi hits", 300, 0., 300., 20, 0., 20.);
		fh_Fi6_ToT = new TH2F("fiber6_ToT", "Fiber6 ToT", 600, 0., 600., 400, 0., 200.);    
		fh_Fi6_ToTvsTime= new TH2F("fiber6_ToTvsTime", "Fiber6 ToT vs Time", 600, 0., 600., 1000, 0., 3000000.);
		fh_Fi6_multihit_s = new TH2F("fiber6_multihit_s", "Fiber6 multi hits of single PMT", 16, 0., 16., 20, 0., 20.);
		fh_Fi6_ToT_s = new TH2F("fiber6_ToT_s", "Fiber6 ToT of single PMT", 600, 0., 600., 400, 0., 200.);    
		fh_Fi6_ToTvsTime_s= new TH2F("fiber6_ToTvsTime_s", "Fiber6 ToT vs Time of single PMT", 600, 0., 600., 1000, 0., 3000000.);
		
		
		TCanvas *cFi6 = new TCanvas("Fiber6", "Fiber6", 10, 10, 500, 500);
		cFi6->Divide(3, 3);
		cFi6->cd(1);
		fh_Fi6_channels->Draw();
		cFi6->cd(2);
		fh_Fi6_fibers->Draw();
		cFi6->cd(3);
		cFi6->cd(4);
		fh_Fi6_multihit->Draw("colz");
		cFi6->cd(5);
		fh_Fi6_ToT->Draw("colz"); 
		cFi6->cd(6);
		fh_Fi6_ToTvsTime->Draw("colz"); 
		cFi6->cd(7);
		fh_Fi6_multihit_s->Draw("colz");
		cFi6->cd(8);
		fh_Fi6_ToT_s->Draw("colz"); 
		cFi6->cd(9);
		fh_Fi6_ToTvsTime_s->Draw("colz"); 
		cFi6->cd(0);
		run->AddObject(cFi6);
    }
//---------------------------------------------------------------------------------------
	//Ptof detector
	fCalItemsPtof = (TClonesArray*)mgr->GetObject("PtofCal");

    if(fCalItemsPtof){

		TCanvas *cPtof_plane = new TCanvas("Ptof_plane", "Ptof plane", 10, 10, 500, 500);
		cPtof_plane->Divide(1, 2);
		
		fh_ptof_channels = new TH1F("Ptof_channels", "Ptof channels", 65, 0., 65.);
		cPtof_plane->cd(1);
		fh_ptof_channels->Draw();

		fh_ptof_channels_cut = new TH1F("Ptof_channels_cut", "Ptof channels with cut", 65, 0., 65.);
		cPtof_plane->cd(2);
		fh_ptof_channels_cut->Draw();
		run->AddObject(cPtof_plane);

		TCanvas *cPtof_test = new TCanvas("Ptof_test", "Ptof test", 10, 10, 500, 500);
		cPtof_test->Divide(1, 1);
		
		fh_ptof_test1 = new TH1F("Ptof_test1", "Ptof test1", 1000, 0., 100.);
		fh_ptof_test2 = new TH1F("Ptof_test2", "Ptof test2", 1000, 0., 100.);
		cPtof_test->cd(1);
		fh_ptof_test1->Draw();
	 //   cPtof_test->cd(2);
	//    fh_ptof_test2->Draw();
		
		run->AddObject(cPtof_test);
	   
	   
		TCanvas* cPtof1;
		cPtof1 = new TCanvas("Ptof1", "Ptof1", 10, 10, 500, 500);
		cPtof1->Divide(8,8);
		for (Int_t j = 1; j < 100; j++){
			char strName[255];
			sprintf(strName, "ToT_Bar_%d_PM_1", j);
			fh_ptof_TotPm1[j] = new TH1F(strName, "", 1000, 0., 100.);
			cPtof1->cd(j);
			fh_ptof_TotPm1[j]->Draw();           
		}
		run->AddObject(cPtof1);
		
		TCanvas* cPtof2;
		cPtof2 = new TCanvas("Ptof2", "Ptof2", 10, 10, 500, 500);
		cPtof2->Divide(8,8);
		for (Int_t j = 1; j < 100; j++){
			char strName[255];
			sprintf(strName, "ToT_Bar_%d_PM_2", j);
			fh_ptof_TotPm2[j] = new TH1F(strName, "", 1000, 0., 100.);
			cPtof2->cd(j);
			fh_ptof_TotPm2[j]->Draw();           
		}
		run->AddObject(cPtof2);
		


	}


//---------------------------------------------------------------------------------------------------
    fMappedItemsTofd = (TClonesArray*)mgr->GetObject("TofdMapped");
    if(fMappedItemsTofd){
		TCanvas *cTofd_planes = new TCanvas("TOFD_planes", "TOFD planes", 10, 10, 500, 500);
		cTofd_planes->Divide(3, 2);
		
		for (Int_t j = 0; j < fNofPlanes; j++)
		{
			 char strName1[255];
			 sprintf(strName1, "tofd_channels_plane_%d", j);
			 char strName2[255];
			 sprintf(strName2, "Tofd channels plane %d", j);        
			 fh_tofd_channels[j] = new TH1F(strName1, strName2, 16, -8., 8.);
			 cTofd_planes->cd(j+1);
			 fh_tofd_channels[j]->Draw();
		}
		
		
	   
		TCanvas* cTofd[fNofPlanes];
		for (Int_t i = 0; i < fNofPlanes; i++)
		{		
			char strName[255];
			sprintf(strName, "TOFD_Plane_%d", i);
			cTofd[i] = new TCanvas(strName, "", 10, 10, 500, 500);
			cTofd[i]->Divide(3,4);
			for (Int_t j = 0; j < fPaddlesPerPlane; j++)
			{
				sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_1", i, j);
				fh_tofd_TotPm1[i][j] = new TH1F(strName, "", 5000, 0., 500.);
				cTofd[i]->cd(2*j+1);
				fh_tofd_TotPm1[i][j]->Draw();
				sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_2", i, j);
				fh_tofd_TotPm2[i][j] = new TH1F(strName, "", 5000, 0., 500.);
				cTofd[i]->cd(2*j+2);
				fh_tofd_TotPm1[i][j]->Draw();
			   
			}
		}
   

    }
    fCalItemsTofd = (TClonesArray*)mgr->GetObject("TofdCal");

// -------------------------------------------------------------------------
    
    // Pspx Data
    fMappedItemsPspx = (TClonesArray*)mgr->GetObject("PspxMappedData");
    fCalItemsPspx = (TClonesArray*)mgr->GetObject("PspxCalData");
    
    if(fMappedItemsPspx || fCalItemsPspx){
		for(UInt_t i=0;i<4;i++){
			  fh_pspx_strips_psp[i] = new TH1F(Form("pspx_strips_psp%d",i), Form("Pspx strips PSP %d",i+1), 16, 1, 17); 
		
		  fh_pspx_energy_psp[i] = new TH1F(Form("pspx_energy_psp%d",i), Form("Pspx cathode energy PSP %d",i+1), 200, 0, 35000); 
		  
		  fh_pspx_multiplicity_psp[i] = new TH1F(Form("pspx_multiplicity_psp%d",i), Form("Pspx multiplicity PSP %d",i+1), 10, 0, 10); 
		}
		
		fh_pspx_strips_psp[0]->GetXaxis()->SetTitle("y position / strips with 3mm width");
		fh_pspx_strips_psp[1]->GetXaxis()->SetTitle("x position / strips with 3mm width");
		fh_pspx_strips_psp[2]->GetXaxis()->SetTitle("y position / strips with 3mm width");
		fh_pspx_strips_psp[3]->GetXaxis()->SetTitle("x position / strips with 3mm width"); 
		
		fh_pspx_pos1_strips = new TH2F("pspx_pos1_strips", "Pspx Position1", 16, 1, 17,16,1,17); 
		fh_pspx_pos2_strips = new TH2F("pspx_pos2_strips", "Pspx Position2", 16, 1, 17,16,1,17);  
		
		fh_pspx_pos1_strips->GetYaxis()->SetTitle("y position / strips with 3mm width");
		fh_pspx_pos1_strips->GetXaxis()->SetTitle("x position / strips with 3mm width"); 
		fh_pspx_pos2_strips->GetYaxis()->SetTitle("y position / strips with 3mm width");
		fh_pspx_pos2_strips->GetXaxis()->SetTitle("x position / strips with 3mm width");
		
		fh_pspx_pos1_energy = new TH2F("pspx_pos1_energy", "Pspx Position1", 32, -1, 1,32,-1,1); 
		fh_pspx_pos2_energy = new TH2F("pspx_pos2_energy", "Pspx Position2", 32, -1, 1,32,-1,1);  
		
		fh_pspx_pos1_energy->GetYaxis()->SetTitle("y position / a.u.");
		fh_pspx_pos1_energy->GetXaxis()->SetTitle("x position / a.u."); 
		fh_pspx_pos2_energy->GetYaxis()->SetTitle("y position / a.u.");
		fh_pspx_pos2_energy->GetXaxis()->SetTitle("x position / a.u.");
		
		
		fh_pspx_cor_x_strips = new TH2F("pspx_cor_x_strips", "Pspx Position1", 16, 1, 17,16,1,17); 
		fh_pspx_cor_y_strips = new TH2F("pspx_cor_y_strips", "Pspx Position2", 16, 1, 17,16,1,17);  
		
		fh_pspx_cor_x_strips->GetYaxis()->SetTitle("x position / strips with 3mm width");
		fh_pspx_cor_x_strips->GetXaxis()->SetTitle("x position / strips with 3mm width"); 
		fh_pspx_cor_y_strips->GetYaxis()->SetTitle("y position / strips with 3mm width");
		fh_pspx_cor_y_strips->GetXaxis()->SetTitle("y position / strips with 3mm width");
		
		fh_pspx_cor_x_energy = new TH2F("pspx_cor_x_energy", "Pspx Position1", 32, -1, 1,32,-1,1); 
		fh_pspx_cor_y_energy = new TH2F("pspx_cor_y_energy", "Pspx Position2", 32, -1, 1,32,-1,1);  
		
		fh_pspx_cor_x_energy->GetYaxis()->SetTitle("x position / a.u.");
		fh_pspx_cor_x_energy->GetXaxis()->SetTitle("x position / a.u."); 
		fh_pspx_cor_y_energy->GetYaxis()->SetTitle("y position / a.u.");
		fh_pspx_cor_y_energy->GetXaxis()->SetTitle("y position / a.u.");
		
		TCanvas *cpspx_position = new TCanvas("pspx_position", "pspx_position", 10, 10, 500, 500);
		cpspx_position->Divide(2, 2);
		
		cpspx_position->cd(1);
		fh_pspx_pos1_strips->Draw();
		cpspx_position->cd(2);
		fh_pspx_pos2_strips->Draw();
		cpspx_position->cd(3);
		fh_pspx_pos1_energy->Draw();
		cpspx_position->cd(4);
		fh_pspx_pos2_energy->Draw();
		cpspx_position->cd(0);
		run->AddObject(cpspx_position);
		
		
		TCanvas *cpspx_strips = new TCanvas("pspx_strips", "pspx_strips", 10, 10, 500, 500);
		cpspx_strips->Divide(2, 2);
		
		cpspx_strips->cd(1);
		fh_pspx_strips_psp[0]->Draw();
		cpspx_strips->cd(2);
		fh_pspx_strips_psp[1]->Draw();
		cpspx_strips->cd(3);
		fh_pspx_strips_psp[2]->Draw();
		cpspx_strips->cd(4);
		fh_pspx_strips_psp[3]->Draw();
		cpspx_strips->cd(0);
		run->AddObject(cpspx_strips);
		
		
		TCanvas *cpspx_energy = new TCanvas("pspx_energy", "pspx_energy", 10, 10, 500, 500);
		cpspx_energy->Divide(2, 2);
		
		cpspx_energy->cd(1);
		fh_pspx_energy_psp[0]->Draw();
		cpspx_energy->cd(2);
		fh_pspx_energy_psp[1]->Draw();
		cpspx_energy->cd(3);
		fh_pspx_energy_psp[2]->Draw();
		cpspx_energy->cd(4);
		fh_pspx_energy_psp[3]->Draw();
		cpspx_energy->cd(0);
		run->AddObject(cpspx_energy);
		
		
		TCanvas *cpspx_multiplicity = new TCanvas("pspx_multiplicity", "pspx_multiplicity", 10, 10, 500, 500);
		cpspx_multiplicity->Divide(2, 2);
		
		cpspx_multiplicity->cd(1);
		fh_pspx_multiplicity_psp[0]->Draw();
		cpspx_multiplicity->cd(2);
		fh_pspx_multiplicity_psp[1]->Draw();
		cpspx_multiplicity->cd(3);
		fh_pspx_multiplicity_psp[2]->Draw();
		cpspx_multiplicity->cd(4);
		fh_pspx_multiplicity_psp[3]->Draw();
		cpspx_multiplicity->cd(0);
		run->AddObject(cpspx_energy);
		
		TCanvas *cpspx_cor = new TCanvas("pspx_cor", "pspx_cor", 10, 10, 500, 500);
		cpspx_cor->Divide(2, 2);
		
		cpspx_cor->cd(1);
		fh_pspx_cor_x_strips->Draw();
		cpspx_cor->cd(2);
		fh_pspx_cor_y_strips->Draw();
		cpspx_cor->cd(3);
		fh_pspx_cor_x_energy->Draw();
		cpspx_cor->cd(4);
		fh_pspx_cor_y_energy->Draw();
		cpspx_strips->cd(0);
		run->AddObject(cpspx_cor);
    }
    return kSUCCESS;
}



void R3BOnlineSpectra::Exec(Option_t* option)
{
    Double_t timeLos=0.;
    Double_t cher1=0.;
    Double_t cher2=0.;
    Double_t cher3=0.;
    Double_t cher4=0.;

    
    Double_t timeTofd=0;
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
	// check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
		return;


    if(fMappedItemsLos && fMappedItemsLos->GetEntriesFast())
    {
      Int_t nHits = fMappedItemsLos->GetEntriesFast();
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BLosMappedData* hit = (R3BLosMappedData*)fMappedItemsLos->At(ihit);
         if (!hit) continue;

         // channel numbers are stored 1-based (1..n)
         Int_t iDet = hit->GetDetector(); // 1..
         Int_t iCha = hit->GetChannel();  // 1..
         fh_los_channels->Fill(iCha);				         
      }
    }

    if(fCalItemsLos && fCalItemsLos->GetEntriesFast())
    {
      Int_t nHits = fCalItemsLos->GetEntriesFast();    
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	  R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(ihit);

          Int_t iDet=calData->GetDetector();
          //LOG(DEBUG) << "test1 : "<<flosOffsetX<<"  "<<flosOffsetY<<"  "<<flosVeffX<<"  "<<flosVeffY<<FairLogger::endl;
	      //LOG(DEBUG) <<"test2 : "<<calData->fTime_r_ns<<FairLogger::endl;

          if(iDet==1){
	          fh_los_tres->Fill((calData->fTime_r_ns+calData->fTime_l_ns)/2. -     		
	                  (calData->fTime_t_ns+calData->fTime_b_ns)/2.);
	          fh_los_pos->Fill((calData->fTime_r_ns-calData->fTime_l_ns-flosOffsetX)*flosVeffX,
	                 (calData->fTime_b_ns-calData->fTime_t_ns-flosOffsetY)*flosVeffY);
              timeLos=(calData->fTime_r_ns+calData->fTime_l_ns+calData->fTime_t_ns+calData->fTime_b_ns)/4.;		 
	      }
      }
    }


//----------------------------------------------------------------------
// Fiber detectors
//----------------------------------------------------------------------

    if(fMappedItemsFi1 && fMappedItemsFi1->GetEntriesFast()) 
    {
      Int_t nHits = fMappedItemsFi1->GetEntriesFast();
      std::vector<UInt_t> mapmt_num(256);
      std::vector<UInt_t> spmt_num(16);
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BBunchedFiberMappedData* hit = (R3BBunchedFiberMappedData*)fMappedItemsFi1->At(ihit);
         if (!hit) continue;

         // channel numbers are stored 1-based (1..n)
         Int_t iCha = hit->GetChannel();  // 1..
         
         if (hit->IsMAPMT() && hit->IsLeading()) {
			 fh_Fi1_channels->Fill(iCha);
			 ++mapmt_num.at(hit->GetChannel() - 1);
		 }
         if (!hit->IsMAPMT() && hit->IsLeading()) {
			 ++spmt_num.at(hit->GetChannel() - 1);
		 }
      }
      UInt_t m_tot = 0;
      for (int i = 0; i < 256; ++i) {
        auto m = mapmt_num.at(i);
        if(m > 0) fh_Fi1_multihit->Fill(i,m);
        m_tot += m;
	  }
      for (int i = 0; i < 16; ++i) {
        auto m = spmt_num.at(i);
        if(m > 0) fh_Fi1_multihit_s->Fill(i,m);
	  }
    }


    if(fHitItemsFi1 && fHitItemsFi1->GetEntriesFast()) {
      Int_t nHits = fHitItemsFi1->GetEntriesFast();
      std:vector<UInt_t> fiber1_mult(256);
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItemsFi1->At(ihit);
         if (!hit) continue;

         Int_t iFib = hit->GetFiberId();  // 1..
         if(hit->GetSPMTToT_ns() > 0) {
			 fh_Fi1_fibers->Fill(iFib);
			 ++fiber1_mult.at(iFib - 1);
		 }
         if(hit->GetSPMTToT_ns() > 4){
            fh_Fi1_ToT->Fill(iFib,hit->GetMAPMTToT_ns());
            fh_Fi1_ToTvsTime->Fill(iFib,fNEvents);
            fh_Fi1_ToT_s->Fill(iFib,hit->GetSPMTToT_ns());
            fh_Fi1_ToTvsTime_s->Fill(hit->GetSPMTToT_ns(),fNEvents);
            
	     }
      }
    }


    if(fMappedItemsFi5) 
    {
      Int_t nHits = fMappedItemsFi5->GetEntriesFast();
      std::vector<UInt_t> mapmt_num(256);
      std::vector<UInt_t> spmt_num(16);
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BBunchedFiberMappedData* hit = (R3BBunchedFiberMappedData*)fMappedItemsFi5->At(ihit);
         if (!hit) continue;

         // channel numbers are stored 1-based (1..n)
         Int_t iCha = hit->GetChannel();  // 1..
         
         if (hit->IsMAPMT() && hit->IsLeading()) {
			 fh_Fi5_channels->Fill(iCha);
			 ++mapmt_num.at(hit->GetChannel() - 1);
		 }
         if (!hit->IsMAPMT() && hit->IsLeading()) {
			 ++spmt_num.at(hit->GetChannel() - 1);
		 }
      }
      UInt_t m_tot = 0;
      for (int i = 0; i < 256; ++i) {
        auto m = mapmt_num.at(i);
        if(m > 0) fh_Fi5_multihit->Fill(i,m);
        m_tot += m;
	  }
      for (int i = 0; i < 16; ++i) {
        auto m = spmt_num.at(i);
        if(m > 0) fh_Fi5_multihit_s->Fill(i,m);
	  }
    }

    if(fHitItemsFi5 && fHitItemsFi5->GetEntriesFast()) {
      Int_t nHits = fHitItemsFi5->GetEntriesFast();
      std::vector<UInt_t> fiber5_mult(1024);
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItemsFi5->At(ihit);
         if (!hit) continue;

         Int_t iFib = hit->GetFiberId();  // 1..
         if(hit->GetSPMTToT_ns() > 0) {
			 fh_Fi5_fibers->Fill(iFib);
			 ++fiber5_mult.at(iFib - 1);
		 }
         if(hit->GetSPMTToT_ns() > 4){
            fh_Fi5_ToT->Fill(iFib,hit->GetMAPMTToT_ns());
            fh_Fi5_ToTvsTime->Fill(iFib,fNEvents);
            fh_Fi5_ToT_s->Fill(iFib,hit->GetSPMTToT_ns());
            fh_Fi5_ToTvsTime_s->Fill(hit->GetSPMTToT_ns(),fNEvents);
	     }
      }
    }





    if(fMappedItemsFi6 && fMappedItemsFi6->GetEntriesFast()) 
    {
      Int_t nHits = fMappedItemsFi6->GetEntriesFast();
      std::vector<UInt_t> mapmt_num(256);
      std::vector<UInt_t> spmt_num(16);
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BBunchedFiberMappedData* hit = (R3BBunchedFiberMappedData*)fMappedItemsFi6->At(ihit);
         if (!hit) continue;

         // channel numbers are stored 1-based (1..n)
         Int_t iCha = hit->GetChannel();  // 1..
         
         if (hit->IsMAPMT() && hit->IsLeading()) {
			 fh_Fi6_channels->Fill(iCha);
			 ++mapmt_num.at(hit->GetChannel() - 1);
		 }
         if (!hit->IsMAPMT() && hit->IsLeading()) {
			 ++spmt_num.at(hit->GetChannel() - 1);
		 }
      }
      UInt_t m_tot = 0;
      for (int i = 0; i < 256; ++i) {
        auto m = mapmt_num.at(i);
        if(m > 0) fh_Fi6_multihit->Fill(i,m);
        m_tot += m;
	  }
      for (int i = 0; i < 16; ++i) {
        auto m = spmt_num.at(i);
        if(m > 0) fh_Fi6_multihit_s->Fill(i,m);
	  }
    }


   if(fHitItemsFi6 && fHitItemsFi6->GetEntriesFast()) {
      Int_t nHits = fHitItemsFi6->GetEntriesFast();
      std::vector<UInt_t> fiber6_mult(512);
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
         R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItemsFi6->At(ihit);
         if (!hit) continue;

         Int_t iFib = hit->GetFiberId();  // 1..
         if(hit->GetSPMTToT_ns() > 0) {
			 fh_Fi6_fibers->Fill(iFib);
			 ++fiber6_mult.at(iFib - 1);
		 }
         if(hit->GetSPMTToT_ns() > 4 && hit->GetMAPMTToT_ns()>4){
            fh_Fi6_ToT->Fill(iFib,hit->GetMAPMTToT_ns());
            fh_Fi6_ToTvsTime->Fill(iFib,fNEvents);
            fh_Fi6_ToT_s->Fill(iFib,hit->GetSPMTToT_ns());
            fh_Fi6_ToTvsTime_s->Fill(hit->GetSPMTToT_ns(),fNEvents);
	     }
      }
   }



    if(fMappedItemsTofd && fMappedItemsTofd->GetEntriesFast())
    {
      Int_t nHits = fMappedItemsTofd->GetEntriesFast();    
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	R3BPaddleTamexMappedData *hit = (R3BPaddleTamexMappedData*)fMappedItemsTofd->At(ihit);
        if (!hit) continue; // should not happen

        Int_t iPlane = hit->GetPlaneId(); // 1..n
        Int_t iBar   = hit->GetBarId();   // 1..n

            
        if(iPlane<=fNofPlanes){
          fh_tofd_channels[iPlane-1]->Fill(iBar);
          fh_tofd_channels[iPlane-1]->Fill(-iBar-1);
        }
      }
    }

    
    if(fCalItemsTofd && fCalItemsTofd->GetEntriesFast())
    {
      Double_t tot1=0.;
      Double_t tot2=0.;
      Double_t t1l=0.;
      Double_t t2l=0.;
      Double_t t1t=0.;
      Double_t t2t=0.;
     
      Int_t nHits = fCalItemsTofd->GetEntriesFast();    
      
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	  R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsTofd->At(ihit);
          if (!hit) continue; // should not happen

          Int_t iPlane  = hit->GetPlane();    // 1..n
          Int_t iBar  = hit->GetBar();    // 1..n

                 // get all times of one bar
          t1l=hit->fTime1L_ns;
          t2l=hit->fTime2L_ns;
          t1t=hit->fTime1T_ns;
          t2t=hit->fTime2T_ns;

	      // calculate time over threshold and check if clock counter went out of range
          while(t1t - t1l < 0.) {
	          t1t=t1t+2048.*fClockFreq; 
	      }

          while(t2t-t2l < 0.) {
	          t2t=t2t+2048.*fClockFreq; 
          }
	      while(t1l-timeLos<0.){
	          t1t=t1t+2048.*fClockFreq; 
	          t1l=t1l+2048.*fClockFreq; 
	          t2t=t2t+2048.*fClockFreq; 
	          t2l=t2l+2048.*fClockFreq; 			  
		  }
       
          tot1=t1t - t1l;		      
          if(tot1<0) {
	          LOG(WARNING) << "Negative ToT "<< tot1<<FairLogger::endl;	
	          LOG(WARNING) << "times1: " << t1t << " " << t1l << FairLogger::endl;		  
	      }

          tot2=t2t - t2l;	
          if(tot2<0) {
              LOG(WARNING) << "Negative ToT "<< tot2<<FairLogger::endl;              
              LOG(WARNING) << "times2: " << t2t << " " << t2l << FairLogger::endl;		 
          }
 
          fh_tofd_TotPm1[iPlane-1][iBar-1]->Fill(tot1);
          fh_tofd_TotPm2[iPlane-1][iBar-1]->Fill(tot2);
        	
      }	

    }

    if(fCalItemsPtof)
    {
      Double_t tot1=0.;
      Double_t tot2=0.;
      Double_t t1l=0.;
      Double_t t2l=0.;
      Double_t t1t=0.;
      Double_t t2t=0.;
      Bool_t bar60=false;
      Bool_t bar61=false;
      Bool_t bar62=false;
      Bool_t bar63=false;
      
      Int_t nHits = fCalItemsPtof->GetEntriesFast();    
      LOG(DEBUG) << "nHits: " << nHits << FairLogger::endl;
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	  R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsPtof->At(ihit);
          if (!hit) continue; // should not happen

          Int_t iBar  = hit->GetBar();    // 1..n
                 // get all times of one bar
          t1l=hit->fTime1L_ns;
          t2l=hit->fTime2L_ns;
          t1t=hit->fTime1T_ns;
          t2t=hit->fTime2T_ns;
//          if(!(t1l>0 && t2l>0 && t1t>0 && t2t>0)) continue;
          fh_ptof_channels->Fill(iBar);
      LOG(DEBUG) << "Bar: " << iBar << FairLogger::endl;
      LOG(DEBUG) << "times PM1: " << t1l<<"  "<<t1t<<"  "<<t1t-t1l << FairLogger::endl;
      LOG(DEBUG) << "times PM2: " << t2l<<"  "<<t2t<<"  "<<t2t-t2l << FairLogger::endl;
          if(iBar==60) bar60=true;
          if(iBar==61) bar61=true;
          if(iBar==62) bar62=true;
          if(iBar==63) bar63=true;
          
	      // calculate time over threshold and check if clock counter went out of range
          while(t1t - t1l < 0.) {
	          t1t=t1t+2048.*fClockFreq; 
	      }

          while(t2t-t2l < 0.) {
	          t2t=t2t+2048.*fClockFreq; 
          }
	      while(t1l-timeLos<0.){
	          t1t=t1t+2048.*fClockFreq; 
	          t1l=t1l+2048.*fClockFreq; 
	          t2t=t2t+2048.*fClockFreq; 
	          t2l=t2l+2048.*fClockFreq; 			  
		  }
       
          tot1=t1t - t1l;		      
          if(tot1<0) {
	          LOG(WARNING) << "Negative ToT "<< tot1<<FairLogger::endl;	
	          LOG(WARNING) << "times1: " << t1t << " " << t1l << FairLogger::endl;		  
	      }

          tot2=t2t - t2l;	
          if(tot2<0) {
              LOG(WARNING) << "Negative ToT "<< tot2<<FairLogger::endl;              
              LOG(WARNING) << "times2: " << t2t << " " << t2l << FairLogger::endl;		 
          }
 
//          fh_ptof_TotPm1[iBar]->Fill(tot1);
//          fh_ptof_TotPm2[iBar]->Fill(tot2);
          if(iBar==33) fh_ptof_test1->Fill(sqrt(tot1*tot1));	
      }	
      
      
      //once again
      
      
      nHits = fCalItemsPtof->GetEntriesFast();    
//      LOG(DEBUG) << "nHits: " << nHits << FairLogger::endl;
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	  R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsPtof->At(ihit);
          if (!hit) continue; // should not happen

          Int_t iBar  = hit->GetBar();    // 1..n
                 // get all times of one bar
          t1l=hit->fTime1L_ns;
          t2l=hit->fTime2L_ns;
          t1t=hit->fTime1T_ns;
          t2t=hit->fTime2T_ns;

          
	      // calculate time over threshold and check if clock counter went out of range
          while(t1t - t1l < 0.) {
	          t1t=t1t+2048.*fClockFreq; 
	      }

          while(t2t-t2l < 0.) {
	          t2t=t2t+2048.*fClockFreq; 
          }
	      while(t1l-timeLos<0.){
	          t1t=t1t+2048.*fClockFreq; 
	          t1l=t1l+2048.*fClockFreq; 
	          t2t=t2t+2048.*fClockFreq; 
	          t2l=t2l+2048.*fClockFreq; 			  
		  }
       
          tot1=t1t - t1l;		      
          if(tot1<0) {
//	          LOG(WARNING) << "Negative ToT "<< tot1<<FairLogger::endl;	
//	          LOG(WARNING) << "times1: " << t1t << " " << t1l << FairLogger::endl;		  
	      }

          tot2=t2t - t2l;	
          if(tot2<0) {
//              LOG(WARNING) << "Negative ToT "<< tot2<<FairLogger::endl;              
//              LOG(WARNING) << "times2: " << t2t << " " << t2l << FairLogger::endl;		 
          }
 

          if(bar60 && bar61 && bar62 && bar63){
             fh_ptof_TotPm1[iBar]->Fill(tot1);
             fh_ptof_TotPm2[iBar]->Fill(tot2);
	         if(iBar==7) fh_ptof_test2->Fill(sqrt(tot1*tot2));	
             fh_ptof_channels_cut->Fill(iBar);

	      }

      }

   }

    if(fCalItemsPspx && fCalItemsPspx->GetEntriesFast())
    {      
      Int_t nHits = fCalItemsPspx->GetEntriesFast();    
      Double_t max_energy1[4] = {0,0,0,0};
      Double_t max_energy2[4] = {0,0,0,0};
      Double_t max_strip[4] = {0,0,0,0};
      
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
	R3BPspxCalData *calData = (R3BPspxCalData*)fCalItemsPspx->At(ihit);
	if(calData->GetEnergy1()>max_energy1[calData->GetDetector()-1] && calData->GetStrip()!=17){
	  max_energy1[calData->GetDetector()-1]=calData->GetEnergy1();
	  max_energy2[calData->GetDetector()-1]=calData->GetEnergy2();
	  max_strip[calData->GetDetector()-1]=calData->GetStrip();
	} 
	
	
	if(calData->GetDetector()==1){
	  if(calData->GetStrip()==17){
	    fh_pspx_energy_psp[0]->Fill(calData->GetEnergy1());  
	  }
	} else if(calData->GetDetector()==2){
	  if(calData->GetStrip()==17){
	    fh_pspx_energy_psp[1]->Fill(calData->GetEnergy1());  
	  }
	} else if(calData->GetDetector()==3){ 
	  if(calData->GetStrip()==17){
	    fh_pspx_energy_psp[2]->Fill(calData->GetEnergy1());  
	  }
	} else if(calData->GetDetector()==4){
	  if(calData->GetStrip()==17){
	    fh_pspx_energy_psp[3]->Fill(calData->GetEnergy1());  
	  }
	} 
      }
      
      fh_pspx_strips_psp[0]->Fill(max_strip[0]);  
      fh_pspx_strips_psp[1]->Fill(max_strip[1]);  
      fh_pspx_strips_psp[2]->Fill(max_strip[2]);  
      fh_pspx_strips_psp[3]->Fill(max_strip[3]);  
      
      if(max_energy1[0]!=0 && max_energy1[1]!=0){
	  fh_pspx_pos1_strips->Fill(max_strip[1],max_strip[0]); 
	  if(max_energy2[0]!=0 && max_energy2[1]!=0){
	      fh_pspx_pos1_energy->Fill(-(max_energy1[0]-max_energy2[0])/(max_energy1[0]+max_energy2[0]),(max_energy1[1]-max_energy2[1])/(max_energy1[1]+max_energy2[1]));
	  }
      }
      if(max_energy1[2]!=0 && max_energy1[3]!=0){
	  fh_pspx_pos2_strips->Fill(max_strip[3],max_strip[2]);
	  if(max_energy2[2]!=0 && max_energy2[3]!=0){
	      fh_pspx_pos2_energy->Fill(-(max_energy1[2]-max_energy2[2])/(max_energy1[2]+max_energy2[2]),-(max_energy1[3]-max_energy2[3])/(max_energy1[3]+max_energy2[3]));
	  }
      }
      
      if(max_energy1[1]!=0 && max_energy1[3]!=0){
	  fh_pspx_cor_x_strips->Fill(max_strip[1],max_strip[3]);
	  if(max_energy2[1]!=0 && max_energy2[3]!=0){
	      fh_pspx_cor_x_energy->Fill(-(max_energy1[1]-max_energy2[1])/(max_energy1[1]+max_energy2[1]),-(max_energy1[3]-max_energy2[3])/(max_energy1[3]+max_energy2[3]));
	  }
      }
      if(max_energy1[0]!=0 && max_energy1[2]!=0){
	  fh_pspx_cor_y_strips->Fill(max_strip[0],max_strip[2]);
	  if(max_energy2[0]!=0 && max_energy2[2]!=0){
	      fh_pspx_cor_y_energy->Fill(-(max_energy1[0]-max_energy2[0])/(max_energy1[0]+max_energy2[0]),-(max_energy1[2]-max_energy2[2])/(max_energy1[2]+max_energy2[2]));
	  }
      }
      
    }

   
    if(fMappedItemsPspx && fMappedItemsPspx->GetEntriesFast())
    {
      Int_t mult1=0;
      Int_t mult2=0;
      Int_t mult3=0;
      Int_t mult4=0;
      
      Int_t nHits = fMappedItemsPspx->GetEntriesFast();    
      
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
	R3BPspxMappedData *mappedData = (R3BPspxMappedData*)fMappedItemsPspx->At(ihit);
	if(mappedData->GetDetector()==1){
	  mult1++;
	} else if(mappedData->GetDetector()==2){
	  mult2++;
	} else if(mappedData->GetDetector()==3){
	  mult3++;
	} else if(mappedData->GetDetector()==4){
	  mult4++;
	} 
	
      }
      fh_pspx_multiplicity_psp[0]->Fill(mult1);
      fh_pspx_multiplicity_psp[1]->Fill(mult2);
      fh_pspx_multiplicity_psp[2]->Fill(mult3);
      fh_pspx_multiplicity_psp[3]->Fill(mult4);
      
    }
   
    fNEvents += 1;

}

void R3BOnlineSpectra::FinishEvent()
{
 
    if (fCalItemsLos)
    {
        fCalItemsLos->Clear();
    }
    if (fCalItemsTofd)
    {
        fCalItemsTofd->Clear();
    }
    if (fCalItemsPtof)
    {
        fCalItemsPtof->Clear();
    }
    if (fMappedItemsLos)
    {
        fMappedItemsLos->Clear();
    }
    if (fMappedItemsTofd)
    {
        fMappedItemsTofd->Clear();
    }
    if (fMappedItemsPspx)
    {
        fMappedItemsPspx->Clear();
    }
    if (fMappedItemsPspx)
    {
        fMappedItemsPspx->Clear();
    }
    if (fMappedItemsFi1)
    {
        fMappedItemsFi1->Clear();
    }
    if (fHitItemsFi1)
    {
        fHitItemsFi1->Clear();
    }
    if (fMappedItemsFi6)
    {
        fMappedItemsFi6->Clear();
    }
    if (fHitItemsFi6)
    {
        fHitItemsFi6->Clear();
    }
}

void R3BOnlineSpectra::FinishTask()
{
    if (fMappedItemsFi1){
		fh_Fi1_channels ->Write();
		fh_Fi1_fibers ->Write();    
		fh_Fi1_multihit ->Write();
		fh_Fi1_ToT->Write();
		fh_Fi1_ToTvsTime->Write();
    }
    
    if (fMappedItemsFi5){
		fh_Fi5_channels ->Write();
		fh_Fi5_fibers ->Write();    
		fh_Fi5_multihit ->Write();
		fh_Fi5_ToT->Write();
		fh_Fi5_ToTvsTime->Write();
    }
        
    if (fMappedItemsFi6){
		fh_Fi6_channels ->Write();
		fh_Fi6_fibers ->Write();    
		fh_Fi6_multihit ->Write();
		fh_Fi6_ToT->Write();
		fh_Fi6_ToTvsTime->Write();
    }
}

ClassImp(R3BOnlineSpectra)
