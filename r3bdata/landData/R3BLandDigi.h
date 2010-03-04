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
  R3BLandDigi(Int_t paddle, Double_t tdcL, Double_t tdcR,
   Double_t tdc, Double_t qdcL, Double_t qdcR, Double_t qdc,
   Double_t xx, Double_t yy, Double_t zz,
   Double_t x0, Double_t y0, Double_t z0, Double_t T0);

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
  
  void SetTdc(Double_t time){fTdc = time;}
  Double_t GetTdc(){return fTdc;}

  void SetQdcR(Double_t charge){fQdcR = charge;}
  Double_t GetQdcR(){return fQdcR;}

  void SetQdcL(Double_t charge){fQdcL = charge;}
  Double_t GetQdcL(){return fQdcL;}

  void SetQdc(Double_t charge){fQdc = charge;}
  Double_t GetQdc(){return fQdc;}

  void SetXX(Double_t pos){fxx = pos;}
  Double_t GetXX(){return fxx;}

  void SetYY(Double_t pos){fyy = pos;}
  Double_t GetYY(){return fyy;}

  void SetZZ(Double_t pos){fzz = pos;}
  Double_t GetZZ(){return fzz;}

  void SetPaddleNr(Int_t paddleNb){fPaddleNb = paddleNb;}
  Int_t GetPaddleNr(){return fPaddleNb;}

  void SetX0(Double_t pos){fx0 = pos;}
  Double_t GetX0(){return fx0;}

  void SetY0(Double_t pos){fy0 = pos;}
  Double_t GetY0(){return fy0;}

  void SetZ0(Double_t pos){fz0 = pos;}
  Double_t GetZ0(){return fz0;}

  void SetT0(Double_t time){fT0 = time;}
  Double_t GetT0(){return fT0;}


 protected:

  Int_t fPaddleNb;
  Double32_t fTdcL;
  Double32_t fTdcR;
  Double32_t fTdc;
  Double32_t fQdcL;
  Double32_t fQdcR;
  Double32_t fQdc;
  Double32_t fxx;
  Double32_t fyy;
  Double32_t fzz;
  Double32_t fx0;
  Double32_t fy0;
  Double32_t fz0;
  Double32_t fT0;

  ClassDef(R3BLandDigi,1)

};

#endif
