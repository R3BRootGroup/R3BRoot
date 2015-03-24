// ----------------------------------------------------------------------
// -----                         R3BTimeStampUnpack                 -----
// -----                  Created 26-02-2015 by D. Kresan           -----
// ----------------------------------------------------------------------

#ifndef R3BTIMESTAMPUNPACK
#define R3BTIMESTAMPUNPACK

#include "FairUnpack.h"

class R3BEventHeader;

class R3BTimeStampUnpack : public FairUnpack
{
  public:
    R3BTimeStampUnpack(Short_t type, Short_t subType, Short_t procId, Short_t subCrate, Short_t control);
    virtual ~R3BTimeStampUnpack();

    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual void Reset();
    
  protected:
    virtual void Register() {}

  private:
    R3BEventHeader* fHeader;

  public:
    ClassDef(R3BTimeStampUnpack, 1)
};

#endif
