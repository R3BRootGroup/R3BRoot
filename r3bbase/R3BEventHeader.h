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
    
    inline void SetTimeStamp(const ULong_t& timeStamp)
    {
        fTimeStamp = timeStamp;
    }

    inline const Int_t& GetTrigger() const
    {
        return fTrigger;
    }
    
    inline const ULong_t& GetTimeStamp() const
    {
        return fTimeStamp;
    }

  private:
    Int_t fTrigger;
    ULong_t fTimeStamp;

  public:
    ClassDef(R3BEventHeader, 2)
};

#endif
