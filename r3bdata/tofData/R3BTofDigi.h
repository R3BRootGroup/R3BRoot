// -------------------------------------------------------------------------
// -----                      R3BTofDigi header file                  -----
// -----                  Created 29/06/11  by D.Bertini/Justyna               -----
// -------------------------------------------------------------------------


/**  R3BTofDigi.h
 **/


#ifndef R3BTOFDIGI_H
//#define R3BTOFDDIGI_H
#define R3BTOFDIGI_H

#include "TObject.h"
#include "TVector3.h"



class R3BTofDigi : public TObject
{

 public:

  /** Default constructor **/
  R3BTofDigi();
  R3BTofDigi(Int_t tfmul,Double_t tfwx_p1,Double_t tfwy_p1,Double_t tfwt_p1);
  
  /** Copy constructor **/
  R3BTofDigi(const R3BTofDigi& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BTofDigi();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  
  void Settfmul(Int_t mul){Tfmul = mul;}
  Double_t Gettfmul(){return Tfmul;}
  
  void Settfwx_p1(Double_t pos){Tfx_p1 = pos;}
  Double_t Gettfwx_p1(){return Tfx_p1;}
  
  void Settfwy_p1(Double_t pos){Tfy_p1 = pos;}
  Double_t Gettfwy_p1(){return Tfy_p1;}
  
  void Settfwt_p1(Double_t time){Tft_p1 = time;}
  Double_t Gettfwt_p1(){return Tft_p1;}


 protected:
 
   Int_t Tfmul;
   Double32_t Tfx_p1;
   Double32_t Tfy_p1;
   Double32_t Tft_p1;

  ClassDef(R3BTofDigi,1)

};

#endif
