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
   Ntfpx=0.;
   Ntfpy=0.;
   Ntfpz=0.;

}

R3BmTofDigi::R3BmTofDigi(Int_t ntmul,Double_t ntfx,Double_t ntfy,Double_t ntft,Double_t ntfpath,Double_t ntfpx,
Double_t ntfpy,Double_t ntfpz) {

   Ntmul = ntmul;
   Ntx = ntfx;
   Nty = ntfy;
   Ntt = ntft;
   Ntpath = ntfpath;
   Ntfpx=ntfpx;
   Ntfpy=ntfpy;
   Ntfpz=ntfpz;

}

// -----   Destructor   ----------------------------------------------------
R3BmTofDigi::~R3BmTofDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BmTofDigi::Print(const Option_t* opt) const {

}
// -------------------------------------------------------------------------

ClassImp(R3BmTofDigi)
