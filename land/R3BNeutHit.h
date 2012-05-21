// ------------------------------------------------------------------------
// -----                        R3BNeutHit                            -----
// -----              Created 21.05.2012 by D.Kresan                  -----
// ------------------------------------------------------------------------

#ifndef R3BNEUTHIT_H
#define R3BNEUTHIT_H

#include "TObject.h"
#include "TMath.h"


class R3BNeutHit : public TObject {

 public:
  R3BNeutHit();
  R3BNeutHit(Double_t x, Double_t y, Double_t z, Double_t t);
  virtual ~R3BNeutHit();

 private:
  Double32_t fX;
  Double32_t fY;
  Double32_t fZ;
  Double32_t fT;

 public:
  inline Double_t GetX() const {  return fX;  }
  inline Double_t GetY() const {  return fY;  }
  inline Double_t GetZ() const {  return fZ;  }
  inline Double_t GetT() const {  return fT;  }
  inline Double_t GetD() const {  return TMath::Sqrt(fX*fX+fY*fY+fZ*fZ); }
  Double_t GetP() const;

  ClassDef(R3BNeutHit, 1)
};


#endif
