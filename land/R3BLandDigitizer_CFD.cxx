#include "R3BLandDigitizer_CFD.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"


#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TH2F.h"
#include <string>
#include <iostream>
#include <stdlib.h>

#include "R3BMCTrack.h"		

using std::cout;
using std::endl;

struct R3BLandDigitizer_CFD_Paddle_Hit
{
	R3BLandDigitizer_CFD_Paddle_Hit* previousHit;
	Double_t time;
	Int_t segment;
	Double_t energyDepo;
	R3BLandPoint* hitData;

	R3BLandDigitizer_CFD_Paddle_Hit()
	{
		previousHit = NULL;
		time = 0.;
		segment=0;
		energyDepo = 0.;
		hitData = NULL;
	}
};

struct R3BLandDigitizer_CFD_Paddle_PM
{
	Int_t nrEvents;
	Int_t indexSortedStart;
	Double_t tdc;
	Double_t qdc;
	R3BLandDigitizer_CFD_Paddle_Hit* pulse;

	R3BLandDigitizer_CFD_Paddle_PM()
	{
		nrEvents = 0;
		indexSortedStart = -1;
		tdc = 0.;
		qdc = 0.;
		pulse = NULL;
	}
};

struct R3BLandDigitizer_CFD_Paddle
{
	int nrOfHits;
	R3BLandDigitizer_CFD_Paddle_PM* Left;
	R3BLandDigitizer_CFD_Paddle_PM* Right;

	R3BLandDigitizer_CFD_Paddle()
	{
		nrOfHits = 0;
		Left = new R3BLandDigitizer_CFD_Paddle_PM;
		Right = new R3BLandDigitizer_CFD_Paddle_PM;
	}
};		

R3BLandDigitizer_CFD::R3BLandDigitizer_CFD() :
  FairTask("R3B Land Digitization scheme ") { 
	iVerbose=1;
	cfd = new R3BConstantFraction();
}

R3BLandDigitizer_CFD::R3BLandDigitizer_CFD(Int_t _iVerbose) :
  FairTask("R3B Land Digitization scheme ") {
	iVerbose=_iVerbose;
	cfd = new R3BConstantFraction();
}

R3BLandDigitizer_CFD::~R3BLandDigitizer_CFD() {
}


void R3BLandDigitizer_CFD::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fLandDigiPar = (R3BLandDigiPar*)(rtdb->getContainer("R3BLandDigiPar"));

  if ( fLandDigiPar ) {
		cout << "-I- R3BLandDigitizer_CFD::SetParContainers() "<< endl;
		cout << "-I- Container R3BLandDigiPar  loaded " << endl;
  }
}


InitStatus R3BLandDigitizer_CFD::Init() {
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("LandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
	
  // Register output array LandDigi
  fLandDigi = new TClonesArray("R3BLandDigi",1000);
  ioman->Register("LandDigi", "Digital_response_in_Land", fLandDigi, kTRUE);

  // Parameter retrieval
  // Only after Init one retrieve the Digitization Parameters!
  nPaddles = fLandDigiPar->GetMaxPaddle();

  plength = 100.; // half length of paddle
	att = 0.008; // light attenuation factor [1/cm]
	v_eff = 16.; // Effective speed of light in scintillator [cm/ns]
	inv_v_eff = 1/v_eff; // Inverse effective speed of light in scintillator [ns/cm]

	//Set the parameters fo th CFD
	cfdPulseDefiningParameterStruct* cfdPulseParameters = new cfdPulseDefiningParameterStruct();

	//The curve parameters for the CFD
	cfdPulseParameters->x0=0; cfdPulseParameters->x1=1; cfdPulseParameters->x2=3.5; cfdPulseParameters->x3=6; cfdPulseParameters->x4=15;

	cfd->Init(cfdPulseParameters);

	eventID=0; // Might rather be read from other object

	qdcGate=200; //[ns]

	delete cfdPulseParameters;

  return kSUCCESS;
}


void R3BLandDigitizer_CFD::SetCfdParameters(double thresholdTrigger, double timeShift, double amplitudeScaling)
{
	if(iVerbose > 0)
		cout << "\nR3BLandDigitizer_CFD: Using Constant Fraction Discriminator...\n";

	cfd->SetParameters(thresholdTrigger, timeShift, amplitudeScaling);
}

void R3BLandDigitizer_CFD::SetLeParameters(double thresholdTrigger)
{
	if(iVerbose > 0)
		cout << "\nR3BLandDigitizer_CFD: Using Leading Edge Discriminator...\n";

	cfd->SetParameters(thresholdTrigger);
}

// -----	Public method Exec	--------------------------------------------
void R3BLandDigitizer_CFD::Exec(Option_t* opt)
{
	if (iVerbose > 0)
		cout << "Event: " << eventID << endl;

	Int_t nentries = fLandPoints->GetEntries();

	R3BLandDigitizer_CFD_Paddle* paddle = new R3BLandDigitizer_CFD_Paddle[nPaddles];
	R3BLandDigitizer_CFD_Paddle_Hit* paddleHits = new R3BLandDigitizer_CFD_Paddle_Hit[nentries*2];

	//Get entries from Land object and sort them into paddles
	for (Int_t iEntry=0; iEntry<nentries; iEntry++)
	{
		R3BLandPoint *land_obj = (R3BLandPoint*) fLandPoints->At(iEntry);

		Bool_t inScintilator = (int)(land_obj->GetPaddleType())==3;
		Bool_t energyLoss = land_obj->GetEnergyLoss()>0;

		if (inScintilator && energyLoss)
		{
			// Get the Land Object in array
			paddleHits[iEntry].hitData = land_obj;
			paddleHits[nentries+iEntry].hitData = land_obj;

			Int_t paddleNr = int(land_obj->GetSector()-1); //note that paddle starts at 1

			paddleHits[iEntry].previousHit = paddle[paddleNr].Left->pulse;
			paddle[paddleNr].Left->pulse = &paddleHits[iEntry];

			paddleHits[nentries+iEntry].previousHit = paddle[paddleNr].Right->pulse;
			paddle[paddleNr].Right->pulse = &paddleHits[nentries+iEntry];
			
			Double_t landHitTime = land_obj->GetTime();
			Double_t eloss = land_obj->GetEnergyLoss()*1000.; // [MeV]
			Double_t lightyield = land_obj->GetLightYield()*1000.;

			paddle[paddleNr].nrOfHits++;

			if(paddleNr > (nPaddles)/2)
			{
				// vertical paddles
				Double_t y = (land_obj->GetYIn() + land_obj->GetYOut()) * 0.5;
				paddle[paddleNr].Left->pulse->energyDepo = lightyield*exp(-att*(plength-y));
				paddle[paddleNr].Right->pulse->energyDepo = lightyield*exp(-att*(plength+y));
				paddle[paddleNr].Left->pulse->time = landHitTime + (plength - y) * inv_v_eff;
				paddle[paddleNr].Right->pulse->time = landHitTime + (plength + y) * inv_v_eff;
			}
			else
			{
				// horizontal paddles
				Double_t x = (land_obj->GetXIn() + land_obj->GetXOut()) * 0.5;
				paddle[paddleNr].Left->pulse->energyDepo = lightyield*exp(-att*(plength-x));
				paddle[paddleNr].Right->pulse->energyDepo = lightyield*exp(-att*(plength+x));
				paddle[paddleNr].Left->pulse->time = landHitTime + (plength - x) * inv_v_eff;
				paddle[paddleNr].Right->pulse->time = landHitTime + (plength + x) * inv_v_eff;
			}
		}
	}
	//Loop through the paddles and calculate QDC andt TDC
	for (Int_t iPaddles=0; iPaddles < nPaddles; iPaddles++)
	{
		int nrOfPaddleHits=paddle[iPaddles].nrOfHits;
		if (nrOfPaddleHits > 0)
		{
			double timeOfHitsLeft[nrOfPaddleHits];
			double timeOfHitsRight[nrOfPaddleHits];
			double energyOfHitsLeft[nrOfPaddleHits];
			double energyOfHitsRight[nrOfPaddleHits];

			R3BLandDigitizer_CFD_Paddle_Hit* hitPointerLeft = paddle[iPaddles].Left->pulse;
			R3BLandDigitizer_CFD_Paddle_Hit* hitPointerRight = paddle[iPaddles].Right->pulse;

			for (Int_t iHit=0; iHit < nrOfPaddleHits; iHit++)
			{
				timeOfHitsLeft[iHit] = hitPointerLeft->time;
				timeOfHitsRight[iHit] = hitPointerRight->time;
				energyOfHitsLeft[iHit] = hitPointerLeft->energyDepo;
				energyOfHitsRight[iHit] = hitPointerRight->energyDepo;

				hitPointerLeft = hitPointerLeft->previousHit;
				hitPointerRight = hitPointerRight->previousHit;
			}
			paddle[iPaddles].Left->tdc = cfd->Calculate(nrOfPaddleHits, timeOfHitsLeft, energyOfHitsLeft);
			paddle[iPaddles].Right->tdc = cfd->Calculate(nrOfPaddleHits, timeOfHitsRight, energyOfHitsRight);

			//Calculate QDC
			if (!TMath::IsNaN(paddle[iPaddles].Left->tdc) || !TMath::IsNaN(paddle[iPaddles].Right->tdc))
			{	
				Double_t tdcTime;
				if(TMath::IsNaN(paddle[iPaddles].Left->tdc))
					tdcTime=paddle[iPaddles].Right->tdc;
				else if(TMath::IsNaN(paddle[iPaddles].Right->tdc))
					tdcTime=paddle[iPaddles].Left->tdc;
				else
					tdcTime=(paddle[iPaddles].Left->tdc + paddle[iPaddles].Right->tdc)*0.5;

				for (Int_t iHit=0; iHit < nrOfPaddleHits; iHit++)
				{
					if (abs(timeOfHitsLeft[iHit]-tdcTime) < qdcGate)
						paddle[iPaddles].Left->qdc += energyOfHitsLeft[iHit];

					if (abs(timeOfHitsRight[iHit]-tdcTime) < qdcGate)
						paddle[iPaddles].Right->qdc += energyOfHitsRight[iHit];
				}

				//Save the data from the paddle
				AddHit( iPaddles, paddle[iPaddles].Right->tdc, paddle[iPaddles].Left->tdc, paddle[iPaddles].Right->qdc, paddle[iPaddles].Left->qdc );

				if (iVerbose > 1)
				{
					cout << "   Paddle number: " << iPaddles << endl;
					cout << "      QDC Left: " << paddle[iPaddles].Left->qdc << endl;
					cout << "      CFD Left: " << paddle[iPaddles].Left->tdc << endl;

					cout << "      QDC Right: " << paddle[iPaddles].Right->qdc << endl;
					cout << "      CFD Right: " << paddle[iPaddles].Right->tdc << endl << endl;
				}
			}
		}
	}
	eventID++;

	delete [] paddle;
	delete [] paddleHits;
}

/*************************************************************************/

void R3BLandDigitizer_CFD::FinishEvent()
{
	//Clear event data
	fLandDigi->Clear();
}

R3BLandDigi* R3BLandDigitizer_CFD::AddHit(Int_t paddleNr, Double_t tdcR, Double_t tdcL,
					  Double_t qdcR,Double_t qdcL){
  // It fills the R3BLandDigi array
  TClonesArray& clref = *fLandDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BLandDigi(paddleNr, tdcR, tdcL, qdcR, qdcL);
}

ClassImp(R3BLandDigitizer_CFD)
