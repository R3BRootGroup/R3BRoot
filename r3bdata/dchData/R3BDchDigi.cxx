// -------------------------------------------------------------------------
// -----                      R3BDchPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BDchDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BDchDigi::R3BDchDigi() {
   
   Pd1mul=0;
   Pd1x_p1=0.;
   Pd1y_p1=0.;
   
   Pd2mul=0;
   Pd2x_p1=0.;
   Pd2y_p1=0.;
   
}

R3BDchDigi::R3BDchDigi(Int_t pd1mul,Double_t Pdx1_p1,Double_t Pdy1_p1,Int_t pd2mul,Double_t Pdx2_p1,Double_t Pdy2_p1) {
   
   Pd1mul = pd1mul;
   Pd1x_p1 = Pdx1_p1;
   Pd1y_p1 = Pdy1_p1;
   
   Pd2mul = pd2mul;
   Pd2x_p1 = Pdx2_p1;
   Pd2y_p1 = Pdy2_p1;
   
}

// -----   Destructor   ----------------------------------------------------
R3BDchDigi::~R3BDchDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BDchDigi::Print(const Option_t* opt) const {

}
// -------------------------------------------------------------------------

ClassImp(R3BDchDigi)
