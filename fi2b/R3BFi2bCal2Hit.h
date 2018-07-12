#ifndef R3BFI2BCAL2HIT
#define R3BFI2BCAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi2bCal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi2bCal2Hit(Direction, Int_t = 1);
    virtual ~R3BFi2bCal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi2bCal2Hit, 1)
};

#endif
