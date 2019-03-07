// ------------------------------------------------------------
// -----                  R3BTofdCal2Hit                  -----
// -----          Created May 2016 by M.Heil        -----
// ------------------------------------------------------------

#include "R3BTofdCal2Hit.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BPaddleCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdHitPar.h"
#include "R3BTofdHitModulePar.h"
#include "R3BTCalEngine.h"
 
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THnSparse.h"

#include "TClonesArray.h"
#include "TMath.h"
using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44

R3BTofdCal2Hit::R3BTofdCal2Hit()
    : FairTask("TofdCal2Hit", 1)
    , fCalItems(NULL)
    , fCalItemsLos(NULL)
    , fHitItemsLos(NULL)
    , fHitItems(new TClonesArray("R3BTofdHitData"))
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fTrigger(-1)
    , fNofPlanes(5)  
    , fPaddlesPerPlane(6)
    , fTofdQ(1)
    , fnEvents(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhxy = NULL;		
    fhCharge = NULL;	
    fhChargevsTof = NULL;
    fhChargevsPos = NULL;
    fhQp12 = NULL;
    fhQp34 = NULL;
	
//    fhSaturation1 = NULL;
//    fhSaturation2 = NULL;
    fhLosXYP = NULL;
    fhChargeLosTofD = NULL;
    fh_los_pos = NULL;
    
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
/*
        fhPos[i] = NULL;
        fhTotPm1[i] = NULL;		
		fhTotPm2[i] = NULL;
        fhTotPm1Sat[i] = NULL;		
		fhTotPm2Sat[i] = NULL;
*/
        fhQPm1[i] = NULL;		
		fhQPm2[i] = NULL;
		fhQ[i] = NULL;
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
/*			
            fhQ1vsPos[i][j] = NULL;		
	        fhQ2vsPos[i][j] = NULL;
	        fhQvsPos[i][j] = NULL;
	        fhSqrtQvsPos[i][j] = NULL;
*/	        
/*
	        fhTotPm1vsTotPm2[i][j] = NULL;
	        fhTotPm1satvsTotPm2sat[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;		
	        fhTot2vsPos[i][j] = NULL;
*/
	        fhTdiffvsQ[i][2*j] = NULL;
	        fhTdiffvsQ[i][2*j+1] = NULL;
	        fhQvsQ[i][2*j] = NULL;
	        fhQvsQ[i][2*j+1] = NULL;
	        fhQvsTof[i][j] = NULL;
	    }
    }
}

R3BTofdCal2Hit::R3BTofdCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fCalItemsLos(NULL)
    , fHitItemsLos(NULL)
    , fHitItems(new TClonesArray("R3BTofdHitData"))
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fTrigger(-1)
    , fNofPlanes(5)  
    , fPaddlesPerPlane(6)    
    , fTofdQ(1)
    , fnEvents(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhxy = NULL;		
    fhCharge = NULL;		
    fhChargevsTof = NULL;
    fhChargevsPos = NULL;
    fhQp12 = NULL;
    fhQp34 = NULL;
    
//    fhSaturation1 = NULL;
//    fhSaturation2 = NULL;
    fhLosXYP = NULL;
    fhChargeLosTofD = NULL;
    fh_los_pos = NULL;
    
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
/*
        fhPos[i] = NULL;
        fhTotPm1[i] = NULL;		
        fhTotPm2[i] = NULL;
        fhTotPm1Sat[i] = NULL;		
        fhTotPm2Sat[i] = NULL;
*/
        fhQPm1[i] = NULL;		
        fhQPm2[i] = NULL;
        fhQ[i] = NULL;
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
/*			
            fhQ1vsPos[i][j] = NULL;		
	        fhQ2vsPos[i][j] = NULL;
	        fhQvsPos[i][j] = NULL;
	        fhSqrtQvsPos[i][j] = NULL;
*/	    
/*
	        fhTotPm1vsTotPm2[i][j] = NULL;
	        fhTotPm1satvsTotPm2sat[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;		
	        fhTot2vsPos[i][j] = NULL;
*/
 	        fhTdiffvsQ[i][2*j] = NULL;
 	        fhTdiffvsQ[i][2*j+1] = NULL;
 	        fhQvsQ[i][2*j] = NULL;
 	        fhQvsQ[i][2*j+1] = NULL;
 	        fhQvsTof[i][j] = NULL;
        } 
    }
}

R3BTofdCal2Hit::~R3BTofdCal2Hit()
{
    if (fhxy) delete fhxy ;		
    if (fhChargevsTof) delete  fhChargevsTof;	
    if (fhChargevsPos) delete  fhChargevsPos;	
    if (fhQp12) delete fhQp12;	
    if (fhQp34) delete fhQp34;	
    if (fhCharge) delete  fhCharge;	
	
//    if (fhSaturation1) delete fhSaturation1;
//    if (fhSaturation2) delete fhSaturation2;
    if (fhLosXYP) delete fhLosXYP;
    if (fhChargeLosTofD) delete fhChargeLosTofD;
    if (fh_los_pos) delete fh_los_pos;
   
      
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
/*
        if (fhPos[i]) delete fhPos[i];
        if (fhTotPm1[i]) delete fhTotPm1[i];
        if (fhTotPm2[i]) delete fhTotPm2[i];
        if (fhTotPm1Sat[i]) delete fhTotPm1[i];
        if (fhTotPm2Sat[i]) delete fhTotPm2[i];
*/
        if (fhQPm1[i]) delete fhQPm1[i];
        if (fhQPm2[i]) delete fhQPm2[i];
        if (fhQ[i]) delete fhQ[i];
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
/*
            if (fhQ1vsPos[i][j]) delete fhQ1vsPos[i][j];
            if (fhQ2vsPos[i][j]) delete fhQ2vsPos[i][j];
            if (fhQvsPos[i][j]) delete fhQvsPos[i][j];
            if (fhSqrtQvsPos[i][j]) delete fhSqrtQvsPos[i][j];
*/            
            if (fhTdiffvsQ[i][2*j]) delete fhTdiffvsQ[i][2*j];
            if (fhTdiffvsQ[i][2*j+1]) delete fhTdiffvsQ[i][2*j+1];
/*
            if (fhTotPm1vsTotPm2[i][j]) delete fhTotPm1vsTotPm2[i][j];
            if (fhTotPm1satvsTotPm2sat[i][j]) delete fhTotPm1satvsTotPm2sat[i][j];
            if (fhTot1vsPos[i][j]) delete fhTot1vsPos[i][j];
            if (fhTot2vsPos[i][j]) delete fhTot2vsPos[i][j];
*/
	    if (fhQvsQ[i][2*j]) delete fhQvsQ[i][2*j];
	    if (fhQvsQ[i][2*j+1]) delete fhQvsQ[i][2*j+1];
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
        LOG(fatal) << "FairRootManager not found";
    fCalItems = (TClonesArray*)mgr->GetObject("TofdCal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch TofdCal not found";
    fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");
     if (NULL == fCalItemsLos)
        LOG(fatal) << "Branch LosCal not found";

    fHitItemsLos = (TClonesArray*)mgr->GetObject("LosHit");
     if (NULL == fHitItemsLos) LOG(ERROR) << "Branch LosHit not found" << FairLogger::endl;

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
	if(fnEvents/1000000.==(int)fnEvents/1000000) cout<<"Events: "<<fnEvents<<endl;
	Double_t tot1=0.;
	Double_t tot2=0.;
	Double_t tot1sat=0.;
	Double_t tot2sat=0.;
	Double_t t1l=0.;
	Double_t t2l=0.;
	Double_t t1t=0.;
	Double_t t2t=0.;
	Double_t ToF=0.;
	Double_t q1=0.;
	Double_t q2=0.;
	Double_t pos=1000.;
	Double_t timeRef=0.;
	Double_t timeLos=0;
	Double_t LosTresM=0;
	Double_t LosQ=0;
	Double_t xLosP=1000;
	Double_t yLosP=1000;
	Double_t trl=0;
	Double_t tou=0;
	Double_t trescorr=0;
	Double_t tofrlcorr=0;
	Double_t tofoucorr=0;
	Double_t randx;
	
	Double_t t[N_TOFD_HIT_PLANE_MAX+1][N_TOFD_HIT_PADDLE_MAX*2+1],x[N_PLANE_MAX+1][N_PADDLE_MAX*2+1];
	Double_t y[N_TOFD_HIT_PLANE_MAX+1][N_TOFD_HIT_PADDLE_MAX*2+1],Q[N_PLANE_MAX+1][N_PADDLE_MAX*2+1]; 
	Double_t Qbeta[N_PLANE_MAX+1][N_PADDLE_MAX*2+1];    
	Double_t tof[N_TOFD_HIT_PLANE_MAX+1][N_TOFD_HIT_PADDLE_MAX*2+1];     
	for(Int_t i=1;i<=fNofPlanes;i++){
		for(Int_t j=0;j<=2*fPaddlesPerPlane;j++){
			t[i][j]=0.;
			tof[i][j]=0.;
			x[i][j]=0.;
			y[i][j]=0.;
			Q[i][j]=0.;	  
			Qbeta[i][j]=0.;	  
        }    
	}
	
	if(fHitItemsLos){
		Int_t nHits = fHitItemsLos->GetEntriesFast();
		for (Int_t ihit = 0; ihit < nHits; ihit++)     
		{
			LOG(WARNING) << "LOS Ihit  "<< ihit<<" "<<nHits<<FairLogger::endl;
			R3BLosHitData *hitData = (R3BLosHitData*)fHitItemsLos->At(ihit);
			timeLos=hitData->fTime_ns;

			if (NULL == fh_los_pos)
			{
				char strName[255];
				sprintf(strName, "LOS_X_vs_Y_MCFD");
				//     fh_los_pos = new TH2F(strName, "", 2000, -10., 10., 2000, -10., 10.);
				fh_los_pos = new TH2F(strName, "", 2000, -10., 10., 2000, -10., 10.);              
			}	      	                             
			fh_los_pos->Fill(hitData->fX_cm,hitData->fY_cm); 

			if (ihit==0) LosQ=hitData->fZ;
			if (NULL == fhChargeLosTofD)
			{
				char strName[255];
				sprintf(strName, "LosQ_vs_TofdQ");
				fhChargeLosTofD = new TH2F(strName, "", 400, 0., 100., 800, 0., 200.);              
				fhChargeLosTofD->GetYaxis()->SetTitle("Charge LOS");
				fhChargeLosTofD->GetXaxis()->SetTitle("Charge ToFD");
			}	      	                             

		}
	}
	
/*	
	if(fCalItemsLos)
	{
		Int_t nHits = fCalItemsLos->GetEntriesFast();    
		for (Int_t ihit = 0; ihit < nHits; ihit++)     
		{
			LOG(WARNING) << "LOS Ihit  "<< ihit<<" "<<nHits<<FairLogger::endl;
			  
			R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(ihit);
			timeLos=(calData->fTimeV_r_ns+calData->fTimeV_l_ns+calData->fTimeV_t_ns+calData->fTimeV_b_ns)/4.;
			LosTresM=(calData->fTimeV_r_ns+calData->fTimeV_l_ns)/2.-(calData->fTimeV_t_ns+calData->fTimeV_b_ns)/2.;
            
			LOG(WARNING) << "LOS MCFD  "<< LosTresM<<" "<<timeLos<<FairLogger::endl;
              
              
			if (NULL == fh_los_pos)
			{
				char strName[255];
				sprintf(strName, "LOS_X_vs_Y_MCFD");
				//     fh_los_pos = new TH2F(strName, "", 2000, -10., 10., 2000, -10., 10.);
				fh_los_pos = new TH2F(strName, "", 2000, -100., 100., 2000, -100., 100.);              
			}	      	                             
			fh_los_pos->Fill((calData->fTimeV_r_ns-calData->fTimeV_l_ns+2.15)*5.3,    
	                         (calData->fTimeV_b_ns-calData->fTimeV_t_ns+1.28)*5.2);   // Xe: (2.15,5.3), (1.28,5.2)

			if (NULL == fhLosTimeM )
			{
				char strName[255];
				sprintf(strName, "LOS_Time_Resolution_MCFD");
				//     fhLosTimeM = new TH1F(strName, "", 4000, -2., 2.);
				fhLosTimeM = new TH1F(strName, "", 4000, -2000., 2000.);              
			}	      	  
			fhLosTimeM->Fill(LosTresM);	 
              
		}						 
	}
*/
    
	std::vector<UInt_t> multPlane1(N_TOFD_HIT_PADDLE_MAX);
	std::vector<UInt_t> multPlane2(N_TOFD_HIT_PADDLE_MAX);
	std::vector<UInt_t> multPlane3(N_TOFD_HIT_PADDLE_MAX);
	std::vector<UInt_t> multPlane4(N_TOFD_HIT_PADDLE_MAX);
        
	Int_t nHits = fCalItems->GetEntries();    
	// loop over hits of ToFD      
	for (Int_t ihit = 0; ihit < nHits; ihit++)     
	{
		R3BPaddleCalData *hit = (R3BPaddleCalData*)fCalItems->At(ihit);
		if (!hit) continue; // should not happen

		Int_t iPlane  = hit->GetPlane();    // 1..n
		Int_t iBar  = hit->GetBar();    // 1..n

//	cout<<" "<<endl;
//	cout<<"new event Plane "<<iPlane<<" Bar "<<iBar<<endl;
    
		
		if(iPlane==1) ++multPlane1.at(iBar - 1);     // multihit of a given bar
		if(iPlane==2) ++multPlane2.at(iBar - 1);     // multihit of a given bar
		if(iPlane==3) ++multPlane3.at(iBar - 1);     // multihit of a given bar
		if(iPlane==4) ++multPlane4.at(iBar - 1);     // multihit of a given bar
        
        if(multPlane1.at(iBar - 1)>1) cout<< "multi hit in plane 1: "<<multPlane1.at(iBar - 1)<<endl;
        if(multPlane2.at(iBar - 1)>1) cout<< "multi hit in plane 2: "<<multPlane2.at(iBar - 1)<<endl;
        if(multPlane3.at(iBar - 1)>1) cout<< "multi hit in plane 3: "<<multPlane3.at(iBar - 1)<<endl;
        if(multPlane4.at(iBar - 1)>1) cout<< "multi hit in plane 4: "<<multPlane4.at(iBar - 1)<<endl;
        
//	LOG(WARNING) << "Test plane  "<< iPlane<<" bar "<<iBar<<FairLogger::endl;
//		cout<<"Test plane  "<< iPlane<<" bar "<<iBar<<endl;
		// create histograms if not already existing
 
 /*         
		if (NULL == fhPos[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "PositionPlane_%d", iPlane);
			char strName2[255];
			sprintf(strName2, "Positions Plane %d", iPlane);
			fhPos[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 200, -100., 100.);
		}
		if (NULL == fhTotPm1[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "ToT_Plane_%d_PM_1", iPlane);
			char strName2[255];
			sprintf(strName2, "ToT Plane %d PM1", iPlane);
			fhTotPm1[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
		}
		if (NULL == fhTotPm2[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "ToT_Plane_%d_PM_2", iPlane);
			char strName2[255];
			sprintf(strName2, "ToT Plane %d PM2", iPlane);
			fhTotPm2[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
		}
		if (NULL == fhTotPm1Sat[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "ToT_Plane_%d_PM_1_after_saturation", iPlane);
			char strName2[255];
			sprintf(strName2, "ToT Plane %d PM1 after saturation", iPlane);
			fhTotPm1Sat[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
		}
		if (NULL == fhTotPm2Sat[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "ToT_Plane_%d_PM_2_after_saturation", iPlane);
			char strName2[255];
			sprintf(strName2, "ToT Plane %d PM2 after saturation", iPlane);
			fhTotPm2Sat[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
		}
		if (NULL == fhTotPm1vsTotPm2[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Tot_PM1_vs_Tot_PM2_Plane_%d_Bar_%d", iPlane, iBar);
			fhTotPm1vsTotPm2[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, 0, 200, 200, 0., 200.);
		}
		if (NULL == fhTotPm1satvsTotPm2sat[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Tot_PM1_vs_Tot_PM2_Plane_%d_Bar_%d_after_saturation", iPlane, iBar);
			fhTotPm1satvsTotPm2sat[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, 0, 200, 200, 0., 200.);
		}
		if (NULL == fhTot1vsPos[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Tot1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
			fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., 200.);
		}
		if (NULL == fhTot2vsPos[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Tot2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
			fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., 200.);
		}
*/
        Double_t max_charge=80.;
        
		if (NULL == fhQPm1[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "Q_Plane_%d_PM_1", iPlane);
			char strName2[255];
			sprintf(strName2, "Q_Plane %d PM1", iPlane);
			fhQPm1[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 100, 0., max_charge);
			fhQPm1[iPlane - 1]->GetYaxis()->SetTitle("Charge PM1");
			fhQPm1[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
		}
		if (NULL == fhQPm2[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "Q_Plane_%d_PM_2", iPlane);
			char strName2[255];
			sprintf(strName2, "Q_Plane %d PM2", iPlane);
			fhQPm2[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 100, 0., max_charge);
			fhQPm2[iPlane - 1]->GetYaxis()->SetTitle("Charge PM2");
			fhQPm2[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
		}
		if (NULL == fhQ[iPlane - 1])
		{
			char strName1[255];
			sprintf(strName1, "Q_Plane_%d", iPlane);
			char strName2[255];
			sprintf(strName2, "Q_Plane %d ", iPlane);
			fhQ[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 1000, 0., max_charge);
			fhQ[iPlane - 1]->GetYaxis()->SetTitle("Charge");
			fhQ[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
		}
/*
		if (NULL == fhQ1vsPos[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Q1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
			fhQ1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., max_charge);
			fhQ1vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge PM1");
			fhQ1vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
		}
		if (NULL == fhQ2vsPos[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Q2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
			fhQ2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., max_charge);
			fhQ2vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge PM2");
			fhQ2vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
		}
		if (NULL == fhQvsPos[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Q_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
			fhQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 500, 0., max_charge);
			fhQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge");
			fhQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
		}
		if (NULL == fhSqrtQvsPos[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "SqrtQ_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
			fhSqrtQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 500, 0., max_charge);
			fhSqrtQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("sqrt(PM1*PM2)");
			fhSqrtQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
		}
*/
		if (NULL == fhTdiffvsQ[iPlane - 1][2*iBar - 2])
		{
			char strName[255];
			sprintf(strName, "Tdiff_Plane_%dand%d_Bar_%dvsQ", iPlane, iPlane+1, iBar*2-1);
			fhTdiffvsQ[iPlane - 1][iBar*2 - 2] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
			fhTdiffvsQ[iPlane - 1][iBar*2 - 2]->GetYaxis()->SetTitle("charge");
			fhTdiffvsQ[iPlane - 1][iBar*2 - 2]->GetXaxis()->SetTitle("dt in ns");
//			cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 - 1<<endl;
		}
		if(iPlane == 1 || iPlane == 3){
			if (NULL == fhTdiffvsQ[iPlane - 1][2*iBar - 3])
			{
				char strName[255];
				sprintf(strName, "Tdiff_Plane_%dand%d_Bar_%dvsQ", iPlane, iPlane+1, iBar*2-2);
				fhTdiffvsQ[iPlane - 1][iBar*2 - 3] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
				fhTdiffvsQ[iPlane - 1][iBar*2 - 3]->GetYaxis()->SetTitle("charge");
				fhTdiffvsQ[iPlane - 1][iBar*2 - 3]->GetXaxis()->SetTitle("dt in ns");
//			cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 - 2<<endl;
			}
		}
		if(iPlane == 2 || iPlane == 4){
			if (NULL == fhTdiffvsQ[iPlane - 1][2*iBar-1])
			{
				char strName[255];
				sprintf(strName, "Tdiff_Plane_%dand%d_Bar_%dvsQ", iPlane, iPlane+1, iBar*2);
				fhTdiffvsQ[iPlane - 1][iBar*2 - 1] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
				fhTdiffvsQ[iPlane - 1][iBar*2 - 1]->GetYaxis()->SetTitle("charge");
				fhTdiffvsQ[iPlane - 1][iBar*2 - 1]->GetXaxis()->SetTitle("dt in ns");
//			cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 <<endl;
			}
		}
		
		if (NULL == fhQvsQ[iPlane - 1][iBar*2 - 2])
		{
			char strName[255];
			sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar*2-1);
			fhQvsQ[iPlane - 1][iBar*2 - 2] = new TH2F(strName, "", 1000, 0, max_charge, 1000, 0., max_charge);
			char strNamex[255];
			if(iPlane==4){
				sprintf(strNamex, "Charge plane %d", 1);
			}
			else {
				sprintf(strNamex, "Charge plane %d", iPlane+1);		
			}	
			char strNamey[255];
			sprintf(strNamey, "Charge plane %d", iPlane);			
			fhQvsQ[iPlane - 1][iBar*2 - 2]->GetYaxis()->SetTitle(strNamey);
			fhQvsQ[iPlane - 1][iBar*2 - 2]->GetXaxis()->SetTitle(strNamex);
		}		
		if(iPlane == 1 || iPlane == 3){
			if (NULL == fhQvsQ[iPlane - 1][iBar*2 - 3])
			{
				char strName[255];
				sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar*2-2);
				fhQvsQ[iPlane - 1][iBar*2 - 3] = new TH2F(strName, "", 1000, 0, max_charge, 1000, 0., max_charge);
				char strNamex[255];
				if(iPlane==4){
					sprintf(strNamex, "Charge plane %d", 1);
				}
				else {
					sprintf(strNamex, "Charge plane %d", iPlane+1);		
				}	
				char strNamey[255];
				sprintf(strNamey, "Charge plane %d", iPlane);			
				fhQvsQ[iPlane - 1][iBar*2 - 3]->GetYaxis()->SetTitle(strNamey);
				fhQvsQ[iPlane - 1][iBar*2 - 3]->GetXaxis()->SetTitle(strNamex);
			}
		}
		if(iPlane == 2 || iPlane == 4){
			if (NULL == fhQvsQ[iPlane - 1][iBar*2 -1])
			{
				char strName[255];
				sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar*2);
				fhQvsQ[iPlane - 1][iBar*2 -1] = new TH2F(strName, "", 1000, 0, max_charge, 1000, 0., max_charge);
				char strNamex[255];
				if(iPlane==4){
					sprintf(strNamex, "Charge plane %d", 1);
				}
				else {
					sprintf(strNamex, "Charge plane %d", iPlane+1);		
				}	
				char strNamey[255];
				sprintf(strNamey, "Charge plane %d", iPlane);			
				fhQvsQ[iPlane - 1][iBar*2 - 1]->GetYaxis()->SetTitle(strNamey);
				fhQvsQ[iPlane - 1][iBar*2 - 1]->GetXaxis()->SetTitle(strNamex);
			}
		}

/*

		if (NULL == fhQvsQ[iPlane - 1][iBar*2 - 2])	{
			char strName[255];
			sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar*2-1);
			Int_t bins[2] = {1000, 1000};
			Double_t min[2] = {0., 0.};
			Double_t max[2] = {100., 100.};
			fhQvsQ[iPlane - 1][iBar*2 - 2] = new THnSparseF(strName, "", 2, bins, min, max);			
		}
		if(iPlane == 1 || iPlane == 3){
			if (NULL == fhQvsQ[iPlane - 1][iBar*2 - 3])	{
				char strName[255];
				sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar*2-2);
				Int_t bins[2] = {1000, 1000};
				Double_t min[2] = {0., 0.};
				Double_t max[2] = {100., 100.};
				fhQvsQ[iPlane - 1][iBar*2 - 3] = new THnSparseF(strName, "", 2, bins, min, max);
			}
		}
		if(iPlane == 2 || iPlane == 4){
			if (NULL == fhQvsQ[iPlane - 1][iBar*2 - 1])	{
				char strName[255];
				sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar*2);
				Int_t bins[2] = {1000, 1000};
				Double_t min[2] = {0., 0.};
				Double_t max[2] = {100., 100.};
				fhQvsQ[iPlane - 1][iBar*2 - 1] = new THnSparseF(strName, "", 2, bins, min, max);
			}
		}
*/

		
		if (NULL == fhxy)
		{
			char strName[255];
			sprintf(strName, "xy_of_TofD");
			fhxy = new TH2F(strName, "", 200, -100, 100, 200, -100., 100.);
			fhxy->GetYaxis()->SetTitle("y-position in cm");
			fhxy->GetXaxis()->SetTitle("x-position in cm");
		}

		if (NULL == fhQvsTof[iPlane - 1][iBar - 1])
		{
			char strName[255];
			sprintf(strName, "Q_vs_ToF_Plane_%d_Bar_%d", iPlane, iBar);
			fhQvsTof[iPlane - 1][iBar - 1] = new TH2F(strName, "", 1000, 0., max_charge, 1000, -10, 40 );
			fhQvsTof[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("ToF in ns");
			fhQvsTof[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Charge");
		}
		if (NULL == fhCharge)
		{
			char strName[255];
			sprintf(strName, "Charge_of_TofD");
			fhCharge = new TH1F(strName, "", 1000, 0., max_charge);
			fhCharge->GetYaxis()->SetTitle("Counts");
			fhCharge->GetXaxis()->SetTitle("Charge");
		}
		if (NULL == fhChargevsTof)
		{
			char strName[255];
			sprintf(strName, "Charge_vs_Tof_of_TofD");
			fhChargevsTof = new TH2F(strName, "", 1000, 0, 100, 2000, -10., 40.);
			fhChargevsTof->GetYaxis()->SetTitle("Charge");
			fhChargevsTof->GetXaxis()->SetTitle("ToF in ns");
		}
		if (NULL == fhChargevsPos)
		{
			char strName[255];
			sprintf(strName, "Charge_vs_Pos_of_TofD");
			fhChargevsPos = new TH2F(strName, "", 100, 0, 100, 1000, 0., 100.);
			fhChargevsPos->GetYaxis()->SetTitle("Charge");
			fhChargevsPos->GetXaxis()->SetTitle("Bar number");
		}

		if (NULL == fhQp12)
		{
			char strName[255];
			sprintf(strName, "Charge_vs_Pos_p12");
			fhQp12 = new TH2F(strName, "", 100, 0, 100, 1000, 0., max_charge);
			fhQp12->GetYaxis()->SetTitle("Average charge of plane 1 and 2");
			fhQp12->GetXaxis()->SetTitle("Bar number");
		}

		if (NULL == fhQp34)
		{
			char strName[255];
			sprintf(strName, "Charge_vs_Pos_p34");
			fhQp34 = new TH2F(strName, "", 100, 0, 100, 1000, 0., max_charge);
			fhQp34->GetYaxis()->SetTitle("Average charge of plane 3 and 4");
			fhQp34->GetXaxis()->SetTitle("Bar number");
		}

/*
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
*/          
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
		while(t1t - t1l <0.) {
			t1t=t1t+2048.*fClockFreq; 
		}
		while(t2t-t2l<0.) {
			t2t=t2t+2048.*fClockFreq; 
		}
		while(t1l-timeLos<0.){
			t1t=t1t+2048.*fClockFreq; 
			t1l=t1l+2048.*fClockFreq; 
			t2t=t2t+2048.*fClockFreq; 
			t2l=t2l+2048.*fClockFreq; 			  
		}
		// calculate position
		//
		if(iPlane == 1 || iPlane == 3) 
			y[iPlane][iBar*2-2]=((t1l+par->GetOffset1())-(t2l+par->GetOffset2()))*par->GetVeff();
		if(iPlane == 2 || iPlane == 4) 
			y[iPlane][iBar*2]=((t1l+par->GetOffset1())-(t2l+par->GetOffset2()))*par->GetVeff();
		y[iPlane][iBar*2-1]=((t1l+par->GetOffset1())-(t2l+par->GetOffset2()))*par->GetVeff();

		Double_t Pos=((t1l+par->GetOffset1())-(t2l+par->GetOffset2()))*par->GetVeff();

		// correction for position dependence only valid in the range -40cm to 40cm
		if (Pos>-400 && Pos<400) {          
			tot1=t1t - t1l;		      
			if(tot1<0) {
				LOG(WARNING) << "Negative ToT "<< tot1<<" for plane "<< iPlane <<" and bar "<< iBar <<FairLogger::endl;	
				LOG(WARNING) << "times1: " << t1t << " " << t1l << FairLogger::endl;		  
			}

			tot2=t2t - t2l;	
			if(tot2<0) {
				LOG(WARNING) << "Negative ToT "<< tot2<<" for plane "<< iPlane <<" and bar "<< iBar <<FairLogger::endl;              
				LOG(WARNING) << "times2: " << t2t << " " << t2l << FairLogger::endl;		 
			}
 
			if(timeLos==0) LOG(WARNING) << "Los Time is zero! "<< FairLogger::endl;		  
			ToF=(t1l+t2l)/2.-timeLos-par->GetSync();
			if(iPlane == 1 || iPlane == 3) 
				tof[iPlane][iBar*2-2]=(t1l+t2l)/2.-timeLos-par->GetSync();
			if(iPlane == 2 || iPlane == 4) 
				tof[iPlane][iBar*2]=(t1l+t2l)/2.-timeLos-par->GetSync();
			tof[iPlane][iBar*2-1]=(t1l+t2l)/2.-timeLos-par->GetSync();
 
              
	          // walk corrections
//            t1=t1-walk(tot1);
//	          t2=t2-walk(tot2);
  
			// time of flight of the bar, check against LOS time that clock did not go out of range
			if(iPlane == 1 || iPlane == 3) 
				t[iPlane][iBar*2-2]=(t1l+t2l)/2.;         
			if(iPlane == 2 || iPlane == 4) 
				t[iPlane][iBar*2]=(t1l+t2l)/2.;         
			t[iPlane][iBar*2-1]=(t1l+t2l)/2.;         

/*	     	 	  
			fhTotPm1[iPlane-1]->Fill(iBar,tot1);
			fhTotPm2[iPlane-1]->Fill(iBar,tot2);
			fhTotPm1vsTotPm2[iPlane-1][iBar-1]->Fill(tot2, tot1);
			fhPos[iPlane-1]->Fill(iBar,Pos);
*/	
			randx = (std::rand() / (float)RAND_MAX);
			if(iPlane==1 || iPlane==3){
				x[iPlane][iBar*2-2]=iBar*2.8-22*2.8-1.4-1.4*randx;
				x[iPlane][iBar*2-1]=iBar*2.8-22*2.8-1.4*randx;
				
			}
			else if (iPlane==2 || iPlane==4){
				x[iPlane][iBar*2]=iBar*2.8-22*2.8+1.4-1.4*randx;	
				x[iPlane][iBar*2-1]=iBar*2.8-22*2.8-1.4*randx;	
			}
	
			// correction for saturation of PMT	
			//tot1sat=tot1+saturation(tot1);
			//tot2sat=tot2+saturation(tot2);
			tot1sat=tot1;
			tot2sat=tot2;
/*		
			fhTotPm1Sat[iPlane-1]->Fill(iBar,tot1sat);
			fhTotPm2Sat[iPlane-1]->Fill(iBar,tot2sat);
			fhTotPm1satvsTotPm2sat[iPlane-1][iBar-1]->Fill(tot2sat, tot1sat);
*/		
		
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
	
			if(iPlane==1 || iPlane==3)
				Q[iPlane][iBar*2-2]=(q1+q2)/2.; 
			if(iPlane==2 || iPlane==4)
				Q[iPlane][iBar*2]=(q1+q2)/2.; 				
			Q[iPlane][iBar*2-1]=(q1+q2)/2.; 

			// fill control histograms
			fhQPm1[iPlane-1]->Fill(iBar,q1);
			fhQPm2[iPlane-1]->Fill(iBar,q2);
			fhQ[iPlane-1]->Fill(iBar,(q1+q2)/2.);
//			fhQ1vsPos[iPlane-1][iBar-1]->Fill(Pos, q1);
//			fhQ2vsPos[iPlane-1][iBar-1]->Fill(Pos, q2);
//			fhQvsPos[iPlane-1][iBar-1]->Fill(Pos, (q1+q2)/2.);
//			fhSqrtQvsPos[iPlane-1][iBar-1]->Fill(Pos, sqrt(tot1*tot2));
//			fhTot1vsPos[iPlane-1][iBar-1]->Fill(Pos, tot1);
//			fhTot2vsPos[iPlane-1][iBar-1]->Fill(Pos, tot2);
			fhQvsTof[iPlane-1][iBar-1]->Fill((q1+q2)/2.,ToF);
		      
		      
			// Time reference in case on has the master signal in one of the TDC channels. 
			// Not used at the moment.
			timeRef=0;	

//			cout<< "Plane: "<< iPlane<< " Bar: "<< iBar<< " losTime: " << timeLos << ", q1: " << q1<<
//			" q2: "<<q2<<" q "<< Q[iPlane][iBar] << " tof: " << tof[iPlane][iBar] << endl;		  


		}
	}

/*      
        if (fhLosQ1) fhLosQ1->Fill(q1Los); // Fill spektrum only if it exists
        if (fhLosTimeP) fhLosTimeP->Fill(LosTresP);
        if (fhLosTimeP_vs_TimeM) fhLosTimeP_vs_TimeM->Fill(LosTresP,LosTresM);
        if (fhLosQ2) fhLosQ2->Fill(q2Los);
        if (fhLosQ) fhLosQ->Fill(TofLos,qLos);
        if (fhLosQvsTP) fhLosQvsTP->Fill(LosTresP,qLos);            
        if (fhLosQvsTM) fhLosQvsTM->Fill(LosTresM,qLos);
    	if (fhLosQvsX) fhLosQvsX->Fill(q1Los,xLosP);
	    if (fhLosQvsY) fhLosQvsY->Fill(q2Los,yLosP);	
	    if (fhLosXYP) fhLosXYP->Fill(xLosP,yLosP);
*/
	    
	
 
      // Now, since all hits are analyzed we take the one with the largest 
      // charge in each plane and average them

	Double_t max_Z[5], max_t[5], max_x[5], max_y[5];
	Double_t maxmax_Z=0.;
	Double_t maxmax_t=0.;
	Double_t maxmax_x=0.;
	Double_t maxmax_y=0.;
	Double_t Qp12[fPaddlesPerPlane*2+1]; // average of plane 1 and 2
	Double_t Qp34[fPaddlesPerPlane*2+1]; // average of plane 3 and 4
	Int_t maxmax_plane=0;
	Int_t maxmax_bar=0;
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
		for(Int_t j=1;j<fPaddlesPerPlane*2;j++){
			if(i == fNofPlanes){
				if(tof[i][j]>0. && tof[i-1][j]>0. && Q[i][j]>0.) {
					Double_t delta=tof[i][j]-tof[i-1][j];
					Qbeta[i][j] = Q[i][j]+betaCorr(delta);
					fhTdiffvsQ[i-1][j-1]->Fill(delta, Qbeta[i][j]);
					Q[i][j]=Qbeta[i][j];
				}
			}
			else{
				if(tof[i][j]>0. && tof[i+1][j]>0. && Q[i][j]>0.) {
					Double_t delta=tof[i+1][j]-tof[i][j];
					Qbeta[i][j] = Q[i][j]+betaCorr(delta);
					fhTdiffvsQ[i-1][j-1]->Fill(delta, Qbeta[i][j]);
					Q[i][j]=Qbeta[i][j];
				}
			}
			if(Q[i][j]>0. && Q[i+1][j]>0.) {
/*				Double_t v[2];
				v[0] = Q[1][j];
				v[1] = Q[i][j];
				fhQvsQ[i-1][j-1]->Fill(v);*/
				fhQvsQ[i-1][j-1]->Fill(Q[i+1][j], Q[i][j]);
			}
			if(Q[i][j]>0. && Q[1][j]>0. && i==fNofPlanes) {
				// if last plane use first plane instead
/*				Double_t v[2];
				v[0] = Q[1][j];
				v[1] = Q[i][j];
				fhQvsQ[i-1][j-1]->Fill(v);*/
				fhQvsQ[i-1][j-1]->Fill(Q[1][j], Q[i][j]);
			}

			if (i==1 && j==40){
				if(Q[i][j]>0.) fhChargeLosTofD->Fill(Q[i][j],LosQ);
			}
			//average plane 1 and 2
			if(i==1 && Q[1][j]>0 && Q[2][j]>0 && abs(Q[1][j] - Q[2][j])<2.5){
				Qp12[j]=(Q[1][j]+Q[2][j])/2.;
				fhQp12->Fill(j,Qp12[j]);
			}
			//average plane 3 and 4
			if(i==3 && Q[3][j]>0 && Q[4][j]>0 && abs(Q[3][j] - Q[4][j])<2.5){
				Qp34[j]=(Q[3][j]+Q[4][j])/2.;
				fhQp34->Fill(j,Qp34[j]);
			}

			if(Q[i][j]>max_Z[i]){
				max_t[i]=tof[i][j];
				max_x[i]=x[i][j];
				max_y[i]=y[i][j];
				max_Z[i]=Q[i][j];                
				max_bar[i]=j;
				if(Q[i][j]>maxmax_Z){
					maxmax_t=tof[i][j];
					maxmax_x=x[i][j];
					maxmax_y=y[i][j];
					maxmax_Z=Q[i][j];                
					maxmax_plane=i;
					maxmax_bar=j;
				}					
			}
	          
		}

			
		
		
		if(max_Z[i]>0.){
			hitBars[i]++;
		}
	}
      // average measurements of 4 planes
/*      
	  maxmax_Z=max_Z[1];
	  maxmax_x=max_x[1];
      maxmax_y=max_y[1];
      maxmax_t=max_t[1];
      maxmax_bar=max_bar[1];
      maxmax_plane=1;
*/      
	  Double_t Z_ave=maxmax_Z;
	  Double_t x_ave=maxmax_x;
      Double_t y_ave=maxmax_y;
      Double_t t_ave=maxmax_t;

      for(Int_t p=1;p<5;p++){
		  // only add to average if they have seen similar Q, otherwise a reaction took place or no hit 
		if(abs(maxmax_Z-max_Z[p])<2.5 && abs(maxmax_bar-max_bar[p])<2 && p!=maxmax_plane){
          Z_ave=(Z_ave+max_Z[p])/2.;
          x_ave=(x_ave+max_x[p])/2.;
          y_ave=(y_ave+max_y[p])/2.;
          t_ave=(t_ave+max_t[p])/2.;
		}
      }

      if(Z_ave>35. && Z_ave<32.){
		  cout<<"Average: "<<Z_ave<<endl;
		  cout<<"p1: "<<max_Z[1]<<" p2: "<<max_Z[2]<<" p3: "<<max_Z[3]<<" p4: "<<max_Z[4]<<endl;
     		  
	  }
      if(Z_ave>0){
		fhCharge->Fill(Z_ave);     
		fhxy->Fill(x_ave,y_ave);
	    fhChargevsTof->Fill(Z_ave,t_ave);
		fhChargevsPos->Fill(maxmax_bar,Z_ave);

//	cout<<"Average: "<<Z_ave<<endl;
	      
		// write out calibrated event
		new ((*fHitItems)[fNofHitItems]) R3BTofdHitData(t_ave,  x_ave , y_ave, Z_ave, t_ave, Z_ave, 1);
		fNofHitItems += 1;
	  }
	fnEvents++;
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
    if (fHitItemsLos)
    {
        fHitItemsLos->Clear();
    }

}

void R3BTofdCal2Hit::FinishTask()
{

    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
/*
		if (fhPos[i]) fhPos[i]->Write();
        if (fhTotPm1[i]) fhTotPm1[i]->Write();
        if (fhTotPm2[i]) fhTotPm2[i]->Write();
		if (fhTotPm1Sat[i]) fhTotPm1Sat[i]->Write();
		if (fhTotPm2Sat[i]) fhTotPm2Sat[i]->Write();
*/		
        if (fhQPm1[i]) fhQPm1[i]->Write();
        if (fhQPm2[i]) fhQPm2[i]->Write();
        if (fhQ[i]) fhQ[i]->Write();
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
/*			
            if (fhQ1vsPos[i][j]) fhQ1vsPos[i][j]->Write();
            if (fhQ2vsPos[i][j]) fhQ2vsPos[i][j]->Write();
            if (fhQvsPos[i][j]) fhQvsPos[i][j]->Write();
            if (fhSqrtQvsPos[i][j]) fhSqrtQvsPos[i][j]->Write();
*/
/*
            if (fhTotPm1vsTotPm2[i][j]) fhTotPm1vsTotPm2[i][j]->Write();
            if (fhTotPm1satvsTotPm2sat[i][j]) fhTotPm1satvsTotPm2sat[i][j]->Write();
            if (fhTot1vsPos[i][j]) fhTot1vsPos[i][j]->Write();
            if (fhTot2vsPos[i][j]) fhTot2vsPos[i][j]->Write();
*/
            if (fhTdiffvsQ[i][2*j]) fhTdiffvsQ[i][2*j]->Write();
            if (fhTdiffvsQ[i][2*j+1]) fhTdiffvsQ[i][2*j+1]->Write();
            if (fhQvsQ[i][2*j]) fhQvsQ[i][2*j]->Write();
            if (fhQvsQ[i][2*j+1]) fhQvsQ[i][2*j+1]->Write();
            if (fhQvsTof[i][j]) fhQvsTof[i][j]->Write();
	    
	    }
    }
    if (fhxy) fhxy->Write();
    if (fhCharge) fhCharge->Write();
    if (fhChargevsTof) fhChargevsTof->Write();
    if (fhChargevsPos) fhChargevsPos->Write();
    if (fhQp12) fhQp12->Write();
    if (fhQp34) fhQp34->Write();
  //  if (fhSaturation1) fhSaturation1->Write();
  //  if (fhSaturation2) fhSaturation2->Write();
    
	if (fhLosXYP) fhLosXYP->Write();
	if (fhChargeLosTofD) fhChargeLosTofD->Write();
	if (fh_los_pos) fh_los_pos->Write();
}

Double_t R3BTofdCal2Hit::betaCorr(Double_t delta)
{
	Double_t corr=-3.*delta;
//	corr=0.;
	return corr;
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
