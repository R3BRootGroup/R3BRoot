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
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
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
    , flosVeffX1(1.)
    , flosVeffY1(1.)    
    , flosOffsetX1(0.)
    , flosOffsetY1(0.)
    , flosVeffX2(1.)
    , flosVeffY2(1.)    
    , flosOffsetX2(0.)
    , flosOffsetY2(0.)    
    , flosVeffX1T(1.)
    , flosVeffY1T(1.)    
    , flosOffsetX1T(0.)
    , flosOffsetY1T(0.)
    , flosVeffX2T(1.)
    , flosVeffY2T(1.)    
    , flosOffsetX2T(0.)
    , flosOffsetY2T(0.)
    , flosVeffX1wc(1.)
    , flosVeffY1wc(1.)    
    , flosOffsetX1wc(0.)
    , flosOffsetY1wc(0.)
    , flosVeffX2wc(1.)
    , flosVeffY2wc(1.)    
    , flosOffsetX2wc(0.)
    , flosOffsetY2wc(0.) 
    , flosVeffX1Q(1.)
    , flosVeffY1Q(1.)    
    , flosOffsetX1Q(0.)
    , flosOffsetY1Q(0.)
    , flosVeffX2Q(1.)
    , flosVeffY2Q(1.)    
    , flosOffsetX2Q(0.)
    , flosOffsetY2Q(0.) 
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
    , flosVeffX1(1.)
    , flosVeffY1(1.)    
    , flosOffsetX1(0.)
    , flosOffsetY1(0.)
    , flosVeffX2(1.)
    , flosVeffY2(1.)    
    , flosOffsetX2(0.)
    , flosOffsetY2(0.)    
    , flosVeffX1T(1.)
    , flosVeffY1T(1.)    
    , flosOffsetX1T(0.)
    , flosOffsetY1T(0.)
    , flosVeffX2T(1.)
    , flosVeffY2T(1.)    
    , flosOffsetX2T(0.)
    , flosOffsetY2T(0.)
    , flosVeffX1wc(1.)
    , flosVeffY1wc(1.)    
    , flosOffsetX1wc(0.)
    , flosOffsetY1wc(0.)
    , flosVeffX2wc(1.)
    , flosVeffY2wc(1.)    
    , flosOffsetX2wc(0.)
    , flosOffsetY2wc(0.) 
    , flosVeffX1Q(1.)
    , flosVeffY1Q(1.)    
    , flosOffsetX1Q(0.)
    , flosOffsetY1Q(0.)
    , flosVeffX2Q(1.)
    , flosVeffY2Q(1.)    
    , flosOffsetX2Q(0.)
    , flosOffsetY2Q(0.)   
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
 
    TCanvas *cLos = new TCanvas("Los", "LOS", 10, 10, 850, 950);
    cLos->Divide(2, 3);
 
    fh_los_channels = new TH1F("los_channels", "LOS channels", 10, 0., 10.); 
    fh_los_channels->GetXaxis()->SetTitle("Channel number");
    fh_los_tres_MCFD = new TH1F("los_time_res_MCFD", "LOS MCFD Time resolution - raw", 8000, -4., 4.);
    fh_los_tres_MCFD->GetXaxis()->SetTitle("Time MCFD / ns");
    fh_los_tres_TAMEX = new TH1F("los_time_res_TAMEX", "LOS TAMEX Time resolution -raw ", 8000, -4., 4.);  
    fh_los_tres_TAMEX->GetXaxis()->SetTitle("Time TAMEX / ns"); 
    fh_los_pos = new TH2F("los_position", "LOS xy position not-calibrated", 2000, -10., 10.,2000, -10., 10.);
    fh_los_pos->GetXaxis()->SetTitle("X position / ns");
    fh_los_pos->GetYaxis()->SetTitle("Y position / ns"); 
    fh_los_tot = new TH1F("los_tot","LOS Time-over-Threshold",2500,0.,250.); 
    fh_los_tot->GetXaxis()->SetTitle("ToT / ns");
    
    cLos->cd(1);
    fh_los_channels->Draw();
    cLos->cd(3);
    fh_los_tres_MCFD->Draw();
    cLos->cd(4);
    fh_los_tres_TAMEX->Draw();
    cLos->cd(5);    
    fh_los_pos->Draw("colZ");
    cLos->cd(6);
    fh_los_tot->Draw();
    cLos->cd(0);
    run->AddObject(cLos);

    return kSUCCESS;
}



void R3BOnlineSpectra::Exec(Option_t* option)
{
       Double_t time_r_V = 0.0/0.0;   
       Double_t time_t_V = 0.0/0.0;       
       Double_t time_l_V = 0.0/0.0;       
       Double_t time_b_V = 0.0/0.0;           
       Double_t time_r_L = 0.0/0.0;
       Double_t time_r_T = 0.0/0.0;
       Double_t time_l_L = 0.0/0.0;
       Double_t time_l_T = 0.0/0.0;
       Double_t time_t_L = 0.0/0.0;
       Double_t time_t_T = 0.0/0.0;
       Double_t time_b_L = 0.0/0.0;
       Double_t time_b_T = 0.0/0.0;  
       Double_t time_rt_V = 0.0/0.0;   
       Double_t time_lt_V = 0.0/0.0;       
       Double_t time_lb_V = 0.0/0.0;       
       Double_t time_rb_V = 0.0/0.0;           
       Double_t time_rt_L = 0.0/0.0;
       Double_t time_rt_T = 0.0/0.0;
       Double_t time_lb_L = 0.0/0.0;
       Double_t time_lb_T = 0.0/0.0;
       Double_t time_lt_L = 0.0/0.0;
       Double_t time_lt_T = 0.0/0.0;
       Double_t time_rb_L = 0.0/0.0;
       Double_t time_rb_T = 0.0/0.0;              
       Double_t t_hor_M = 0.0/0.0;
       Double_t t_ver_M = 0.0/0.0;
       Double_t t_45_M = 0.0/0.0;
       Double_t t_135_M = 0.0/0.0;
       Double_t t_hor_T = 0.0/0.0;
       Double_t t_ver_T = 0.0/0.0;
       Double_t t_45_T = 0.0/0.0;
       Double_t t_135_T = 0.0/0.0;
       Double_t timeLosM = 0.0/0.0;
       Double_t LosTresM = 0.0/0.0;
       Double_t timeLosT = 0.0/0.0;
       Double_t LosTresT = 0.0/0.0;
       Double_t tot=0./0.0; 
       Double_t totr = 0.0/0.0;
       Double_t totl = 0.0/0.0;
       Double_t tott = 0.0/0.0;
       Double_t totb = 0.0/0.0; 
       Double_t totrt = 0.0/0.0;
       Double_t totlt = 0.0/0.0;
       Double_t totlb = 0.0/0.0;
       Double_t totrb = 0.0/0.0;
       Double_t tot_45 = 0.0/0.0;
       Double_t tot_135 = 0.0/0.0;
       Double_t tot_hor = 0.0/0.0;
       Double_t tot_ver = 0.0/0.0;
       Double_t x_cm = 0.0/0.0;
       Double_t y_cm = 0.0/0.0;
       Double_t x1_cm = 0.0/0.0;
       Double_t y1_cm = 0.0/0.0;
       Double_t x2_cm = 0.0/0.0;
       Double_t y2_cm = 0.0/0.0;
       
           
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
      Int_t nCha = fCalItemsLos->GetEntriesFast();    
      Int_t iDet = 0;
      
                
      for (Int_t iCha = 0; iCha < nCha; iCha++)     
      {
          R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(iCha);
    	  iDet=calData->GetDetector();

       if(iCha == 0) 
        {   	     	                 
   // Channels 1-4:
          if(!(IS_NAN(calData->fTimeV_r_ns))) time_r_V = calData->fTimeV_r_ns;
          if(!(IS_NAN(calData->fTimeV_t_ns))) time_t_V = calData->fTimeV_t_ns;
          if(!(IS_NAN(calData->fTimeV_l_ns))) time_l_V = calData->fTimeV_l_ns;
          if(!(IS_NAN(calData->fTimeV_b_ns))) time_b_V = calData->fTimeV_b_ns;  
           // Channels 5-8:
          if(!(IS_NAN(calData->fTimeV_rt_ns))) time_rt_V = calData->fTimeV_rt_ns;
          if(!(IS_NAN(calData->fTimeV_lt_ns))) time_lt_V = calData->fTimeV_lt_ns;
          if(!(IS_NAN(calData->fTimeV_lb_ns))) time_lb_V = calData->fTimeV_lb_ns;
          if(!(IS_NAN(calData->fTimeV_rb_ns))) time_rb_V = calData->fTimeV_rb_ns; 
          
	    }
       else if(iCha == 1)
	   {                 
  // Channels 1-4:      
          if(!(IS_NAN(calData->fTimeL_r_ns))) time_r_L = calData->fTimeL_r_ns;
          if(!(IS_NAN(calData->fTimeT_r_ns))) time_r_T = calData->fTimeT_r_ns;
          if(!(IS_NAN(calData->fTimeL_t_ns))) time_t_L = calData->fTimeL_t_ns;
          if(!(IS_NAN(calData->fTimeT_t_ns))) time_t_T = calData->fTimeT_t_ns;  
          if(!(IS_NAN(calData->fTimeL_l_ns))) time_l_L = calData->fTimeL_l_ns;
          if(!(IS_NAN(calData->fTimeT_l_ns))) time_l_T = calData->fTimeT_l_ns; 
          if(!(IS_NAN(calData->fTimeL_b_ns))) time_b_L = calData->fTimeL_b_ns;
          if(!(IS_NAN(calData->fTimeT_b_ns))) time_b_T = calData->fTimeT_b_ns;  
          // Channels 5-8:
          if(!(IS_NAN(calData->fTimeL_rt_ns))) time_rt_L = calData->fTimeL_rt_ns;
          if(!(IS_NAN(calData->fTimeT_rt_ns))) time_rt_T = calData->fTimeT_rt_ns;
          if(!(IS_NAN(calData->fTimeL_lt_ns))) time_lt_L = calData->fTimeL_lt_ns;
          if(!(IS_NAN(calData->fTimeT_lt_ns))) time_lt_T = calData->fTimeT_lt_ns;  
          if(!(IS_NAN(calData->fTimeL_lb_ns))) time_lb_L = calData->fTimeL_lb_ns;
          if(!(IS_NAN(calData->fTimeT_lb_ns))) time_lb_T = calData->fTimeT_lb_ns; 
          if(!(IS_NAN(calData->fTimeL_rb_ns))) time_rb_L = calData->fTimeL_rb_ns;
          if(!(IS_NAN(calData->fTimeT_rb_ns))) time_rb_T = calData->fTimeT_rb_ns;  
                     
         }           
      }
      
          LOG(DEBUG) << "test1 : "<<flosOffsetX1<<"  "<<flosOffsetY1<<"  "<<flosVeffX1<<"  "<<flosVeffY1<<FairLogger::endl;

	                 
   if(iDet==1){

	  UInt_t iLOSTypeMCFD = 0;
	  UInt_t iLOSTypeTAMEX = 0;
	  UInt_t iLOSType = 0;
	  	 
      if(time_r_V > 0. && !(IS_NAN(time_r_V)) && time_t_V > 0. && !(IS_NAN(time_t_V)) && time_l_V > 0. && !(IS_NAN(time_l_V)) && time_b_V > 0. && !(IS_NAN(time_b_V)) 
      && IS_NAN(time_rt_V) && IS_NAN(time_rb_V) && IS_NAN(time_lt_V) && IS_NAN(time_lb_V))
      {        
       iLOSTypeMCFD = 1; // 4PM LOS
      }
      if(time_r_V > 0. && !(IS_NAN(time_r_V)) && time_t_V > 0. && !(IS_NAN(time_t_V)) && time_l_V > 0. && !(IS_NAN(time_l_V)) && time_b_V > 0. && !(IS_NAN(time_b_V)) 
      && !(IS_NAN(time_rt_V)) && !(IS_NAN(time_rb_V)) && !(IS_NAN(time_lt_V)) && !(IS_NAN(time_lb_V))
      && time_rt_V > 0. && time_lt_V > 0. && time_lb_V > 0. && time_rb_V > 0.)
      {        
       iLOSTypeMCFD = 2; // 8PM LOS
      } 
      if(time_r_L > 0. && !(IS_NAN(time_r_L)) && time_t_L > 0. && !(IS_NAN(time_t_L)) && time_l_L > 0. && !(IS_NAN(time_l_L)) && time_b_L > 0. && !(IS_NAN(time_b_L))
      && time_r_T > 0. && !(IS_NAN(time_r_T)) && time_t_T > 0. && !(IS_NAN(time_t_T)) && time_l_T > 0. && !(IS_NAN(time_l_T)) && time_b_T > 0. && !(IS_NAN(time_b_T))
      && IS_NAN(time_rt_L) && IS_NAN(time_rb_L) && IS_NAN(time_lt_L) && IS_NAN(time_lb_L))
      {        
       iLOSTypeTAMEX = 1;  // 4PM LOS
      }
      if(time_r_L > 0. && !(IS_NAN(time_r_L)) && time_t_L > 0. && !(IS_NAN(time_t_L)) && time_l_L > 0. && !(IS_NAN(time_l_L)) && time_b_L > 0. && !(IS_NAN(time_b_L))
      && time_r_T > 0. && !(IS_NAN(time_r_T)) && time_t_T > 0. && !(IS_NAN(time_t_T)) && time_l_T > 0. && !(IS_NAN(time_l_T)) && time_b_T > 0. && !(IS_NAN(time_b_T))
      && !(IS_NAN(time_rt_L)) && !(IS_NAN(time_rb_L)) && !(IS_NAN(time_lt_L)) && !(IS_NAN(time_lb_L))
      && time_rt_L > 0. && time_lt_L > 0. && time_lb_L > 0. && time_rb_L > 0.
      && !(IS_NAN(time_rt_T)) && !(IS_NAN(time_rb_T)) && !(IS_NAN(time_lt_T)) && !(IS_NAN(time_lb_T))
      && time_rt_T > 0. && time_lt_T > 0. && time_lb_T > 0. && time_rb_T > 0.)
      {        
       iLOSTypeTAMEX = 2; // 8PMLOS
      }            
      
// We will consider only events in which booth MCFD and TAMEX see same number of channels:
      if(iLOSTypeTAMEX == 1 && iLOSTypeMCFD == 1) iLOSType = 1;
      if(iLOSTypeTAMEX == 2 && iLOSTypeMCFD == 2) iLOSType = 2;
       
           if(iLOSType == 1 || iLOSType == 2) 
          {
            t_hor_M = (time_r_V + time_l_V)/2.; 		  
	    t_ver_M = (time_t_V + time_b_V)/2.;
	    x1_cm=(time_r_V-time_l_V-flosOffsetX1)*flosVeffX1;
	    y1_cm=(time_b_V-time_t_V-flosOffsetY1)*flosVeffY1;
			
            t_hor_T = (time_r_L + time_l_L)/2.;       		  
            t_ver_T = (time_t_L + time_b_L)/2.;	
            
            while(time_r_T - time_r_L <0.) 
            {
	         time_r_T=time_r_T+2048.*fClockFreq; 
	        }  
            while(time_l_T - time_l_L <0.) 
            {
	         time_l_T=time_l_T+2048.*fClockFreq; 
	        }	      
            while(time_t_T - time_t_L <0.) 
            {
	         time_t_T=time_t_T+2048.*fClockFreq; 
	        }	                 
            while(time_b_T - time_b_L <0.) 
            {
	         time_b_T=time_b_T+2048.*fClockFreq; 
	        }
	     totr = time_r_T - time_r_L;
             totl = time_l_T - time_l_L;
             tott = time_t_T - time_t_L;
             totb = time_b_T - time_b_L;
             
             tot_ver = (tott+totb)/2.;
  	     tot_hor = (totr+totl)/2.;
     			
		  }
		  	  
          if(iLOSType == 2 ) 
          {
            t_45_M  = (time_rb_V + time_lt_V)/2.; 		  
	    t_135_M = (time_rt_V + time_lb_V)/2.; 
	    x2_cm=(time_rb_V-time_lt_V-flosOffsetX2)*flosVeffX2;
	    y2_cm=(time_rt_V-time_lb_V-flosOffsetY2)*flosVeffY2;	
		
	    t_45_T = (time_rb_L + time_lt_L)/2.; 		  
	    t_135_T = (time_rt_L + time_lb_L)/2.;
	        
	   while(time_rt_T - time_rt_L <0.) 
	       {
	        time_rt_T=time_rt_T+2048.*fClockFreq; 
	       }  
           while(time_lt_T - time_lt_L <0.) 
           {
	        time_lt_T=time_lt_T+2048.*fClockFreq; 
	       }	      
           while(time_rb_T - time_rb_L <0.) 
           {
	        time_rb_T=time_rb_T+2048.*fClockFreq; 
	       }	                 
           while(time_lb_T - time_lb_L <0.) 
           {
	        time_lb_T=time_lb_T+2048.*fClockFreq; 
	       } 
            totrb = time_rb_T - time_rb_L;
            totlt = time_lt_T - time_lt_L;
            totrt = time_rt_T - time_rt_L;
            totlb = time_lb_T - time_lb_L;	
            
            tot_135 = (totrt+totlb)/2.;
            tot_45 = (totlt+totrb)/2.;
        	
	      }	      	  
		  
          if (iLOSType == 1) 
          {
            timeLosM = (t_hor_M + t_ver_M)/2.;
            LosTresM = t_hor_M - t_ver_M;
            fh_los_tres_MCFD->Fill(LosTresM);
            x_cm = x1_cm;
            y_cm = y1_cm;
            fh_los_pos->Fill(x_cm,y_cm);
            
            timeLosT = (t_hor_T + t_ver_T)/2.;
            LosTresT = t_hor_T - t_ver_T;         
            fh_los_tres_TAMEX->Fill(LosTresT);
            
            tot = (tot_ver+tot_hor)/2.;
            fh_los_tot->Fill(tot);
        
          }
          
	if (iLOSType == 2) 
          {
            timeLosM = (t_hor_M + t_ver_M + t_45_M + t_135_M)/4.;
            LosTresM = ((t_hor_M + t_ver_M) - (t_45_M + t_135_M))/2.;
            fh_los_tres_MCFD->Fill(LosTresM);
            x_cm = (x1_cm + (x2_cm/sqrt(2.)+y2_cm/sqrt(2.)))/2.;
	    y_cm = (y2_cm + (y2_cm/sqrt(2.)-x2_cm/sqrt(2.)))/2.;
            fh_los_pos->Fill(x_cm,y_cm);
	    
	    timeLosT = (t_hor_T + t_ver_T + t_45_T + t_135_T)/4.;
            LosTresT = ((t_hor_T + t_ver_T) - (t_45_T + t_135_T))/2.;         
            fh_los_tres_TAMEX->Fill(LosTresT);
	    
	    tot = (tot_ver+tot_hor+tot_45+tot_135)/4.;
            fh_los_tot->Fill(tot);
           }
      }
    } else {
	  cout<<"Wrong detector ID for LOS!"<<endl;
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
	      while(t1l-timeLosM<0.){
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
	      while(t1l-timeLosM<0.){
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
	      while(t1l-timeLosM<0.){
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
