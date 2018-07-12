#include "R3BFi8Cal2Hit.h"

R3BFi8Cal2Hit::R3BFi8Cal2Hit(Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi8", a_verbose, VERTICAL, 2, 256, 1)
{
}

R3BFi8Cal2Hit::~R3BFi8Cal2Hit()
{
}

UInt_t R3BFi8Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (201 == a_fiber_id) {
//    a_fiber_id = 200;
//  }
//  if (200 == a_fiber_id) {
//    a_fiber_id = 201;
//  }
  return a_fiber_id;
}

ClassImp(R3BFi8Cal2Hit)
