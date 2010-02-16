
#include "R3BNeutronTrack.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BNeutronTrack::R3BNeutronTrack() {
  fTdcL     = fTdcR    =  fQdcR  = fQdcL = 0.;
  fPaddleNb = -1;
}
R3BNeutronTrack::R3BNeutronTrack(Int_t paddle, Double_t tdcR, Double_t tdcL,
			               Double_t qdcR, Double_t qdcL){
    fTdcL    = tdcR;
    fTdcR    = tdcL;
    fQdcL    = qdcR;
    fQdcR    = qdcL;

    fPaddleNb = paddle;
}

// -----   Destructor   ----------------------------------------------------
R3BNeutronTrack::~R3BNeutronTrack() { }

// -----   Public method Print   -------------------------------------------
void R3BNeutronTrack::Print(const Option_t* opt) const {

  cout << "-I- R3BNeutronTrack: LAND Digi in detector " << endl;
  cout << "    PaddleNr: " << fPaddleNb << endl;
  cout << "    TdcR: " << fTdcR << "    TdcL " << fTdcL << endl;
  cout << "    QdcR: " << fQdcR << "    QdcL " << fQdcL << endl;


}
// -------------------------------------------------------------------------

ClassImp(R3BNeutronTrack)
