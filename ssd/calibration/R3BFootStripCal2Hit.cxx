/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------------------
// -----            R3BFootStripCal2Hit source file                   -----
// -----       Created 05/11/21 by J.L. Rodriguez-Sanchez             -----
// ------------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TSpectrum.h"
#include <iomanip>
#include <vector>

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// FOOT headers
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootMappingPar.h"
#include "R3BFootStripCal2Hit.h"
#include "R3BLogger.h"


using namespace std;



// R3BFootStripCal2Hit::Default Constructor -------------------------------------
R3BFootStripCal2Hit::R3BFootStripCal2Hit()
    : R3BFootStripCal2Hit("R3BFootStripCal2Hit", 1)
{
}

// R3BFootStripCal2HitPar::Standard Constructor ---------------------------------
R3BFootStripCal2Hit::R3BFootStripCal2Hit(const TString& name, Int_t iVerbose)
	: FairTask(name, iVerbose)
	, fPitch(157.7)
	, fMiddle(50.)
	, fThSum(20.)
	, fMaxNumDet(16)
	, fExpId(522)
	, fMaxNumClusters(200) // Max number of clusters per foot detector set to 3
	, fFootCalData(NULL)
	, fFootHitData(NULL)
	, fMap_Par(NULL)
	, fOnline(kFALSE)
		  , fDistTarget{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
		  , fAngleTheta{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
		  , fAnglePhi{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
		  , fOffsetX{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
		  , fOffsetY{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
{
}

// Virtual R3BFootStripCal2Hit::Destructor --------------------------------------
R3BFootStripCal2Hit::~R3BFootStripCal2Hit()
{
	R3BLOG(debug1, "");
	if (fFootHitData)
		delete fFootHitData;
}

void R3BFootStripCal2Hit::SetParContainers()
{
	// Parameter Container
	// Reading footMappingPar from FairRuntimeDb
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	R3BLOG_IF(error, !rtdb, "FairRuntimeDb not found");

	fMap_Par = (R3BFootMappingPar*)rtdb->getContainer("footMappingPar");
	if (!fMap_Par)
	{
		R3BLOG(error, "Couldn't get handle on footMappingPar container");
	}
	else
	{
		R3BLOG(info, "footMappingPar found");
	}
}

void R3BFootStripCal2Hit::SetParameter()
{
	if (!fMap_Par)
	{
		R3BLOG(warn, "Container footMappingPar not found");
		return;
	}
	//--- Parameter Container ---
	fMaxNumDet = fMap_Par->GetNumDets(); // Number of foot detectors
	LOG(info) << "R3BFootStripCal2Hit::NumDet from mapping " << fMaxNumDet;
	for (Int_t i = 0; i < fMaxNumDet; i++)
	{
		fDistTarget[i] = fMap_Par->GetDist2target(i + 1);
		fAngleTheta[i] = fMap_Par->GetAngleTheta(i + 1);
		fAnglePhi[i] = fMap_Par->GetAnglePhi(i + 1);
		fOffsetX[i] = fMap_Par->GetOffsetX(i + 1);
		fOffsetY[i] = fMap_Par->GetOffsetY(i + 1);
	}
	fMap_Par->printParams();
}

Double_t R3BFootStripCal2Hit::EtaCorr_Barriere(TF1 *ffZ4, TF1* ffZ5, TF1* ffZ6, TF1* ffZ7, TF1* ffZ8, TF1* ffZ9, TF1 *ffZ4NoOff, TF1* ffZ5NoOff, TF1* ffZ6NoOff,   TF1* ffZ7NoOff, TF1* ffZ8NoOff, TF1* ffZ9NoOff, double fEnergy, double fEta)
{

	Double_t EZ4 = ffZ4->Eval(fEta);
	Double_t EZ5 = ffZ5->Eval(fEta);
	Double_t EZ6 = ffZ6->Eval(fEta);
	Double_t EZ7 = ffZ7->Eval(fEta);
	Double_t EZ8 = ffZ8->Eval(fEta);
	Double_t EZ9 = ffZ9->Eval(fEta);
	Double_t EnergyCorr = NAN;

	if(fEnergy<EZ4){

		Double_t gap = EZ4;  // or constant
		Double_t prop = fEnergy/gap;

		Double_t corr1 = 1.-ffZ4NoOff->Eval(fEta);
		Double_t corr2 = 1.;

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

        }else if(fEnergy>EZ4 && fEnergy<EZ5){

                Double_t gap = EZ5-EZ4;
                Double_t prop = (fEnergy-EZ4)/gap;

                Double_t corr1 = 1.-ffZ5NoOff->Eval(fEta);
                Double_t corr2 = 1.-ffZ4NoOff->Eval(fEta);

                EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ5 && fEnergy<EZ6){

		Double_t gap = EZ6-EZ5;
		Double_t prop = (fEnergy-EZ5)/gap;

		Double_t corr1 = 1.-ffZ6NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ5NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ6 && fEnergy<EZ7){

		Double_t gap = EZ7-EZ6;
		Double_t prop = (fEnergy-EZ6)/gap;

		Double_t corr1 = 1.-ffZ7NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ6NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ7 && fEnergy<EZ8){

		Double_t gap = EZ8-EZ7;
		Double_t prop = (fEnergy-EZ7)/gap;

		Double_t corr1 = 1.-ffZ8NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ7NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ8 && fEnergy<EZ9){
		Double_t gap = EZ9-EZ8;
		Double_t prop = (fEnergy-EZ8)/gap;

		Double_t corr1 = 1.-ffZ9NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ8NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ9){

		Double_t gap = 4000-EZ9;  // or constant
		Double_t prop = (fEnergy-EZ9)/gap;

		Double_t corr1 = 1.;
		Double_t corr2 = 1.-ffZ9NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}

	return EnergyCorr;

}

Double_t R3BFootStripCal2Hit::EtaCorr_Lagni(TF1* ffZ3,TF1* ffZ4, TF1* ffZ5, TF1* ffZ6, TF1* ffZ7, TF1* ffZ3NoOff, TF1* ffZ4NoOff, TF1* ffZ5NoOff, TF1* ffZ6NoOff, TF1* ffZ7NoOff, double fEnergy, double fEta)
{

	Double_t EZ3 = ffZ3->Eval(fEta);
	Double_t EZ4 = ffZ4->Eval(fEta);
	Double_t EZ5 = ffZ5->Eval(fEta);
	Double_t EZ6 = ffZ6->Eval(fEta);
	Double_t EZ7 = ffZ7->Eval(fEta);
	Double_t EnergyCorr = NAN;

	if(fEnergy<EZ4 && fEnergy>EZ3){

		Double_t gap = EZ4-EZ3;
		Double_t prop = (fEnergy-EZ3)/gap;

		Double_t corr1 = 1.-ffZ4NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ3NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy<EZ5 && fEnergy>EZ4){

		Double_t gap = EZ5-EZ4;
		Double_t prop = (fEnergy-EZ4)/gap;

		Double_t corr1 = 1.-ffZ5NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ4NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ5 && fEnergy<EZ6){

		Double_t gap = EZ6-EZ5;
		Double_t prop = (fEnergy-EZ5)/gap;

		Double_t corr1 = 1.-ffZ6NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ5NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy<EZ4){

		Double_t gap = EZ4;
		Double_t prop = fEnergy/gap;

		Double_t corr1 = 1.-ffZ4NoOff->Eval(fEta);
		Double_t corr2 = 1.;

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ6 && fEnergy<EZ7){

		Double_t gap = EZ7-EZ6;
		Double_t prop = (fEnergy-EZ6)/gap;

		Double_t corr1 = 1.-ffZ7NoOff->Eval(fEta);
		Double_t corr2 = 1.-ffZ6NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}else if(fEnergy>EZ7){

		Double_t gap = 2000.-EZ7;
		Double_t prop = (fEnergy-EZ7)/gap;

		Double_t corr1 = 1.;
		Double_t corr2 = 1.-ffZ7NoOff->Eval(fEta);

		EnergyCorr = fEnergy+prop*corr1+(1.-prop)*corr2;

	}

	return EnergyCorr;

}

// -----   Public method Init   -------------------------------------------------
InitStatus R3BFootStripCal2Hit::Init()
{
	R3BLOG(info, "");

	FairRootManager* rootManager = FairRootManager::Instance();
	if (!rootManager)
	{
		R3BLOG(fatal, "FairRootManager not found");
		return kFATAL;
	}

	// INPUT DATA
	fFootCalData = (TClonesArray*)rootManager->GetObject("FootCalData");
	if (!fFootCalData)
	{
		R3BLOG(fatal, "FootCalData not found");
		return kFATAL;
	}

	// Set container with mapping parameters
	SetParameter();

	R3BFootStripCal2Hit::fZ3 = new TF1("fZ3","TMath::Exp(3.9982-28.8242*x)+18.59",0.,0.5);
	R3BFootStripCal2Hit::fZ4 = new TF1("fZ4","TMath::Exp(5.34286-22.9916*x)+101.193",0.,0.5);
	R3BFootStripCal2Hit::fZ5 = new TF1("fZ5","TMath::Exp(6.12994-15.9781*x)+216.47",0.,0.5);
	R3BFootStripCal2Hit::fZ6 = new TF1("fZ6","TMath::Exp(6.59235-13.8799*x)+324.633",0.,0.5);
	R3BFootStripCal2Hit::fZ7 = new TF1("fZ7","TMath::Exp(6.95883-10.7827583*x)+381.679",0.,0.5);
	R3BFootStripCal2Hit::fZ3NoOff = new TF1("fZ3NoOff","TMath::Exp(3.9982-28.8242*x)",0.,0.5);
	R3BFootStripCal2Hit::fZ4NoOff = new TF1("fZ4NoOff","TMath::Exp(5.34286-22.9916*x)",0.,0.5);
	R3BFootStripCal2Hit::fZ5NoOff = new TF1("fZ5NoOff","TMath::Exp(6.12994-15.9781*x)",0.,0.5);
	R3BFootStripCal2Hit::fZ6NoOff = new TF1("fZ6NoOff","TMath::Exp(6.59235-13.8799*x)",0.,0.5);
	R3BFootStripCal2Hit::fZ7NoOff = new TF1("fZ7NoOff","TMath::Exp(6.95883-10.7827583*x)",0.,0.5);

	fZ4_0 = new TF1("fZ4_0"," 432.145 -3055.91*TMath::Power(x,1) + 5817.12*TMath::Power(x,2) + 51526.8*TMath::Power(x,3) -316706*TMath::Power(x,4) +  761651*TMath::Power(x,5) -938861*TMath::Power(x,6) + 585658*TMath::Power(x,7) -146060*TMath::Power(x,8)",0.,1.);
	fZ5_0 = new TF1("fZ5_0"," 814.617 -6322.35*TMath::Power(x,1) + 33083.1*TMath::Power(x,2) -99704.7*TMath::Power(x,3) + 184652*TMath::Power(x,4) -204802*TMath::Power(x,5) + 123208*TMath::Power(x,6) -31778.6*TMath::Power(x,7) + 1642.45*TMath::Power(x,8)",0.,1.);
	fZ6_0 = new TF1("fZ6_0","1401.1 -11021.5*TMath::Power(x,1) + 55672.4*TMath::Power(x,2) -148018*TMath::Power(x,3) + 219131*TMath::Power(x,4) -172195*TMath::Power(x,5) + 56406.1*TMath::Power(x,6)",0.,1.);
	fZ7_0 = new TF1("fZ7_0","2049.14 -13434.6*TMath::Power(x,1) + 58496.8*TMath::Power(x,2) -134912*TMath::Power(x,3) + 179795*TMath::Power(x,4) -135295*TMath::Power(x,5) + 45377.9*TMath::Power(x,6)",0.,1.);
	fZ8_0 = new TF1("fZ8_0"," 2809.67 -17008.4*TMath::Power(x,1) + 71037*TMath::Power(x,2) -189403*TMath::Power(x,3) + 413817*TMath::Power(x,4) -680092*TMath::Power(x,5) + 680437*TMath::Power(x,6) -351489*TMath::Power(x,7) + 72753.6*TMath::Power(x,8)",0.,1.);
	fZ9_0 = new TF1("fZ9_0","3562.03 -12504.2*TMath::Power(x,1) -1545.28*TMath::Power(x,2) +161037*TMath::Power(x,3) -410651*TMath::Power(x,4) +393902*TMath::Power(x,5) -130218*TMath::Power(x,6)",0.,1.);
	fZ4NoOff_0 = new TF1("fZ4NoOff_0","-3055.91*TMath::Power(x,1) + 5817.12*TMath::Power(x,2) + 51526.8*TMath::Power(x,3) -316706*TMath::Power(x,4) +  761651*TMath::Power(x,5) -938861*TMath::Power(x,6) + 585658*TMath::Power(x,7) -146060*TMath::Power(x,8)",0.,1.);
	fZ5NoOff_0 = new TF1("fZ5NoOff_0","-6322.35*TMath::Power(x,1) + 33083.1*TMath::Power(x,2) -99704.7*TMath::Power(x,3) + 184652*TMath::Power(x,4) -204802*TMath::Power(x,5) + 123208*TMath::Power(x,6) -31778.6*TMath::Power(x,7) + 1642.45*TMath::Power(x,8)",0.,1.);
	fZ6NoOff_0 = new TF1("fZ6NoOff_0","-11021.5*TMath::Power(x,1) + 55672.4*TMath::Power(x,2) -148018*TMath::Power(x,3) + 219131*TMath::Power(x,4) -172195*TMath::Power(x,5) + 56406.1*TMath::Power(x,6)",0.,1.);
	fZ7NoOff_0 = new TF1("fZ7NoOff_0","-13434.6*TMath::Power(x,1) + 58496.8*TMath::Power(x,2) -134912*TMath::Power(x,3) + 179795*TMath::Power(x,4) -135295*TMath::Power(x,5) + 45377.9*TMath::Power(x,6)",0.,1.);
	fZ8NoOff_0 = new TF1("fZ8NoOff_0","-17008.4*TMath::Power(x,1) + 71037*TMath::Power(x,2) -189403*TMath::Power(x,3) + 413817*TMath::Power(x,4) -680092*TMath::Power(x,5) + 680437*TMath::Power(x,6) -351489*TMath::Power(x,7) + 72753.6*TMath::Power(x,8)",0.,1.);
	fZ9NoOff_0 = new TF1("fZ9NoOff_0","-12504.2*TMath::Power(x,1) -1545.28*TMath::Power(x,2) +161037*TMath::Power(x,3) -410651*TMath::Power(x,4) +393902*TMath::Power(x,5) -130218*TMath::Power(x,6)",0.,1.);


	fZ4_1 = new TF1("fZ4_1"," 527.723 -6255.2*TMath::Power(x,1) + 51037*TMath::Power(x,2) -246452*TMath::Power(x,3) + 728201*TMath::Power(x,4) -1.31393e+06*TMath::Power(x,5) + 1.40604e+06*TMath::Power(x,6) -819001*TMath::Power(x,7) + 200348*TMath::Power(x,8)",0.,1.);
	fZ5_1 = new TF1("fZ5_1"," 921.426 -8374.7*TMath::Power(x,1) + 51320.9*TMath::Power(x,2) -180619*TMath::Power(x,3) + 392189*TMath::Power(x,4) -534352*TMath::Power(x,5) + 445654*TMath::Power(x,6) -209234*TMath::Power(x,7) + 43320.3*TMath::Power(x,8)",0.,1.);
	fZ6_1 = new TF1("fZ6_1","1433.24 -10438.7*TMath::Power(x,1) +50909.7*TMath::Power(x,2) -134211*TMath::Power(x,3) + 201420*TMath::Power(x,4) -162495*TMath::Power(x,5) + 54854.5*TMath::Power(x,6)",0.,1.);
	fZ7_1 = new TF1("fZ7_1"," 2150.47 -13187.1*TMath::Power(x,1) +54254.5*TMath::Power(x,2) -146320*TMath::Power(x,3) + 348520*TMath::Power(x,4) -652272*TMath::Power(x,5) + 757650*TMath::Power(x,6) -467906*TMath::Power(x,7) + 119232*TMath::Power(x,8)",0.,1.);
	fZ8_1 = new TF1("fZ8_1"," 2885.51 -16405.3*TMath::Power(x,1) + 73326.8*TMath::Power(x,2) -287338*TMath::Power(x,3) + 995538*TMath::Power(x,4) -2.19634e+06*TMath::Power(x,5) + 2.69491e+06*TMath::Power(x,6) -1.68807e+06*TMath::Power(x,7) + 424364*TMath::Power(x,8)",0.,1.);
	fZ9_1 = new TF1("fZ9_1","3489.31  -11011.7*TMath::Power(x,1) -16804.2*TMath::Power(x,2) + 274979*TMath::Power(x,3) -873580*TMath::Power(x,4) + 1.38936e+06*TMath::Power(x,5) -1.27768e+06*TMath::Power(x,6) +  673161 *TMath::Power(x,7) -158419*TMath::Power(x,8)",0.,1.);
	fZ4NoOff_1 = new TF1("fZ4NoOff_1","-6255.2*TMath::Power(x,1) + 51037*TMath::Power(x,2) -246452*TMath::Power(x,3) + 728201*TMath::Power(x,4) -1.31393e+06*TMath::Power(x,5) + 1.40604e+06*TMath::Power(x,6) -819001*TMath::Power(x,7) + 200348*TMath::Power(x,8)",0.,1.);
	fZ5NoOff_1 = new TF1("fZ5NoOff_1","-8374.7*TMath::Power(x,1) + 51320.9*TMath::Power(x,2) -180619*TMath::Power(x,3) + 392189*TMath::Power(x,4) -534352*TMath::Power(x,5) + 445654*TMath::Power(x,6) -209234*TMath::Power(x,7) + 43320.3*TMath::Power(x,8)",0.,1.);
	fZ6NoOff_1 = new TF1("fZ6NoOff_1","-10438.7*TMath::Power(x,1) +50909.7*TMath::Power(x,2) -134211*TMath::Power(x,3) + 201420*TMath::Power(x,4) -162495*TMath::Power(x,5) + 54854.5*TMath::Power(x,6)",0.,1.);
	fZ7NoOff_1 = new TF1("fZ7NoOff_1","-13187.1*TMath::Power(x,1) +54254.5*TMath::Power(x,2) -146320*TMath::Power(x,3) + 348520*TMath::Power(x,4) -652272*TMath::Power(x,5) + 757650*TMath::Power(x,6) -467906*TMath::Power(x,7) + 119232*TMath::Power(x,8)",0.,1.);
	fZ8NoOff_1 = new TF1("fZ8NoOff_1","-16405.3*TMath::Power(x,1) + 73326.8*TMath::Power(x,2) -287338*TMath::Power(x,3) + 995538*TMath::Power(x,4) -2.19634e+06*TMath::Power(x,5) + 2.69491e+06*TMath::Power(x,6) -1.68807e+06*TMath::Power(x,7) + 424364*TMath::Power(x,8)",0.,1. );
	fZ9NoOff_1 = new TF1("fZ9NoOff_1","-11011.7*TMath::Power(x,1) -16804.2*TMath::Power(x,2) + 274979*TMath::Power(x,3) -873580*TMath::Power(x,4) + 1.38936e+06*TMath::Power(x,5) -1.27768e+06*TMath::Power(x,6) +  673161 *TMath::Power(x,7) -158419*TMath::Power(x,8)",0.,1.);


	fZ4_14 = new TF1("fZ4_14"," 409.691 -4759.13*TMath::Power(x,1) +32996.5*TMath::Power(x,2) -126309*TMath::Power(x,3) + 290363*TMath::Power(x,4) -414311*TMath::Power(x,5) + 366673*TMath::Power(x,6) -189648*TMath::Power(x,7) + 44991.1*TMath::Power(x,8)",0.,1.);
	fZ5_14 = new TF1("fZ5_14"," 892.163 -9394.02*TMath::Power(x,1) + 64705.2*TMath::Power(x,2) -261413*TMath::Power(x,3) +  655753*TMath::Power(x,4) -1.02738e+06*TMath::Power(x,5) +  977901*TMath::Power(x,6) -519750*TMath::Power(x,7) + 119450*TMath::Power(x,8)",0.,1.);
	fZ6_14 = new TF1("fZ6_14"," 1421.98 -12021*TMath::Power(x,1) +68054.7*TMath::Power(x,2) -217793*TMath::Power(x,3) + 436500*TMath::Power(x,4) -588350*TMath::Power(x,5) + 547085*TMath::Power(x,6) -322740*TMath::Power(x,7) + 89244.5*TMath::Power(x,8)",0.,1.);
	fZ7_14 = new TF1("fZ7_14"," 2022.66 -18440.3*TMath::Power(x,1) + 102587*TMath::Power(x,2) -299689*TMath::Power(x,3) + 501158*TMath::Power(x,4) -494168*TMath::Power(x,5) + 298289*TMath::Power(x,6) -125047*TMath::Power(x,7) + 35305*TMath::Power(x,8)",0.,1.);
	fZ8_14 = new TF1("fZ8_14"," 2610.14 -21480.7*TMath::Power(x,1) + 126854*TMath::Power(x,2) -484633*TMath::Power(x,3) + 1.32256e+06*TMath::Power(x,4) -2.44607e+06*TMath::Power(x,5) + 2.79628e+06*TMath::Power(x,6) -1.75588e+06*TMath::Power(x,7) + 462459*TMath::Power(x,8)",0.,1.);
	fZ9_14 = new TF1("fZ9_14"," 3355.2 -14287.3*TMath::Power(x,1) -25923.3*TMath::Power(x,2) + 582450*TMath::Power(x,3) -2.47564e+06*TMath::Power(x,4) +5.17891e+06*TMath::Power(x,5) -5.92022e+06*TMath::Power(x,6) + 3.53276e+06*TMath::Power(x,7) -857949*TMath::Power(x,8)",0.,1.);
	fZ4NoOff_14 = new TF1("fZ4NoOff_14","-4759.13*TMath::Power(x,1) +32996.5*TMath::Power(x,2) -126309*TMath::Power(x,3) + 290363*TMath::Power(x,4) -414311*TMath::Power(x,5) +     366673*TMath::Power(x,6) -189648*TMath::Power(x,7) + 44991.1*TMath::Power(x,8)",0.,1.);
	fZ5NoOff_14 = new TF1("fZ5NoOff_14","-9394.02*TMath::Power(x,1) + 64705.2*TMath::Power(x,2) -261413*TMath::Power(x,3) +  655753*TMath::Power(x,4) -1.02738e+06*TMath::Power(x,  5) +  977901*TMath::Power(x,6) -519750*TMath::Power(x,7) + 119450*TMath::Power(x,8)",0.,1.);
	fZ6NoOff_14 = new TF1("fZ6NoOff_14","-12021*TMath::Power(x,1) +68054.7*TMath::Power(x,2) -217793*TMath::Power(x,3) + 436500*TMath::Power(x,4) -588350*TMath::Power(x,5) +       547085*TMath::Power(x,6) -322740*TMath::Power(x,7) + 89244.5*TMath::Power(x,8)",0.,1.);
	fZ7NoOff_14 = new TF1("fZ7NoOff_14","-18440.3*TMath::Power(x,1) + 102587*TMath::Power(x,2) -299689*TMath::Power(x,3) + 501158*TMath::Power(x,4) -494168*TMath::Power(x,5) +     298289*TMath::Power(x,6) -125047*TMath::Power(x,7) + 35305*TMath::Power(x,8)",0.,1.);
	fZ8NoOff_14 = new TF1("fZ8NoOff_14","-21480.7*TMath::Power(x,1) + 126854*TMath::Power(x,2) -484633*TMath::Power(x,3) + 1.32256e+06*TMath::Power(x,4) -2.44607e+06*     TMath::Power(x,5) + 2.79628e+06*TMath::Power(x,6) -1.75588e+06*TMath::Power(x,7) + 462459*TMath::Power(x,8)",0.,1.);
	fZ9NoOff_14 = new TF1("fZ9NoOff_14","-14287.3*TMath::Power(x,1) -25923.3*TMath::Power(x,2) + 582450*TMath::Power(x,3) -2.47564e+06*TMath::Power(x,4) +5.17891e+06*    TMath::Power(x,5) -5.92022e+06*TMath::Power(x,6) + 3.53276e+06*TMath::Power(x,7) -857949*TMath::Power(x,8)",0.,1.);

	fZ4_15 = new TF1("fZ4_15"," 397.864 -3004.03*TMath::Power(x,1) + 10628.8*TMath::Power(x,2) + 2035.04*TMath::Power(x,3) -110125*TMath::Power(x,4) + 308386*TMath::Power(x,5) -388480*TMath::Power(x,6) +  236311*TMath::Power(x,7) -55772*TMath::Power(x,8)",0.,1.);
	fZ5_15 = new TF1("fZ5_15","870.582 -7907.16*TMath::Power(x,1) + 41256.8*TMath::Power(x,2) -103744*TMath::Power(x,3) + 114252*TMath::Power(x,4) + 12775.8*TMath::Power(x,5) -156417*TMath::Power(x,6) + 137262*TMath::Power(x,7) -37548.3*TMath::Power(x,8)",0.,1.);
	fZ6_15 = new TF1("fZ6_15","1343.69 -4893.2*TMath::Power(x,1) -37656.8*TMath::Power(x,2) + 446440*TMath::Power(x,3) -1.75329e+06*TMath::Power(x,4) + 3.55425e+06*TMath::Power(x,5) -3.99139e+06*TMath::Power(x,6) + 2.35327e+06*TMath::Power(x,7) -566612*TMath::Power(x,8)",0.,1.);
	fZ7_15 = new TF1("fZ7_15"," 1994.05 -1624.21*TMath::Power(x,1) -108250*TMath::Power(x,2) + 854544*TMath::Power(x,3) -2.95857e+06*TMath::Power(x,4) + 5.7073e+06*TMath::Power(x,5) -6.38378e+06*TMath::Power(x,6) + 3.86603e+06*TMath::Power(x,7) -975614*TMath::Power(x,8)",0.,1.);
	fZ8_15 = new TF1("fZ8_15"," 2671.19 + 2714.9*TMath::Power(x,1) -171731*TMath::Power(x,2) + 1.08462e+06*TMath::Power(x,3) -3.17033e+06*TMath::Power(x,4) + 5.32047e+06*TMath::Power(x,5) -5.42766e+06*TMath::Power(x,6) + 3.16485e+06*TMath::Power(x,7) -802716*TMath::Power(x,8)",0.,1.);
	fZ9_15 = new TF1("fZ9_15"," 3366.32 + 10494.8*TMath::Power(x,1) -229350*TMath::Power(x,2) + 1.01872e+06*TMath::Power(x,3) -1.78568e+06*TMath::Power(x,4) + 1.11927e+06*TMath::Power(x,5) + 206856*TMath::Power(x,6) -408202*TMath::Power(x,7) + 67874.6*TMath::Power(x,8)",0.,1.);
	fZ4NoOff_15 = new TF1("fZ4NoOff_15","-3004.03*TMath::Power(x,1) + 10628.8*TMath::Power(x,2) + 2035.04*TMath::Power(x,3) -110125*TMath::Power(x,4) + 308386*TMath::Power(x,5) -  388480*TMath::Power(x,6) +  236311*TMath::Power(x,7) -55772*TMath::Power(x,8)",0.,1.);
	fZ5NoOff_15 = new TF1("fZ5NoOff_15","-7907.16*TMath::Power(x,1) + 41256.8*TMath::Power(x,2) -103744*TMath::Power(x,3) + 114252*TMath::Power(x,4) + 12775.8*TMath::Power(x,5) -   156417*TMath::Power(x,6) + 137262*TMath::Power(x,7) -37548.3*TMath::Power(x,8)",0.,1.);
	fZ6NoOff_15 = new TF1("fZ6NoOff_15","-4893.2*TMath::Power(x,1) -37656.8*TMath::Power(x,2) + 446440*TMath::Power(x,3) -1.75329e+06*TMath::Power(x,4) + 3.55425e+06*TMath::Power(x,5) -3.99139e+06*TMath::Power(x,6) + 2.35327e+06*TMath::Power(x,7) -566612*TMath::Power(x,8)",0.,1.);
	fZ7NoOff_15 = new TF1("fZ7NoOff_15","-1624.21*TMath::Power(x,1) -108250*TMath::Power(x,2) + 854544*TMath::Power(x,3) -2.95857e+06*TMath::Power(x,4) + 5.7073e+06*TMath::Power(x,5) -6.38378e+06*TMath::Power(x,6) + 3.86603e+06*TMath::Power(x,7) -975614*TMath::Power(x,8)",0.,1.);
	fZ8NoOff_15 = new TF1("fZ8NoOff_15","2714.9*TMath::Power(x,1) -171731*TMath::Power(x,2) + 1.08462e+06*TMath::Power(x,3) -3.17033e+06*TMath::Power(x,4) + 5.32047e+06*TMath::Power(x,5) -5.42766e+06*TMath::Power(x,6) + 3.16485e+06*TMath::Power(x,7) -802716*TMath::Power(x,8)",0.,1.);
	fZ9NoOff_15 = new TF1("fZ9NoOff_15","10494.8*TMath::Power(x,1) -229350*TMath::Power(x,2) + 1.01872e+06*TMath::Power(x,3) -1.78568e+06*TMath::Power(x,4) + 1.11927e+06*TMath::Power(x,5) + 206856*TMath::Power(x,6) -408202*TMath::Power(x,7) + 67874.6*TMath::Power(x,8)",0.,1.);

	// Output data
	fFootHitData = new TClonesArray("R3BFootHitData");
	rootManager->Register("FootHitData", "FOOT Hit", fFootHitData, !fOnline);
	fFootHitData->Clear();

	char Name[255];
	for (Int_t i = 0; i < fMaxNumDet; i++)
	{
		sprintf(Name, "hssd_%d", i + 1);
		hssd.push_back(new TH1F(Name, "", 640, -0.5, 640.5));
	}

	return kSUCCESS;
}

// -----   Public method ReInit   -----------------------------------------------
InitStatus R3BFootStripCal2Hit::ReInit()
{
	SetParContainers();
	SetParameter();
	return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BFootStripCal2Hit::Exec(Option_t* option)
{
	// Reset entries in output arrays, local arrays
	Reset();

	// Reading the Input Cal Data
	Int_t nHits = fFootCalData->GetEntriesFast();
	if (nHits == 0)
		return;

	// Data from cal level
	R3BFootCalData** calData = new R3BFootCalData*[nHits];
	Int_t detId;
	Int_t stripId;
	Double_t energy;
	Double_t x = 0., y = 0., z = 0.;
	vector<vector<Int_t>> StripI;
	vector<vector<Double_t>> StripE;

	// Clustering algorithm - A. Revel
	// Init
	StripI.clear();
	StripE.clear();
	ClusterMult.clear();
	ClusterPos.clear();
	ClusterESum.clear();
	ClusterNStrip.clear();
	Eta.clear();
	ClusterI.clear();
	ClusterE.clear();

	StripI.resize(fMaxNumDet);
	StripE.resize(fMaxNumDet);

	ClusterMult.resize(fMaxNumDet);
	ClusterPos.resize(fMaxNumDet);
	ClusterESum.resize(fMaxNumDet);
	ClusterNStrip.resize(fMaxNumDet);
	Eta.resize(fMaxNumDet);
	ClusterI.resize(fMaxNumDet);
	ClusterE.resize(fMaxNumDet);

	for(int i = 0; i<ClusterMult.size(); i++){ ////should be fMaxNumDet

		ClusterPos[i].resize(200); ////we cannot apply the fMaxNumClusters limit here => sorting at the end
		ClusterESum[i].resize(200);
		Eta[i].resize(200);
		ClusterNStrip[i].resize(200);
		ClusterI[i].resize(200);
		ClusterE[i].resize(200);

	}

	// Filling vectors
	for (Int_t i = 0; i < nHits; i++)
	{
		calData[i] = (R3BFootCalData*)(fFootCalData->At(i));
		detId = calData[i]->GetDetId() - 1;
		stripId = calData[i]->GetStripId() - 1;
		energy = calData[i]->GetEnergy();

		StripI[detId].push_back(stripId); 
		StripE[detId].push_back(energy);
	}

	// Sort (should be good by default but just in case)
	for (Int_t i = 0; i < fMaxNumDet; i++)
	{
		if (StripI[i].size() > 1)
		{
			for (Int_t j = 0; j < StripI[i].size() - 1; j++)
			{
				for (Int_t k = j + 1; k < StripI[i].size(); k++)
				{
					if (StripI[i][j] > StripI[i][k])
					{
						Int_t tempI = StripI[i][j];
						StripI[i][j] = StripI[i][k];
						StripI[i][k] = tempI;
						Int_t tempD = StripE[i][j];
						StripE[i][j] = StripE[i][k];
						StripE[i][k] = tempD;
					}
				}
			}
		}
	}

	// Clustering
	for (Int_t i = 0; i < fMaxNumDet; i++)
	{

		Int_t ClusterCount = 0;
		Int_t TempI = 0;

		Int_t j = 0;

		while (j < StripI[i].size())
		{

			if (j == 0)
			{
				TempI = StripI[i][j];
			}

			ClusterNStrip[i][ClusterCount]++;
			ClusterI[i][ClusterCount].push_back(StripI[i][j]);
			ClusterE[i][ClusterCount].push_back(StripE[i][j]);

			Int_t k = j + 1;

			while (k < StripI[i].size() && (StripI[i][k] - TempI) == 1)
			{
				ClusterNStrip[i][ClusterCount]++;
				ClusterI[i][ClusterCount].push_back(StripI[i][k]);
				ClusterE[i][ClusterCount].push_back(StripE[i][k]);
				TempI = StripI[i][k];
				StripI[i].erase(StripI[i].begin() + k);
				StripE[i].erase(StripE[i].begin() + k);
			}

			StripI[i].erase(StripI[i].begin() + j);
			StripE[i].erase(StripE[i].begin() + j);

			ClusterCount++;
		}
		ClusterMult[i] = ClusterCount;
	}

	// Compute Sum Energy, Position and Eta
	for (Int_t i = 0; i < fMaxNumDet; i++)
	{
		for (Int_t j = 0; j < ClusterMult[i]; j++)
		{
			for (Int_t k = 0; k < ClusterNStrip[i][j]; k++)
			{
				ClusterESum[i][j] += ClusterE[i][j][k];
				ClusterPos[i][j] += ClusterE[i][j][k] * ClusterI[i][j][k];
			}
			ClusterPos[i][j] = ClusterPos[i][j] / ClusterESum[i][j];
			Eta[i][j] = ClusterPos[i][j] - (Int_t)ClusterPos[i][j];
		}
	}

	// Sort Cluster from Higher to Lower Energy
	for (Int_t i = 0; i < fMaxNumDet; i++)
	{
		for (Int_t j = 0; j < ClusterMult[i] - 1; j++)
		{
			for (Int_t k = j + 1; k < ClusterMult[i]; k++)
			{
				if (ClusterESum[i][j] < ClusterESum[i][k])
				{

					Int_t tempI = ClusterNStrip[i][j];
					ClusterNStrip[i][j] = ClusterNStrip[i][k];
					ClusterNStrip[i][k] = tempI;

					Double_t tempD = ClusterESum[i][j];
					ClusterESum[i][j] = ClusterESum[i][k];
					ClusterESum[i][k] = tempD;

					tempD = ClusterPos[i][j];
					ClusterPos[i][j] = ClusterPos[i][k];
					ClusterPos[i][k] = tempD;

					tempD = Eta[i][j];
					Eta[i][j] = Eta[i][k];
					Eta[i][k] = tempD;
				}
			}
		}
	}

	// Filling HitData
	for (Int_t i = 0; i < fMaxNumDet; i++)
	{
		for (Int_t j = 0; j < ClusterMult[i]; j++)
		{

			Double_t pos = 100. * ClusterPos[i][j] / 640. - 50.;

			if (fAnglePhi[i] == 0.)
			{ // X-Foot (StripId numbered from left to right)
				x = pos * TMath::Cos(fAngleTheta[i] * TMath::DegToRad()) + fOffsetX[i];
				y = fOffsetY[i];
				z = pos * TMath::Sin(fAngleTheta[i] * TMath::DegToRad()) + fDistTarget[i];
			}
			else if (fAnglePhi[i] == 90.)
			{ // Y-Foot (StripId numbered from bottom to top)
				x = fOffsetX[i];
				y = pos + fOffsetY[i];
				z = fDistTarget[i];
			}
			else if (fAnglePhi[i] == 180.)
			{ // X-Foot (StripId numbered from right to left)
				x = -pos * TMath::Cos(fAngleTheta[i] * TMath::DegToRad()) + fOffsetX[i];
				y = fOffsetY[i];
				z = -pos * TMath::Sin(fAngleTheta[i] * TMath::DegToRad()) + fDistTarget[i];
			}
			else if (fAnglePhi[i] == 270.)
			{ // Y-Foot (StripId numbered from top to bottom)
				x = fOffsetX[i];
				y = -pos + fOffsetY[i];
				z = fDistTarget[i];
			}
			else
			{
				LOG(info) << "R3BFootStripCal2Hit::AnglePhi is Wrong !";
			}

			TVector3 master(x, y, z);

			if(Eta[0][j]>0 && Eta[0][j]<1){
				if(fExpId==522){    
					if(Eta[i][j]<0.5){

						if(i==0) ClusterESum[i][j] = EtaCorr_Lagni(fZ3, fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], Eta[i][j])*2.9629+76.509;
						if(i==1) ClusterESum[i][j] = EtaCorr_Lagni(fZ3, fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], Eta[i][j])*3.0443+40.953;
						if(i==14) ClusterESum[i][j] = EtaCorr_Lagni(fZ3, fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], Eta[i][j])*3.1708+96.621;
						if(i==15) ClusterESum[i][j] = EtaCorr_Lagni(fZ3,fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], Eta[i][j])*2.8509+57.689;


					} else {

						if(i==0) ClusterESum[i][j] = EtaCorr_Lagni(fZ3, fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], 1.-Eta[i][j])*2.9629+76.509;     
						if(i==1) ClusterESum[i][j] = EtaCorr_Lagni(fZ3, fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], 1.-Eta[i][j])*3.0443+40.953;
						if(i==14) ClusterESum[i][j] = EtaCorr_Lagni(fZ3, fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], 1.-Eta[i][j])*3.1708+96.621;
						if(i==15) ClusterESum[i][j] = EtaCorr_Lagni(fZ3, fZ4, fZ5, fZ6, fZ7, fZ3NoOff, fZ4NoOff, fZ5NoOff, fZ6NoOff, fZ7NoOff, ClusterESum[i][j], 1.-Eta[i][j])*2.8509+57.689;

					}
				}


				if(fExpId==509){


					if(i==0) ClusterESum[i][j] = EtaCorr_Barriere(fZ4_0, fZ5_0, fZ6_0, fZ7_0, fZ8_0, fZ9_0, fZ4NoOff_0, fZ5NoOff_0, fZ6NoOff_0, fZ7NoOff_0, fZ8NoOff_0, fZ9NoOff_0, ClusterESum[i][j], Eta[i][j]);

					if(i==1) ClusterESum[i][j] = EtaCorr_Barriere(fZ4_1, fZ5_1, fZ6_1, fZ7_1, fZ8_1, fZ9_1, fZ4NoOff_1, fZ5NoOff_1, fZ6NoOff_1, fZ7NoOff_1, fZ8NoOff_1, fZ9NoOff_1, ClusterESum[i][j], Eta[i][j]);

					if(i==14) ClusterESum[i][j] = EtaCorr_Barriere(fZ4_14, fZ5_14, fZ6_14, fZ7_14, fZ8_14, fZ9_14, fZ4NoOff_14, fZ5NoOff_14, fZ6NoOff_14, fZ7NoOff_14, fZ8NoOff_14, fZ9NoOff_14, ClusterESum[i][j], Eta[i][j]);

					if(i==15) ClusterESum[i][j] = EtaCorr_Barriere(fZ4_15, fZ5_15, fZ6_15, fZ7_15, fZ8_15, fZ9_15, fZ4NoOff_15, fZ5NoOff_15, fZ6NoOff_15, fZ7NoOff_15, fZ8NoOff_15, fZ9NoOff_15, ClusterESum[i][j], Eta[i][j]);
				}
			}


			/*// Sort Cluster from Higher to Lower Energy after the eta correction step
			  for (Int_t i = 0; i < fMaxNumDet; i++)
			  {
			  for (Int_t j = 0; j < ClusterMult[i] - 1; j++)
			  {
			  for (Int_t k = j + 1; k < ClusterMult[i]; k++)
			  {
			  if (ClusterESum[i][j] < ClusterESum[i][k])
			  {

			  Int_t tempI = ClusterNStrip[i][j];
			  ClusterNStrip[i][j] = ClusterNStrip[i][k];
			  ClusterNStrip[i][k] = tempI;

			  Double_t tempD = ClusterESum[i][j];
			  ClusterESum[i][j] = ClusterESum[i][k];
			  ClusterESum[i][k] = tempD;

			  tempD = ClusterPos[i][j];
			  ClusterPos[i][j] = ClusterPos[i][k];
			  ClusterPos[i][k] = tempD;

			  tempD = Eta[i][j];
			  Eta[i][j] = Eta[i][k];
			  Eta[i][k] = tempD;
			  }
			  }
			  }
			  }*/   // we cannot use that directly -> AddHitData is working cluster by cluster  => sorting in analysis macros...

			if (ClusterESum[i][j] > fThSum && j < fMaxNumClusters)
			{
				AddHitData(i + 1, ClusterNStrip[i][j], pos, Eta[i][j], master, ClusterESum[i][j], ClusterMult[i]);
			}
		}
	}

	for (Int_t i = 0; i < fMaxNumDet; i++)
		hssd[i]->Reset();
	if (calData)
		delete[] calData;
	// delete ss;
	return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BFootStripCal2Hit::Reset()
{
	R3BLOG(debug, "Clearing HitData Structure");
	if (fFootHitData)
		fFootHitData->Clear();
}

// -----   Private method AddHitData  -------------------------------------------
R3BFootHitData* R3BFootStripCal2Hit::AddHitData(Int_t detid,
		Int_t numhit,
		Double_t s,
		Double_t eta,
		TVector3 master,
		Double_t energy_s,
		Int_t mulS)
{
	// It fills the R3BFootHitData
	TClonesArray& clref = *fFootHitData;
	Int_t size = clref.GetEntriesFast();
	return new (clref[size]) R3BFootHitData(detid, numhit, s, eta, master, energy_s, mulS);
}

ClassImp(R3BFootStripCal2Hit);
