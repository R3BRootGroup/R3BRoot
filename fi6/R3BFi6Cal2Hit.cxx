#include "R3BFi6Cal2Hit.h"

R3BFi6Cal2Hit::R3BFi6Cal2Hit(Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi6", a_verbose, VERTICAL, 2, 256, 2)
{
}

R3BFi6Cal2Hit::~R3BFi6Cal2Hit()
{
}

UInt_t R3BFi6Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (1 == a_fiber_id) a_fiber_id = 241;
 

  return a_fiber_id;
}

ClassImp(R3BFi6Cal2Hit)
