#ifndef R3BNEUTRONTRACKER_H
#define R3BNEUTRONTRACKER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BLandDigiPar.h"
#include "R3BLandDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;
class R3BNeutronTrack;

struct PM_HIT
{
  Double_t paddle,tdcL,tdcR,tdc,qdcL,qdcR,qdc,x,y,z,x0,y0,z0,t0;
};
struct PRIM_PART
{
  Double_t pdg,px,py,pz,pt,p,x,y,z,t,A,M;
};
struct NEUT1_HIT
{
  Double_t x,y,z,t,px,py,pz,p;
};
struct NEUT2_HIT
{
  Double_t x,y,z,t,px,py,pz,p;
};

class R3BNeutronTracker : public FairTask
{

 public:

  /** Default constructor **/  
  R3BNeutronTracker();


  /** Destructor **/
  ~R3BNeutronTracker();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BNeutronTrack* AddHit(Int_t paddleNr, Double_t tdcR, Double_t tdcL,
        Double_t qdcR, Double_t qdcL );


  protected:
  // Reused structure from previous
  // Algorithm
  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 
  TClonesArray* fLandDigi;

  // Parameter class
  R3BLandDigiPar* fLandDigiPar;

  // Created structure from this
  // Algorithm
  TClonesArray* fNeutronTracks;

  // Add here control hist for tracker
  TH1F *hNeutmult1;
  TH1F *hNeutmult2;
  TH1F *hMinv;
  TH1F *hMinv0;
  TH1F *hDeltaPx1;
  TH1F *hDeltaPy1;
  TH1F *hDeltaPz1;
  TH1F *hDeltaPx2;
  TH1F *hDeltaPy2;
  TH1F *hDeltaPz2;

  Int_t npaddles;
  Int_t nplanes;

  Double_t plength; // half length of paddle
  Double_t att; // light attenuation factor [1/cm]
  Double_t amu; //atomic mass unit in MeV/c**2
  Double_t c;
  Double_t cMedia; // speed of light in material in cm/ns
  Double_t calFactor; //calibration factor energy of LAND paddles
  PM_HIT *PM_hit;
  PRIM_PART *PRIM_part;
  NEUT1_HIT *NEUT1_hit;
  NEUT2_HIT *NEUT2_hit;
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BNeutronTracker,1);
  
};

#endif
