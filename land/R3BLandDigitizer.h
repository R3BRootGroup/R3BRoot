#ifndef R3BLANDDIGITIZER_H
#define R3BLANDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BLandDigiPar.h"
#include "R3BLandDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

struct PM_RES
{
  Double_t Ltime,Rtime,LlightCFD,RlightCFD,LlightQDC,RlightQDC,Lenergy,Renergy;
};


class R3BLandDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BLandDigitizer();


  /** Constructor **/  
  R3BLandDigitizer(Int_t verbose);


  /** Destructor **/
  ~R3BLandDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();

  inline void UseBeam(const Double_t& beamEnergy) {  fBeamEnergy = beamEnergy;  }


  R3BLandDigi* AddHit(Int_t paddleNr, Double_t tdcL, Double_t tdcR, Double_t tdc,
     Double_t qdcL, Double_t qdcR, Double_t qdc, Double_t xx,Double_t yy, Double_t zz );

  protected:
  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 
  TClonesArray* fLandDigi;

  // Parameter class
  R3BLandDigiPar* fLandDigiPar;

  //- Control Hitograms
  TH1F *hPMl;
  TH1F *hPMr;
  TH1F *hTotalLight;
  TH1F *hTotalLight1;
  TH1F *hTotalEnergy;
  TH1F *hTotalEnergy1;
  TH1F *hTotalLightRel;
  TH1F *hTotalEnergyRel;
  TH1F *hTotalLightRel2;
  TH1F *hMult1;
  TH1F *hMult2;
  TH1F *hParticle;
  TH1F *hPaddleEnergy;
  TH1F *hFirstEnergy;
  
  TH2F *hElossLight;
 
  Int_t eventNo;
  Int_t npaddles;
  Int_t nplanes;
  Int_t paddle_per_plane;
  Double_t plength; // half length of paddle
  Double_t att; // light attenuation factor [1/cm]
  Double_t mn; // mass of neutron in MeV/c**2
  Double_t mnu; // mass of neutron in atomic mass units
  Double_t amu; //atomic mass unit in MeV/c**2
  Double_t c;
  Double_t cMedia; // speed of light in material in cm/ns
  Double_t calFactor; //calibration factor energy of LAND paddles
  Double_t fBeamEnergy;
  PM_RES **PM_res;
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BLandDigitizer,1);
  
};

#endif
