#include "R3BFi0Cal2Hit.h"

R3BFi0Cal2Hit::R3BFi0Cal2Hit(Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi0", a_verbose, VERTICAL, 1, 256, 0)
{
}

R3BFi0Cal2Hit::~R3BFi0Cal2Hit()
{
}

UInt_t R3BFi0Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (201 == a_fiber_id) {
//    a_fiber_id = 200;
//  }
//  if (200 == a_fiber_id) {
//    a_fiber_id = 201;
//  }
  return a_fiber_id;
}

ClassImp(R3BFi0Cal2Hit)
