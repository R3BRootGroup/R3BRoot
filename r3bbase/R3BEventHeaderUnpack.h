// ----------------------------------------------------------------------
// -----                       R3BEventHeaderUnpack                 -----
// -----                  Created 02-06-2014 by D. Kresan           -----
// ----------------------------------------------------------------------

#ifndef R3BEVENTHEADERUNPACK
#define R3BEVENTHEADERUNPACK

#include "FairUnpack.h"

class R3BEventHeader;

class R3BEventHeaderUnpack : public FairUnpack
{
  public:
    R3BEventHeaderUnpack();
    virtual ~R3BEventHeaderUnpack();

    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual void Reset();

  private:
    R3BEventHeader* fHeader;
    
  protected:
    virtual void Register();

  public:
    ClassDef(R3BEventHeaderUnpack, 1)
};

#endif
