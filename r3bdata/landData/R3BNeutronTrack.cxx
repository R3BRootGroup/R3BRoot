#include "R3BNeutronTrack.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BNeutronTrack::R3BNeutronTrack() {
  fX  = fY  = fZ  = 0.;
  fX_out      = fY_out  = fZ_out  = 0.;
  fPx_out     = fPy_out = fPz_out = 0.;
}

R3BNeutronTrack::R3BNeutronTrack(TVector3 posIn, 
TVector3 posOut, TVector3 momOut, Double_t tof){
  fX  = posIn.X();
  fY  = posIn.Y();
  fZ  = posIn.Z();
  fX_out  = posOut.X();
  fY_out  = posOut.Y();
  fZ_out  = posOut.Z();
  fPx_out = momOut.Px();
  fPy_out = momOut.Py();
  fPz_out = momOut.Pz();

}

// -----   Destructor   ----------------------------------------------------

R3BNeutronTrack::~R3BNeutronTrack() { }

// -----   Public method Print   -------------------------------------------
void R3BNeutronTrack::Print(const Option_t* opt) const {

  cout << "-I- R3BNeutronTrack: LAND Digi in detector " << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx_out << ", " << fPy_out << ", " << fPz_out
       << ") GeV" << endl;

}
// -------------------------------------------------------------------------

ClassImp(R3BNeutronTrack)
