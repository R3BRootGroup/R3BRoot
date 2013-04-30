// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BLandUnpack                           -----
// -----                    Created 12.04.2013 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDUNPACK_H
#define R3BLANDUNPACK_H

#include "FairUnpack.h"


class TLANDEvent;
class TRootLANDEvent;
class TClonesArray;


class R3BLandUnpack : public FairUnpack {
 public:
  R3BLandUnpack(char *strCalDir,
		Int_t type = 94, Int_t subType = 9400);
  virtual ~R3BLandUnpack();

  virtual Bool_t Init();
  virtual Bool_t DoUnpack(Int_t *data, Int_t size);
  virtual void Reset();

 private:
  TLANDEvent *fLand;
  TRootLANDEvent *fRootLand;
  TClonesArray *fRawData;

 protected:
  virtual void Register();

 public:
  ClassDef(R3BLandUnpack, 0)
};


#endif


