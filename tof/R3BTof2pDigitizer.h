#ifndef R3BTOF2PDIGITIZER_H
#define R3BTOF2PDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BTof2pDigiPar.h"
#include "R3BTof2pDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BTof2pDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BTof2pDigitizer();


  /** Destructor **/
  ~R3BTof2pDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BTof2pDigi* AddHit(Int_t tfmul,Double_t tfwx_p1,Double_t tfwy_p1,Double_t tfwt_p1,
  Double_t tfwx_p2,Double_t tfwy_p2,Double_t tfwt_p2,Double_t tfwpath_p1,Double_t tfwpath_p2,
  //Double_t tfwpx_p1,Double_t tfwpy_p1,Double_t tfwpz_p1,Double_t tfwpx_p2,Double_t tfwpy_p2,Double_t tfwpz_p2);
  Double_t tfwpx_p1,Double_t tfwpy_p1,Double_t tfwpz_p1,Double_t tfwpx_p2,Double_t tfwpy_p2,Double_t tfwpz_p2,
  Double_t tfwe_p1,Double_t tfwe_p2);


  protected: 
  TClonesArray* fTof2pDigi;
  TClonesArray* fTofPoints;
  TClonesArray* fTofMCTrack;

  // Parameter class
  R3BTof2pDigiPar* fTof2pDigiPar;

  //- Control Hitograms

  
  TH1F * TOFelosshis;
  
  TH1F * TfwXhis;
  TH1F * TfwYhis;
  TH1F * TfwThis;
  
  Int_t eventNoTof;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BTof2pDigitizer,1);
  
};

#endif
