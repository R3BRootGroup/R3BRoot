
/******************************************/
/*                                        */
/*        Code for digitisation of        */
/*        detectors with CFD or LE        */
/*                                        */
/*   Hits within same paddle/readout are  */
/*   grouped together and sent to CFD/LE  */
/*      which gives time and energy       */
/*           for detected hit             */
/*                                        */
/*        Johan Gill & Staffan Wranne     */
/*               10/2 2011                */
/*                                        */
/******************************************/


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
#include <limits>

#include "R3BMCTrack.h"		

using std::cout;
using std::endl;




R3BLandDigitizer_CFD::R3BLandDigitizer_CFD(Int_t _iVerbose) :
  FairTask("R3B Land Digitization scheme ") {
	iVerbose=_iVerbose;
	cfd = new R3BConstantFraction();
}

R3BLandDigitizer_CFD::~R3BLandDigitizer_CFD() {
	delete [] paddles;
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
  nPlanes = fLandDigiPar->GetMaxPlane();
  nrPaddlePerPlane = nPaddles/nPlanes;

  plength = fLandDigiPar->GetPaddleLength(); // half length of paddles
	att = 0.008; // light attenuation factor [1/cm]
	v_eff = 16.; // Effective speed of light in scintillator [cm/ns]
	inv_v_eff = 1/v_eff; // Inverse effective speed of light in scintillator [ns/cm]

	//Set the parameters for the CFD
	cfdPulseDefiningParameterStruct cfdPulseParameters;

	//The curve parameters for the CFD
	cfdPulseParameters.x0=0; cfdPulseParameters.x1=1; cfdPulseParameters.x2=3.5; cfdPulseParameters.x3=6; cfdPulseParameters.x4=15;

	cfd->Init(&cfdPulseParameters);

	eventID=0; // Might rather be read from other object

	qdcGate=1000; //[ns]

	paddles = new R3BLandDigitizer_CFD_Paddle[nPaddles];

  return kSUCCESS;
}


void R3BLandDigitizer_CFD::SetCfdParameters(double threshold, double delay, double fraction)
{
	if(iVerbose > 0)
		cout << "\n<I> R3BLandDigitizer_CFD: Using Constant Fraction Discriminator...\n";

  double attComp = 0.525; //compensation for energy loss due to attenuation and Birks law
                         //Calibrated with muons in 2m paddles

	cfd->SetParameters(threshold*attComp, delay, fraction);
}

void R3BLandDigitizer_CFD::SetLeParameters(double threshold)
{
	if(iVerbose > 0)
		cout << "\n<I> R3BLandDigitizer_CFD: Using Leading Edge Discriminator...\n";

  double attComp = 0.525; //compensation for energy loss due to attenuation and Birks law
                         //Calibrated with muons in 2m paddles

	cfd->SetParameters(threshold*attComp);
}


// -----	Public method Exec	--------------------------------------------
void R3BLandDigitizer_CFD::Exec(Option_t* opt)
{
	if (iVerbose > 0)
		cout << "Event: " << eventID << endl;

	Int_t nentries = fLandPoints->GetEntries();

//	R3BLandDigitizer_CFD_Paddle_Hit* paddleHits = new R3BLandDigitizer_CFD_Paddle_Hit[nentries*2];
	paddleHits.reserve(nentries*2);

	//Get entries from Land object and sort them into paddles
	for (Int_t iEntry=0; iEntry<nentries; iEntry++)
	{
		R3BLandPoint *land_obj = (R3BLandPoint*) fLandPoints->At(iEntry);

		Bool_t inScintilator = (int)(land_obj->GetPaddleType())==3;
		Bool_t energyLoss = land_obj->GetEnergyLoss()>0;

		if (inScintilator && energyLoss)
		{
			// Get the Land Object in array
			paddleHits[iEntry].hitData = land_obj;            //Left paddles
			paddleHits[nentries+iEntry].hitData = land_obj;   //Right paddles

			Int_t paddleNr = int(land_obj->GetSector()-1); //note that paddles starts at 1

			R3BLandDigitizer_CFD_Paddle* activePaddle = &paddles[paddleNr];

			paddleHits[iEntry].previousHit = activePaddle->Left->pulse;
			activePaddle->Left->pulse = &paddleHits[iEntry];

			paddleHits[nentries+iEntry].previousHit = activePaddle->Right->pulse;
			activePaddle->Right->pulse = &paddleHits[nentries+iEntry];
			
			Double_t landHitTime = land_obj->GetTime();
//			Double_t eloss = land_obj->GetEnergyLoss()*1000.; // [MeV]
			Double_t lightyield = land_obj->GetLightYield()*1000.;

			activePaddle->nrOfHits++;

			Double_t pos;

			if((paddleNr/nrPaddlePerPlane)%2 == 1)
			{
				// vertical paddles
				pos = (land_obj->GetYIn() + land_obj->GetYOut()) * 0.5;
				activePaddle->Left->pulse->energyDepo = lightyield*exp(-att*(plength-pos));
				activePaddle->Right->pulse->energyDepo = lightyield*exp(-att*(plength+pos));
				activePaddle->Left->pulse->time = landHitTime + (plength - pos) * inv_v_eff;
				activePaddle->Right->pulse->time = landHitTime + (plength + pos) * inv_v_eff;
			}
			else
			{
				// horizontal paddles
				pos = (land_obj->GetXIn() + land_obj->GetXOut()) * 0.5;
				activePaddle->Left->pulse->energyDepo = lightyield*exp(-att*(plength-pos));
				activePaddle->Right->pulse->energyDepo = lightyield*exp(-att*(plength+pos));
				activePaddle->Left->pulse->time = landHitTime + (plength - pos) * inv_v_eff;
				activePaddle->Right->pulse->time = landHitTime + (plength + pos) * inv_v_eff;
			}
		}
	}

	//Loop through the paddles and calculate QDC andt TDC
	for (Int_t iPaddles=0; iPaddles < nPaddles; iPaddles++)
	{
		int nrOfPaddleHits=paddles[iPaddles].nrOfHits;
		if (nrOfPaddleHits > 0)
		{
			timeOfHitsLeft.reserve(nrOfPaddleHits);
			timeOfHitsRight.reserve(nrOfPaddleHits);
			energyOfHitsLeft.reserve(nrOfPaddleHits);
			energyOfHitsRight.reserve(nrOfPaddleHits);

			R3BLandDigitizer_CFD_Paddle_Hit* hitPointerLeft = paddles[iPaddles].Left->pulse;
			R3BLandDigitizer_CFD_Paddle_Hit* hitPointerRight = paddles[iPaddles].Right->pulse;

			for (Int_t iHit=0; iHit < nrOfPaddleHits; iHit++)
			{
				timeOfHitsLeft[iHit] = hitPointerLeft->time;
				timeOfHitsRight[iHit] = hitPointerRight->time;
				energyOfHitsLeft[iHit] = hitPointerLeft->energyDepo;
				energyOfHitsRight[iHit] = hitPointerRight->energyDepo;

				hitPointerLeft = hitPointerLeft->previousHit;
				hitPointerRight = hitPointerRight->previousHit;
			}
			paddles[iPaddles].Left->tdc = cfd->Calculate(nrOfPaddleHits, &timeOfHitsLeft[0], &energyOfHitsLeft[0]);
			paddles[iPaddles].Right->tdc = cfd->Calculate(nrOfPaddleHits, &timeOfHitsRight[0], &energyOfHitsRight[0]);

      if (paddles[iPaddles].Left->tdc > qdcGate)
        paddles[iPaddles].Left->tdc = std::numeric_limits<double>::quiet_NaN();
      if (paddles[iPaddles].Right->tdc > qdcGate)
        paddles[iPaddles].Right->tdc = std::numeric_limits<double>::quiet_NaN();
      
			//Calculate QDC
			if (!TMath::IsNaN(paddles[iPaddles].Left->tdc) || !TMath::IsNaN(paddles[iPaddles].Right->tdc))
			{	
				for (Int_t iHit=0; iHit < nrOfPaddleHits; iHit++)
				{
					if (timeOfHitsLeft[iHit] < qdcGate)
						paddles[iPaddles].Left->qdc += energyOfHitsLeft[iHit];

					if (timeOfHitsRight[iHit] < qdcGate)
						paddles[iPaddles].Right->qdc += energyOfHitsRight[iHit];
				}

        //Exact data of the hit
//        double x0 = (paddles[iPaddles].Right->pulse->hitData->GetXIn()
//                     + paddles[iPaddles].Right->pulse->hitData->GetXOut())/2;
//        double y0 = (paddles[iPaddles].Right->pulse->hitData->GetYIn() 
//                     + paddles[iPaddles].Right->pulse->hitData->GetYOut())/2;
//        double z0 = (paddles[iPaddles].Right->pulse->hitData->GetZIn() 
//                     + paddles[iPaddles].Right->pulse->hitData->GetZOut())/2;
//        double t0 = paddles[iPaddles].Right->pulse->hitData->GetTime();
        
        //Save the data from the paddles
        AddHit( iPaddles, paddles[iPaddles].Right->tdc, paddles[iPaddles].Left->tdc, paddles[iPaddles].Right->qdc, paddles[iPaddles].Left->qdc);
        
        if (iVerbose > 1)
        {
          cout << "   Paddle number: " << iPaddles << endl;
          cout << "      QDC Left: " << paddles[iPaddles].Left->qdc << endl;
          cout << "      CFD Left: " << paddles[iPaddles].Left->tdc << endl;
          
          cout << "      QDC Right: " << paddles[iPaddles].Right->qdc << endl;
          cout << "      CFD Right: " << paddles[iPaddles].Right->tdc << endl << endl;
        }
			}
		}
	}
	eventID++;
}

/*************************************************************************/

void R3BLandDigitizer_CFD::FinishEvent()
{
	//Clear event data
	fLandDigi->Clear();

	for(Int_t i=0; i < nPaddles; i++)
	{
		paddles[i].nrOfHits = 0;

		paddles[i].Left->nrEvents = 0;
		paddles[i].Left->indexSortedStart = -1;
		paddles[i].Left->tdc = 0.;
		paddles[i].Left->qdc = 0.;
		paddles[i].Left->pulse = NULL;

		paddles[i].Right->nrEvents = 0;
		paddles[i].Right->indexSortedStart = -1;
		paddles[i].Right->tdc = 0.;
		paddles[i].Right->qdc = 0.;
		paddles[i].Right->pulse = NULL;

	}
}


R3BLandDigi* R3BLandDigitizer_CFD::AddHit(Int_t paddleNr, Double_t tdcR, Double_t tdcL,
					  Double_t qdcR,Double_t qdcL ){
  // It fills the R3BLandDigi array
  TClonesArray& clref = *fLandDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BLandDigi(paddleNr, tdcR, tdcL, qdcR, qdcL); 
}

ClassImp(R3BLandDigitizer_CFD)
