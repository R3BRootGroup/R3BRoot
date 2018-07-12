#ifndef R3BFI1ACAL2HIT
#define R3BFI1ACAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi1aCal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi1aCal2Hit(Direction, Int_t = 1);
    virtual ~R3BFi1aCal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi1aCal2Hit, 1)
};

#endif
