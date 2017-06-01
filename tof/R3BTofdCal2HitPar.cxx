// ----------------------------------------------------------------
// -----            R3BTofdCal2HitPar             -----
// -----           Created May 2016 by M.Heil             -----
// ----------------------------------------------------------------

/* Some notes:
 * 
 * 
 */

#include "R3BTofdCal2HitPar.h"
#include "R3BPaddleCalData.h"
#include "R3BEventHeader.h"
#include "R3BTofdHitPar.h"
#include "R3BLosMappedData.h"
#include "R3BLosCalData.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunIdGenerator.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TProfile.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BTofdCal2HitPar::R3BTofdCal2HitPar()
    : FairTask("R3BTofdCal2HitPar", 1)
    , fCalItemsLos(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTofdY(0.)
    , fTofdQ(0.)
    , fParaFile("")
{
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            fhTdiff[i][j] = NULL;
            fhTotPm1[i][j] = NULL;		
	        fhTotPm2[i][j] = NULL;
	        fhTot1vsTot2[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;		
            fhTot2vsPos[i][j] = NULL;
            fhTsync[i][j] = NULL;
	    }
    }
}


R3BTofdCal2HitPar::R3BTofdCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTofdY(0.)
    , fTofdQ(0.)
    , fParaFile("")
{
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            fhTdiff[i][j] = NULL;
            fhTotPm1[i][j] = NULL;		
            fhTotPm2[i][j] = NULL;
            fhTot1vsTot2[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;		
            fhTot2vsPos[i][j] = NULL;
            fhTsync[i][j] = NULL;
        } 
    }
}


R3BTofdCal2HitPar::~R3BTofdCal2HitPar()
{
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            if (fhTdiff[i][j]) delete fhTdiff[i][j];
            if (fhTotPm1[i][j]) delete fhTotPm1[i][j];
            if (fhTotPm2[i][j]) delete fhTotPm2[i][j];
            if (fhTot1vsTot2[i][j]) delete fhTot1vsTot2[i][j];
            if (fhTot1vsPos[i][j]) delete fhTot1vsPos[i][j];
            if (fhTot2vsPos[i][j]) delete fhTot2vsPos[i][j];
            if (fhTsync[i][j]) delete fhTsync[i][j];      
        }
    }
    if (fCal_Par)
    {
        delete fCal_Par;
    }
}

InitStatus R3BTofdCal2HitPar::Init()
{	
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    
    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
	// may be = NULL!

	
    fCalData = (TClonesArray*)rm->GetObject("TofdCal");
    if (!fCalData)
    {
        return kFATAL;
    }

    if (!fNofModules)
    {
		LOG(ERROR) << "R3BTofdCal2HitPar::Init() Number of modules not set. " << FairLogger::endl;
        return kFATAL;
    }
    fCalItemsLos = (TClonesArray*)rm->GetObject("LosCal");
    if (NULL == fCalItemsLos)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LosCal not found");


    return kSUCCESS;
}

void R3BTofdCal2HitPar::SetParContainers()
{
	// container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
	// to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCal_Par = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
	if (!fCal_Par)
	{
		LOG(ERROR) << "R3BTofdCal2HitPar::Init() Couldn't get handle on TofdHitPar. " << FairLogger::endl;
	}
//	    fCal_Par->setChanged();
}

void R3BTofdCal2HitPar::Exec(Option_t* option)
{
	// test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger)) 
		return;

    Double_t tdiff=0.;
    Double_t tot1=0.;
    Double_t tot2=0.;
    Double_t tot1sat=0.;
    Double_t tot2sat=0.;
    Double_t t1l=0.;
    Double_t t2l=0.;
    Double_t t1t=0.;
    Double_t t2t=0.;
    Double_t timeLos=0;
    Double_t t[N_PLANE_MAX+1][N_PADDLE_MAX+1], Q[N_PLANE_MAX+1][N_PADDLE_MAX+1];     
     
    for(Int_t i=1;i<=N_PLANE_MAX;i++){
        for(Int_t j=1;j<=N_PADDLE_MAX;j++){
            t[i][j]=0.;
            Q[i][j]=0.;	  
        }    
    }
    
    if(fCalItemsLos)
    {
        Int_t nHits = fCalItemsLos->GetEntriesFast();    
        for (Int_t ihit = 0; ihit < nHits; ihit++)     
        {
    	    R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(ihit);
            timeLos=(calData->fTimeV_r_ns+calData->fTimeV_l_ns+calData->fTimeV_t_ns+calData->fTimeV_b_ns)/4.;			 
        }						 
    }
    
    Int_t nHits = fCalData->GetEntries();
    
//    LOG(WARNING) << " new event "<< timeLos<< FairLogger::endl;

    // Loop over cal hits
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
    	R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalData->At(ihit);
        if (!hit) continue; // should not happen

        Int_t iPlane  = hit->GetPlane();    // 1..n
        Int_t iBar  = hit->GetBar();    // 1..n

        
        if (iPlane>=fNofPlanes) // this also errors for iDetector==0
        {
            LOG(ERROR) << "R3BTofdCal2HitPar::Exec() : more detectors than expected! Det: " << iPlane << " allowed are 1.." << fNofPlanes << FairLogger::endl;
            continue;
        }
        if (iBar>=fPaddlesPerPlane) // same here
        {
            LOG(ERROR) << "R3BTofdCal2HitPar::Exec() : more bars then expected! Det: " << iBar << " allowed are 1.." << fPaddlesPerPlane << FairLogger::endl;
            continue;
        }


        if (NULL == fhTdiff[iPlane - 1][iBar - 1])
        {
            char strName1[255];
            char strName2[255];
            sprintf(strName1, "Time_Diff_Plane_%d_Bar_%d", iPlane, iBar);
            sprintf(strName2, "Time Diff Plane %d Bar %d", iPlane, iBar);
            fhTdiff[iPlane - 1][iBar - 1] = new TH1F(strName1, strName2, 4000, -20., 20.);
            fhTdiff[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Time difference (PM1 - PM2) in ns");
            fhTdiff[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Counts");
        }
        if (NULL == fhTotPm1[iPlane - 1][iBar - 1])
        {
            char strName[255];
            sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_1", iPlane, iBar);
            fhTotPm1[iPlane - 1][iBar - 1] = new TH1F(strName, "", 5000, 0., 500.);
            fhTotPm1[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM1 in ns");
            fhTotPm1[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Counts");
        }
        if (NULL == fhTotPm2[iPlane - 1][iBar - 1])
        {
            char strName[255];
            sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_2", iPlane, iBar);
            fhTotPm2[iPlane - 1][iBar - 1] = new TH1F(strName, "", 5000, 0., 500.);
            fhTotPm2[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM2 in ns");
            fhTotPm2[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Counts");
        }
        if (NULL == fhTot1vsTot2[iPlane - 1][iBar - 1])
        {
            char strName[255];
            sprintf(strName, "Plane_%d_Bar_%d_ToT1vsToT2", iPlane, iBar);
            fhTot1vsTot2[iPlane - 1][iBar - 1] = new TH2F(strName, "", 500, 0., 500.,500,0.,500.);
            fhTot1vsTot2[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM2 in ns");
            fhTot1vsTot2[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Tot of PM1 in ns");
        }
        if (NULL == fhTot1vsPos[iPlane - 1][iBar - 1])
        {
            char strName[255];
            sprintf(strName, "Tot1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
            if(iPlane<3) fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 5000, 0., 1000.);
            if(iPlane>2) fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 5000, 0., 1000.);
            fhTot1vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM2 in ns");
            fhTot1vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Tot of PM1 in ns");
             
           
        }
        if (NULL == fhTot2vsPos[iPlane - 1][iBar - 1])
        {
            char strName[255];
            sprintf(strName, "Tot2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
            if(iPlane<3) fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 5000, 0., 1000.);
            if(iPlane>2) fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 5000, 0., 1000.);
        }
        if (NULL == fhTsync[iPlane - 1][iBar - 1])
        {
            char strName[255];
            sprintf(strName, "Time_Sync_Plane_%d_Bar_%d", iPlane, iBar);
	    // for runs starting at 025:
            if (iPlane<3) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 20000, 180., 280.);
            if (iPlane>2) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 100000, 2100., 2400.);            
	    // for runs starting at 054:
            //if (iPlane<3) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 20000,  15700., 15800.);
            //if (iPlane>2) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 20000,  6800., 6900.);            
	    // for runs starting at 071:
            //if (iPlane<3) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 40000,  6850., 7050.);
            //if (iPlane>2) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 20000,  11300., 11400.);            
	    // for runs starting at 105:
            //if (iPlane<3) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 40000,  15400., 15600.);
            //if (iPlane>2) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 40000,  15400., 15600.);            
	    // for runs starting at 137:
           // fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 50000, 4400., 4500.);
 	    // for runs starting at 160:
            //if (iPlane<3) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 50000, 13800., 13900.);
            //if (iPlane>2) fhTsync[iPlane - 1][iBar - 1] = new TH1F(strName, "", 50000, 13800., 13900.);            
        }

//LOG(WARNING) << "Plane "<<iPlane << " Bar "<<iBar <<" leading "<< hit->fTime1L_ns<<" trailing "<<hit->fTime1T_ns<<FairLogger::endl;

	//Time over threshold ***********************
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



        // correction for saturation of PMT
//	tot1sat=tot1+saturation(tot1);
//	tot2sat=tot2+saturation(tot2);

        fhTotPm1[iPlane - 1][iBar - 1]->Fill(tot1);
        fhTotPm2[iPlane - 1][iBar - 1]->Fill(tot2);
        fhTot1vsTot2[iPlane - 1][iBar - 1]->Fill(tot2,tot1);

//        LOG(WARNING) << "Plane "<<iPlane << " Bar "<<iBar <<" ToT1 "<< tot1<<" ToT2 "<<tot2<<FairLogger::endl;
//        LOG(WARNING) << "Plane "<<iPlane << " Bar "<<iBar <<" sat1 "<< saturation(tot1)<<" sat2 "<<saturation(tot2)<<FairLogger::endl;
        
        if (iPlane>10 && iBar>0) LOG(WARNING) << " ToT1 "<< tot1<<" ToT2 "<<tot2<<FairLogger::endl;
	
        //Time differences of one paddle **************************
        
	// walk corrections
//        t1=t1-walk(tot1);
//        t2=t2-walk(tot2);
//        LOG(WARNING) << "Plane "<<iPlane << " Bar "<<iBar <<" wlk1 "<< walk(tot1)<<" wlk2 "<<walk(tot2)<<FairLogger::endl;
//        LOG(WARNING) << "Plane "<<iPlane << " Bar "<<iBar <<" t1 "<< t1<<" t2 "<< t2<<FairLogger::endl;
        tdiff=t1l-t2l;
        fhTdiff[iPlane - 1][iBar - 1]->Fill(tdiff);
 
        t[iPlane][iBar]=(t1l+t2l)/2.;
        
        Q[iPlane][iBar]=sqrt(tot1*tot2);
        fhTsync[iPlane-1][iBar-1]->Fill(t[iPlane][iBar]-timeLos);
//        LOG(WARNING) << "Plane "<<iPlane << " Bar "<<iBar <<" Los "<< timeLos <<" t "<<t[iPlane][iBar]<<" final "<<t[iPlane][iBar]-timeLos <<FairLogger::endl;
//        LOG(WARNING) << "t1 "<< t1 << " t2 "<<t2 <<FairLogger::endl;
        
        
        if(fTofdQ!=0){
            R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(iPlane, iBar);
	
	        Double_t Pos=((t1l+par->GetOffset1())-(t2l+par->GetOffset2())) * par->GetVeff();
	  
	
            fhTot1vsPos[iPlane-1][iBar-1]->Fill(Pos, tot1);
            fhTot2vsPos[iPlane-1][iBar-1]->Fill(Pos, tot2);
	    }
    }
    // Increment events
    fNEvents += 1;
}

void R3BTofdCal2HitPar::FinishEvent()
{
	    if (fCalItemsLos)
    {
        fCalItemsLos->Clear();
    }

}

void R3BTofdCal2HitPar::FinishTask()
{
 
   for (Int_t i = 0; i < N_PLANE_MAX; i++)
  {
      for (Int_t j = 0; j < N_PADDLE_MAX; j++)
      { 
          if (fhTot1vsPos[i][j]) fhTot1vsPos[i][j]->Write();
          if (fhTot2vsPos[i][j]) fhTot2vsPos[i][j]->Write();
          if (fhTotPm1[i][j]) fhTotPm1[i][j]->Write();
          if (fhTotPm2[i][j]) fhTotPm2[i][j]->Write();
          if (fhTot1vsTot2[i][j]) fhTot1vsTot2[i][j]->Write();
          if (fhTot1vsTot2[i][j]) fhTot1vsTot2[i][j]->Write();
          if (fhTsync[i][j]) fhTsync[i][j]->Write();
          if (fhTdiff[i][j]) fhTdiff[i][j]->Write();

      }
   }
 
   // Determine time offset of the 2 PMTs of one paddle. This procedure 
   // assumes a sweep run in the middle of the ToF wall horizontally. 
   // Since all paddles are mounted vertically one can determine the offset. 
   // Half of the offset is added to PM1 and half to PM2.


   if(fTofdY==0 && fTofdQ==0){
     calcOffset();
     calcSync();
   }
   else if(fTofdY!=0 && fTofdQ==0){
     calcVeff();
   }

   Double_t para[4];
   Double_t min=-35;
   Double_t max= 35;
   if(fTofdQ!=0){
      if(fParaFile!=""){
//		  char filename1[100];
//	      sprintf(filename1,"para_paddles_run137.dat"); 
          ifstream a_file ( fParaFile );
          if(!a_file){
              cout<<"Could not open file!!!"<<endl;
              exit(1);
          }
          Int_t plane, bar, pm, p;
          
          while(!a_file.eof()){ 
	          for(Int_t j=0;j<=3;j++){   
	              a_file>> plane >> bar >> pm >> p >> para[j];
	              cout<<"Reading Parameter, plane "<<plane<< " bar "<<bar<<" PM "<<pm <<" #"<<p<<" : "<<para[j]<<endl;
	          }
	          R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(plane, bar);
			  Double_t offset1=par->GetOffset1();
	          Double_t offset2=par->GetOffset2();
	          Double_t veff=par->GetVeff();
	          Double_t sync=par->GetSync();
	          if(pm==1){
	              par->SetPar1a(para[0]);
	              par->SetPar1b(para[1]);
	              par->SetPar1c(para[2]);
	              par->SetPar1d(para[3]);
	          }
	          else{
	              par->SetPar2a(para[0]);
	              par->SetPar2b(para[1]);
	              par->SetPar2c(para[2]);
	              par->SetPar2d(para[3]);
	 	      }
          }
          a_file.close();	 
	  } 
      else{
	      for (Int_t i = 0; i < N_PLANE_MAX; i++)
	      {
	         for (Int_t j = 0; j < N_PADDLE_MAX; j++)
	         {
	            if (fhTot1vsPos[i][j]) {
	                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i+1, j+1);
	                
	                doubleExp(fhTot1vsPos[i][j],min,max,para);
			        Double_t offset1=par->GetOffset1();
			        Double_t offset2=par->GetOffset2();
			        Double_t veff=par->GetVeff();
			        Double_t sync=par->GetSync();
			        par->SetPar1a(para[0]);
	 		        par->SetPar1b(para[1]);
			        par->SetPar1c(para[2]);
			        par->SetPar1d(para[3]);
		        }   		     	  
	
	            if (fhTot2vsPos[i][j]) {
	                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i+1, j+1);
	                doubleExp(fhTot2vsPos[i][j],min ,max,para);
			        Double_t offset1=par->GetOffset1();
			        Double_t offset2=par->GetOffset2();
			        Double_t veff=par->GetVeff();
			        Double_t sync=par->GetSync();
	                par->SetPar2a(para[0]);
	                par->SetPar2b(para[1]);
	                par->SetPar2c(para[2]);
	                par->SetPar2d(para[3]);
	            }
	         }
	      } 
      }
      fCal_Par->setChanged();
  }

}

void R3BTofdCal2HitPar::calcOffset()
{

/*
   const Int_t npeaks = 10;
   Double_t sigma=1.;
   Double_t sens=2.;   
   Double_t xp;
   Int_t nfound;

 
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            if (fhTdiff[i][j]) {
                LOG(WARNING) << " Plane "<< i+1<<" Bar "<< j+1<<FairLogger::endl;  
                nfound=0;
                TH1F *h = (TH1F*) fhTdiff[i][j]->Clone("h");
		        h->Draw();
                TSpectrum *s = new TSpectrum(2*npeaks);
                LOG(DEBUG) << " Search for peaks "<<  FairLogger::endl;  
                nfound = s->Search(h,sigma,"", sens);
                LOG(DEBUG) << " Found "<< nfound<<" peaks."<< FairLogger::endl;  

                Double_t *xpeaks = s->GetPositionX();
                for (Int_t p=0;p<nfound;p++) {
                    xp = xpeaks[p];		    
                    LOG(WARNING) << " Position "<< xp<< FairLogger::endl;  
                }
             }
	     }
    }
  */
  
   	R3BTofdHitModulePar *mpar;
	for (Int_t i = 0; i < N_PLANE_MAX; i++)
	{
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
        	mpar = new R3BTofdHitModulePar();
        	Double_t offset = 0.;
        	if (fhTdiff[i][j]) {
                Double_t veff = mpar->GetVeff();
                Int_t binmax = fhTdiff[i][j]->GetMaximumBin();
                offset = fhTdiff[i][j]->GetXaxis()->GetBinCenter(binmax);
                LOG(WARNING) << " Plane  "<< i+1 << " Bar "<< j+1<< " Offset  "<<  offset<< FairLogger::endl;                   
                mpar->SetPlane(i+1);
                mpar->SetPaddle(j+1);
                mpar->SetOffset1(-offset/2.);
                mpar->SetOffset2(offset/2.);
                mpar->SetVeff(veff);
                fCal_Par->AddModulePar(mpar);
            }
        }
	}
    fCal_Par->setChanged();

}

void R3BTofdCal2HitPar::calcSync()
{

    for (Int_t i = 0; i < N_PLANE_MAX; i++){	    
        for (Int_t j = 0; j < N_PADDLE_MAX; j++){        
			if(fhTsync[i][j]){
                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i+1, j+1);
                Double_t offset1=par->GetOffset1();
                Double_t offset2=par->GetOffset2();
                Int_t binmax = fhTsync[i][j]->GetMaximumBin();
                Double_t sync = fhTsync[i][j]->GetXaxis()->GetBinCenter(binmax);
                par->SetOffset1(offset1);
                par->SetOffset2(offset2);		
                par->SetSync(sync);		
                LOG(WARNING) << " Plane  "<< i+1 << " Bar "<< j+1<< " Sync  "<<  sync << FairLogger::endl;    
		    }
        }               
       
    }

    fCal_Par->setChanged();

}

void R3BTofdCal2HitPar::calcVeff()
{
	    for (Int_t i = 0; i < N_PLANE_MAX; i++)
	    {
            for (Int_t j = 0; j < N_PADDLE_MAX; j++)
            {
        	Double_t max = 0.;
		    Double_t veff = 7.;
        	if (fhTdiff[i][j]) {
                R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(i+1, j+1);
                if (!par)
                {
                    LOG(INFO) << "R3BTofdCal2Hit::Exec : Hit par not found, Plane: " << 
                    i+1 << ", Bar: " << j+1 << FairLogger::endl;
                    continue;
                }

                Int_t binmax = fhTdiff[i][j]->GetMaximumBin();
                max = fhTdiff[i][j]->GetXaxis()->GetBinCenter(binmax);
		        Double_t offset1=par->GetOffset1();
		        Double_t offset2=par->GetOffset2();
		        Double_t sync=par->GetSync();
		    
		        max = max+offset1-offset2;
		        veff=fTofdY/max; // effective speed of light in [cm/s]
                LOG(WARNING) << " Plane  "<< i+1 << " Bar "<< j+1<< " offset  "<< par->GetOffset1() << FairLogger::endl;                   
                LOG(WARNING) << " Plane  "<< i+1 << " Bar "<< j+1<< " max  "<<  max << FairLogger::endl;                   
                LOG(WARNING) << " Plane  "<< i+1 << " Bar "<< j+1<< " veff  "<<  veff << FairLogger::endl;                   
          	    par->SetVeff(veff);
        	}

        }
	}
    fCal_Par->setChanged();

}

void R3BTofdCal2HitPar::doubleExp(TH2F *histo, Double_t min, Double_t max, Double_t *para)
{
	// This fits the exponential decay of the light in a paddle. The 2 PMTs are fit with the same function but one 
	// side will deliver negative attenuation parameters and the other positive.
    Double_t y[1000], x[1000]; 
    Int_t n=0;
    for(Int_t j=0;j<=3;j++){   
        para[j]=0;
    }
    TGraph *gr1=new TGraph();
    TGraph *gr2=new TGraph();
    TCanvas *cfit_exp=new TCanvas("cfit_exp","fit exponential",0,0,800,800);   

    cfit_exp->Clear();
    cfit_exp->Divide(1,3);
    cfit_exp->cd(1);
    TH2F *histo1=(TH2F*)histo->Clone();
    TH2F *histo2=(TH2F*)histo->Clone();

    histo1->Draw("colz");
    
    cfit_exp->cd(2);
   
    for(Int_t i=1; i<histo1->GetNbinsX()-1; i++){
        TH1F *histo_py = (TH1F*)histo1->ProjectionY("histo_py",i,i,"");
        histo_py->Draw();
        x[n]=histo1->GetXaxis()->GetBinCenter(i);      
        Int_t binmax = histo_py->GetMaximumBin();
        y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
        if(histo_py->GetMaximum()>100) n++; 
        delete histo_py;
    }

    gr1=new TGraph(n,x,y);
    gr1->Draw("A*");

    TF1 *f1 = new TF1("f1","[0]*(exp(-[1]*(x+50))+exp(-[2]*(x+50)))+[3]", -40, 40);
    f1->SetParameters(110.,0.01 ,1.1 ,40.);
    f1->SetLineColor(2); 
    gr1->Fit("f1","","",min,max);
    for(Int_t j=0;j<=3;j++){   
        para[j] = f1->GetParameter(j); 
        cout<<"Parameter: "<<para[j]<<endl;
    }
    
    // fit again but with more information and better cuts
    n=0;
    cfit_exp->cd(3);
    
    for(Int_t i=1; i<histo2->GetNbinsX(); i++){
        Double_t pos= histo2->GetXaxis()->GetBinCenter(i);
        Double_t ymean= para[0]*(exp(-para[1]*(pos+50))+exp(-para[2]*(pos+50)))+para[3];
        histo2->SetAxisRange(ymean-5.,ymean+5.,"Y");
        histo2->Draw("colz");
        TH1F *histo_py = (TH1F*)histo2->ProjectionY("histo_py",i,i,"");
        histo_py->Draw();
        x[n]=histo2->GetXaxis()->GetBinCenter(i);      
        Int_t binmax = histo_py->GetMaximumBin();
        y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
        if(histo_py->GetMaximum()>100) n++; 
        delete histo_py;
   }
    
    gr2=new TGraph(n,x,y);
    gr2->Draw("A*");

    TF1 *f2 = new TF1("f2","[0]*(exp(-[1]*(x+50))+exp(-[2]*(x+50)))+[3]", -40, 40);
    f2->SetParameters(para[0],para[1],para[2],para[3]);
    f2->SetLineColor(2); 
    gr2->Fit("f2","","",min,max);
    for(Int_t j=0;j<=3;j++){   
        para[j] = f2->GetParameter(j); 
        cout<<"Parameter: "<<para[j]<<endl;
    }
    
    cfit_exp->Update();
    gPad->WaitPrimitive();    
    delete gr1;
    delete gr2;
    delete f1; 
    delete f2; 
}


Double_t R3BTofdCal2HitPar::walk(Double_t Q)
{
    Double_t par1,par2,par3,par4,par5;
    Double_t y=0;
       
    Int_t voltage=600;    

   if(voltage==500){
     par1=1.64344e+01;    
     par2=2.84000e-01;
     par3= 3.47659e+02  ;    
     par4= -2.70050e-01 ;   
     par5= 3.61515e-04  ;  
    }
    
  
    
    if(voltage==600){
     par1= 1.22606e+01;   
     par2= 3.12697e-01  ;
     par3= 4.40109e+02  ;    
     par4=-1.86328e-01  ;   
     par5= 1.49519e-04  ;  
    
    }

    if(Q>0 && Q<430)   y=-30.2+par1*TMath::Power(Q,par2)+par3/Q+par4*Q+par5*Q*Q; 
    else y=0;
    
    return y;
}

Double_t R3BTofdCal2HitPar::saturation(Double_t x)
{
   Double_t kor;
   Int_t voltage=600;
   
   if(voltage==600){
     if(x<173) {
        kor=0.;
     }
     else if (x>208) {
      kor= -1.73665e+03+2.82009e+01*208.-1.53846e-01*(208.*208.)+2.82425e-04*(208.*208.*208.);
		 
     }
     else  {
      kor= -1.73665e+03+2.82009e+01*x-1.53846e-01*(x*x)+2.82425e-04*(x*x*x);
     }
   }
   
   
     if(voltage==500){
     if(x<95.5) {
        kor=0.;
     }
     else if (x>124) {
       kor=1.08*x-112.44;
     }
     else  {
      kor=643.257-16.7823*x+0.139822*(x*x)-0.000362154  *(x*x*x);
     }
   }
   
   
   
   
    if(voltage==700){
     if(x<198) {
        kor=0.;
     }
     else if (x>298) {
       kor=0.21*x-45.54;
     }
     else  {
      kor= -19067+383.93*x -3.05794*(x*x)+0.0120429 *(x*x*x)-2.34619e-05*(x*x*x*x)+ 1.81076e-08 *(x*x*x*x*x);
     }
   }
   
   
   
   
   
   return kor;


}


ClassImp(R3BTofdCal2HitPar)
