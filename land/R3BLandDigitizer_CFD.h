#ifndef R3BLandDigitizer_CFD_H
#define R3BLandDigitizer_CFD_H 1

#include "FairTask.h"
#include <map>
#include <string>
#include "R3BLandDigiPar.h"
#include "R3BLandDigi.h"
#include "R3BLandPoint.h"
#include "R3BConstantFraction.h"
#include "TStopwatch.h"
#include <vector>

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;


class R3BLandDigitizer_CFD : public FairTask
{

 public:

  /** Default constructor **/  
  R3BLandDigitizer_CFD(Int_t _iVerbose=1);


  /** Destructor **/
  ~R3BLandDigitizer_CFD();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

	virtual void FinishEvent();

	void SetCfdParameters(double thresholdTrigger, double timeShift, double amplitudeScaling);
	void SetLeParameters(double thresholdTrigger);


  R3BLandDigi* AddHit(Int_t paddleNr, Double_t tdcR, Double_t tdcL,
		     Double_t qdcR,Double_t qdcL, 
				 double x0, double y0, double z0, double t0 );


  protected:
  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 
  TClonesArray* fLandDigi;

  // Parameter class
  R3BLandDigiPar* fLandDigiPar;

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

  Int_t nPaddles;
  Int_t nPlanes;
  Int_t nrPaddlePerPlane;
	R3BLandDigitizer_CFD_Paddle* paddles;
	std::vector<R3BLandDigitizer_CFD_Paddle_Hit> paddleHits;

  Double_t plength; 	// half length of paddle
  Double_t att; 			// light attenuation factor [1/cm]
  Double_t v_eff;			// Effective speed of light in scintillator [cm/ns]
  Double_t inv_v_eff;	// Inverse effective speed of light in scintillator [ns/cm]
	Double_t qdcGate;

	std::vector<double> timeOfHitsLeft;
	std::vector<double> timeOfHitsRight;
	std::vector<double> energyOfHitsLeft;
	std::vector<double> energyOfHitsRight;
  
  private:
  virtual void SetParContainers();
	Int_t	iVerbose;
	Int_t eventID;

	R3BConstantFraction* cfd;

 
  ClassDef(R3BLandDigitizer_CFD,1);
  
};

#endif
