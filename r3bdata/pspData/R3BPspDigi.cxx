// -------------------------------------------------------------------------
// -----                      R3BPspPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BPspDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BPspDigi::R3BPspDigi()
: Ps03mul(0)
{
}

R3BPspDigi::R3BPspDigi(Int_t psp3mul,Double_t psp3x,Double_t psp3y,Double_t psp3e)
: R3BHit(0, psp3x, psp3y, psp3e, 0.)
, Ps03mul(psp3mul)
{
}

// -----   Destructor   ----------------------------------------------------
R3BPspDigi::~R3BPspDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BPspDigi::Print(const Option_t* opt) const {

}
// -------------------------------------------------------------------------

ClassImp(R3BPspDigi)
