// -------------------------------------------------------------------------
// -----                      R3BPspDigi header file                  -----
// -----                  Created 28/06/11  by D.Bertini/Justyna               -----
// -------------------------------------------------------------------------


/**  R3BPspDigi.h
 **/


#ifndef R3BPSPDIGI_H
#define R3BPSPDIGI_H



#include "R3BHit.h"
#include "TVector3.h"



class R3BPspDigi : public R3BHit
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
  


 protected:

   Int_t Ps03mul;

  ClassDef(R3BPspDigi,2)


};

#endif
