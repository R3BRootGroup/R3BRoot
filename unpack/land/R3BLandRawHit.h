// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            R3BLandRawHit                          -----
// -----                    Created 23.04.2013 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDRAWHIT_H
#define R3BLANDRAWHIT_H

#include "TObject.h"


class R3BLandRawHit : public TObject {
 public:
  R3BLandRawHit();
  R3BLandRawHit(Int_t padId, Double_t qdc1, Double_t qdc2);
  virtual ~R3BLandRawHit();

 protected:
  Int_t fPadId;
  Double32_t fQdc1;
  Double32_t fQdc2;

 public:
  inline Int_t    GetPadId() const { return fPadId; }
  inline Double_t GetQdc1()  const { return fQdc1;  }
  inline Double_t GetQdc2()  const { return fQdc2;  }

  ClassDef(R3BLandRawHit, 1)
};


#endif




