#ifndef R3BFI11CAL2HIT
#define R3BFI11CAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi11Cal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi11Cal2Hit(Int_t = 1);
    virtual ~R3BFi11Cal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi11Cal2Hit, 1)
};

#endif
