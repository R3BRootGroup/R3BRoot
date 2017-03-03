// ------------------------------------------------------------
// -----                R3BFi5Mapped2Cal                -----
// -----            Created Jan 13th 2018 by M.Heil       -----
// ----- Convert mapped data to time calibrated data      -----
// ------------------------------------------------------------

#ifndef R3BFI5MAPPED2CAL
#define R3BFI5MAPPED2CAL

#include "R3BBunchedFiberMapped2Cal.h"

class R3BFi5Mapped2Cal: public R3BBunchedFiberMapped2Cal
{
  public:
    R3BFi5Mapped2Cal(Int_t = 1);
    virtual ~R3BFi5Mapped2Cal();

    ClassDef(R3BFi5Mapped2Cal, 1)
};

#endif
