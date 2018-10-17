#include "R3BFi3bCal2Hit.h"

R3BFi3bCal2Hit::R3BFi3bCal2Hit(Direction a_direction, Bool_t a_is_calibrator, Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi3b", a_verbose, a_direction, 1, 256, 2, a_is_calibrator)
{
}

R3BFi3bCal2Hit::~R3BFi3bCal2Hit()
{
}

UInt_t R3BFi3bCal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (201 == a_fiber_id) {
//    a_fiber_id = 200;
//  }
//  if (200 == a_fiber_id) {
//    a_fiber_id = 201;
//  }
  return a_fiber_id;
}

ClassImp(R3BFi3bCal2Hit)
