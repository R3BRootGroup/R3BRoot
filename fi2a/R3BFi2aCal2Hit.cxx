#include "R3BFi2aCal2Hit.h"

R3BFi2aCal2Hit::R3BFi2aCal2Hit(Direction a_direction, Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi2a", a_verbose, a_direction, 1, 256, 1)
{
}

R3BFi2aCal2Hit::~R3BFi2aCal2Hit()
{
}

UInt_t R3BFi2aCal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (201 == a_fiber_id) {
//    a_fiber_id = 200;
//  }
//  if (200 == a_fiber_id) {
//    a_fiber_id = 201;
//  }
  return a_fiber_id;
}

ClassImp(R3BFi2aCal2Hit)
