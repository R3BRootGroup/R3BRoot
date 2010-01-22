// -------------------------------------------------------------------------
// -----                      R3BLandPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BLandDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BLandDigi::R3BLandDigi() {
  fTdcL     = fTdcR    =  fQdcR  = fQdcL = 0.;
  fPaddleNb = -1;
}
R3BLandDigi::R3BLandDigi(Int_t paddle, Double_t tdcR, Double_t tdcL,
			               Double_t qdcR, Double_t qdcL){
    fTdcL    = tdcR;
    fTdcR    = tdcL;
    fQdcL    = qdcR;
    fQdcR    = qdcL;

    fPaddleNb = paddle;
}

// -----   Destructor   ----------------------------------------------------
R3BLandDigi::~R3BLandDigi() { }

// -----   Public method Print   -------------------------------------------
void R3BLandDigi::Print(const Option_t* opt) const {

  cout << "-I- R3BLandDigi: LAND Digi in detector " << endl;
  cout << "    PaddleNr: " << fPaddleNb << endl;
  cout << "    TdcR: " << fTdcR << "    TdcL " << fTdcL << endl;
  cout << "    QdcR: " << fQdcR << "    QdcL " << fQdcL << endl;


}
// -------------------------------------------------------------------------

ClassImp(R3BLandDigi)
