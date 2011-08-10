// -------------------------------------------------------------------------
// -----                      R3BDch2pPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BDch2pDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BDch2pDigi::R3BDch2pDigi() {
   
   Pd1mul=0;
   Pd1x_p1=0.;
   Pd1y_p1=0.;
   Pd1x_p2=0.;
   Pd1y_p2=0.;
   
   Pd2mul=0;
   Pd2x_p1=0.;
   Pd2y_p1=0.;
   Pd2x_p2=0.;
   Pd2y_p2=0.;
   
}

R3BDch2pDigi::R3BDch2pDigi(Int_t pd1mul,Double_t Pdx1_p1,Double_t Pdy1_p1,Double_t Pdx1_p2,Double_t Pdy1_p2,
Int_t pd2mul,Double_t Pdx2_p1,Double_t Pdy2_p1,Double_t Pdx2_p2,Double_t Pdy2_p2) {
   
   Pd1mul = pd1mul;
   Pd1x_p1 = Pdx1_p1;
   Pd1y_p1 = Pdy1_p1;
   Pd1x_p2 = Pdx1_p2;
   Pd1y_p2 = Pdy1_p2;
   
   Pd2mul = pd2mul;
   Pd2x_p1 = Pdx2_p1;
   Pd2y_p1 = Pdy2_p1;
   Pd2x_p2 = Pdx2_p2;
   Pd2y_p2 = Pdy2_p2;
   
}

// -----   Destructor   ----------------------------------------------------
R3BDch2pDigi::~R3BDch2pDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BDch2pDigi::Print(const Option_t* opt) const {

}
// -------------------------------------------------------------------------

ClassImp(R3BDch2pDigi)
