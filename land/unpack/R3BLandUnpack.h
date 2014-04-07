// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandUnpack                                     -----
// -----                           Version 0.1                                       -----
// -----         Adapted by M.I. Cherciu @ 01.2014 after Y.Gonzalez code             -----
// -----                                                                             -----
// -----        * modification @ 03.2014: added TClonesArray for R3BLandHit          -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLANDUNPACK_H
#define R3BLANDUNPACK_H

#include "FairUnpack.h"

class TClonesArray;
class TH1F;

class R3BLandUnpack : public FairUnpack
{
  public:
    // Constructor
    R3BLandUnpack(char* strCalDir, Short_t type = 94, Short_t subType = 9400, Short_t procId = 10, Short_t subCrate = 1, Short_t control = 3);

    // Destructor
    virtual ~R3BLandUnpack();

    // Fair specific
    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual void Reset();

  protected:
    virtual void Register();

  private:
    TClonesArray* fRawData;
    Int_t fNHits;

  public:
    // Class definition
    ClassDef(R3BLandUnpack, 0)
};

#endif
