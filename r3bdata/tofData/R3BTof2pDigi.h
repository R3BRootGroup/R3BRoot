// -------------------------------------------------------------------------
// -----                      R3BTofDigi header file                  -----
// -----                  Created 29/06/11  by D.Bertini/Justyna               -----
// -------------------------------------------------------------------------


/**  R3BTofDigi.h
 **/


#ifndef R3BTOF2PDIGI_H
#define R3BTOF2PDDIGI_H


#include "TObject.h"
#include "TVector3.h"



class R3BTof2pDigi : public TObject
{

 public:

  /** Default constructor **/
  R3BTof2pDigi();
  R3BTof2pDigi(Int_t tfmul,Double_t tfwx_p1,Double_t tfwy_p1,Double_t tfwt_p1,
  Double_t tfwx_p2,Double_t tfwy_p2,Double_t tfwt_p2,Double_t tfwpath_p1,Double_t tfwpath_p2);
  
  /** Copy constructor **/
  R3BTof2pDigi(const R3BTof2pDigi& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BTof2pDigi();

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
  
  void Settfwpath_p1(Double_t length){Tfpath_p1 = length;}
  Double_t Gettfwpath_p1(){return Tfpath_p1;}
  
  
  
  void Settfwx_p2(Double_t pos){Tfx_p2 = pos;}
  Double_t Gettfwx_p2(){return Tfx_p2;}
  
  void Settfwy_p2(Double_t pos){Tfy_p2 = pos;}
  Double_t Gettfwy_p2(){return Tfy_p2;}
  
  void Settfwt_p2(Double_t time){Tft_p2 = time;}
  Double_t Gettfwt_p2(){return Tft_p2;} 
  
  void Settfwpath_p2(Double_t length){Tfpath_p2 = length;}
  Double_t Gettfwpath_p2(){return Tfpath_p2;} 


 protected:
 
   Int_t Tfmul;
   Double32_t Tfx_p1;
   Double32_t Tfy_p1;
   Double32_t Tft_p1;
   Double32_t Tfpath_p1;
   Double32_t Tfx_p2;
   Double32_t Tfy_p2;
   Double32_t Tft_p2;
   Double32_t Tfpath_p2;

  ClassDef(R3BTof2pDigi,1)

};

#endif
