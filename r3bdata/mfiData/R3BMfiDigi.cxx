// -------------------------------------------------------------------------
// -----                      R3BMfiPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BMfiDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BMfiDigi::R3BMfiDigi() {
   
   Mf1mul=0;
   Mf1x=0.;
   Mf01ni=0;
   
}

R3BMfiDigi::R3BMfiDigi(Int_t mf1mul,Double_t mf1x, Int_t fiber) {
   
   Mf1mul=mf1mul;
   Mf1x=mf1x;
   Mf01ni=fiber;
   
}

// -----   Destructor   ----------------------------------------------------
R3BMfiDigi::~R3BMfiDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BMfiDigi::Print(const Option_t* opt) const {

}
// -------------------------------------------------------------------------

ClassImp(R3BMfiDigi)
