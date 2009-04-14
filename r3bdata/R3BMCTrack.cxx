// -------------------------------------------------------------------------
// -----                      R3BMCTrack source file                   -----
// -----                  Created 03/08/04  by V. Friese               -----
// -------------------------------------------------------------------------


#include <iostream>

#include "R3BMCTrack.h"
#include <limits>
using namespace std;
// -----   Default constructor   -------------------------------------------
R3BMCTrack::R3BMCTrack() {
  fPdgCode = fMotherID = 0;
  fPoints = 0;
  fStartX = fStartY  = fStartZ = fStartT = 0.;
  fPx = fPy = fPz = 0.;

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BMCTrack::R3BMCTrack(Int_t pdgCode, Int_t motherID, TVector3 startVertex, 
		       Double_t startTime, TVector3 momentum, Int_t  nPoint){
  fPdgCode  = pdgCode;
  fMotherID = motherID;
  fStartX   = startVertex.X();
  fStartY   = startVertex.Y();
  fStartZ   = startVertex.Z();
  fStartT   = startTime;
  fPx       = momentum.X();
  fPy       = momentum.Y();
  fPz       = momentum.Z();
  if (nPoint >= 0) fPoints = nPoint;
  else             fPoints = 0;

}
// -------------------------------------------------------------------------



// -----   Copy constructor   ----------------------------------------------
R3BMCTrack::R3BMCTrack(const R3BMCTrack& track) { 
  *this = track;
}
// -------------------------------------------------------------------------



// -----   Constructor from TParticle   ------------------------------------
R3BMCTrack::R3BMCTrack(TParticle* part) {
  fPdgCode  = part->GetPdgCode();
  fMotherID = part->GetMother(0);
  fStartX   = part->Vx();
  fStartY   = part->Vy();
  fStartZ   = part->Vz();
  fStartT   = part->T()*1e09;
  fPx       = part->Px();
  fPy       = part->Py();
  fPz       = part->Pz();
  fPoints   = Int_t(part->GetMother(1));
}
// -------------------------------------------------------------------------


  
// -----   Destructor   ----------------------------------------------------
R3BMCTrack::~R3BMCTrack() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void R3BMCTrack::Print(Int_t trackID) const {
  cout << "Track " << trackID << ", mother : " << fMotherID << ", Type "
       << fPdgCode << ", momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
}
// -------------------------------------------------------------------------

TLorentzVector R3BMCTrack::Get4Momentum() const {

    Double_t mass=0.0;
    Double_t ene=0.0;
    TParticlePDG*
    fParticlePDG = TDatabasePDG::Instance()->GetParticle(fPdgCode);

    if (fParticlePDG)
     mass   = fParticlePDG->Mass();

    if ( mass >= 0 ) {
	ene  = TMath::Sqrt(mass*mass + fPx*fPx +fPy*fPy +fPz*fPz);
    }

   return TLorentzVector(fPx,fPy,fPz,ene);
}

// -----   Public method GetNPoints   --------------------------------------
Int_t  R3BMCTrack::GetNPoints(DetectorId detId) const {
  if      ( detId == kCAL ) return ( (fPoints  & (3 <<  0) ) >>  0 );
  else if ( detId == kDCH ) return ( (fPoints  & (3 <<  2) ) >>  2 );
  else if ( detId == kGFI ) return ( (fPoints  & (3 <<  4) ) >>  4 );
  else if ( detId == kLAND ) return ( (fPoints  & (3 <<  6) ) >>  6 );
  else if ( detId == kMTOF ) return ( (fPoints  & (3 <<  8) ) >>  8 );
  else if ( detId == kTOF ) return ( (fPoints  & (3 << 10) ) >> 10 );
  else if ( detId == kTRA ) return ( (fPoints  & (3 << 12) ) >> 12 );
	
  else {
    cout << "-E- R3BMCTrack::GetNPoints: Unknown detector ID "
	 << detId << endl;
    return 0;
  }
}
void R3BMCTrack::SetNPoints(Int_t iDet, Int_t  nPoints) {

  if        ( iDet == kCAL ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 3 ) nPoints = 3;
    fPoints = ( fPoints & ( ~ ( 3 <<  0 ) ) )  |  ( nPoints <<  0 );
  }

  else if ( iDet == kDCH ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 3 ) nPoints = 3;
    fPoints = ( fPoints & ( ~ ( 3 <<  2 ) ) )  |  ( nPoints <<  2 );
  }

  else if ( iDet == kGFI ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 3 ) nPoints = 3;
    fPoints = ( fPoints & ( ~ (  3 << 4 ) ) )  |  ( nPoints <<  4 );
  }

  else if ( iDet == kLAND ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 3 ) nPoints = 3;
    fPoints = ( fPoints & ( ~ ( 3 <<  6 ) ) )  |  ( nPoints <<  6);
  }

  else if ( iDet == kMTOF ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 3 ) nPoints = 3;
    fPoints = ( fPoints & ( ~ ( 3 <<  8 ) ) )  |  ( nPoints <<  8 );
  }

  else if ( iDet == kTOF ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 3 ) nPoints = 3;
    fPoints = ( fPoints & ( ~ ( 3 << 10 ) ) )  |  ( nPoints << 10 );
  }

  else if ( iDet == kTRA ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 3 ) nPoints = 3;
    fPoints = ( fPoints & ( ~ ( 3 << 12 ) ) )  |  ( nPoints << 12 );
  }

	
  else cout << "-E- R3BMCTrack::SetNPoints: Unknown detector ID "
	    << iDet << endl;

}
//


ClassImp(R3BMCTrack)
