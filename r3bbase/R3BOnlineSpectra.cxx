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
    if(fh_FibervsTime_Fib[i]) delete(fh_FibervsTime_Fib[i]);
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

// create histograms of all detectors    

//------------------------------------------------------------------------ 
// Los detector
	// get access to Mapped data
    fMappedItemsLos = (TClonesArray*)mgr->GetObject("LosMapped");
	// get access to cal data
    fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");
 

	if(fMappedItemsLos){
		TCanvas *cLos = new TCanvas("Los", "LOS", 10, 10, 950, 650);
 
		fh_los_channels = new TH1F("los_channels", "LOS channels", 10, 0., 10.); 
		fh_los_channels->GetXaxis()->SetTitle("Channel number");
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
		
		cLos->Divide(3, 2);
		cLos->cd(1);
		fh_los_channels->Draw();
		cLos->cd(2);
		fh_los_tres_MCFD->Draw();
		cLos->cd(3);
		fh_los_tres_TAMEX->Draw();
		cLos->cd(4);gPad->SetLogz();
		fh_los_tot->Draw("colz");
		cLos->cd(5);
		fh_los_tot_mean->Draw();
		cLos->cd(0);
		run->AddObject(cLos);
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
    // Fiber vs event number: 
       histName10<<cfibName<<"_FibervsEventNo";
       tempName=histName10.str();
	   chistName=tempName.c_str();
	   histTitle10<<cfibName<<" Fiber vs Event No";
	   tempTitle=histTitle10.str();
	   chistTitle=tempTitle.c_str();
       fh_FibervsTime_Fib[ifibcount]= new TH2F(chistName, chistTitle, 2100, 0., 2100., 1000, 0., 100000.);
       fh_FibervsTime_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
       fh_FibervsTime_Fib[ifibcount]->GetYaxis()->SetTitle("Event number");
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
                   
       FibCanvas[ifibcount]->Divide(3, 4);
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
       fh_FibervsTime_Fib[ifibcount]->Draw("colz");        
       FibCanvas[ifibcount]->cd(7);gPad->SetLogz();
       fh_ToT_m_Fib[ifibcount]->Draw("colz");
       FibCanvas[ifibcount]->cd(8);gPad->SetLogz();
       fh_ToT_s_Fib[ifibcount]->Draw("colz");
       FibCanvas[ifibcount]->cd(9);gPad->SetLogz();
       fh_time_Fib[ifibcount]->Draw("colz");        
       FibCanvas[ifibcount]->cd(10);gPad->SetLogz();
       fh_Fib_ToF[ifibcount]->Draw("colz");
       FibCanvas[ifibcount]->cd(11);
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
		TCanvas *cTofd_planes = new TCanvas("TOFD_planes", "TOFD planes", 10, 10, 900, 900);
		cTofd_planes->Divide(4, 4);
		
		for (Int_t j = 0; j < 4; j++)
		{
			 char strName1[255];
			 sprintf(strName1, "tofd_channels_plane_%d", j);
			 char strName2[255];
			 sprintf(strName2, "Tofd channels plane %d", j);        
			 fh_tofd_channels[j] = new TH1F(strName1, strName2, 44, -22., 22.);		 
			 fh_tofd_channels[j]->GetXaxis()->SetTitle("Channel");
             fh_tofd_channels[j]->GetYaxis()->SetTitle("Counts");
       
			 char strName3[255];
			 sprintf(strName3, "tofd_ToT1_plane_%d", j);
			 char strName4[255];
			 sprintf(strName4, "Tofd ToT1 plane %d", j);        
			 fh_tofd_TotPm1[j] = new TH2F(strName3, strName4, 50,0,50,30000,0.,300.);
			 fh_tofd_TotPm1[j]->GetXaxis()->SetTitle("Bar number");
             fh_tofd_TotPm1[j]->GetYaxis()->SetTitle("ToT / ns");
             
			 char strName5[255];
			 sprintf(strName5, "tofd_ToT2_plane_%d", j);
			 char strName6[255];
			 sprintf(strName6, "Tofd ToT2 plane %d", j);        
			 fh_tofd_TotPm2[j] = new TH2F(strName5, strName6, 50,0,50,30000,0.,300.);			 			 
		     fh_tofd_TotPm2[j]->GetXaxis()->SetTitle("Bar number");
             fh_tofd_TotPm2[j]->GetYaxis()->SetTitle("ToT / ns");
             
			 char strName7[255];
			 sprintf(strName7, "tofd_ToF_plane_%d", j);
			 char strName8[255];
			 sprintf(strName8, "TOFD-LOS ToF plane %d", j); 
		     fh_tofd_ToF[j] = new TH2F(strName7, strName8, 15,0,15,40000, -2000., 2000.);
		     fh_tofd_ToF[j]->GetXaxis()->SetTitle("Bar number");
             fh_tofd_ToF[j]->GetYaxis()->SetTitle("ToF / ns");
             
		}
		
			 cTofd_planes->cd(1);
			 fh_tofd_channels[0]->Draw();
			 cTofd_planes->cd(2);gPad->SetLogz();
			 fh_tofd_TotPm1[0]->Draw("colz");
			 cTofd_planes->cd(3);gPad->SetLogz();
			 fh_tofd_TotPm2[0]->Draw("colz");
			 cTofd_planes->cd(4);gPad->SetLogz();
			 fh_tofd_ToF[0]->Draw("colz");
			
			 cTofd_planes->cd(5);
			 fh_tofd_channels[1]->Draw();
			 cTofd_planes->cd(6);gPad->SetLogz();
			 fh_tofd_TotPm1[1]->Draw("colz");
			 cTofd_planes->cd(7);gPad->SetLogz();
			 fh_tofd_TotPm2[1]->Draw("colz");			 
			 cTofd_planes->cd(8);gPad->SetLogz();
			 fh_tofd_ToF[1]->Draw("colz");
			 
			 cTofd_planes->cd(9);
			 fh_tofd_channels[2]->Draw();
			 cTofd_planes->cd(10);gPad->SetLogz();
			 fh_tofd_TotPm1[2]->Draw("colz");
			 cTofd_planes->cd(11);gPad->SetLogz();
			 fh_tofd_TotPm2[2]->Draw("colz");			 
			 cTofd_planes->cd(12);gPad->SetLogz();
			 fh_tofd_ToF[2]->Draw("colz");
			 
			 cTofd_planes->cd(13);
			 fh_tofd_channels[3]->Draw();
			 cTofd_planes->cd(14);gPad->SetLogz();
			 fh_tofd_TotPm1[3]->Draw("colz");
			 cTofd_planes->cd(15);gPad->SetLogz();
			 fh_tofd_TotPm2[3]->Draw("colz");			 
			 cTofd_planes->cd(16);gPad->SetLogz();
			 fh_tofd_ToF[3]->Draw("colz");
			 
		     cTofd_planes->cd(0);
             run->AddObject(cTofd_planes);
             
    }
    
       fCalItemsTofd = (TClonesArray*)mgr->GetObject("TofdCal");

// -------------------------------------------------------------------------
 
 
    return kSUCCESS;
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
	Double_t time_V[8] = {0.0/0.0};           
	Double_t time_L[8] = {0.0/0.0};
	Double_t time_T[8] = {0.0/0.0};          
	Double_t timeLosM = 0.0;
	Double_t LosTresM = 0.0/0.0;
	Double_t timeLosT = 0.0;
	Double_t LosTresT = 0.0/0.0;
	Double_t totsum = 0.0; 
	Double_t tot[8] = {0.0/0.0};
 
           
    Double_t timeTofd=0;
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
	// check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
		return;

    Bool_t LOSID = false; 
    
    if(fMappedItemsLos && fMappedItemsLos->GetEntriesFast())
    {
      Int_t nHits = fMappedItemsLos->GetEntriesFast();
  
// At the moment no multi-particle events are analyzed; only those with nHits=24!!!     
      if(nHits == 24) LOSID = true;
           
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
      Int_t nPart = fCalItemsLos->GetEntriesFast();    
      Int_t iDet = 0;
                     
      for (Int_t iPart = 0; iPart < nPart; iPart++)     
      {

/* 
 * nPart is the number of particle passing through LOS detector in one event
 */ 
          R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(iPart);
    	  iDet=calData->GetDetector();

// At the moment no multi-particle events are analyzed; only those with nHits=24!!!

          cout<<iPart<<", "<<calData->fTimeV_r_ns<<", "<<calData->fTimeV_t_ns<<", "<<calData->fTimeV_b_ns<<", "<<
                             calData->fTimeV_rt_ns<<", "<<calData->fTimeV_lt_ns<<", "<<calData->fTimeV_lb_ns<<", "<<calData->fTimeV_rb_ns<<endl;

// lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7 
        if( iPart == 0) 
        {   	     	                 
   // VFTX Channels 1-4:
          if(!(IS_NAN(calData->fTimeV_r_ns))) time_V[5] = calData->fTimeV_r_ns;
          if(!(IS_NAN(calData->fTimeV_t_ns))) time_V[7] = calData->fTimeV_t_ns;
          if(!(IS_NAN(calData->fTimeV_l_ns))) time_V[1] = calData->fTimeV_l_ns;
          if(!(IS_NAN(calData->fTimeV_b_ns))) time_V[3] = calData->fTimeV_b_ns;  
   // VFTX Channels 5-8:
          if(!(IS_NAN(calData->fTimeV_rt_ns))) time_V[6] = calData->fTimeV_rt_ns;
          if(!(IS_NAN(calData->fTimeV_lt_ns))) time_V[0] = calData->fTimeV_lt_ns;
          if(!(IS_NAN(calData->fTimeV_lb_ns))) time_V[2] = calData->fTimeV_lb_ns;
          if(!(IS_NAN(calData->fTimeV_rb_ns))) time_V[4] = calData->fTimeV_rb_ns;                 
   // TAMEX Channels 1-4:      
          if(!(IS_NAN(calData->fTimeL_r_ns))) time_L[5] = calData->fTimeL_r_ns;
          if(!(IS_NAN(calData->fTimeT_r_ns))) time_T[5] = calData->fTimeT_r_ns;
          if(!(IS_NAN(calData->fTimeL_t_ns))) time_L[7] = calData->fTimeL_t_ns;
          if(!(IS_NAN(calData->fTimeT_t_ns))) time_T[7] = calData->fTimeT_t_ns;  
          if(!(IS_NAN(calData->fTimeL_l_ns))) time_L[1] = calData->fTimeL_l_ns;
          if(!(IS_NAN(calData->fTimeT_l_ns))) time_T[1] = calData->fTimeT_l_ns; 
          if(!(IS_NAN(calData->fTimeL_b_ns))) time_L[3] = calData->fTimeL_b_ns;
          if(!(IS_NAN(calData->fTimeT_b_ns))) time_T[3] = calData->fTimeT_b_ns;  
    // TAMEX Channels 5-8:
          if(!(IS_NAN(calData->fTimeL_rt_ns))) time_L[6] = calData->fTimeL_rt_ns;
          if(!(IS_NAN(calData->fTimeT_rt_ns))) time_T[6] = calData->fTimeT_rt_ns;
          if(!(IS_NAN(calData->fTimeL_lt_ns))) time_L[0] = calData->fTimeL_lt_ns;
          if(!(IS_NAN(calData->fTimeT_lt_ns))) time_T[0] = calData->fTimeT_lt_ns;  
          if(!(IS_NAN(calData->fTimeL_lb_ns))) time_L[2] = calData->fTimeL_lb_ns;
          if(!(IS_NAN(calData->fTimeT_lb_ns))) time_T[2] = calData->fTimeT_lb_ns; 
          if(!(IS_NAN(calData->fTimeL_rb_ns))) time_L[4] = calData->fTimeL_rb_ns;
          if(!(IS_NAN(calData->fTimeT_rb_ns))) time_T[4] = calData->fTimeT_rb_ns;  
                     
         }

      }
      
	                 
   if(iDet==1){
// lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7    
	        	        
	        for(int ipm=0; ipm<8; ipm++){     
             
             while(time_T[ipm] - time_L[ipm] <0.) 
             {
	          time_T[ipm] = time_T[ipm] + 2048.*fClockFreq; 
	         }
	        
             tot[ipm] = time_T[ipm] - time_L[ipm];
             totsum += tot[ipm];
             
             if(tot[ipm] != 0. ) fh_los_tot->Fill(ipm+1,tot[ipm]);
             
             timeLosM += time_V[ipm];
             timeLosT += time_L[ipm];
            }
            
            if(LOSID){
			 totsum = totsum/8.;
             timeLosM = timeLosM/8.;
             timeLosT = timeLosT/8.;        
             
			 LosTresM = ((time_V[0]+time_V[2]+time_V[4]+time_V[6]) - 
			             (time_V[1]+time_V[3]+time_V[5]+time_V[7]))/4.;  	  		          
             LosTresT = ((time_L[0]+time_L[2]+time_L[4]+time_L[6]) - 
			             (time_L[1]+time_L[3]+time_L[5]+time_L[7]))/4.;          
             
             fh_los_tres_MCFD->Fill(LosTresM);
             fh_los_tres_TAMEX->Fill(LosTresT);
             fh_los_tot_mean->Fill(totsum);
             
             if(!(timeLosM>0.) && IS_NAN(timeLosM)) cout<<"R3BOnline WARNING!! LOS time < 0 or nan! No ToF info for this event!! "<<timeLosM<<endl;
           }
  
    } else {
	  cout<<"Wrong detector ID for LOS!"<<endl;
      }
    }	


//----------------------------------------------------------------------
// Fiber detectors
//----------------------------------------------------------------------
      Double_t dtime = 0.0/0.0;
        
  for(Int_t ifibcount = 0; ifibcount < 14; ifibcount++){ 
	   
	   Int_t iFib = 0;  

	   if(FibPresent[ifibcount]){	   	 
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
         
         if (hit->IsMAPMT() && hit->IsLeading()) {
			 fh_channels_Fib[ifibcount]->Fill(iCha);    // Fill which clockTDC channel has events
		 
			 ++mapmt_num.at(hit->GetChannel() - 1);     // multihit of a given clockTDC channel
		  }
		  
         if (!hit->IsMAPMT() && hit->IsLeading()) {
			         
	         ++spmt_num.at(hit->GetChannel() - 1);      // multihit of a given PADI channel
	      }
      }

      for (int i = 0; i < 512; ++i) { 
        auto m = mapmt_num.at(i);
        if(m > 0) fh_multihit_m_Fib[ifibcount]->Fill(i,m);  // multihit of a given clockTDC channel      
	  }
      
      for (int i = 0; i < 16; ++i) {
        auto s = spmt_num.at(i);
        if(s > 0) fh_multihit_s_Fib[ifibcount]->Fill(i,s); // multihit of a given PADI channel
	  }

	  	  
    } // end if(aMapped[ifibcount])
    
    
    if(aHit.at(ifibcount)) {
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
			while(dtime < 1024.) {
				tSPMT = tSPMT - 2048.; 
				dtime = tMAPMT - tSPMT;
			}
			while(dtime > 1024.) {
				tSPMT = tSPMT + 2048.; 
				dtime = tMAPMT - tSPMT;
			}
			
			
	// "Push" the Fib times in the same cycle with LOS:
	      if(timeLosM>0. && !(IS_NAN(timeLosM))){
	        while(tMAPMT - timeLosM < 5000.){
				tMAPMT = tMAPMT + 2048.*4.; 
				tSPMT = tSPMT + 2048.*4.; 		  
			}		
			while(tMAPMT - timeLosM > 5000.){
				tMAPMT = tMAPMT - 2048.*4.; 
				tSPMT = tSPMT - 2048.*4.; 		  
			}
		  }
  
  // Not-calibrated ToF:	 
        tfib = (tMAPMT + tSPMT) / 2.;    
        if(tfib > 0. && !(IS_NAN(tfib)) && timeLosM>0. && !(IS_NAN(timeLosM))) tof_fib = tfib - timeLosM;

  // Not-calibrated position:  
        randx = (std::rand() / (float)RAND_MAX);
	    if(iFib > 0) posfib = (-n_fiber[ifibcount]/2.+iFib+(0.5-randx)); 
             
         if(hit->GetSPMTToT_ns() > 0) {
	     fh_fibers_Fib[ifibcount]->Fill(iFib);  
	     fh_ToT_s_Fib[ifibcount]->Fill(iFib,hit->GetSPMTToT_ns());
	     fh_ToT_m_Fib[ifibcount]->Fill(iFib,hit->GetMAPMTToT_ns());
	     fh_FibervsTime_Fib[ifibcount]->Fill(iFib,fNEvents); 
	     fh_time_Fib[ifibcount]->Fill(iFib,tMAPMT-tSPMT);
         fh_Fib_ToF[ifibcount]->Fill(iFib,tof_fib);
         fh_Fib_pos[ifibcount]->Fill(posfib);	     
	     }  
       
    }  // end for(ihit)
        
	    fh_mult_Fib[ifibcount]->Fill(nHits);            
          
   }  // end if(aHit[ifibcount]) 


 } // end for(ifibcount)

    if(fMappedItemsTofd )
    {
      Int_t nHits = fMappedItemsTofd->GetEntriesFast();  	
      
      fPaddlesPerPlane = 6;	  
      Int_t nMult = nHits/4/fPaddlesPerPlane;
    
    if(nMult == 0) return;
    if(nMult > 1) return; // Multihit to be included!!      
      
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

    
    if(fCalItemsTofd)
    {
 // This has to be commented or adjusted (at the moment onl 6 paddles are included in DAQ,
 // Paddle: 1,2,5,6,7,8		
	fPaddlesPerPlane = 6;	
    Int_t nHits = fCalItemsTofd->GetEntriesFast();
    Int_t nMult = nHits/4/fPaddlesPerPlane;
 
 // Two lines below: only events with nHits=fPaddlesPerPlane*4 are considered!!! Other events to be included!!!
    if(nMult == 0) return;
    if(nMult > 1) return; 
    
      Double_t tot1[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      Double_t tot2[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      Double_t t_paddle[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      Double_t t1l[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      Double_t t2l[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      Double_t t1t[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      Double_t t2t[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      Double_t ToF[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={0./0.};
      
      Bool_t Bar_present[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD]={false};

   for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
    	  R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItemsTofd->At(ihit);
          if (!hit) continue; // should not happen

          Int_t iPlane  = hit->GetPlane();    // 1..n
          Int_t iBar  = hit->GetBar();    // 1..n
          Bar_present[iPlane-1][iBar-1] = true;

                 // get all times of one bar
         
            if(!(IS_NAN(hit->fTime1L_ns))) t1l[iPlane-1][iBar-1]=hit->fTime1L_ns;
            if(!(IS_NAN(hit->fTime1T_ns))) t1t[iPlane-1][iBar-1]=hit->fTime1T_ns;
            if(!(IS_NAN(hit->fTime2L_ns))) t2l[iPlane-1][iBar-1]=hit->fTime2L_ns;
            if(!(IS_NAN(hit->fTime2T_ns))) t2t[iPlane-1][iBar-1]=hit->fTime2T_ns;
      }
       
      for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
       for(Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++){  
       if(Bar_present[ipl][ibr]) {
		  Int_t iPlane  = ipl+1;    // 1..n
          Int_t iBar  = ibr+1;    // 1..n
		   
	      // calculate time over threshold and check if clock counter went out of range
       
          while(t1t[iPlane-1][iBar-1] - t1l[iPlane-1][iBar-1] < 0.) {
	          t1t[iPlane-1][iBar-1]=t1t[iPlane-1][iBar-1]+2048.*fClockFreq; 
	      }

          while(t2t[iPlane-1][iBar-1]-t2l[iPlane-1][iBar-1] < 0.) {
	          t2t[iPlane-1][iBar-1]=t2t[iPlane-1][iBar-1]+2048.*fClockFreq; 
          }
	    
	      while(t1l[iPlane-1][iBar-1]-timeLosM < 0.){
	          t1t[iPlane-1][iBar-1]=t1t[iPlane-1][iBar-1]+2048.*fClockFreq; 
	          t1l[iPlane-1][iBar-1]=t1l[iPlane-1][iBar-1]+2048.*fClockFreq; 
	          t2t[iPlane-1][iBar-1]=t2t[iPlane-1][iBar-1]+2048.*fClockFreq; 
	          t2l[iPlane-1][iBar-1]=t2l[iPlane-1][iBar-1]+2048.*fClockFreq; 			  
		  }
		  	
	// ToF	  
		  t_paddle[iPlane-1][iBar-1] = (t1l[iPlane-1][iBar-1] + t2l[iPlane-1][iBar-1])/2.;
		  ToF[iPlane-1][iBar-1] = t_paddle[iPlane-1][iBar-1] - timeLosM;
		  fh_tofd_ToF[iPlane-1]->Fill(iBar,ToF[iPlane-1][iBar-1]);
		   
    // ToT   
          tot1[iPlane-1][iBar-1]=t1t[iPlane-1][iBar-1] - t1l[iPlane-1][iBar-1];		      
          if(tot1[iPlane-1][iBar-1]<0) {
	        cout<<"Negative ToT "<<tot1[iPlane-1]<<endl; 	
	        cout<<"Times1: "<<t1t[iPlane-1][iBar-1]<<" "<<t1l[iPlane-1][iBar-1]<<endl; 		  
	      }

          tot2[iPlane-1][iBar-1]=t2t[iPlane-1][iBar-1] - t2l[iPlane-1][iBar-1];	
                            
          if(tot2[iPlane-1][iBar-1]<0) {
              cout<<"Negative ToT "<<tot2[iPlane-1][iBar-1]<<endl;              
              cout<<"Times2: "<<t2t[iPlane-1][iBar-1]<<" "<<t2l[iPlane-1][iBar-1]<<endl;		 
          }
          fh_tofd_TotPm1[iPlane-1]->Fill(iBar,tot1[iPlane-1][iBar-1]);
          fh_tofd_TotPm2[iPlane-1]->Fill(iBar,tot2[iPlane-1][iBar-1]);
        	
      }
     }
    }

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
				
			if(bar_quer1 && bar_quer2){
//				fh_ptof_TotPm1[iBar]->Fill(tot1);
//				fh_ptof_TotPm2[iBar]->Fill(tot2);
//				if(iBar==2) fh_ptof_test2->Fill(sqrt(tot1*tot2));	
				fh_ptof_channels_cut->Fill(iBar);			
			}			
		}
	}

   
    fNEvents += 1;
//	cout<<"R3BOnlineSpectra end"<<endl;

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
     fh_FibervsTime_Fib[ifibcount]->Write();			
      
    }
 }   		 
 

}

ClassImp(R3BOnlineSpectra)
