#include "R3BFi9Cal2Hit.h"

R3BFi9Cal2Hit::R3BFi9Cal2Hit(Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi9", a_verbose, VERTICAL, 2, 256, 1)
{
}

R3BFi9Cal2Hit::~R3BFi9Cal2Hit()
{
}

UInt_t R3BFi9Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (201 == a_fiber_id) {
//    a_fiber_id = 200;
//  }
//  if (200 == a_fiber_id) {
//    a_fiber_id = 201;
//  }
  return a_fiber_id;
}

ClassImp(R3BFi9Cal2Hit)
