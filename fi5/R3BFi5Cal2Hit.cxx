#include "R3BFi5Cal2Hit.h"

R3BFi5Cal2Hit::R3BFi5Cal2Hit(Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi5", a_verbose, VERTICAL, 2, 256, 4)
{
}

R3BFi5Cal2Hit::~R3BFi5Cal2Hit()
{
}

UInt_t R3BFi5Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (192 == a_fiber_id) a_fiber_id = 15;
//  if (256 == a_fiber_id) a_fiber_id = 16;
 

  return a_fiber_id;
}

ClassImp(R3BFi5Cal2Hit)
