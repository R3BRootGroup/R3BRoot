// -------------------------------------------------------------------------
// -----                      R3BTofPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BTofDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BTofDigi::R3BTofDigi() {
   
   Tfmul=0;
   Tfx_p1=0.;
   Tfy_p1=0.;
   Tft_p1=0.;
   Tfpath_p1=0.;
   Tfwpx_p1=0.;
   Tfwpy_p1=0.;
   Tfwpz_p1=0.;
}

R3BTofDigi::R3BTofDigi(Int_t tfmul,Double_t tfwx_p1,Double_t tfwy_p1,Double_t tfwt_p1,Double_t tfwpath_p1,
Double_t tfwpx_p1,Double_t tfwpy_p1,Double_t tfwpz_p1) {
   
   Tfmul = tfmul;
   Tfx_p1 = tfwx_p1;
   Tfy_p1 = tfwy_p1;
   Tft_p1 = tfwt_p1;
   Tfpath_p1 = tfwpath_p1;
   Tfwpx_p1 = tfwpx_p1;
   Tfwpy_p1 = tfwpy_p1;
   Tfwpz_p1 = tfwpz_p1;
}

// -----   Destructor   ----------------------------------------------------
R3BTofDigi::~R3BTofDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BTofDigi::Print(const Option_t* opt) const {

}
// -------------------------------------------------------------------------

ClassImp(R3BTofDigi)
