// ------------------------------------------------------------
// -----                  R3BTofdCal2Hit                  -----
// -----          Created May 2016 by M.Heil        -----
// ------------------------------------------------------------

#include "R3BTofdCal2Hit.h"
#include "R3BLosCalData.h"
#include "R3BPaddleCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdHitPar.h"
#include "R3BTofdHitModulePar.h"
#include "R3BTCalEngine.h"

#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TClonesArray.h"
#include "TMath.h"

#define N_PLANE_MAX 100
#define N_PADDLE_MAX 100

R3BTofdCal2Hit::R3BTofdCal2Hit()
    : FairTask("TofdCal2Hit", 1)
    , fCalItems(NULL)
    , fCalItemsLos(NULL)
    , fHitItems(new TClonesArray("R3BTofdHitData"))
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fTrigger(-1)
    , fNofPlanes(4)  
    , fPaddlesPerPlane(6)    
    , fTofdQ(1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhxy = NULL;		
    fhCharge = NULL;		
    fhCharge3mm = NULL;		
    fhCharge5mm = NULL;		
    fhSaturation1 = NULL;
    fhSaturation2 = NULL;
    fhCharge3mm_vs_x = NULL;
    fhCharge3mm_vs_y = NULL;
    fhCharge5mm_vs_x = NULL;
    fhCharge5mm_vs_y = NULL;
    
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            fhPos[i][j] = NULL;
            fhTotPm1[i][j] = NULL;		
	        fhTotPm2[i][j] = NULL;
            fhTotPm1Sat[i][j] = NULL;		
	        fhTotPm2Sat[i][j] = NULL;
            fhQPm1[i][j] = NULL;		
	        fhQPm2[i][j] = NULL;
	        fhQ[i][j] = NULL;
            fhQ1vsPos[i][j] = NULL;		
	        fhQ2vsPos[i][j] = NULL;
	        fhQvsPos[i][j] = NULL;
	        fhSqrtQvsPos[i][j] = NULL;
	        fhTotPm1vsTotPm2[i][j] = NULL;
	        fhTotPm1satvsTotPm2sat[i][j] = NULL;
	        fhTdiffvsQ[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;		
	        fhTot2vsPos[i][j] = NULL;
	        fhQvsQ[i][j] = NULL;
	        fhQvsTof[i][j] = NULL;
	    }
    }
}

R3BTofdCal2Hit::R3BTofdCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fCalItemsLos(NULL)
    , fHitItems(new TClonesArray("R3BTofdHitData"))
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fTrigger(-1)
    , fNofPlanes(4)  
    , fPaddlesPerPlane(6)    
    , fTofdQ(1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhxy = NULL;		
    fhCharge = NULL;		
    fhCharge3mm = NULL;		
    fhCharge5mm = NULL;		
    fhSaturation1 = NULL;
    fhSaturation2 = NULL;
    fhCharge3mm_vs_x = NULL;
    fhCharge3mm_vs_y = NULL;
    fhCharge5mm_vs_x = NULL;
    fhCharge5mm_vs_y = NULL;
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            fhPos[i][j] = NULL;
            fhTotPm1[i][j] = NULL;		
            fhTotPm2[i][j] = NULL;
            fhTotPm1Sat[i][j] = NULL;		
            fhTotPm2Sat[i][j] = NULL;
            fhQPm1[i][j] = NULL;		
            fhQPm2[i][j] = NULL;
            fhQ[i][j] = NULL;
            fhQ1vsPos[i][j] = NULL;		
	        fhQ2vsPos[i][j] = NULL;
	        fhQvsPos[i][j] = NULL;
	        fhSqrtQvsPos[i][j] = NULL;
	        fhTotPm1vsTotPm2[i][j] = NULL;
	        fhTotPm1satvsTotPm2sat[i][j] = NULL;
 	        fhTdiffvsQ[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;		
	        fhTot2vsPos[i][j] = NULL;
 	        fhQvsQ[i][j] = NULL;
 	        fhQvsTof[i][j] = NULL;
        } 
    }
}

R3BTofdCal2Hit::~R3BTofdCal2Hit()
{
    if (fhxy) delete fhxy ;		
    if (fhCharge) delete  fhCharge;		
    if (fhCharge3mm) delete  fhCharge3mm;		
    if (fhCharge5mm) delete  fhCharge5mm;		
    if (fhSaturation1) delete fhSaturation1;
    if (fhSaturation2) delete fhSaturation2;
    if (fhCharge3mm_vs_x) delete  fhCharge3mm_vs_x;		
    if (fhCharge3mm_vs_y) delete  fhCharge3mm_vs_y;		
    if (fhCharge5mm_vs_x) delete  fhCharge5mm_vs_x;		
    if (fhCharge5mm_vs_y) delete  fhCharge5mm_vs_y;		
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            if (fhPos[i][j]) delete fhPos[i][j];
            if (fhTotPm1[i][j]) delete fhTotPm1[i][j];
            if (fhTotPm2[i][j]) delete fhTotPm2[i][j];
            if (fhTotPm1Sat[i][j]) delete fhTotPm1[i][j];
            if (fhTotPm2Sat[i][j]) delete fhTotPm2[i][j];
            if (fhQPm1[i][j]) delete fhQPm1[i][j];
            if (fhQPm2[i][j]) delete fhQPm2[i][j];
            if (fhQ[i][j]) delete fhQ[i][j];
            if (fhQ1vsPos[i][j]) delete fhQ1vsPos[i][j];
            if (fhQ2vsPos[i][j]) delete fhQ2vsPos[i][j];
            if (fhQvsPos[i][j]) delete fhQvsPos[i][j];
            if (fhSqrtQvsPos[i][j]) delete fhSqrtQvsPos[i][j];
            if (fhTotPm1vsTotPm2[i][j]) delete fhTotPm1vsTotPm2[i][j];
            if (fhTotPm1satvsTotPm2sat[i][j]) delete fhTotPm1satvsTotPm2sat[i][j];
            if (fhTdiffvsQ[i][j]) delete fhTdiffvsQ[i][j];
            if (fhTot1vsPos[i][j]) delete fhTot1vsPos[i][j];
            if (fhTot2vsPos[i][j]) delete fhTot2vsPos[i][j];
	        if (fhQvsQ[i][j]) delete fhQvsQ[i][j];
	        if (fhQvsTof[i][j]) delete fhQvsTof[i][j];
        }
    }
    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
}

InitStatus R3BTofdCal2Hit::Init()
{
    fHitPar = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fHitPar)
    {
		LOG(ERROR) << "Could not get access to TofdHitPar-Container." << FairLogger::endl;
		fNofHitPars=0;
		return kFATAL;
    }

    fNofHitPars = fHitPar->GetNumModulePar();
    if (fNofHitPars==0)
    {
	LOG(ERROR) << "There are no Hit parameters in container TofdHitPar" << FairLogger::endl;
	return kFATAL;
    }
    
    // get access to Cal data
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    fCalItems = (TClonesArray*)mgr->GetObject("TofdCal");
    if (NULL == fCalItems)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch TofdCal not found");
    fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");
     if (NULL == fCalItemsLos)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LosCal not found");


    // request storage of Hit data in output tree
    mgr->Register("TofdHit", "Land", fHitItems, kTRUE);


    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BTofdCal2Hit::SetParContainers()
{
    fHitPar = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fHitPar)
    {
		LOG(ERROR) << "Could not get access to TofdHitPar-Container." << FairLogger::endl;
		fNofHitPars=0;
		return;
	}
}

InitStatus R3BTofdCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BTofdCal2Hit::Exec(Option_t* option)
{
      Double_t tot1=0.;
      Double_t tot2=0.;
      Double_t tot1sat=0.;
      Double_t tot2sat=0.;
      Double_t t1l=0.;
      Double_t t2l=0.;
      Double_t t1t=0.;
      Double_t t2t=0.;
      Double_t q1=0.;
      Double_t q2=0.;
      Double_t pos=1000.;
      Double_t timeRef=0.;
      Double_t q41=0;
      Double_t q42=0;
      Double_t q31=0;
      Double_t q32=0;
      Double_t timeLos=0;
    
      Double_t t[N_PLANE_MAX+1][N_PADDLE_MAX+1],x[N_PLANE_MAX+1][N_PADDLE_MAX+1];
      Double_t y[N_PLANE_MAX+1][N_PADDLE_MAX+1],Q[N_PLANE_MAX+1][N_PADDLE_MAX+1];     
      Double_t tof[N_PLANE_MAX+1][N_PADDLE_MAX+1];     
      Double_t t_3mm[N_PADDLE_MAX*2], t_5mm[N_PADDLE_MAX*2];
      
      for(Int_t i=1;i<=fNofPlanes;i++){
        for(Int_t j=1;j<=fPaddlesPerPlane;j++){
          t[i][j]=0.;
          x[i][j]=0.;
          y[i][j]=0.;
          Q[i][j]=0.;	  
          t_3mm[2*j-1]=0.;
          t_3mm[2*j]=0.;
          t_5mm[2*j-1]=0.;
          t_5mm[2*j]=0.;
         
        }    
      }
      if(fCalItemsLos)
      {
          Int_t nHits = fCalItemsLos->GetEntriesFast();    
          for (Int_t ihit = 0; ihit < nHits; ihit++)     
          {
    	      R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(ihit);
              timeLos=(calData->fTime_r_ns+calData->fTime_l_ns+calData->fTime_t_ns+calData->fTime_b_ns)/4.;			 
          }						 
      }
        
      Int_t nHits = fCalItems->GetEntries();    
      // loop over hits      
      for (Int_t ihit = 0; ihit < nHits; ihit++)     
      {
          R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItems->At(ihit);
          if (!hit) continue; // should not happen

          Int_t iPlane  = hit->GetPlane();    // 1..n
          Int_t iBar  = hit->GetBar();    // 1..n
        
//	LOG(WARNING) << "Test plane  "<< iPlane<<" bar "<<iBar<<FairLogger::endl;
          // create histograms if not already existing
	      if (NULL == fhPos[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "PositionPlane_%d_Bar_%d", iPlane, iBar);
              fhPos[iPlane - 1][iBar - 1] = new TH1F(strName, "", 2000, -100., 100.);
          }
          if (NULL == fhTotPm1[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_1", iPlane, iBar);
              fhTotPm1[iPlane - 1][iBar - 1] = new TH1F(strName, "", 5000, 0., 500.);
          }
          if (NULL == fhTotPm2[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_2", iPlane, iBar);
              fhTotPm2[iPlane - 1][iBar - 1] = new TH1F(strName, "", 5000, 0., 500.);
          }
          if (NULL == fhTotPm1Sat[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_1_after_saturation", iPlane, iBar);
              fhTotPm1Sat[iPlane - 1][iBar - 1] = new TH1F(strName, "", 5000, 0., 500.);
          }
          if (NULL == fhTotPm2Sat[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_2_after_saturation", iPlane, iBar);
              fhTotPm2Sat[iPlane - 1][iBar - 1] = new TH1F(strName, "", 5000, 0., 500.);
          }
          if (NULL == fhQPm1[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q_Plane_%d_Bar_%d_PM_1", iPlane, iBar);
              fhQPm1[iPlane - 1][iBar - 1] = new TH1F(strName, "", 2000, 0., 100.);
          }
          if (NULL == fhQPm2[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q_Plane_%d_Bar_%d_PM_2", iPlane, iBar);
              fhQPm2[iPlane - 1][iBar - 1] = new TH1F(strName, "", 2000, 0., 100.);
          }
          if (NULL == fhQ[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q_Plane_%d_Bar_%d", iPlane, iBar);
              fhQ[iPlane - 1][iBar - 1] = new TH1F(strName, "", 2000, 0., 100.);
          }
          if (NULL == fhQ1vsPos[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
              fhQ1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 2000, 0., 100.);
          }
          if (NULL == fhQ2vsPos[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
              fhQ2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 2000, 0., 100.);
          }
          if (NULL == fhQvsPos[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
              fhQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 2000, 0., 100.);
          }
          if (NULL == fhSqrtQvsPos[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "SqrtQ_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
              fhSqrtQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 5000, 0., 500.);
          }
          if (NULL == fhTotPm1vsTotPm2[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Tot_PM1_vs_Tot_PM2_Plane_%d_Bar_%d", iPlane, iBar);
              fhTotPm1vsTotPm2[iPlane - 1][iBar - 1] = new TH2F(strName, "", 1000, 0, 1000, 1000, 0., 1000.);
          }
          if (NULL == fhTotPm1satvsTotPm2sat[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Tot_PM1_vs_Tot_PM2_Plane_%d_Bar_%d_after_saturation", iPlane, iBar);
              fhTotPm1satvsTotPm2sat[iPlane - 1][iBar - 1] = new TH2F(strName, "", 1000, 0, 1000, 1000, 0., 1000.);
          }
          if (NULL == fhTdiffvsQ[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Tdiff_Plane_%dand%d_Bar_%dvsQ", iPlane, iPlane+1, iBar);
              fhTdiffvsQ[iPlane - 1][iBar - 1] = new TH2F(strName, "", 10000, -50, 50, 1000, 0., 100.);
          }
          if (NULL == fhTot1vsPos[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Tot1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
              fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 1000, 0., 1000.);
          }
          if (NULL == fhTot2vsPos[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Tot2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
              fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 1000, 0., 1000.);
          }
          if (NULL == fhxy)
          {
              char strName1[255];
              char strName2[255];
              sprintf(strName1, "xy_of_TofD");
              sprintf(strName2, "xy of TofD");
              fhxy = new TH2F(strName1, strName2, 2000, -100, 100, 2000, -100., 100.);
              fhxy->GetXaxis()->SetTitle("X position in cm");
              fhxy->GetYaxis()->SetTitle("Y position in cm");
          }
          if (NULL == fhQvsQ[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar);
              fhQvsQ[iPlane - 1][iBar - 1] = new TH2F(strName, "", 2000, 0, 100, 2000, 0., 100.);
          }
          if (NULL == fhQvsTof[iPlane - 1][iBar - 1])
          {
              char strName[255];
              sprintf(strName, "Q_vs_ToF_Plane_%d_Bar_%d", iPlane, iBar);
              fhQvsTof[iPlane - 1][iBar - 1] = new TH2F(strName, "", 1000, -5, 5, 2000, 0., 100.);
          }
	      if (NULL == fhCharge)
          {
              char strName[255];
              sprintf(strName, "Charge_of_TofD");
              fhCharge = new TH1F(strName, "", 2000, 0., 100.);
          }
 	      if (NULL == fhCharge3mm)
          {
              char strName[255];
              sprintf(strName, "Charge_of_3mm_bars");
              fhCharge3mm = new TH1F(strName, "", 2000, 0., 100.);
          }
	      if (NULL == fhCharge5mm)
          {
              char strName[255];
              sprintf(strName, "Charge_of_5mm_bars");
              fhCharge5mm = new TH1F(strName, "", 2000, 0., 100.);
          }
         if (NULL == fhSaturation1)
          {
              char strName[255];
              sprintf(strName, "Saturation1_of_TofD");
              fhSaturation1 = new TH2F(strName, "", 1000, 0, 1000, 1000, 0., 1000.);
          }
          if (NULL == fhSaturation2)
          {
              char strName[255];
              sprintf(strName, "Saturation2_of_TofD");
              fhSaturation2 = new TH2F(strName, "", 1000, 0, 1000, 1000, 0., 1000.);
          }
          if (NULL == fhCharge3mm_vs_x)
          {
              char strName1[255];
              char strName2[255];
              sprintf(strName1, "Q_vs_x_3mm");
              sprintf(strName2, "Q vs. x for 3 mm paddles");
              fhCharge3mm_vs_x = new TH2F(strName1, strName2, 14, -7.425, 12.825, 2000, 0., 100.);
              fhCharge3mm_vs_x->GetXaxis()->SetTitle("X position in cm");
              fhCharge3mm_vs_x->GetYaxis()->SetTitle("Q");
          }
          if (NULL == fhCharge3mm_vs_y)
          {
              char strName1[255];
              char strName2[255];
              sprintf(strName1, "Q_vs_y_3mm");
              sprintf(strName2, "Q vs. y for 3 mm paddles");
              fhCharge3mm_vs_y = new TH2F(strName1, strName2, 1000, -50, 50, 2000, 0., 100.);
              fhCharge3mm_vs_y->GetXaxis()->SetTitle("Y position in cm");
              fhCharge3mm_vs_y->GetYaxis()->SetTitle("Q");
          }
          if (NULL == fhCharge5mm_vs_x)
          {
              char strName1[255];
              char strName2[255];
              sprintf(strName1, "Q_vs_x_5mm");
              sprintf(strName2, "Q vs. x for 5 mm paddles");
              fhCharge5mm_vs_x = new TH2F(strName1, strName2, 14, -7.425, 12.825, 2000, 0., 100.);
              fhCharge5mm_vs_x->GetXaxis()->SetTitle("X position in cm");
              fhCharge5mm_vs_x->GetYaxis()->SetTitle("Q");
         }
          if (NULL == fhCharge5mm_vs_y)
          {
              char strName1[255];
              char strName2[255];
              sprintf(strName1, "Q_vs_y_5mm");
              sprintf(strName2, "Q vs. y for 5 mm paddles");
              fhCharge5mm_vs_y = new TH2F(strName1, strName2, 1000, -50, 50, 2000, 0., 100.);
              fhCharge5mm_vs_y->GetXaxis()->SetTitle("Y position in cm");
              fhCharge5mm_vs_y->GetYaxis()->SetTitle("Q");
          }

          // Read in calibration parameter for the bar which was hit
          R3BTofdHitModulePar* par = fHitPar->GetModuleParAt(iPlane, iBar);
          if (!par)
          {
              LOG(INFO) << "R3BTofdCal2Hit::Exec : Hit par not found, Plane: " << 
              iPlane << ", Bar: " << iBar << FairLogger::endl;
              continue;
          }

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
	      // calculate position
 	      y[iPlane][iBar]=((t1l+par->GetOffset1())-(t2l+par->GetOffset2()))*par->GetVeff();
          Double_t Pos=y[iPlane][iBar];


          // correction for position dependence only valid in the range -35cm to 35cm
          if (Pos>-350 && Pos<350) {
          
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

              if(iPlane==4 && iBar==1) {
				  q41=tot1;
				  q42=tot2;
			  }
              if(iPlane==3 && iBar==1) {
				  q31=tot1;
				  q32=tot2;
			  }

              if(timeLos==0) LOG(WARNING) << "Los Time is zero! "<< FairLogger::endl;		  
			  tof[iPlane][iBar]=(t1l+t2l)/2.-timeLos-par->GetSync();
//              LOG(WARNING) << "losTime: " << timeLos << ", Bar: " << (t1l+t2l)/2. << FairLogger::endl;		  
//              LOG(WARNING) << "par sync: " << par->GetSync() <<" tof: "<< tof[iPlane][iBar]<< FairLogger::endl;		  

	          // walk corrections
//            t1=t1-walk(tot1);
//	          t2=t2-walk(tot2);
  
              // time of flight of the bar, check against LOS time that clock did not go out of range
              t[iPlane][iBar]=(t1l+t2l)/2.;         
		 	  
              fhTotPm1[iPlane-1][iBar-1]->Fill(tot1);
              fhTotPm2[iPlane-1][iBar-1]->Fill(tot2);
              fhTotPm1vsTotPm2[iPlane-1][iBar-1]->Fill(tot2, tot1);
              fhPos[iPlane-1][iBar-1]->Fill(Pos);	         
	
		      if(iPlane==1){
	  	          x[iPlane][iBar]=iBar*2.7-3*2.7;
		      }
		      else if (iPlane==2){
	  	          x[iPlane][iBar]=iBar*2.7-3*2.7+2.7/2.;	
		      }
		      else if (iPlane==3){
	  	          x[iPlane][iBar]=iBar*2.7-3*2.7+2.7/2.;	
		      }
		      else if (iPlane==4){
	  	          x[iPlane][iBar]=iBar*2.7-3*2.7;	
		      }
		      else if (iPlane==5){
	  	          x[iPlane][iBar]=iBar*2.7-3*2.7+2.7/2.;	
		      }
		      else if (iPlane==6){
	  	          x[iPlane][iBar]=iBar*2.7-3*2.7;	
		      }
	
	          // correction for saturation of PMT	
		      //tot1sat=tot1+saturation(tot1);
		      //tot2sat=tot2+saturation(tot2);
		      tot1sat=tot1;
		      tot2sat=tot2;
		
	          fhTotPm1Sat[iPlane-1][iBar-1]->Fill(tot1sat);
	          fhTotPm2Sat[iPlane-1][iBar-1]->Fill(tot2sat);
	          fhTotPm1satvsTotPm2sat[iPlane-1][iBar-1]->Fill(tot2sat, tot1sat);
		
		
		      // correct for position dependence and calculate nuclear charge Z 
	          Double_t para[4];	  
		      para[0] = par->GetPar1a();
		      para[1] = par->GetPar1b();
		      para[2] = par->GetPar1c();
		      para[3] = par->GetPar1d();
	          q1=tot1sat/( para[0]*(exp(-para[1]*(Pos+50))+exp(-para[2]*(Pos+50.))) + para[3] );  
	          
	          para[0] = par->GetPar2a();
		      para[1] = par->GetPar2b();
		      para[2] = par->GetPar2c();
		      para[3] = par->GetPar2d();        
	          q2=tot2sat/( para[0]*(exp(-para[1]*(Pos+50.))+exp(-para[2]*(Pos+50.))) + para[3] );  
	
	          q1=q1*fTofdQ;
	          q2=q2*fTofdQ;
	
		      Q[iPlane][iBar]=(q1+q2)/2.; 
		
		      // fill control histograms
		      fhQPm1[iPlane-1][iBar-1]->Fill(q1);
		      fhQPm2[iPlane-1][iBar-1]->Fill(q2);
		      fhQ[iPlane-1][iBar-1]->Fill(Q[iPlane][iBar]);
		      fhQ1vsPos[iPlane-1][iBar-1]->Fill(Pos, q1);
		      fhQ2vsPos[iPlane-1][iBar-1]->Fill(Pos, q2);
		      fhQvsPos[iPlane-1][iBar-1]->Fill(Pos, Q[iPlane][iBar]);
		      fhSqrtQvsPos[iPlane-1][iBar-1]->Fill(Pos, sqrt(tot1*tot2));
		      fhTot1vsPos[iPlane-1][iBar-1]->Fill(Pos, tot1);
		      fhTot2vsPos[iPlane-1][iBar-1]->Fill(Pos, tot2);
		      fhQvsTof[iPlane-1][iBar-1]->Fill(tof[iPlane][iBar], Q[iPlane][iBar]);
		      
		      
		      // Time reference in case on has the master signal in one of the TDC channels. 
		      // Not used at the moment.
	          timeRef=0;	
	      }
      }
      
      // find out which 2 paddles in plane 1 and 2 or plane 3 and 4 are hit.
      
      for(Int_t j=1;j<=fPaddlesPerPlane;j++){
		  if(t[2][j]>0 && t[1][j]>0 && y[2][j]>-35. && y[2][j]<35. && y[1][j]>-35. && y[1][j]<35.) {
			  t_3mm[2*j-1]=(t[2][j]+t[1][j])/2.;
		  }
		  if(t[1][j]>0 && t[2][j+1]>0 && y[1][j]>-35. && y[1][j]<35. && y[2][j+1]>-35. && y[2][j+1]<35.) {
			  t_3mm[2*j]=(t[1][j]+t[2][j+1])/2.;
		  }
		  if(t[3][j]>0 && t[4][j]>0 && y[3][j]>-35. && y[3][j]<35. && y[4][j]>-35. && y[4][j]<35.) {
			  t_5mm[2*j-1]=(t[3][j]+t[4][j])/2.;
		  }
		  if(t[4][j]>0 && t[3][j+1]>0 && y[4][j]>-35. && y[4][j]<35. && y[3][j+1]>-35. && y[3][j+1]<35.) {
			  t_5mm[2*j]=(t[4][j]+t[3][j+1])/2.;
		  }
		  
      
      }
      
      // Now, since all hits are analyzed we take the one with the largest 
      // charge in each plane and average them

      Double_t max_Z[5], max_t[5], max_x[5], max_y[5];
      Int_t max_bar[5];
      Int_t hitBars[5];
      // find hit with highest signal in each plane
      for(Int_t i=1;i<=fNofPlanes;i++){
          max_Z[i]=0.;
	      max_t[i]=0;
	      max_bar[i]=0;
	      max_x[i]=-1000.;
	      max_y[i]=-1000.;
	      hitBars[i]=0;
          for(Int_t j=1;j<=fPaddlesPerPlane;j++){
	          if(Q[i][j]>max_Z[i]){
   	              max_t[i]=t[i][j];
	              max_x[i]=x[i][j];
	              max_y[i]=y[i][j];
	              max_Z[i]=Q[i][j];                
	              max_bar[i]=j;
		      }
              if(i<fNofPlanes){
                  if(t[i][j]>0.&&t[i+1][j]>0.) {
		              fhTdiffvsQ[i-1][j-1]->Fill(t[i+1][j]-t[i][j], Q[i][j]);
		          }
	          }
              if(i<fNofPlanes){
                  if(Q[i][j]>0.&&Q[i+1][j]>0.) {
		              fhQvsQ[i-1][j-1]->Fill(Q[i+1][j], Q[i][j]);
		          }
	          }
          }
          if(max_Z[i]>0.){
	          hitBars[i]++;
          }
      }
      //average the two 3 mm planes
      Double_t Z_ave3=0.;
      Double_t x_ave3=0.;
      Double_t y_ave3=0.;
      Double_t t_ave3=0.;
     if(abs(max_Z[1]-max_Z[2])<3. && max_Z[1]>0. && max_Z[2]>0. && abs(max_bar[1]-max_bar[2])<2){
        Z_ave3=(max_Z[1]+max_Z[2])/2.;
        fhCharge3mm->Fill(Z_ave3);     
        x_ave3=(max_x[1]+max_x[2])/2.;
        y_ave3=(max_y[1]+max_y[2])/2.;
        t_ave3=(max_t[1]+max_t[2])/2.;
        fhCharge3mm_vs_x->Fill(x_ave3,Z_ave3);
        fhCharge3mm_vs_y->Fill(y_ave3,Z_ave3);
        
      }
      //average the two 5 mm planes
      Double_t Z_ave5=0.;
      Double_t x_ave5=0.;
      Double_t y_ave5=0.;
      Double_t t_ave5=0.;
      if(abs(max_Z[3]-max_Z[4])<3.&& max_Z[3]>0. && max_Z[4]>0. && abs(max_bar[3]-max_bar[4])<2){
        Z_ave5=(max_Z[3]+max_Z[4])/2.;
        fhCharge5mm->Fill(Z_ave5);      
        x_ave5=(max_x[3]+max_x[4])/2.;
        y_ave5=(max_y[3]+max_y[4])/2.;
        t_ave5=(max_t[3]+max_t[4])/2.;
        fhCharge5mm_vs_x->Fill(x_ave5,Z_ave5);
        fhCharge5mm_vs_y->Fill(y_ave5,Z_ave5);
     }
      
      
      Double_t Z_ave=0.;
      Double_t x_ave=0.;
      Double_t y_ave=0.;
      Double_t t_ave=0.;
     
      if(Z_ave3>0. && Z_ave5>0. && abs(Z_ave3-Z_ave5)<3.){
          Z_ave=(Z_ave3+Z_ave5)/2.;
          x_ave=(x_ave3+x_ave5)/2.;
          y_ave=(y_ave3+y_ave5)/2.;
          t_ave=(t_ave3+t_ave5)/2.;
         
          fhxy->Fill(x_ave,y_ave);
          fhCharge->Fill(Z_ave);
      
          // write out calibrated event
          new ((*fHitItems)[fNofHitItems]) R3BTofdHitData(t_ave5, x_ave5 , y_ave5, Z_ave5, timeRef);
          fNofHitItems += 1;
       }
       
       if(q41>0. && q31>0.) fhSaturation1->Fill(q41,q31);
       if(q42>0. && q32>0.) fhSaturation2->Fill(q42,q32);
      
}

void R3BTofdCal2Hit::FinishEvent()
{

    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
    }
    if (fCalItemsLos)
    {
        fCalItemsLos->Clear();
    }

}

void R3BTofdCal2Hit::FinishTask()
{
    for (Int_t i = 0; i < N_PLANE_MAX; i++)
    {
        for (Int_t j = 0; j < N_PADDLE_MAX; j++)
        {
            if (fhPos[i][j]) fhPos[i][j]->Write();
            if (fhTotPm1[i][j]) fhTotPm1[i][j]->Write();
            if (fhTotPm2[i][j]) fhTotPm2[i][j]->Write();
            if (fhTotPm1Sat[i][j]) fhTotPm1Sat[i][j]->Write();
            if (fhTotPm2Sat[i][j]) fhTotPm2Sat[i][j]->Write();
            if (fhQPm1[i][j]) fhQPm1[i][j]->Write();
            if (fhQPm2[i][j]) fhQPm2[i][j]->Write();
            if (fhQ[i][j]) fhQ[i][j]->Write();
            if (fhQ1vsPos[i][j]) fhQ1vsPos[i][j]->Write();
            if (fhQ2vsPos[i][j]) fhQ2vsPos[i][j]->Write();
            if (fhQvsPos[i][j]) fhQvsPos[i][j]->Write();
            if (fhSqrtQvsPos[i][j]) fhSqrtQvsPos[i][j]->Write();
            if (fhTotPm1vsTotPm2[i][j]) fhTotPm1vsTotPm2[i][j]->Write();
            if (fhTotPm1satvsTotPm2sat[i][j]) fhTotPm1satvsTotPm2sat[i][j]->Write();
            if (fhTdiffvsQ[i][j]) fhTdiffvsQ[i][j]->Write();
            if (fhTot1vsPos[i][j]) fhTot1vsPos[i][j]->Write();
            if (fhTot2vsPos[i][j]) fhTot2vsPos[i][j]->Write();
            if (fhQvsQ[i][j]) fhQvsQ[i][j]->Write();
            if (fhQvsTof[i][j]) fhQvsTof[i][j]->Write();
	    
	    }
    }
    if (fhxy) fhxy->Write();
    if (fhCharge) fhCharge->Write();
    if (fhCharge3mm) fhCharge3mm->Write();
    if (fhCharge5mm) fhCharge5mm->Write();
    if (fhSaturation1) fhSaturation1->Write();
    if (fhSaturation2) fhSaturation2->Write();
    if (fhCharge3mm_vs_x) fhCharge3mm_vs_x->Write();
    if (fhCharge3mm_vs_y) fhCharge3mm_vs_y->Write();
    if (fhCharge5mm_vs_x) fhCharge5mm_vs_x->Write();
    if (fhCharge5mm_vs_y) fhCharge5mm_vs_y->Write();
}



Double_t R3BTofdCal2Hit::walk(Double_t Q)
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

Double_t R3BTofdCal2Hit::saturation(Double_t x)
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

ClassImp(R3BTofdCal2Hit)
