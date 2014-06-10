// ----------------------------------------------------------------------
// -----                          R3BEventHeader                    -----
// -----                  Created 02-06-2014 by D. Kresan           -----
// ----------------------------------------------------------------------

#ifndef R3BEVENTHEADER
#define R3BEVENTHEADER

#include "FairEventHeader.h"

class R3BEventHeader : public FairEventHeader
{
  public:
    R3BEventHeader();
    virtual ~R3BEventHeader();

    inline void SetTrigger(const Int_t& trigger)
    {
        fTrigger = trigger;
    }

    inline const Int_t& GetTrigger() const
    {
        return fTrigger;
    }

  private:
    Int_t fTrigger;

  public:
    ClassDef(R3BEventHeader, 1)
};

#endif
