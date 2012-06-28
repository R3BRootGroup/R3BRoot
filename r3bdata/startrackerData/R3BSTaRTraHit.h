// -------------------------------------------------------------------------
// -----                      R3BSTaRTraHit header file                  -----
// -----                  Created 07/07/09  by M. Labiche               -----
// -------------------------------------------------------------------------



#ifndef R3BSTARTRAHIT_H
#define R3BSTARTRAHIT_H


#include "TObject.h"
#include "TVector3.h"

#include "FairHit.h"


class R3BSTaRTraHit : public FairHit
{

 public:

  /** Default constructor **/
  R3BSTaRTraHit();

  /** Copy constructor **/
  R3BSTaRTraHit(const R3BSTaRTraHit& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BSTaRTraHit();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


  void SetEnergy(Double_t en){fEnergy = en;}
  Double_t GetEnergy(){return fEnergy;}

  void SetToF(Double_t time){fTof = time;}
  Double_t GetToF(){return fTof;}

  void SetDetID(Int_t id){ fDetectorID = id; }
  Int_t GetDetID(){ return fDetectorID; }



 protected:

  Double32_t fEnergy;
  Double32_t fTof;
  
  ClassDef(R3BSTaRTraHit,1)

};

#endif
