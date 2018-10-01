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

#include "R3BSci8CalData.h"
#include "R3BSci8MappedData.h"

#include "R3BTofdCalData.h"
#include "R3BTofdMappedData.h"

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
#include "THttpServer.h"

#include "TClonesArray.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <array>
#include "TMath.h"
#include <cstdlib>
#include <ctime>
#include <TRandom3.h>
#include <TRandomGen.h>
#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;


R3BOnlineSpectra::R3BOnlineSpectra()
    : FairTask("OnlineSpectra", 1)
    , fCalItemsLos(NULL)
    , fCalItemsSci8(NULL)
    , fCalItemsTofd(NULL)
    , fCalItemsPspx(NULL)
    , fHitItemsFi0(NULL)
    , fHitItemsFi1a(NULL)
    , fHitItemsFi1b(NULL)
    , fHitItemsFi2a(NULL)
    , fHitItemsFi2b(NULL)
    , fHitItemsFi3a(NULL)
    , fHitItemsFi3b(NULL)
    , fHitItemsFi4(NULL)
    , fHitItemsFi5(NULL)
    , fHitItemsFi6(NULL)
    , fHitItemsFi7(NULL)
    , fHitItemsFi8(NULL)
    , fHitItemsFi9(NULL)
    , fHitItemsFi10(NULL)
    , fHitItemsFi11(NULL)
    , fCalItemsPtof(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsSci8(NULL)
    , fMappedItemsTofd(NULL)
    , fMappedItemsPspx(NULL)
    , fMappedItemsFi0(NULL)
    , fMappedItemsFi1a(NULL)
    , fMappedItemsFi1b(NULL)
    , fMappedItemsFi2a(NULL)
    , fMappedItemsFi2b(NULL)
    , fMappedItemsFi3a(NULL)
    , fMappedItemsFi3b(NULL)
    , fMappedItemsFi4(NULL)
    , fMappedItemsFi5(NULL)
    , fMappedItemsFi6(NULL)
    , fMappedItemsFi7(NULL)
    , fMappedItemsFi8(NULL)
    , fMappedItemsFi9(NULL)
    , fMappedItemsFi10(NULL)
    , fMappedItemsFi11(NULL)
    , fTrigger(-1)
    , fNofPlanes(4)  
    , fPaddlesPerPlane(6)     
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
    , fNEvents1(0)
{
}

R3BOnlineSpectra::R3BOnlineSpectra(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fCalItemsSci8(NULL)
    , fCalItemsTofd(NULL)
    , fCalItemsPspx(NULL)
    , fHitItemsFi0(NULL)
    , fHitItemsFi1a(NULL)
    , fHitItemsFi1b(NULL)
    , fHitItemsFi2a(NULL)
    , fHitItemsFi2b(NULL)
    , fHitItemsFi3a(NULL)
    , fHitItemsFi3b(NULL)
    , fHitItemsFi4(NULL)
    , fHitItemsFi5(NULL)
    , fHitItemsFi6(NULL)
    , fHitItemsFi7(NULL)
    , fHitItemsFi8(NULL)
    , fHitItemsFi9(NULL)
    , fHitItemsFi10(NULL)
    , fHitItemsFi11(NULL)
    , fCalItemsPtof(NULL)
    , fMappedItemsLos(NULL)
    , fMappedItemsSci8(NULL)
    , fMappedItemsTofd(NULL)
    , fMappedItemsPspx(NULL)
    , fMappedItemsFi0(NULL)
    , fMappedItemsFi1a(NULL)
    , fMappedItemsFi1b(NULL)
    , fMappedItemsFi2a(NULL)
    , fMappedItemsFi2b(NULL)
    , fMappedItemsFi3a(NULL)
    , fMappedItemsFi3b(NULL)
    , fMappedItemsFi4(NULL)
    , fMappedItemsFi5(NULL)
    , fMappedItemsFi6(NULL)
    , fMappedItemsFi7(NULL)
    , fMappedItemsFi8(NULL)
    , fMappedItemsFi9(NULL)
    , fMappedItemsFi10(NULL)
    , fMappedItemsFi11(NULL)
    , fTrigger(-1)
    , fNofPlanes(4)  
    , fPaddlesPerPlane(6)    
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
    , fNEvents1(0)
{
}

R3BOnlineSpectra::~R3BOnlineSpectra()
{
   for(int i=0; i<14; i++){	
	if(fh_channels_Fib[i]) delete(fh_channels_Fib[i]);
    if(fh_fibers_Fib[i]) delete(fh_fibers_Fib[i]);
    if(fh_mult_Fib[i]) delete(fh_mult_Fib[i]);
    if(fh_time_Fib[i]) delete(fh_time_Fib[i]);
    if(fh_multihit_m_Fib[i]) delete(fh_multihit_m_Fib[i]);   
    if(fh_multihit_s_Fib[i]) delete(fh_multihit_s_Fib[i]);
    if(fh_ToT_m_Fib[i]) delete(fh_ToT_m_Fib[i]);
    if(fh_ToT_s_Fib[i]) delete(fh_ToT_s_Fib[i]);
   } 
}

InitStatus R3BOnlineSpectra::Init()
{

	// Initialize random number:
	std::srand(std::time(0)); //use current time as seed for random generator


	LOG(INFO) << "R3BOnlineSpectra::Init " << FairLogger::endl;

	// try to get a handle on the EventHeader. EventHeader may not be 
	// present though and hence may be null. Take care when using.

	FairRootManager* mgr = FairRootManager::Instance();
	if (NULL == mgr)
		FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
	header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
	FairRunOnline *run = FairRunOnline::Instance();

	run->GetHttpServer()->Register("/Tasks", this);

	// create histograms of all detectors  

	//------------------------------------------------------------------------ 
	// Sci8 detector
	// get access to Mapped data
	fMappedItemsSci8 = (TClonesArray*)mgr->GetObject("Sci8Mapped");
	// get access to cal data
	fCalItemsSci8 = (TClonesArray*)mgr->GetObject("Sci8Cal");


	if(fMappedItemsSci8){
		TCanvas *cSci8 = new TCanvas("Sci8", "SCI8", 10, 10, 850, 850);

		fh_sci8_channels = new TH1F("sci8_channels", "SCI8 channels", 4, 0., 4.); 
		fh_sci8_channels->GetXaxis()->SetTitle("Channel number");
		fh_sci8_multihit = new TH1F("sci8_multihit", "SCI8 multihit",20, 0., 20.);
		fh_sci8_multihit->GetXaxis()->SetTitle("Multihit");		
		fh_sci8_multihitVFTX = new TH2F("sci8_vftx_multihit", "SCI8 VFTX multihit",4, 0., 4.,20, 0., 20.);
		fh_sci8_multihitVFTX->GetYaxis()->SetTitle("Multihit ");
		fh_sci8_multihitVFTX->GetXaxis()->SetTitle("Channel number");
		fh_sci8_multihitLEAD = new TH2F("sci8_lead_multihit", "SCI8 TAMEX Leading multihit",4, 0., 4.,20, 0., 20.);
		fh_sci8_multihitLEAD->GetYaxis()->SetTitle("Multihit ");
		fh_sci8_multihitLEAD->GetXaxis()->SetTitle("Channel number");
		fh_sci8_multihitTRAI = new TH2F("sci8_trai_multihit", "SCI8 TAMEX Trailing multihit",4, 0., 4.,20, 0., 20.);
		fh_sci8_multihitTRAI->GetYaxis()->SetTitle("Multihit ");		
		fh_sci8_multihitTRAI->GetXaxis()->SetTitle("Channel number");
		fh_sci8_dt_hits = new TH1F("sci8_dt_hits", "SCI8 dt multihits",40000,-2000,2000);
		fh_sci8_dt_hits->GetXaxis()->SetTitle("dt_VFTX between two hits / ns");
		fh_sci8_dt_hits_l = new TH1F("sci8_dt_hits_l", "SCI8 dt multihits leading",40000,-2000,2000);
		fh_sci8_dt_hits_l->GetXaxis()->SetTitle("dt_LEAD between two hits / ns");
		fh_sci8_dt_hits_t = new TH1F("sci8_dt_hits_t", "SCI8 dt multihits trailing",40000,-2000,2000);
		fh_sci8_dt_hits_t->GetXaxis()->SetTitle("dt_TRAIL between two hits / ns");
		fh_sci8_tres_MCFD = new TH1F("sci8_time_res_MCFD", "SCI8 MCFD Time resolution - raw", 8000, -4., 4.);
		fh_sci8_tres_MCFD->GetXaxis()->SetTitle("Time MCFD / ns");
		fh_sci8_tres_TAMEX = new TH1F("sci8_time_res_TAMEX", "SCI8 TAMEX Time resolution -raw ", 8000, -4., 4.);  
		fh_sci8_tres_TAMEX->GetXaxis()->SetTitle("Time TAMEX / ns"); 
		fh_sci8_tot = new TH2F("sci8_tot","SCI8 ToT vs PM",4,0,4,3000,0.,300.); 
		fh_sci8_tot->GetXaxis()->SetTitle("PMT number");
		fh_sci8_tot->GetYaxis()->SetTitle("ToT / ns");
		fh_sci8_tot_mean = new TH1F("sci8_tot_mean","SCI8 mean ToT",30000,0.,300.); 
		fh_sci8_tot_mean->GetYaxis()->SetTitle("Counts");
		fh_sci8_tot_mean->GetXaxis()->SetTitle("ToT / ns");
		fh_tof_sci8 = new TH1F("sci8_tof", "SCI8-LOS ToF",40000,-20,20);
		fh_tof_sci8->GetXaxis()->SetTitle("ToF / ns");


		cSci8->Divide(3, 3);
		cSci8->cd(1);
		fh_sci8_channels->Draw();		
		cSci8->cd(2);gPad->SetLogy();
		fh_sci8_multihit->Draw();
		cSci8->cd(3);gPad->SetLogz();
		fh_sci8_tot->Draw("colz");	
		cSci8->cd(4);
		fh_sci8_tres_MCFD->Draw();
		cSci8->cd(5);
		fh_sci8_tres_TAMEX->Draw();
		cSci8->cd(6);
		fh_tof_sci8->Draw();		
		cSci8->cd(7);
		fh_sci8_tot_mean->Draw();

		cSci8->cd(0);
		run->AddObject(cSci8);

		run->GetHttpServer()->RegisterCommand("Reset_SCI8", Form("/Tasks/%s/->Reset_SCI8_Histo()", GetName()));
		/*
		   TCanvas *cSci8a = new TCanvas("Sci8_adittional", "SCI8_additional", 10, 10, 850, 750);

		   cSci8a->Divide(3, 2);
		   cSci8a->cd(1);		
		   fh_sci8_dt_hits->Draw();	
		   cSci8a->cd(2);		
		   fh_sci8_dt_hits_l->Draw();	
		   cSci8a->cd(3);		
		   fh_sci8_dt_hits_t->Draw();				
		   cSci8a->cd(4);gPad->SetLogz();
		   fh_sci8_multihitVFTX->Draw("colz");		
		   cSci8a->cd(5);gPad->SetLogz();
		   fh_sci8_multihitLEAD->Draw("colz");		
		   cSci8a->cd(6);gPad->SetLogz();
		   fh_sci8_multihitTRAI->Draw("colz");		
		   cSci8a->cd(0);
		 */ 
	}

	//------------------------------------------------------------------------ 
	// Los detector
	// get access to Mapped data
	fMappedItemsLos = (TClonesArray*)mgr->GetObject("LosMapped");
	// get access to cal data
	fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");


	if(fMappedItemsLos){
		TCanvas *cLos = new TCanvas("Los", "LOS", 10, 10, 750, 850);


		fh_los_channels = new TH1F("los_channels", "LOS channels", 10, 0., 10.); 
		fh_los_channels->GetXaxis()->SetTitle("Channel number");
		fh_los_multihit = new TH1F("los_multihit", "LOS multihit",20, 0., 20.);
		fh_los_multihit->GetXaxis()->SetTitle("Multihit");		
		fh_los_multihitVFTX = new TH2F("los_vftx_multihit", "LOS VFTX multihit",10, 0., 10.,20, 0., 20.);
		fh_los_multihitVFTX->GetYaxis()->SetTitle("Multihit ");
		fh_los_multihitVFTX->GetXaxis()->SetTitle("Channel number");
		fh_los_multihitLEAD = new TH2F("los_lead_multihit", "LOS TAMEX Leading multihit",10, 0., 10.,20, 0., 20.);
		fh_los_multihitLEAD->GetYaxis()->SetTitle("Multihit ");
		fh_los_multihitLEAD->GetXaxis()->SetTitle("Channel number");
		fh_los_multihitTRAI = new TH2F("los_trai_multihit", "LOS TAMEX Trailing multihit",10, 0., 10.,20, 0., 20.);
		fh_los_multihitTRAI->GetYaxis()->SetTitle("Multihit ");		
		fh_los_multihitTRAI->GetXaxis()->SetTitle("Channel number");
		fh_los_dt_hits = new TH1F("los_dt_hits", "LOS dt multihits",40000,-2000,2000);
		fh_los_dt_hits->GetXaxis()->SetTitle("dt_VFTX between two hits / ns");
		fh_los_dt_hits_l = new TH1F("los_dt_hits_l", "LOS dt multihits leading",40000,-2000,2000);
		fh_los_dt_hits_l->GetXaxis()->SetTitle("dt_LEAD between two hits / ns");
		fh_los_dt_hits_t = new TH1F("los_dt_hits_t", "LOS dt multihits trailing",40000,-2000,2000);
		fh_los_dt_hits_t->GetXaxis()->SetTitle("dt_TRAIL between two hits / ns");
		fh_los_tres_MCFD = new TH1F("los_time_res_MCFD", "LOS MCFD Time resolution - raw", 8000, -4., 4.);
		fh_los_tres_MCFD->GetXaxis()->SetTitle("Time MCFD / ns");
		fh_los_tres_TAMEX = new TH1F("los_time_res_TAMEX", "LOS TAMEX Time resolution -raw ", 8000, -4., 4.);  
		fh_los_tres_TAMEX->GetXaxis()->SetTitle("Time TAMEX / ns"); 
		fh_los_tot = new TH2F("los_tot","LOS ToT vs PM",10,0,10,3000,0.,300.); 
		fh_los_tot->GetXaxis()->SetTitle("PMT number");
		fh_los_tot->GetYaxis()->SetTitle("ToT / ns");
		fh_los_tot_mean = new TH1F("los_tot_mean","LOS mean ToT",30000,0.,300.); 
		fh_los_tot_mean->GetYaxis()->SetTitle("Counts");
		fh_los_tot_mean->GetXaxis()->SetTitle("ToT / ns");

		cLos->Divide(2,3);
		cLos->cd(1);
		fh_los_channels->Draw();		
		cLos->cd(2);gPad->SetLogy();
		fh_los_multihit->Draw();
		cLos->cd(3);gPad->SetLogz();
		fh_los_tot->Draw("colz");
		cLos->cd(4);
		fh_los_tot_mean->Draw();	
		cLos->cd(5);
		fh_los_tres_MCFD->Draw();
		cLos->cd(6);
		fh_los_tres_TAMEX->Draw();		
		cLos->cd(0);
		run->AddObject(cLos);

		run->GetHttpServer()->RegisterCommand("Reset_LOS", Form("/Tasks/%s/->Reset_LOS_Histo()", GetName()));

		TCanvas *cLos1 = new TCanvas("Los_additional", "LOS-additional", 10, 10, 850, 950);
		cLos1->Divide(3, 4);
		cLos1->cd(4);		
		fh_los_dt_hits->Draw();	
		cLos1->cd(5);		
		fh_los_dt_hits_l->Draw();	
		cLos1->cd(6);		
		fh_los_dt_hits_t->Draw();				
		cLos1->cd(7);gPad->SetLogz();
		fh_los_multihitVFTX->Draw("colz");		
		cLos1->cd(8);gPad->SetLogz();
		fh_los_multihitLEAD->Draw("colz");		
		cLos1->cd(9);gPad->SetLogz();
		fh_los_multihitTRAI->Draw("colz");		
		cLos1->cd(0);

	}


	//-----------------------------------------------------------------------
	// Fiber Detectors 1-14

	// First see if a given FiberI is present (there is probably a better way to do it):
	fMappedItemsFi1a = (TClonesArray*)mgr->GetObject("Fi1aMapped");
	if(fMappedItemsFi1a) FibPresent[0] = true;
	fMappedItemsFi1b = (TClonesArray*)mgr->GetObject("Fi1bMapped");
	if(fMappedItemsFi1b) FibPresent[1] = true;
	fMappedItemsFi2a = (TClonesArray*)mgr->GetObject("Fi2aMapped");
	if(fMappedItemsFi2a) FibPresent[2] = true;  
	fMappedItemsFi2b = (TClonesArray*)mgr->GetObject("Fi2bMapped");
	if(fMappedItemsFi2b) FibPresent[3] = true;      
	fMappedItemsFi3a = (TClonesArray*)mgr->GetObject("Fi3aMapped");
	if(fMappedItemsFi3a) FibPresent[4] = true;      
	fMappedItemsFi3b = (TClonesArray*)mgr->GetObject("Fi3bMapped");
	if(fMappedItemsFi3b) FibPresent[5] = true;      
	fMappedItemsFi4 = (TClonesArray*)mgr->GetObject("Fi4Mapped");
	if(fMappedItemsFi4) FibPresent[6] = true;      
	fMappedItemsFi5 = (TClonesArray*)mgr->GetObject("Fi5Mapped");
	if(fMappedItemsFi5) FibPresent[7] = true;     
	fMappedItemsFi6 = (TClonesArray*)mgr->GetObject("Fi6Mapped");
	if(fMappedItemsFi6) FibPresent[8] = true;      
	fMappedItemsFi7 = (TClonesArray*)mgr->GetObject("Fi7Mapped");
	if(fMappedItemsFi7) FibPresent[9] = true;      
	fMappedItemsFi8 = (TClonesArray*)mgr->GetObject("Fi8Mapped");
	if(fMappedItemsFi8) FibPresent[10] = true;     
	fMappedItemsFi9 = (TClonesArray*)mgr->GetObject("Fi9Mapped");
	if(fMappedItemsFi9) FibPresent[11] = true;      
	fMappedItemsFi10 = (TClonesArray*)mgr->GetObject("Fi10Mapped");
	if(fMappedItemsFi10) FibPresent[12] = true;      
	fMappedItemsFi11 = (TClonesArray*)mgr->GetObject("Fi11Mapped");
	if(fMappedItemsFi11) FibPresent[13] = true; 

	char canvName[255];
	TCanvas *FibCanvas[14];

	for(Int_t ifibcount = 0; ifibcount < 14; ifibcount++){

		std::stringstream fibhitobj;
		std::stringstream fibmapobj;
		std::stringstream FiName;   
		std::string temp;	  

		std::stringstream histName1,histName2,histName3,histName4,histName5,histName6,histName7,histName8,histName9,histName10,histName11,histName12;
		std::stringstream histTitle1,histTitle2,histTitle3,histTitle4,histTitle5,histTitle6,histTitle7,histTitle8,histTitle9,histTitle10,histTitle11,histTitle12;	   	    	 

		if(FibPresent[ifibcount]){
			if(ifibcount < 6){
				if(ifibcount == 0 || ifibcount ==1) ifibdet = 1; 
				if(ifibcount == 2 || ifibcount ==3) ifibdet = 2; 
				if(ifibcount == 4 || ifibcount ==5) ifibdet = 3;
				if(ifibcount % 2 == 0) FiName<<"Fi"<<ifibdet<<"a";	
				else FiName<<"Fi"<<ifibdet<<"b";				 
			}	
			if(ifibcount > 5) {
				ifibdet = ifibcount-2;
				FiName<<"Fi"<<ifibdet;
			}	      

			// get access to Mapped data     
			fibmapobj<<FiName.str()<<"Mapped";  
			temp = fibmapobj.str();
			cMapped[ifibcount] = Mapped[ifibcount].c_str();
			aMapped.at(ifibcount) = (TClonesArray*)mgr->GetObject(cMapped[ifibcount]); 

			// get access to cal data     
			fibhitobj<<FiName.str()<<"Hit";  
			temp = fibhitobj.str();
			cHit[ifibcount] = Hit[ifibcount].c_str(); 
			aHit.at(ifibcount) = (TClonesArray*)mgr->GetObject(cHit[ifibcount]);
		} 

		if(aMapped.at(ifibcount)){

			const char* chistName;
			const char* chistTitle;
			const char* cfibName;
			std::string tempTitle;
			std::string tempName;
			std::stringstream tempFibName;
			std::string tempFibNames;
			std::stringstream tempCanvName;
			std::string stempCanvNames;

			tempFibNames = FiName.str(); 
			cfibName = tempFibNames.c_str();
			stempCanvNames = FiName.str(); 

			cout<<"I am creating canvas "<<stempCanvNames.c_str()<<endl;

			FibCanvas[ifibcount]=new TCanvas(stempCanvNames.c_str(),stempCanvNames.c_str(), 10, 10, 910, 910);

			// Channels:   
			histName1<<cfibName<<"_channels";
			tempName=histName1.str();
			chistName=tempName.c_str();
			histTitle1<<cfibName<<" channels";
			tempTitle=histTitle1.str();
			chistTitle=tempTitle.c_str();
			fh_channels_Fib[ifibcount] = new TH1F(chistName, chistTitle, 520, 0., 520.);
			fh_channels_Fib[ifibcount]->GetXaxis()->SetTitle("Channel number");
			fh_channels_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");
			tempName.clear();
			tempTitle.clear();      
			// Fibers:
			histName2<<cfibName<<"_fibers";
			tempName=histName2.str();
			chistName=tempName.c_str();
			histTitle2<<cfibName<<" fibers";
			tempTitle=histTitle2.str();
			chistTitle=tempTitle.c_str();
			fh_fibers_Fib[ifibcount] = new TH1F(chistName, chistTitle, 2100, 0., 2100.); 
			fh_fibers_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
			fh_fibers_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");
			tempName.clear();
			tempTitle.clear();
			// Multiplicity (no of hitted fibers):
			histName3<<cfibName<<"_mult";
			tempName=histName3.str();
			chistName=tempName.c_str();
			histTitle3<<cfibName<<" No hitted fibers ";
			tempTitle=histTitle3.str();
			chistTitle=tempTitle.c_str();
			fh_mult_Fib[ifibcount] = new TH1F(chistName, chistTitle, 100, 0., 100.);	   
			fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
			fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");
			tempName.clear();
			tempTitle.clear();   
			// Multihit MAPMT:
			histName5<<cfibName<<"_multihit_m";
			tempName=histName5.str();
			chistName=tempName.c_str();
			histTitle5<<cfibName<<" multihits MAPMT";
			tempTitle=histTitle5.str();
			chistTitle=tempTitle.c_str();   
			fh_multihit_m_Fib[ifibcount] = new TH2F(chistName, chistTitle, 520, 0., 520., 20, 0., 20.);
			fh_multihit_m_Fib[ifibcount]->GetXaxis()->SetTitle("MAPMT channel");
			fh_multihit_m_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");
			tempName.clear();
			tempTitle.clear();	   
			// Multihit SAPMT:
			histName6<<cfibName<<"_multihit_s";
			tempName=histName6.str();
			chistName=tempName.c_str();
			histTitle6<<cfibName<<" multihits SAPMT";
			tempTitle=histTitle6.str();
			chistTitle=tempTitle.c_str();	   
			fh_multihit_s_Fib[ifibcount] = new TH2F(chistName, chistTitle, 16, 0., 16., 20, 0., 20.);   	   
			fh_multihit_s_Fib[ifibcount]->GetXaxis()->SetTitle("SAPMT channel");
			fh_multihit_s_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");
			tempName.clear();
			tempTitle.clear(); 
			// ToT MAPMT:  
			histName7<<cfibName<<"_tot_m";
			tempName=histName7.str();
			chistName=tempName.c_str();
			histTitle7<<cfibName<<" ToT of MAPMT";
			tempTitle=histTitle7.str();
			chistTitle=tempTitle.c_str();	   
			fh_ToT_m_Fib[ifibcount] = new TH2F(chistName, chistTitle, 2100, 0., 2100., 400, 0., 200.);   	   
			fh_ToT_m_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
			fh_ToT_m_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");
			tempName.clear();
			tempTitle.clear();
			// ToT SAPMT:  
			histName8<<cfibName<<"_tot_s";
			tempName=histName8.str();
			chistName=tempName.c_str();
			histTitle8<<cfibName<<" ToT of SAPMT";
			tempTitle=histTitle8.str();
			chistTitle=tempTitle.c_str();	   
			fh_ToT_s_Fib[ifibcount] = new TH2F(chistName, chistTitle, 2100, 0., 2100., 400, 0., 200.);   	   
			fh_ToT_s_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
			fh_ToT_s_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");
			tempName.clear();
			tempTitle.clear();	   
			// Time of fiber: 
			histName9<<cfibName<<"_TimevsFiber";
			tempName=histName9.str();
			chistName=tempName.c_str();
			histTitle9<<cfibName<<" Time vs Fiber";
			tempTitle=histTitle9.str();
			chistTitle=tempTitle.c_str();
			fh_time_Fib[ifibcount]= new TH2F(chistName, chistTitle, 2100, 0., 2100., 20000, -1024., 1024.);
			fh_time_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
			fh_time_Fib[ifibcount]->GetYaxis()->SetTitle("tMAPMT-tSPMT");
			tempName.clear();
			tempTitle.clear();
			// ToF LOS -> Fiber:
			histName11<<cfibName<<"_tof";
			tempName=histName11.str();
			chistName=tempName.c_str();
			histTitle11<<cfibName<<" ToF LOS to Fiber ";
			tempTitle=histTitle11.str();
			chistTitle=tempTitle.c_str();
			fh_Fib_ToF[ifibcount] = new TH2F(chistName, chistTitle, 2100, 0., 2100.,10000, -5000., 5000.);	   
			fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
			fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("Fiber number");
			tempName.clear();
			tempTitle.clear();         
			// Not-calibrated position:
			histName12<<cfibName<<"_pos";
			tempName=histName12.str();
			chistName=tempName.c_str();
			histTitle12<<cfibName<<" Not-calibrated position ";
			tempTitle=histTitle12.str();
			chistTitle=tempTitle.c_str();
			fh_Fib_pos[ifibcount] = new TH1F(chistName, chistTitle, 6000, -1500., 1500.);	   
			fh_Fib_pos[ifibcount]->GetXaxis()->SetTitle("Position");
			fh_Fib_pos[ifibcount]->GetYaxis()->SetTitle("Counts");
			tempName.clear();
			tempTitle.clear();

			FibCanvas[ifibcount]->Divide(3, 3);
			FibCanvas[ifibcount]->cd(1);
			fh_channels_Fib[ifibcount]->Draw();
			FibCanvas[ifibcount]->cd(2);gPad->SetLogz();
			fh_multihit_m_Fib[ifibcount]->Draw("colz"); 
			FibCanvas[ifibcount]->cd(3);gPad->SetLogz();
			fh_multihit_s_Fib[ifibcount]->Draw("colz");      
			FibCanvas[ifibcount]->cd(4);
			fh_fibers_Fib[ifibcount]->Draw();
			FibCanvas[ifibcount]->cd(5);
			fh_mult_Fib[ifibcount]->Draw();      
			FibCanvas[ifibcount]->cd(6);gPad->SetLogz();
			fh_ToT_m_Fib[ifibcount]->Draw("colz");
			FibCanvas[ifibcount]->cd(7);gPad->SetLogz();
			fh_ToT_s_Fib[ifibcount]->Draw("colz");
			//FibCanvas[ifibcount]->cd(8);gPad->SetLogz();
			//fh_time_Fib[ifibcount]->Draw("colz");        
			FibCanvas[ifibcount]->cd(8);gPad->SetLogz();
			fh_Fib_ToF[ifibcount]->Draw("colz");
			FibCanvas[ifibcount]->cd(9);
			fh_Fib_pos[ifibcount]->Draw();

			FibCanvas[ifibcount]->cd(0);
			run->AddObject(FibCanvas[ifibcount]);     


		}	 // end if(Mapped)
	} // end for(ifibcount)


	//---------------------------------------------------------------------------------------
	//Ptof detector

	fCalItemsPtof = (TClonesArray*)mgr->GetObject("PtofCal");

	if(fCalItemsPtof){

		TCanvas *cPtof_plane = new TCanvas("Ptof_plane", "Ptof plane", 10, 10, 510, 510);
		cPtof_plane->Divide(1, 2);

		fh_ptof_channels = new TH1F("Ptof_channels", "Ptof channels", 65, 0., 65.);
		cPtof_plane->cd(1);
		fh_ptof_channels->Draw();

		fh_ptof_channels_cut = new TH1F("Ptof_channels_cut", "Ptof channels with cut", 65, 0., 65.);
		cPtof_plane->cd(2);
		fh_ptof_channels_cut->Draw();
		run->AddObject(cPtof_plane);

		TCanvas *cPtof_test = new TCanvas("Ptof_test", "Ptof test", 10, 10, 510, 510);
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
		cPtof1->Divide(3,3);
		for (Int_t j = 1; j < 10; j++){
			char strName[255];
			sprintf(strName, "ToT_Bar_%d_PM_1", j);
			fh_ptof_TotPm1[j] = new TH1F(strName, "", 1000, 0., 100.);
			cPtof1->cd(j);
			fh_ptof_TotPm1[j]->Draw();           
		}
		run->AddObject(cPtof1);

		TCanvas* cPtof2;
		cPtof2 = new TCanvas("Ptof2", "Ptof2", 10, 10, 500, 500);
		cPtof2->Divide(3,3);
		for (Int_t j = 1; j < 10; j++){
			char strName[255];
			sprintf(strName, "ToT_Bar_%d_PM_2", j);
			fh_ptof_TotPm2[j] = new TH1F(strName, "", 1000, 0., 100.);
			cPtof2->cd(j);
			fh_ptof_TotPm2[j]->Draw();           
		}
		run->AddObject(cPtof2);
	}


	//---------------------------------------------------------------------------------------------------
	//TofD detector

	fMappedItemsTofd = (TClonesArray*)mgr->GetObject("TofdMapped");
	if(fMappedItemsTofd){
		TCanvas *cTofd_planes = new TCanvas("TOFD_planes", "TOFD planes", 10, 10, 1100, 1000);
		cTofd_planes->Divide(5, 4);

		for (Int_t j = 0; j < 4; j++)
		{
			char strName1[255];
			sprintf(strName1, "tofd_channels_plane_%d", j+1);
			char strName2[255];
			sprintf(strName2, "Tofd channels plane %d", j+1);        
			fh_tofd_channels[j] = new TH1F(strName1, strName2, 90, -45., 45.);		 
			fh_tofd_channels[j]->GetXaxis()->SetTitle("Channel");
			fh_tofd_channels[j]->GetYaxis()->SetTitle("Counts");

			char strName3[255];
			sprintf(strName3, "tofd_ToT_plane_%d", j+1);
			char strName4[255];
			sprintf(strName4, "Tofd ToT plane %d", j+1);        
			fh_tofd_TotPm[j] = new TH2F(strName3, strName4, 90,-45,45,30000,0.,300.);
			fh_tofd_TotPm[j]->GetXaxis()->SetTitle("Bar number");
			fh_tofd_TotPm[j]->GetYaxis()->SetTitle("ToT / ns");

			char strName7[255];
			sprintf(strName7, "tofd_ToF_plane_%d", j+1);
			char strName8[255];
			sprintf(strName8, "TOFD-LOS ToF plane %d", j+1); 
			fh_tofd_ToF[j] = new TH2F(strName7, strName8, 15,0,15, 1e4, 0, 1e2);
			fh_tofd_ToF[j]->GetXaxis()->SetTitle("Bar number");
			fh_tofd_ToF[j]->GetYaxis()->SetTitle("ToF / ns");

			char strName9[255];
			sprintf(strName9, "tofd_multihit_plane_%d", j+1);
			char strName10[255];
			sprintf(strName10, "Tofd multihit plane %d", j+1);        
			fh_tofd_multihit[j] = new TH2F(strName9, strName10, 45, 0., 45., 10, 0, 10);		 
			fh_tofd_multihit[j]->GetXaxis()->SetTitle("Bar number");
			fh_tofd_multihit[j]->GetYaxis()->SetTitle("Multihit");

			if(j<3)
			{
				int jk = j + 1;
				char strName5[255];
				sprintf(strName5, "tofd_ToF_plane_%d_%d", jk, jk+1);
				char strName6[255];
				sprintf(strName6, "Tofd ToF plane %d and %d ", jk, jk+1); 
				fh_tofd_dt[j] = new TH2F(strName5, strName6, 50,0,50,8000, -40., 40);
				fh_tofd_dt[j]->GetXaxis()->SetTitle("Bar number");
				fh_tofd_dt[j]->GetYaxis()->SetTitle("dt / ns");
			}
		}

		cTofd_planes->cd(1);
		fh_tofd_channels[0]->Draw();
		cTofd_planes->cd(2);gPad->SetLogz();
		fh_tofd_TotPm[0]->Draw("colz");
		cTofd_planes->cd(3);gPad->SetLogz();
		fh_tofd_ToF[0]->Draw("colz");
		cTofd_planes->cd(4);gPad->SetLogz();
		fh_tofd_multihit[0]->Draw("colz");

		cTofd_planes->cd(6);
		fh_tofd_channels[1]->Draw();
		cTofd_planes->cd(7);gPad->SetLogz();
		fh_tofd_TotPm[1]->Draw("colz");
		cTofd_planes->cd(8);gPad->SetLogz();
		fh_tofd_ToF[1]->Draw("colz");
		cTofd_planes->cd(9);gPad->SetLogz();
		fh_tofd_multihit[1]->Draw("colz");
		cTofd_planes->cd(10);gPad->SetLogz();
		fh_tofd_dt[0]->Draw("colz");

		cTofd_planes->cd(11);
		fh_tofd_channels[2]->Draw();
		cTofd_planes->cd(12);gPad->SetLogz();
		fh_tofd_TotPm[2]->Draw("colz");	 
		cTofd_planes->cd(13);gPad->SetLogz();
		fh_tofd_ToF[2]->Draw("colz");
		cTofd_planes->cd(14);gPad->SetLogz();
		fh_tofd_multihit[2]->Draw("colz");
		cTofd_planes->cd(15);gPad->SetLogz();
		fh_tofd_dt[1]->Draw("colz");

		cTofd_planes->cd(16);
		fh_tofd_channels[3]->Draw();
		cTofd_planes->cd(17);gPad->SetLogz();
		fh_tofd_TotPm[3]->Draw("colz");		 
		cTofd_planes->cd(18);gPad->SetLogz();
		fh_tofd_ToF[3]->Draw("colz");
		cTofd_planes->cd(19);gPad->SetLogz();
		fh_tofd_multihit[3]->Draw("colz");
		cTofd_planes->cd(20);gPad->SetLogz();
		fh_tofd_dt[2]->Draw("colz");

		cTofd_planes->cd(0);
		run->AddObject(cTofd_planes);

		run->GetHttpServer()->RegisterCommand("Reset_TOFD", Form("/Tasks/%s/->Reset_TOFD_Histo()", GetName())); 
	}

	fCalItemsTofd = (TClonesArray*)mgr->GetObject("TofdCal");

	// -------------------------------------------------------------------------

	return kSUCCESS;
}

void R3BOnlineSpectra::Reset_LOS_Histo()
{
    fh_los_channels->Reset();
    fh_los_tres_MCFD->Reset();
    fh_los_tres_TAMEX->Reset();
    fh_los_tot->Reset();
    fh_los_tot_mean->Reset();
    fh_los_dt_hits->Reset();
    fh_los_dt_hits_l->Reset();
    fh_los_dt_hits_t->Reset();
    fh_los_multihit->Reset();
    fh_los_multihitVFTX->Reset();
    fh_los_multihitLEAD->Reset();
    fh_los_multihitTRAI->Reset();
}
void R3BOnlineSpectra::Reset_SCI8_Histo()
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
void R3BOnlineSpectra::Reset_TOFD_Histo()
{
    for(int i = 0; i < N_PLANE_MAX_TOFD; i++)
    {
        fh_tofd_channels[i]->Reset();   
        fh_tofd_multihit[i]->Reset();
        fh_tofd_ToF[i]->Reset();
        fh_tofd_TotPm[i]->Reset(); 
    }
    fh_tofd_dt[0]->Reset();
    fh_tofd_dt[1]->Reset();
    fh_tofd_dt[2]->Reset();
}

void R3BOnlineSpectra::Exec(Option_t* option)
{
  //----------------------------------------------------------------------
  // LOS detector
  //----------------------------------------------------------------------
  // Time nomenclature for LOS:
  // lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7   
  //   t / \lt
  //   rt| |l
  //   r | |lb
  //   rb\ /b 
  Double_t time_V[10][8] = {0.0/0.0};  // [multihit][pm]         
  Double_t time_L[10][8] = {0.0/0.0};
  Double_t time_T[10][8] = {0.0/0.0};          
  Double_t timeLosM[10] = {0.0};
  Double_t LosTresM[10] = {0.0/0.0};
  Double_t timeLosT[10] = {0.0};
  Double_t LosTresT[10] = {0.0/0.0};
  Double_t timeLos[10] = {0.0};
  Double_t totsum[10] = {0.0}; 
  Double_t tot[10][8] = {0.0/0.0};


  Double_t timeTofd=0;
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
  // check for requested trigger (Todo: should be done globablly / somewhere else)
  if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
    return;

  Bool_t LOSID = true; 
  Int_t Multip;

  if(fMappedItemsLos && fMappedItemsLos->GetEntriesFast())
  {
    Int_t nHits = fMappedItemsLos->GetEntriesFast();

    Multip = nHits;

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


  Int_t nPart;   
  if(fCalItemsLos && fCalItemsLos->GetEntriesFast())
  {
    nPart = fCalItemsLos->GetEntriesFast();  

    fh_los_multihit->Fill(nPart);

    Int_t iDet = 0;
    Int_t nPart_VFTX[8] = {0};
    Int_t nPart_LEAD[8] = {0};
    Int_t nPart_TRAI[8] = {0};

    for (Int_t iPart = 0; iPart < nPart; iPart++)     
    {

      /* 
       * nPart is the number of particle passing through LOS detector in one event
       */ 
      R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(iPart);
      iDet=calData->GetDetector();

      // lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7 

      // VFTX Channels 1-4:
      if(!(IS_NAN(calData->fTimeV_r_ns))) {
	time_V[iPart][5] = calData->fTimeV_r_ns;
	nPart_VFTX[5] += 1;
      }	  
      if(!(IS_NAN(calData->fTimeV_t_ns))) {
	time_V[iPart][7] = calData->fTimeV_t_ns;
	nPart_VFTX[7] += 1;
      }	
      if(!(IS_NAN(calData->fTimeV_l_ns))) {
	time_V[iPart][1] = calData->fTimeV_l_ns;
	nPart_VFTX[1] += 1;
      }	
      if(!(IS_NAN(calData->fTimeV_b_ns))) {
	time_V[iPart][3] = calData->fTimeV_b_ns;  
	nPart_VFTX[3] += 1;
      }	
      // VFTX Channels 5-8:
      if(!(IS_NAN(calData->fTimeV_rt_ns))) {
	time_V[iPart][6] = calData->fTimeV_rt_ns;
	nPart_VFTX[6] += 1;
      }	
      if(!(IS_NAN(calData->fTimeV_lt_ns))) {
	time_V[iPart][0] = calData->fTimeV_lt_ns;
	nPart_VFTX[0] += 1;
      }	
      if(!(IS_NAN(calData->fTimeV_lb_ns))) {
	time_V[iPart][2] = calData->fTimeV_lb_ns;
	nPart_VFTX[2] += 1;
      }	
      if(!(IS_NAN(calData->fTimeV_rb_ns))) {
	time_V[iPart][4] = calData->fTimeV_rb_ns;                 
	nPart_VFTX [4]+= 1;
      }	
      // TAMEX Channels 1-4:      
      if(!(IS_NAN(calData->fTimeL_r_ns))) {
	time_L[iPart][5] = calData->fTimeL_r_ns;
	nPart_LEAD[5] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_r_ns))) {
	time_T[iPart][5] = calData->fTimeT_r_ns;
	nPart_TRAI[5] += 1;
      }	
      if(!(IS_NAN(calData->fTimeL_t_ns))) {
	time_L[iPart][7] = calData->fTimeL_t_ns;
	nPart_LEAD[7] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_t_ns))) {
	time_T[iPart][7] = calData->fTimeT_t_ns;
	nPart_TRAI[7] += 1;
      }	  
      if(!(IS_NAN(calData->fTimeL_l_ns))) {
	time_L[iPart][1] = calData->fTimeL_l_ns;
	nPart_LEAD[1] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_l_ns))) {
	time_T[iPart][1] = calData->fTimeT_l_ns; 
	nPart_TRAI[1] += 1;
      }	
      if(!(IS_NAN(calData->fTimeL_b_ns))) {
	time_L[iPart][3] = calData->fTimeL_b_ns;
	nPart_LEAD[3] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_b_ns))) {
	time_T[iPart][3] = calData->fTimeT_b_ns;  
	nPart_TRAI[3] += 1;
      }	
      // TAMEX Channels 5-8:
      if(!(IS_NAN(calData->fTimeL_rt_ns))) {
	time_L[iPart][6] = calData->fTimeL_rt_ns;
	nPart_LEAD[6] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_rt_ns))) {
	time_T[iPart][6] = calData->fTimeT_rt_ns;
	nPart_TRAI[6] += 1;
      }	
      if(!(IS_NAN(calData->fTimeL_lt_ns))) {
	time_L[iPart][0] = calData->fTimeL_lt_ns;
	nPart_LEAD[0] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_lt_ns))) {
	time_T[iPart][0] = calData->fTimeT_lt_ns; 
	nPart_TRAI[0] += 1;
      }	 
      if(!(IS_NAN(calData->fTimeL_lb_ns))) {
	time_L[iPart][2] = calData->fTimeL_lb_ns;
	nPart_LEAD[2] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_lb_ns))) {
	time_T[iPart][2] = calData->fTimeT_lb_ns; 
	nPart_TRAI[2] += 1;
      }	
      if(!(IS_NAN(calData->fTimeL_rb_ns))) {
	time_L[iPart][4] = calData->fTimeL_rb_ns;
	nPart_LEAD[4] += 1;
      }	
      if(!(IS_NAN(calData->fTimeT_rb_ns))) {
	time_T[iPart][4] = calData->fTimeT_rb_ns;  
	nPart_TRAI[4] += 1;
      }	


      if(iPart > 0 && Multip%8 == 0) 
      {
	for(int k=0; k<8; k++) 
	{
	  if(time_V[iPart][k] > 0. && time_V[iPart-1][k] > 0. && !(IS_NAN(time_V[iPart][k])) && !(IS_NAN(time_V[iPart-1][k]))) 
	  {
	    fh_los_dt_hits->Fill(time_V[iPart][k]-time_V[iPart-1][k]); 
	  }
	  if(time_L[iPart][k] > 0. && time_L[iPart-1][k] > 0. && !(IS_NAN(time_L[iPart][k])) && !(IS_NAN(time_L[iPart-1][k]))) 
	  {
	    fh_los_dt_hits_l->Fill(time_L[iPart][k]-time_L[iPart-1][k]); 
	  }
	  if(time_T[iPart][k] > 0. && time_T[iPart-1][k] > 0. && !(IS_NAN(time_T[iPart][k])) && !(IS_NAN(time_T[iPart-1][k]))) 
	  {
	    fh_los_dt_hits_t->Fill(time_T[iPart][k]-time_T[iPart-1][k]); 
	  }
	}	
      }	  

      if(iDet==1)
      {

	// lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7    

	//if(iPart >= 0)
	if(1 == 1)
	{ 
	  int nPMT = 0;
	  int nPMV = 0;	        

	  for(int ipm=0; ipm<8; ipm++)
	  {     
	    if(time_T[iPart][ipm] > 0. &&  time_L[iPart][ipm] > 0. && !(IS_NAN(time_T[iPart][ipm])) && !(IS_NAN(time_L[iPart][ipm]))) 
	    {     
	      while(time_T[iPart][ipm] - time_L[iPart][ipm] < 0.) 
	      {
		time_T[iPart][ipm] = time_T[iPart][ipm] + 2048.*fClockFreq; 
	      }

	      nPMT = nPMT +1;
	      tot[iPart][ipm] = time_T[iPart][ipm] - time_L[iPart][ipm];
	    }

	    totsum[iPart] += tot[iPart][ipm];

	    if(tot[iPart][ipm] != 0. && !(IS_NAN(tot[iPart][ipm]))) fh_los_tot->Fill(ipm+1,tot[iPart][ipm]);

	    if(time_L[iPart][ipm] > 0. && !(IS_NAN(time_L[iPart][ipm]))) timeLosT[iPart] += time_L[iPart][ipm];

	    if(time_V[iPart][ipm] > 0. && !(IS_NAN(time_V[iPart][ipm]))) 
	    {
	      timeLosM[iPart] += time_V[iPart][ipm];
	      nPMV = nPMV + 1;
	    }	 
	  }

	  totsum[iPart] = totsum[iPart]/nPMT;
	  /* 
	     if(totsum[iPart] < 88.) cout<<fNEvents<<"; "<<nPart<<"; "<<iPart<<", "<<totsum[iPart]<<tot[iPart][0]<<
	     ", "<<tot[iPart][1]<<", " <<tot[iPart][2]<<", "<<tot[iPart][3]<<", "<<tot[iPart][4]<<", "
	     <<tot[iPart][5]<<", "<<tot[iPart][6]<<", "<<tot[iPart][7]<<endl;
	   */ 

	  timeLosM[iPart] = timeLosM[iPart]/nPMV;

	  timeLosT[iPart] = timeLosT[iPart]/nPMT;

	  timeLos[iPart] = timeLosM[iPart];

	  //    if(!(timeLosM[iPart] > 0.) && IS_NAN(timeLosM[iPart])) 
	  //        cout<<"R3BOnline WARNING!! LOS VFTX time < 0 or nan! If nan we take TAMEX time, if <0 no ToF info for this event!! "<<timeLosM[iPart]<<endl;
	  // Line below only after VFTX and TAMEX clocks are synhronized!
	  //  if(IS_NAN(timeLosM[iPart])) timeLos[iPart] = timeLosT[iPart];


	  if(nPMV == 8) LosTresM[iPart] = ((time_V[iPart][0]+time_V[iPart][2]+time_V[iPart][4]+time_V[iPart][6]) - 
	      (time_V[iPart][1]+time_V[iPart][3]+time_V[iPart][5]+time_V[iPart][7]))/4.;  	  		          
	  if(nPMT == 8) LosTresT[iPart] = ((time_L[iPart][0]+time_L[iPart][2]+time_L[iPart][4]+time_L[iPart][6]) - 
	      (time_L[iPart][1]+time_L[iPart][3]+time_L[iPart][5]+time_L[iPart][7]))/4.;  

	  if(nPMV == 8) fh_los_tres_MCFD->Fill(LosTresM[iPart]);
	  if(nPMT == 8) fh_los_tres_TAMEX->Fill(LosTresT[iPart]);
	  if(nPMT == 8) fh_los_tot_mean->Fill(totsum[iPart]);
	}     

      } 
      else 
      {
	cout<<"Wrong detector ID for LOS!"<<endl;
      }
    } 

    for(int ik=0; ik<8; ik++)
    {	
      fh_los_multihitVFTX->Fill(ik+1,nPart_VFTX[ik]);
      fh_los_multihitLEAD->Fill(ik+1,nPart_LEAD[ik]);
      fh_los_multihitTRAI->Fill(ik+1,nPart_TRAI[ik]);
    }    
  }	



  //----------------------------------------------------------------------
  // SCI8 detector
  //----------------------------------------------------------------------

  Double_t timeS8_V[10][2] = {0.0/0.0};  // [multihit][pm]         
  Double_t timeS8_L[10][2] = {0.0/0.0};
  Double_t timeS8_T[10][2] = {0.0/0.0};          
  Double_t timeSci8M[10] = {0.0};
  Double_t Sci8TresM[10] = {0.0/0.0};
  Double_t timeSci8T[10] = {0.0};
  Double_t Sci8TresT[10] = {0.0/0.0};
  Double_t timeSci8[10] = {0.0};
  Double_t totsumS8[10] = {0.0}; 
  Double_t totS8[10][8] = {0.0/0.0};

  Int_t MultipS8;

  if(fMappedItemsSci8 && fMappedItemsSci8->GetEntriesFast())
  {
    Int_t nHits = fMappedItemsSci8->GetEntriesFast();

    MultipS8 = nHits;

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
      R3BSci8MappedData* hit = (R3BSci8MappedData*)fMappedItemsSci8->At(ihit);
      if (!hit) continue;

      // channel numbers are stored 1-based (1..n)
      Int_t iDet = hit->GetDetector(); // 1..
      Int_t iCha = hit->GetChannel();  // 1..

      fh_sci8_channels->Fill(iCha);				         
    }
  }


  Int_t nPartS8;   
  if(fCalItemsSci8 && fCalItemsSci8->GetEntriesFast())
  {
    nPartS8 = fCalItemsSci8->GetEntriesFast();  

    fh_sci8_multihit->Fill(nPartS8);

    Int_t iDet = 0;
    Int_t nPartS8_VFTX[2] = {0};
    Int_t nPartS8_LEAD[2] = {0};
    Int_t nPartS8_TRAI[2] = {0};

    for (Int_t iPart = 0; iPart < nPartS8; iPart++)     
    {

      /* 
       * nPart is the number of particle passing through Sci8 detector in one event
       */ 
      R3BSci8CalData *calDataS8 = (R3BSci8CalData*)fCalItemsSci8->At(iPart);
      iDet = calDataS8->GetDetector();


      // VFTX Channels 1-2:
      if(!(IS_NAN(calDataS8->fTimeV_r_ns))) {
	timeS8_V[iPart][0] = calDataS8->fTimeV_r_ns;
	nPartS8_VFTX[0] += 1;
      }	  
      if(!(IS_NAN(calDataS8->fTimeV_l_ns))) {
	timeS8_V[iPart][1] = calDataS8->fTimeV_l_ns;
	nPartS8_VFTX[1] += 1;
      }	

      // TAMEX Channels 1-2:      
      if(!(IS_NAN(calDataS8->fTimeL_r_ns))) {
	timeS8_L[iPart][0] = calDataS8->fTimeL_r_ns;
	nPartS8_LEAD[0] += 1;
      }	
      if(!(IS_NAN(calDataS8->fTimeT_r_ns))) {
	timeS8_T[iPart][0] = calDataS8->fTimeT_r_ns;
	nPartS8_TRAI[0] += 1;
      }	
      if(!(IS_NAN(calDataS8->fTimeL_l_ns))) {
	timeS8_L[iPart][1] = calDataS8->fTimeL_l_ns;
	nPartS8_LEAD[1] += 1;
      }	
      if(!(IS_NAN(calDataS8->fTimeT_l_ns))) {
	timeS8_T[iPart][1] = calDataS8->fTimeT_l_ns;
	nPartS8_TRAI[1] += 1;
      }	  



      if(iPart > 0 && MultipS8%2 == 0) 
      {
	for(int k=0; k<2; k++) 
	{
	  if(timeS8_V[iPart][k] > 0. && timeS8_V[iPart-1][k] > 0. && !(IS_NAN(timeS8_V[iPart][k])) && !(IS_NAN(timeS8_V[iPart-1][k]))) 
	  {
	    fh_sci8_dt_hits->Fill(timeS8_V[iPart][k]-timeS8_V[iPart-1][k]); 
	  }
	  if(timeS8_L[iPart][k] > 0. && timeS8_L[iPart-1][k] > 0. && !(IS_NAN(timeS8_L[iPart][k])) && !(IS_NAN(timeS8_L[iPart-1][k]))) 
	  {
	    fh_sci8_dt_hits_l->Fill(timeS8_L[iPart][k]-timeS8_L[iPart-1][k]); 
	  }
	  if(timeS8_T[iPart][k] > 0. && timeS8_T[iPart-1][k] > 0. && !(IS_NAN(timeS8_T[iPart][k])) && !(IS_NAN(timeS8_T[iPart-1][k]))) 
	  {
	    fh_sci8_dt_hits_t->Fill(timeS8_T[iPart][k]-timeS8_T[iPart-1][k]); 
	  }
	}	
      }	  

      if(iDet==1)
      {

	//if(iPart >= 0)
	if(1 == 1)
	{ 
	  int nPMT = 0;
	  int nPMV = 0;	        
	  int ilc = iPart;

	  for(int ipm=0; ipm<2; ipm++)
	  {

	    if(timeS8_T[iPart][ipm] > 0. &&  timeS8_L[iPart][ipm] > 0. && !(IS_NAN(timeS8_T[iPart][ipm])) && !(IS_NAN(timeS8_L[iPart][ipm]))) 
	    {     
	      while(timeS8_T[iPart][ipm] - timeS8_L[iPart][ipm] < 0.) 
	      {
		timeS8_T[iPart][ipm] = timeS8_T[iPart][ipm] + 2048.*fClockFreq; 
	      }

	      nPMT = nPMT +1;
	      totS8[iPart][ipm] = timeS8_T[iPart][ipm] - timeS8_L[iPart][ipm];
	    }

	    totsumS8[iPart] += totS8[iPart][ipm];

	    if(totS8[iPart][ipm] != 0. && !(IS_NAN(totS8[iPart][ipm]))) fh_sci8_tot->Fill(ipm+1,totS8[iPart][ipm]);

	    if(timeS8_L[iPart][ipm] > 0. && !(IS_NAN(timeS8_L[iPart][ipm]))) timeSci8T[iPart] += timeS8_L[iPart][ipm];

	    if(timeS8_V[iPart][ipm] > 0. && !(IS_NAN(timeS8_V[iPart][ipm]))) 
	    {
	      timeSci8M[iPart] += timeS8_V[iPart][ipm];
	      nPMV = nPMV + 1;
	    }	 
	  }

	  totsumS8[iPart] = totsumS8[iPart]/nPMT;
	  /* 
	     if(totsum[iPart] < 88.) cout<<fNEvents<<"; "<<nPart<<"; "<<iPart<<", "<<totsum[iPart]<<tot[iPart][0]<<
	     ", "<<tot[iPart][1]<<", " <<tot[iPart][2]<<", "<<tot[iPart][3]<<", "<<tot[iPart][4]<<", "
	     <<tot[iPart][5]<<", "<<tot[iPart][6]<<", "<<tot[iPart][7]<<endl;
	   */ 
	  timeSci8M[iPart] = timeSci8M[iPart]/nPMV;
	  timeSci8T[iPart] = timeSci8T[iPart]/nPMT;  

	  timeSci8[iPart] = timeSci8M[iPart]; 
	  fh_tof_sci8->Fill(timeSci8[iPart]-timeLos[ilc]); 

	  // cout<<"TOF "<<timeSci8[iPart]-timeLos[ilc]<<endl;             

	  if(nPMV == 2) Sci8TresM[iPart] = (timeS8_V[iPart][1]-timeS8_V[iPart][0]);  	  		          
	  if(nPMT == 2) Sci8TresT[iPart] = (timeS8_L[iPart][1]-timeS8_L[iPart][0]);  

	  if(nPMV == 2) fh_sci8_tres_MCFD->Fill(Sci8TresM[iPart]);
	  if(nPMT == 2) fh_sci8_tres_TAMEX->Fill(Sci8TresT[iPart]);
	  if(nPMT == 2) fh_sci8_tot_mean->Fill(totsumS8[iPart]);
	}     

      } 
      else 
      {
	cout<<"Wrong detector ID for Sci8!"<<endl;
      }
    } 

    for(int ik=0; ik<2; ik++)
    {	
      fh_sci8_multihitVFTX->Fill(ik+1,nPartS8_VFTX[ik]);
      fh_sci8_multihitLEAD->Fill(ik+1,nPartS8_LEAD[ik]);
      fh_sci8_multihitTRAI->Fill(ik+1,nPartS8_TRAI[ik]);
    }    
  }	








  //----------------------------------------------------------------------
  // Fiber detectors
  //----------------------------------------------------------------------
  Double_t dtime = 0.0/0.0;

  for(Int_t ifibcount = 0; ifibcount < 14; ifibcount++)
  { 

    Int_t iFib = 0;  

    if(FibPresent[ifibcount])
    {	   	 
      // get access to Mapped data     
      aMapped.at(ifibcount) = (TClonesArray*)mgr->GetObject(cMapped[ifibcount]); 
      // get access to cal data     
      aHit.at(ifibcount) = (TClonesArray*)mgr->GetObject(cHit[ifibcount]);
    } 

    if(aMapped.at(ifibcount)) 
    {

      Int_t nHits = aMapped.at(ifibcount)->GetEntriesFast();
      std::vector<UInt_t> mapmt_num(512);
      std::vector<UInt_t> spmt_num(16);
      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
	R3BBunchedFiberMappedData* hit = (R3BBunchedFiberMappedData*)aMapped.at(ifibcount)->At(ihit);
	if (!hit) continue;

	// channel numbers are stored 1-based (1..n)
	Int_t iCha = hit->GetChannel();  // 1..

	if (hit->IsMAPMT() && hit->IsLeading()) 
	{
	  fh_channels_Fib[ifibcount]->Fill(iCha);    // Fill which clockTDC channel has events

	  ++mapmt_num.at(hit->GetChannel() - 1);     // multihit of a given clockTDC channel
	}

	if (!hit->IsMAPMT() && hit->IsLeading()) 
	{

	  ++spmt_num.at(hit->GetChannel() - 1);      // multihit of a given PADI channel
	}
      }

      for (int i = 0; i < 512; ++i) 
      { 
	auto m = mapmt_num.at(i);
	if(m > 0) fh_multihit_m_Fib[ifibcount]->Fill(i,m);  // multihit of a given clockTDC channel      
      }

      for (int i = 0; i < 16; ++i) 
      {
	auto s = spmt_num.at(i);
	if(s > 0) fh_multihit_s_Fib[ifibcount]->Fill(i,s); // multihit of a given PADI channel
      }


    } // end if(aMapped[ifibcount])


    if(aHit.at(ifibcount)) 
    {
      Int_t nHits = aHit.at(ifibcount)->GetEntriesFast(); 

      Double_t posfib = 0./0.;
      Double_t totMax = 0.;     
      Double_t tfib=0./0., tof_fib= 0./0.;
      Double_t randx;
      Int_t iFibmax = 0;

      for (Int_t ihit = 0; ihit < nHits; ihit++)
      {
	Double_t tMAPMT = 0./0.;
	Double_t tSPMT = 0./0.;

	R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)aHit.at(ifibcount)->At(ihit);
	if (!hit) continue;

	iFib = hit->GetFiberId();  // 1..

	// times
	tMAPMT = hit->GetMAPMTTime_ns();
	tSPMT = hit->GetSPMTTime_ns();

	dtime = tMAPMT-tSPMT;		

	// "Push" two times in the same clock cycle:	
	while(dtime < 1024.) 
	{
	  tSPMT = tSPMT - 2048.; 
	  dtime = tMAPMT - tSPMT;
	}
	while(dtime > 1024.) 
	{
	  tSPMT = tSPMT + 2048.; 
	  dtime = tMAPMT - tSPMT;
	}			

	// "Push" the Fib times in the same cycle with LOS:
	if(timeLos[0]>0. && !(IS_NAN(timeLos[0])))
	{
	  while(tMAPMT - timeLos[0] < 5000.)
	  {
	    tMAPMT = tMAPMT + 2048.*4.; 
	    tSPMT = tSPMT + 2048.*4.; 		  
	  }		
	  while(tMAPMT - timeLos[0] > 5000.)
	  {
	    tMAPMT = tMAPMT - 2048.*4.; 
	    tSPMT = tSPMT - 2048.*4.; 		  
	  }
	}

	// Not-calibrated ToF:	 
	//tfib = (tMAPMT + tSPMT) / 2.;
	tfib = tSPMT;
	if(tfib > 0. && !(IS_NAN(tfib)) && timeLos[0]>0. && !(IS_NAN(timeLos[0]))) tof_fib = tfib - timeLos[0];

	// Not-calibrated position:  
	randx = (std::rand() / (float)RAND_MAX);
	if(iFib > 0) posfib = (-n_fiber[ifibcount]/2.+iFib+(0.5-randx)); 

	if(hit->GetSPMTToT_ns() > 0) 
	{
	  fh_fibers_Fib[ifibcount]->Fill(iFib);  
	  fh_ToT_s_Fib[ifibcount]->Fill(iFib,hit->GetSPMTToT_ns());
	  fh_ToT_m_Fib[ifibcount]->Fill(iFib,hit->GetMAPMTToT_ns());
	  fh_time_Fib[ifibcount]->Fill(iFib,tMAPMT-tSPMT);
	  fh_Fib_ToF[ifibcount]->Fill(iFib,tof_fib);
	  fh_Fib_pos[ifibcount]->Fill(posfib);	     
	}  

      }  // end for(ihit)

      fh_mult_Fib[ifibcount]->Fill(nHits);            

    }  // end if(aHit[ifibcount]) 


  } // end for(ifibcount)

  //----------------------------------------------------------------------
  // TOFD
  //----------------------------------------------------------------------

  Int_t NumPaddles[4]={0};
  if(fMappedItemsTofd )
  {
    Int_t nMapped = fMappedItemsTofd->GetEntriesFast();  	
    Int_t iPlaneMem = 1, iBarMem = 0; 
    for (Int_t imapped = 0; imapped < nMapped; imapped++)     
    {
      auto mapped = (R3BTofdMappedData const *)fMappedItemsTofd->At(imapped);
      if (!mapped) continue; // should not happen

      Int_t const iPlane = mapped->GetDetectorId(); // 1..n
      Int_t const iBar   = mapped->GetBarId();   // 1..n
      Int_t const iSide  = mapped->GetSideId();   // 1..n
      Int_t const iEdge  = mapped->GetEdgeId(); 

      if(iPlaneMem != iPlane) iBarMem = 0;
      if(iPlane == 1 && iBarMem != iBar && iSide == 1 && iEdge == 1) NumPaddles[0] += 1;
      if(iPlane == 2 && iBarMem != iBar && iSide == 1 && iEdge == 1) NumPaddles[1] += 1;
      if(iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1) NumPaddles[2] += 1;
      if(iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1) NumPaddles[3] += 1;

      //  cout<<imapped<<", "<<iPlane<<"; "<<iPlaneMem<<"; "<<iBar<<"; "<<iBarMem<<", "<<NumPaddles[iPlane-1]<<endl;

      iPlaneMem = iPlane;
      iBarMem = iBar;

      if(iPlane<=fNofPlanes)
      {
	fh_tofd_channels[iPlane-1]->Fill(iBar);
	fh_tofd_channels[iPlane-1]->Fill(-iBar-1);
      }
    }
  }


  if(fCalItemsTofd)
  {
    Int_t nCals = fCalItemsTofd->GetEntriesFast();

    Double_t tot1[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
    Double_t tot2[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
    Double_t t_paddle[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
    Double_t t1l[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
    Double_t t2l[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
    Double_t t1t[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
    Double_t t2t[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
    Double_t ToF[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};

    Bool_t Bar_present[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={false};

    Int_t iBarMem = 0;
    Int_t jmult[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = {0};

    for (Int_t ical = 0; ical < nCals; ical++)     
    {
      auto cal = (R3BTofdCalData const *)fCalItemsTofd->At(ical);
      if (!cal) continue; // should not happen

      Int_t const iPlane  = cal->GetDetectorId();    // 1..n
      Int_t const iBar  = cal->GetBarId();    // 1..n

//      std::cout << iPlane << ' ' << iBar <<
//          ',' << cal->GetTimeBL_ns() <<
//          ' ' << cal->GetTimeBT_ns() <<
//          ' ' << cal->GetTimeTL_ns() <<
//          ' ' << cal->GetTimeTT_ns() << std::endl;

      // get all times of one bar
      /*                 
			 cout<<"TOFD Online: "<<fNEvents<<", "<<nCals<<"; "<<ical<<"; "<<iPlane<<", "<<iBar<<", "<<iBarMem<<", "<<", "<<
			 cal->fTime1L_ns<<", "<<cal->fTime1T_ns<<", "<<
			 cal->fTime2L_ns<<", "<<cal->fTime2T_ns<<endl;
       */        

      if(iBar != iBarMem || NumPaddles[iPlane-1] == 1) // condition on NumPaddles to be removed when more paddles in plane 3 and 4 activ!!! 
      {
	jmult[iPlane-1][iBar-1] = 0;
	if (!IS_NAN(cal->GetTimeBL_ns())) t1l[0][iPlane-1][iBar-1] = cal->GetTimeBL_ns();
	if (!IS_NAN(cal->GetTimeBT_ns())) t1t[0][iPlane-1][iBar-1] = cal->GetTimeBT_ns();
	if (!IS_NAN(cal->GetTimeTL_ns())) t2l[0][iPlane-1][iBar-1] = cal->GetTimeTL_ns();
	if (!IS_NAN(cal->GetTimeTT_ns())) t2t[0][iPlane-1][iBar-1] = cal->GetTimeTT_ns();  
	Bar_present[0][iPlane-1][iBar-1] = true;                        
      }
      else
      {
	jmult[iPlane-1][iBar-1] = jmult[iPlane-1][iBar-1] + 1;
	Int_t jm = jmult[iPlane-1][iBar-1];
	if (!IS_NAN(cal->GetTimeBL_ns())) t1l[jm][iPlane-1][iBar-1] = cal->GetTimeBL_ns();
	if (!IS_NAN(cal->GetTimeBT_ns())) t1t[jm][iPlane-1][iBar-1] = cal->GetTimeBT_ns();
	if (!IS_NAN(cal->GetTimeTL_ns())) t2l[jm][iPlane-1][iBar-1] = cal->GetTimeTL_ns();
	if (!IS_NAN(cal->GetTimeTT_ns())) t2t[jm][iPlane-1][iBar-1] = cal->GetTimeTT_ns();
	Bar_present[jm][iPlane-1][iBar-1] = true;
      }
      iBarMem = iBar;

    }

    /*      		
			for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
			for(Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++)
			{
			fh_tofd_multihit[ipl]->Fill(ibr+1,jmult[ipl][ibr]+1);
			} 
     */ 
    for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
      for(Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++)
	for(Int_t jm=0; jm < jmult[ipl][ibr]+1; jm++)
	{  

	  if(Bar_present[jm][ipl][ibr])
	  {
	    Int_t iPlane  = ipl+1;    // 1..n
	    Int_t iBar  = ibr+1;    // 1..n

	    fh_tofd_multihit[ipl]->Fill(ibr+1,jmult[ipl][ibr]+1);

	    // calculate time over threshold and check if clock counter went out of range

	    while(t1t[jm][iPlane-1][iBar-1] < t1l[jm][iPlane-1][iBar-1]) 
	    {
	      t1t[jm][iPlane-1][iBar-1] += 2048.*fClockFreq; 
	    }

	    while(t2t[jm][iPlane-1][iBar-1] < t2l[jm][iPlane-1][iBar-1]) 
	    {
	      t2t[jm][iPlane-1][iBar-1] += 2048.*fClockFreq; 
	    }


	    int ilc = jm; 

	    if( !(IS_NAN(timeLos[ilc])) && timeLos[ilc]>0.)
	    {

	      while(t1l[jm][iPlane-1][iBar-1] < timeLos[ilc])
	      {
		t1t[jm][iPlane-1][iBar-1] += 2048.*fClockFreq;
		t1l[jm][iPlane-1][iBar-1] += 2048.*fClockFreq;
		}
		  while(t2l[jm][iPlane-1][iBar-1] < timeLos[ilc])
		  {
		t2t[jm][iPlane-1][iBar-1] += 2048.*fClockFreq;
		t2l[jm][iPlane-1][iBar-1] += 2048.*fClockFreq;
	      }
	      // ToF	  
	      t_paddle[jm][iPlane-1][iBar-1] = (t1l[jm][iPlane-1][iBar-1] + t2l[jm][iPlane-1][iBar-1])/2.;

	      //  between 2 bars in 2 planes  

	      if(ipl > 0){
		fh_tofd_dt[ipl-1]->Fill(iBar,t_paddle[jm][ipl][iBar-1]-t_paddle[jm][ipl-1][iBar-1]);

	      }		            

	      // between LOS and paddle          		            
	      ToF[jm][iPlane-1][iBar-1] = fmod(t_paddle[jm][iPlane-1][iBar-1] - timeLos[ilc] + 5*8192, 5*2048);
	      fh_tofd_ToF[iPlane-1]->Fill(iBar,ToF[jm][iPlane-1][iBar-1]);
	    }		   
	    // ToT   
	    tot1[jm][iPlane-1][iBar-1]=t1t[jm][iPlane-1][iBar-1] - t1l[jm][iPlane-1][iBar-1];		      
	    if(tot1[jm][iPlane-1][iBar-1]<0) 
	    {
	      cout<<"Negative ToT "<<tot1[jm][iPlane-1]<<", for hit= "<<jm<<endl; 	
	      cout<<"Times1: "<<t1t[jm][iPlane-1][iBar-1]<<" "<<t1l[jm][iPlane-1][iBar-1]<<endl; 		  
	    }

	    tot2[jm][iPlane-1][iBar-1]=t2t[jm][iPlane-1][iBar-1] - t2l[jm][iPlane-1][iBar-1];	
	    if(tot2[jm][iPlane-1][iBar-1]<0) 
	    {
	      cout<<"Negative ToT "<<tot2[jm][iPlane-1][iBar-1]<<", for hit= "<<jm<<endl;               
	      cout<<"Times2: "<<t2t[jm][iPlane-1][iBar-1]<<" "<<t2l[jm][iPlane-1][iBar-1]<<endl;		 
	    }

	    fh_tofd_TotPm[iPlane-1]->Fill(iBar,tot1[jm][iPlane-1][iBar-1]);
	    fh_tofd_TotPm[iPlane-1]->Fill(-iBar-1,tot2[jm][iPlane-1][iBar-1]);

	  }
	}
  }

  //----------------------------------------------------------------------
  // PTOF
  //----------------------------------------------------------------------

  if(fCalItemsPtof){
    Double_t tot1=0.;
    Double_t tot2=0.;
    Double_t t1l=0.;
    Double_t t2l=0.;
    Double_t t1t=0.;
    Double_t t2t=0.;
    Bool_t bar_quer1=false;
    Bool_t bar_quer2=false;

    Int_t nHits = fCalItemsPtof->GetEntriesFast();    
    LOG(DEBUG) << "nHits: " << nHits << FairLogger::endl;
    for (Int_t ihit = 0; ihit < nHits; ihit++)     {
      R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsPtof->At(ihit);

      if (!hit) continue; // should not happen

      Int_t iBar  = hit->GetBar();    // 1..n
      // get all times of one bar
      t1l=hit->fTime1L_ns;
      t2l=hit->fTime2L_ns;
      t1t=hit->fTime1T_ns;
      t2t=hit->fTime2T_ns;

      if(!(t1l>0 && t2l>0 && t1t>0 && t2t>0)) continue;

      fh_ptof_channels->Fill(iBar);
      LOG(DEBUG) << "Bar: " << iBar << FairLogger::endl;
      LOG(DEBUG) << "times PM1: " << t1l<<"  "<<t1t<<"  "<<t1t-t1l << FairLogger::endl;
      LOG(DEBUG) << "times PM2: " << t2l<<"  "<<t2t<<"  "<<t2t-t2l << FairLogger::endl;
      if(iBar==7) bar_quer1=true;
      if(iBar==8) bar_quer2=true;

      // calculate time over threshold and check if clock counter went out of range
      while(t1t - t1l < 0.) {
	t1t=t1t+2048.*fClockFreq; 
      }

      while(t2t-t2l < 0.) {
	t2t=t2t+2048.*fClockFreq; 
      }
      while(t1l-timeLos[0]<0.){
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

      fh_ptof_TotPm1[iBar]->Fill(tot1);
      fh_ptof_TotPm2[iBar]->Fill(tot2);
      if(iBar==2) fh_ptof_test1->Fill(sqrt(tot1*tot1));	
    }	

    //once again

    nHits = fCalItemsPtof->GetEntriesFast();    
    //		LOG(DEBUG) << "nHits: " << nHits << FairLogger::endl;
    for (Int_t ihit = 0; ihit < nHits; ihit++){
      R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsPtof->At(ihit);

      if (!hit) continue; // should not happen

      Int_t iBar  = hit->GetBar();    // 1..n
      // get all times of one bar
      t1l=hit->fTime1L_ns;
      t2l=hit->fTime2L_ns;
      t1t=hit->fTime1T_ns;
      t2t=hit->fTime2T_ns;

      if(!(t1l>0 && t2l>0 && t1t>0 && t2t>0)) continue;

      // calculate time over threshold and check if clock counter went out of range
      while(t1t - t1l < 0.) {
	t1t=t1t+2048.*fClockFreq; 
      }

      while(t2t-t2l < 0.) {
	t2t=t2t+2048.*fClockFreq; 
      }
      while(t1l-timeLos[0]<0.){
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

      if(bar_quer1 && bar_quer2){
	//				fh_ptof_TotPm1[iBar]->Fill(tot1);
	//				fh_ptof_TotPm2[iBar]->Fill(tot2);
	//				if(iBar==2) fh_ptof_test2->Fill(sqrt(tot1*tot2));	
	fh_ptof_channels_cut->Fill(iBar);			
      }			
    }
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


     for(Int_t ifibcount = 0; ifibcount < 11; ifibcount++){	   	    	
      if(aMapped[ifibcount]) 
      {
		aMapped[ifibcount] ->Clear(); 
	   }
      if(aHit[ifibcount]) 
      {
		aHit[ifibcount] ->Clear(); 
	   }	   
	 }  	  
}

void R3BOnlineSpectra::FinishTask()
{    
    if(fMappedItemsLos){
		fh_los_channels->Write();
        fh_los_tot->Write();
	}
 
 for(Int_t ifibcount = 0; ifibcount < 11; ifibcount++){	   	    	

    if(aMapped[ifibcount]) 
    {
		
     fh_channels_Fib[ifibcount]->Write();
     fh_fibers_Fib[ifibcount]->Write();
     fh_mult_Fib[ifibcount]->Write();
     fh_time_Fib[ifibcount]->Write();
     fh_multihit_m_Fib[ifibcount]->Write();   
     fh_multihit_s_Fib[ifibcount]->Write();
     fh_ToT_m_Fib[ifibcount]->Write();
     fh_ToT_s_Fib[ifibcount]->Write();
      
    }
 }   		 
 

}

ClassImp(R3BOnlineSpectra)
