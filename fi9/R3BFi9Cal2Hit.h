#ifndef R3BFI9CAL2HIT
#define R3BFI9CAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi9Cal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi9Cal2Hit(Int_t = 1);
    virtual ~R3BFi9Cal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi9Cal2Hit, 1)
};

#endif
