#ifndef R3BFI0CAL2HIT
#define R3BFI0CAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi0Cal2Hit: public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi0Cal2Hit(Int_t = 1);
    virtual ~R3BFi0Cal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi0Cal2Hit, 1)
};

#endif
