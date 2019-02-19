#ifndef R3BFI13CAL2HIT
#define R3BFI13CAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi13Cal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi13Cal2Hit(Bool_t, Int_t = 1);
    virtual ~R3BFi13Cal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi13Cal2Hit, 1)
};

#endif
