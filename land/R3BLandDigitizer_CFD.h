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

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;


class R3BLandDigitizer_CFD : public FairTask
{

 public:

  /** Default constructor **/  
  R3BLandDigitizer_CFD();
  R3BLandDigitizer_CFD(Int_t _iVerbose);


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
		     Double_t qdcR,Double_t qdcL );


  protected:
  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 
  TClonesArray* fLandDigi;

  // Parameter class
  R3BLandDigiPar* fLandDigiPar;

  Int_t nPaddles;

  Double_t plength; 	// half length of paddle
  Double_t att; 			// light attenuation factor [1/cm]
  Double_t v_eff;			// Effective speed of light in scintillator [cm/ns]
  Double_t inv_v_eff;	// Inverse effective speed of light in scintillator [ns/cm]
	Double_t qdcGate;
  
  private:
  virtual void SetParContainers();
	Int_t	iVerbose;
	Int_t eventID;

	R3BConstantFraction* cfd;

 
  ClassDef(R3BLandDigitizer_CFD,1);
  
};

#endif
