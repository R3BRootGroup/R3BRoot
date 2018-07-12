#ifndef R3BFI3BCAL2HIT
#define R3BFI3BCAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi3bCal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi3bCal2Hit(Direction, Int_t = 1);
    virtual ~R3BFi3bCal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi3bCal2Hit, 1)
};

#endif
