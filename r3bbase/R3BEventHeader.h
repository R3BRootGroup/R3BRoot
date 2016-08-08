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

    inline void SetEventno(const UInt_t& eventno){   fEventno = eventno;    }
    inline void SetTrigger(const UInt_t& trigger){   fTrigger = trigger;    }
    inline void SetTimeStamp(const ULong_t& timeStamp)    {     fTimeStamp = timeStamp;    }

    inline const UInt_t& GetEventno() const    {       return fEventno;    }
    inline const UInt_t& GetTrigger() const    {       return fTrigger;    }
    inline const ULong_t& GetTimeStamp() const    {        return fTimeStamp;    }

  private:
    UInt_t fEventno;
    UInt_t fTrigger;
    ULong_t fTimeStamp;

  public:
    ClassDef(R3BEventHeader, 3)
};

#endif
