// -------------------------------------------------------------------------
// -----                      R3BDchPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BDchPoint.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BDchPoint::R3BDchPoint() : FairMCPoint() {

  fX_out      = fY_out  = fZ_out  = -99999.;
  fPx_out     = fPy_out = fPz_out = -99999.;

  fLocalX_in      = fLocalY_in  = fLocalZ_in  = -99999.;
  fLocalPx_in     = fLocalPy_in = fLocalPz_in = -99999.;
  fLocalX_out      = fLocalY_out  = fLocalZ_out  = -99999.;
  fLocalPx_out     = fLocalPy_out = fLocalPz_out = -99999.;

  fModule     = fLayer  = fCell   = -99999;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BDchPoint::R3BDchPoint(Int_t trackId, Int_t mod, Int_t layer, Int_t cell,TVector3 posIn,
			 TVector3 posOut, TVector3 momIn, TVector3 momOut,
			 TVector3 lpos1, TVector3 lmom1,
			 TVector3 lpos2, TVector3 lmom2,
			 Double_t tof, Double_t length, Double_t eLoss) 
: FairMCPoint(trackId, mod, posIn, momIn, tof, length, eLoss) {


  fX_out  = posOut.X();
  fY_out  = posOut.Y();
  fZ_out  = posOut.Z();
  fPx_out = momOut.Px();
  fPy_out = momOut.Py();
  fPz_out = momOut.Pz();

  fLocalX_in  = lpos1.X();
  fLocalY_in  = lpos1.Y();
  fLocalZ_in  = lpos1.Z();
  fLocalPx_in = lmom1.Px();
  fLocalPy_in = lmom1.Py();
  fLocalPz_in = lmom1.Pz();

  fLocalX_out  = lpos2.X();
  fLocalY_out  = lpos2.Y();
  fLocalZ_out  = lpos2.Z();
  fLocalPx_out = lmom2.Px();
  fLocalPy_out = lmom2.Py();
  fLocalPz_out = lmom2.Pz();

  fModule  = mod;
  fLayer  = layer;
  fCell   = cell;

}


// -----   Destructor   ----------------------------------------------------
R3BDchPoint::~R3BDchPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BDchPoint::Print(const Option_t* opt) const {
  cout << "-I- R3BDchPoint: STS Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



// -----   Local Point x coordinate from linear extrapolation   ------------------
Double_t R3BDchPoint::GetLocalX(Double_t z) const {
  //  cout << fZ << " " << z << " " << fZ_out << endl;
  if ( (fLocalZ_out-z)*(fLocalZ_in-z) >= 0. ) return (fLocalX_out+fLocalX_in)/2.;
  Double_t dz = fLocalZ_out - fLocalZ_in;
  return ( fLocalX_in + (z-fLocalZ_in) / dz * (fLocalX_out-fLocalX_in) );
}
// -------------------------------------------------------------------------



// -----   Local Point y coordinate from linear extrapolation   ------------------
Double_t R3BDchPoint::GetLocalY(Double_t z) const {
  if ( (fLocalZ_out-z)*(fLocalZ_in-z) >= 0. ) return (fLocalY_out+fLocalY_in)/2.;
  Double_t dz = fLocalZ_out - fLocalZ_in;
  //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
  return ( fLocalY_in + (z-fLocalZ_in) / dz * (fLocalY_out-fLocalY_in) );
}
// -------------------------------------------------------------------------

// -----   Point x coordinate from linear extrapolation   ------------------
Double_t R3BDchPoint::GetX(Double_t z) const {
  //  cout << fZ << " " << z << " " << fZ_out << endl;
  if ( (fZ_out-z)*(fZ-z) >= 0. ) return (fX_out+fX)/2.;
  Double_t dz = fZ_out - fZ;
  return ( fX + (z-fZ) / dz * (fX_out-fX) );
}
// -------------------------------------------------------------------------



// -----   Point y coordinate from linear extrapolation   ------------------
Double_t R3BDchPoint::GetY(Double_t z) const {
  if ( (fZ_out-z)*(fZ-z) >= 0. ) return (fY_out+fY)/2.;
  Double_t dz = fZ_out - fZ;
  //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
  return ( fY + (z-fZ) / dz * (fY_out-fY) );
}
// -------------------------------------------------------------------------



// -----   Public method IsUsable   ----------------------------------------
Bool_t R3BDchPoint::IsUsable() const {
  Double_t dz = fZ_out - fZ;
  if ( TMath::Abs(dz) < 1.e-4 ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------


ClassImp(R3BDchPoint)
