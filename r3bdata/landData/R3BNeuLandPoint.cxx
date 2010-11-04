// -------------------------------------------------------------------------
// -----                      R3BNeuLandPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BNeuLandPoint.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BNeuLandPoint::R3BNeuLandPoint() : FairMCPoint() {
  fX_out      = fY_out  = fZ_out  = 0.;
  fPx_out     = fPy_out = fPz_out = 0.;
  fEventID = -1;
  fMot0TrackID = -1 ;
  fDetID = -1 ;
  fCellID   = -1 ;
  fSegID   = -1 ;
  fMass = 0.0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BNeuLandPoint::R3BNeuLandPoint(Int_t eventID, Int_t trackID,
				 Int_t mot0trackID,
				 Int_t detID, Int_t cellID, Int_t segID,
				 Double_t mass,
				 TVector3 posIn,
				 TVector3 posOut, TVector3 momIn, TVector3 momOut,
				 Double_t tof, Double_t length, Double_t eLoss)
  : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss) {
  fX_out  = posOut.X();
  fY_out  = posOut.Y();
  fZ_out  = posOut.Z();
  fPx_out = momOut.Px();
  fPy_out = momOut.Py();
  fPz_out = momOut.Pz();
  fEventID = eventID;
  fMot0TrackID = mot0trackID;
  fDetID = detID;
  fCellID = cellID;
  fSegID = segID;
  fMass = mass;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BNeuLandPoint::~R3BNeuLandPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BNeuLandPoint::Print(const Option_t* opt) const {
  cout << "-I- R3BNeuLandPoint: LAND Point for track " << fTrackID
	<< " in detector " << fDetID
       << " Cell: " << fCellID << endl;

  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



// -----   Point x coordinate from linear extrapolation   ------------------
Double_t R3BNeuLandPoint::GetX(Double_t z) const {
  //  cout << fZ << " " << z << " " << fZ_out << endl;
  if ( (fZ_out-z)*(fZ-z) >= 0. ) return (fX_out+fX)/2.;
  Double_t dz = fZ_out - fZ;
  return ( fX + (z-fZ) / dz * (fX_out-fX) );
}
// -------------------------------------------------------------------------



// -----   Point y coordinate from linear extrapolation   ------------------
Double_t R3BNeuLandPoint::GetY(Double_t z) const {
  if ( (fZ_out-z)*(fZ-z) >= 0. ) return (fY_out+fY)/2.;
  Double_t dz = fZ_out - fZ;
  //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
  return ( fY + (z-fZ) / dz * (fY_out-fY) );
}
// -------------------------------------------------------------------------



// -----   Public method IsUsable   ----------------------------------------
Bool_t R3BNeuLandPoint::IsUsable() const {
  Double_t dz = fZ_out - fZ;
  if ( TMath::Abs(dz) < 1.e-4 ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------


ClassImp(R3BNeuLandPoint)
