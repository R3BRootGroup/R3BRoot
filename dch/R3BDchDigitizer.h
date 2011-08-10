#ifndef R3BDCHDIGITIZER_H
#define R3BDCHDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BDchDigiPar.h"
#include "R3BDchDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BDchDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BDchDigitizer();


  /** Destructor **/
  ~R3BDchDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BDchDigi* AddHit(Int_t pd1mul,Double_t Pdx1_p1,Double_t Pdy1_p1,Int_t pd2mul,Double_t Pdx2_p1,Double_t Pdy2_p1);


  protected:
  TClonesArray* fDchPoints;
  TClonesArray* fDchMCTrack; 
  TClonesArray* fDchDigi;

  // Parameter class
  R3BDchDigiPar* fDchDigiPar;

  //- Control Hitograms
  TH1F * DCH1Px;
  TH1F * DCH2Px;
  
  TH1F * DCH1Py;
  TH1F * DCH2Py;
  
  TH1F * DCH1Pz;
  TH1F * DCH2Pz;
  
  TH1F * DCH1X;
  TH1F * DCH2X;
  
  TH1F * DCH1Y;
  TH1F * DCH2Y;
  
  TH1F * DCH1elosshis;
  TH1F * DCH2elosshis;
  
  TH1F * TrackPx;
  TH1F * TrackPy;
  TH1F * TrackPz;
  
  TH2F * TrackPxVSDCH1Px;
  
  Int_t eventNoDch;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BDchDigitizer,1);
  
};

#endif
