// -------------------------------------------------------------------------
// -----                      R3BPspDigi header file                  -----
// -----                  Created 28/06/11  by D.Bertini/Justyna               -----
// -------------------------------------------------------------------------


/**  R3BPspDigi.h
 **/


#ifndef R3BPSPDIGI_H
#define R3BPSPDIGI_H



#include "TObject.h"
#include "TVector3.h"



class R3BPspDigi : public TObject
{

 public:

  /** Default constructor **/
  R3BPspDigi();
  R3BPspDigi(Int_t psp3mul,Double_t psp3x,Double_t psp3y,Double_t psp3e);
  
  /** Copy constructor **/
  R3BPspDigi(const R3BPspDigi& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BPspDigi();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


  void SetPsp3mul(Int_t mul){Ps03mul = mul;}
  Double_t GetPsp3mul(){return Ps03mul;}
  
  void SetPsp3x(Double_t pos){Ps03x = pos;}
  Double_t GetPsp3x(){return Ps03x;}
  
  void SetPsp3y(Double_t pos){Ps03y = pos;}
  Double_t GetPsp3y(){return Ps03y;}
  
  
  
  void SetPsp3e(Double_t en){Ps03e = en;}
  Double_t GetPsp3e(){return Ps03e;}
  


 protected:

   Int_t Ps03mul;
   Double32_t Ps03x;
   Double32_t Ps03y;
   Double32_t Ps03e;

  ClassDef(R3BPspDigi,1)


};

#endif
