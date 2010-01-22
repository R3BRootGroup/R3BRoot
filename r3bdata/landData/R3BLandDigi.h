// -------------------------------------------------------------------------
// -----                      R3BLandDigi header file                  -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------


/**  R3BLandDigi.h
 **/


#ifndef R3BLANDDIGI_H
#define R3BLANDDIGI_H


#include "TObject.h"
#include "TVector3.h"



class R3BLandDigi : public TObject
{

 public:

  /** Default constructor **/
  R3BLandDigi();
  R3BLandDigi(Int_t paddle, Double_t tdcR, Double_t tdcL,
	      Double_t qdcR, Double_t qdcL);

  /** Copy constructor **/
  R3BLandDigi(const R3BLandDigi& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BLandDigi();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;



  void SetTdcR(Double_t time){fTdcR = time;}
  Double_t GetTdcR(){return fTdcR;}

  void SetTdcL(Double_t time){fTdcL = time;}
  Double_t GetTdcL(){return fTdcL;}

  void SetQdcR(Double_t charge){fQdcR = charge;}
  Double_t GetQdcR(){return fQdcR;}

  void SetQdcL(Double_t charge){fQdcL = charge;}
  Double_t GetQdcL(){return fQdcL;}

  void SetPaddleNr(Int_t paddleNb){fPaddleNb = paddleNb;}
  Int_t GetPaddleNr(){return fPaddleNb;}



 protected:

  Int_t fPaddleNb;
  Double32_t fTdcL;
  Double32_t fTdcR;
  Double32_t fQdcL;
  Double32_t fQdcR;

  ClassDef(R3BLandDigi,1)

};

#endif
