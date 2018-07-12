#ifndef R3BFI1BCAL2HIT
#define R3BFI1BCAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi1bCal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi1bCal2Hit(Direction, Int_t = 1);
    virtual ~R3BFi1bCal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi1bCal2Hit, 1)
};

#endif
