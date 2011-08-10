// -------------------------------------------------------------------------
// -----                      R3BTargetDigi header file                  -----
// -----                  Created 30/06/11  by D.Bertini/Justyna               -----
// -------------------------------------------------------------------------


/**  R3BTargetDigi.h
 **/


#ifndef R3BTARGETDIGI_H
#define R3BTRADDIGI_H


#include "TObject.h"
#include "TVector3.h"



class R3BTargetDigi : public TObject
{

 public:

  /** Default constructor **/
  R3BTargetDigi();
  R3BTargetDigi(Double_t ss03_smul,Double_t ss03_kmul,Double_t ss06_smul,Double_t ss06_kmul,
  Double_t x0,Double_t y0,Double_t t0,Double_t estar);
  
  /** Copy constructor **/
  R3BTargetDigi(const R3BTargetDigi& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BTargetDigi();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;
  
  
  
  void Setss03_smul(Double_t mul){Ss03_smul = mul;}
  Double_t Getss03_smul(){return Ss03_smul;}
  
  void Setss03_kmul(Double_t mul){Ss03_kmul = mul;}
  Double_t Getss03_kmul(){return Ss03_kmul;}
  
  void Setss06_smul(Double_t mul){Ss06_smul = mul;}
  Double_t Getss06_smul(){return Ss06_smul;}
  
  void Setss06_kmul(Double_t mul){Ss06_kmul = mul;}
  Double_t Getss06_kmul(){return Ss06_kmul;}    
  
  
  
  void Setx0(Double_t pos){X0 = pos;}
  Double_t Getx0(){return X0;}
  
  void Sety0(Double_t pos){Y0 = pos;}
  Double_t Gety0(){return Y0;}
  
  void Sett0(Double_t time){T0 = time;}
  Double_t Gett0(){return T0;}
  
  
  
  void Setestar(Double_t energy){Estar = energy;}
  Double_t Getestar(){return Estar;}

    

 protected:

     
     Double32_t Ss03_smul;
     Double32_t Ss03_kmul;
     Double32_t Ss06_smul;
     Double32_t Ss06_kmul;
     
     Double32_t X0;
     Double32_t Y0;
     Double32_t T0;
     
     Double32_t Estar;
     
     
  

  ClassDef(R3BTargetDigi,1)

};

#endif
