#include "R3BFi3aCal2Hit.h"

R3BFi3aCal2Hit::R3BFi3aCal2Hit(Direction a_direction, Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi3a", a_verbose, a_direction, 1, 256, 1)
{
}

R3BFi3aCal2Hit::~R3BFi3aCal2Hit()
{
}

UInt_t R3BFi3aCal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (201 == a_fiber_id) {
//    a_fiber_id = 200;
//  }
//  if (200 == a_fiber_id) {
//    a_fiber_id = 201;
//  }
  return a_fiber_id;
}

ClassImp(R3BFi3aCal2Hit)
