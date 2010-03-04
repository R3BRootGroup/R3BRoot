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
  fTdcL = fTdcR = fQdcR = fQdcL = fTdc = fQdc = fxx = fyy = fzz = fx0 = fy0 = fz0 = fT0 = 0.;
  fPaddleNb = -1;
}
R3BLandDigi::R3BLandDigi(Int_t paddle, Double_t tdcL, Double_t tdcR,
   Double_t tdc, Double_t qdcL, Double_t qdcR, Double_t qdc,
   Double_t xx,Double_t yy, Double_t zz, Double_t x0,Double_t y0, Double_t z0,Double_t T0 ){
    fTdcL    = tdcL;
    fTdcR    = tdcR;
    fTdc    = tdc;    
    fQdcL    = qdcL;
    fQdcR    = qdcR;
    fQdc    = qdc;
    fxx    = xx;
    fyy    = yy;
    fzz    = zz;
    fx0    = x0;
    fy0    = y0;
    fz0    = z0;
    fT0    = T0;

    fPaddleNb = paddle;
}

R3BLandDigi::R3BLandDigi(Int_t paddle, Double_t tdcR, Double_t tdcL,
			               Double_t qdcR, Double_t qdcL){

		fTdc = fQdc = fxx = fyy = fzz = fx0 = fy0 = fz0 = fT0 = 0.;

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
