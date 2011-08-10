// -------------------------------------------------------------------------
// -----                      R3BmTofPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BmTofDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BmTofDigi::R3BmTofDigi() {
   
   Ntmul=0;
   Ntx=0.;
   Nty=0.;
   Ntt=0.;
   Ntpath=0.;

}

R3BmTofDigi::R3BmTofDigi(Int_t ntmul,Double_t ntfx,Double_t ntfy,Double_t ntft,Double_t ntfpath) {

   Ntmul = ntmul;
   Ntx = ntfx;
   Nty = ntfy;
   Ntt = ntft;
   Ntpath = ntfpath;

}

// -----   Destructor   ----------------------------------------------------
R3BmTofDigi::~R3BmTofDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BmTofDigi::Print(const Option_t* opt) const {

}
// -------------------------------------------------------------------------

ClassImp(R3BmTofDigi)
