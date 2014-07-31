// ---------------------------------------------------------------------------------------
// -----                               R3BMfiUnpack                                  -----
// -----                      Created 30-07-2014 by D.Kresan                         -----
// ---------------------------------------------------------------------------------------

#ifndef R3BMFIUNPACK_H
#define R3BMFIUNPACK_H

#include "FairUnpack.h"

class TClonesArray;

class R3BMfiUnpack : public FairUnpack
{
  public:
    // Constructor
    R3BMfiUnpack(Short_t type = 97, Short_t subType = 9700, Short_t procId = 1, Short_t subCrate = 0, Short_t control = 9);

    // Destructor
    virtual ~R3BMfiUnpack();

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
    ClassDef(R3BMfiUnpack, 0)
};

#endif
