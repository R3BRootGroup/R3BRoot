// -------------------------------------------------------------------------
// -----                      R3BMCTrack source file                   -----
// -----                  Created 03/08/04                             -----
// -------------------------------------------------------------------------
#include "R3BMCTrack.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
R3BMCTrack::R3BMCTrack()
  : fPdgCode(0), fMotherId(-1),
    fPx(0.), fPy(0.), fPz(0.),
    fStartX(0.), fStartY(0.), fStartZ(0.), fStartT(0.),
    fNPoints(0), fMass(0.)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BMCTrack::R3BMCTrack(Int_t pdgCode, Int_t motherId, Double_t px,
		       Double_t py, Double_t pz, Double_t x, Double_t y,
		       Double_t z, Double_t t, Int_t nPoints = 0)
  : fPdgCode(pdgCode), fMotherId(motherId),
    fPx(px), fPy(py), fPz(pz),
    fStartX(x), fStartY(y), fStartZ(z), fStartT(t),
    fNPoints(0), fMass(0.)
{
  if(nPoints >= 0) {
    fNPoints = nPoints;
  } else {
    fNPoints = 0;
  }
}
// -------------------------------------------------------------------------



// -----   Copy constructor   ----------------------------------------------
R3BMCTrack::R3BMCTrack(const R3BMCTrack& right)
  : fPdgCode(right.fPdgCode), fMotherId(right.fMotherId),
    fPx(right.fPx), fPy(right.fPy), fPz(right.fPz),
    fStartX(right.fStartX), fStartY(right.fStartY), fStartZ(right.fStartZ), fStartT(right.fStartT),
    fNPoints(right.fNPoints), fMass(right.fMass)
{
}
// -------------------------------------------------------------------------



// -----   Constructor from TParticle   ------------------------------------
R3BMCTrack::R3BMCTrack(TParticle* part, Int_t fMC)
  : fPdgCode(part->GetPdgCode()), fMotherId(part->GetMother(0)),
    fPx(part->Px()), fPy(part->Py()), fPz(part->Pz()),
    fStartX(part->Vx()), fStartY(part->Vy()), fStartZ(part->Vz()), fStartT(0.),
    fNPoints(0), fMass(part->GetMass()) 
{
  if(fMC == 0) {
    //G3 
    fStartT   = part->T()*1e09;
  } else if(fMC == 1) {
    //G4 
    fStartT   = part->T();
  }
}
// -------------------------------------------------------------------------


  
// -----   Destructor   ----------------------------------------------------
R3BMCTrack::~R3BMCTrack() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void R3BMCTrack::Print(Option_t *option) const {
  cout << "Track " << option << ", mother : " << fMotherId << ", Type "
       << fPdgCode << ", momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "       Ref " << GetNPoints(kREF) << ", CAL " << GetNPoints(kCAL)
       << ", CALIFA " << GetNPoints(kCALIFA) << ", TRACKER " << GetNPoints(kTRA)
       << ", STaRTrack " << GetNPoints(kSTaRTrack)
       << ", TOF " << GetNPoints(kTOF) << ", mTOF " << GetNPoints(kMTOF)
       << ", GFI " << GetNPoints(kGFI) << ", DCH " << GetNPoints(kDCH) 
       << ", LAND " << GetNPoints(kLAND) << ", VETO "<<GetNPoints(kVETO)<<endl;
}
// -------------------------------------------------------------------------



// -----   Public method GetMass   -----------------------------------------
Double_t R3BMCTrack::GetMass() const {
//  if ( TDatabasePDG::Instance() ) {
//    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
//    if ( particle ) return particle->Mass();
//    else return 0.;
//  }
    //  return 0.;
  return fMass;
}
// -------------------------------------------------------------------------




// -----   Public method GetRapidity   -------------------------------------
Double_t R3BMCTrack::GetRapidity() const {
  Double_t e = GetEnergy();
  Double_t y = 0.5 * TMath::Log( (e+fPz) / (e-fPz) );
  return y;
}
// -------------------------------------------------------------------------




// -----   Public method GetNPoints   --------------------------------------
Int_t R3BMCTrack::GetNPoints(DetectorId detId) const {
  if      ( detId == kREF  ) return (  fNPoints &   1);
  else if ( detId == kCAL  ) return ( (fNPoints & ( 7 <<  1) ) >>  1);
  else if ( detId == kCALIFA  ) return ( (fNPoints & (31 <<  4) ) >>  4);
  else if ( detId == kTRA ) return ( (fNPoints & ( 1 <<  9) ) >>  9);
  else if ( detId == kSTaRTrack ) return ( (fNPoints & ( 1 <<  9) ) >>  9); // to be checked (Marc)
  else if ( detId == kTOF ) return ( (fNPoints & (31 << 10) ) >> 10);
  else if ( detId == kMTOF  ) return ( (fNPoints & (31 << 15) ) >> 15);
  else if ( detId == kGFI  ) return ( (fNPoints & (15 << 20) ) >> 20);
  else if ( detId == kDCH ) return ( (fNPoints & ( 1 << 24) ) >> 24);
  else if ( detId == kLAND  ) return ( (fNPoints & ( 1 << 25) ) >> 25);
  else if ( detId == kVETO  ) return ( (fNPoints & ( 1 <<26 ) ) >> 26);
  else if ( detId == kMFI  ) return ( (fNPoints & ( 1 <<27 ) ) >> 27);
  else if ( detId == kPSP  ) return ( (fNPoints & ( 1 <<28 ) ) >> 28);
  else if ( detId == kLUMON  ) return ( (fNPoints & ( 1 <<29 ) ) >> 29);
  else {
    cout << "-E- R3BMCTrack::GetNPoints: Unknown detector ID "
	 << detId << endl;
    return 0;
  }
}
// -------------------------------------------------------------------------



// -----   Public method SetNPoints   --------------------------------------
void R3BMCTrack::SetNPoints(Int_t iDet, Int_t nPoints) {

  if ( iDet == kREF ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ 1 ) )  |  nPoints;
  }

  else if ( iDet == kCAL ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 7 ) nPoints = 7;
    fNPoints = ( fNPoints & ( ~ (  7 <<  1 ) ) )  |  ( nPoints <<  1 );
  }

  else if ( iDet == kCALIFA ) {
    if      ( nPoints <  0 ) nPoints =  0;
    else if ( nPoints > 31 ) nPoints = 31;
    fNPoints = ( fNPoints & ( ~ ( 31 <<  4 ) ) )  |  ( nPoints <<  4 );
  }

  else if ( iDet == kTRA ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ (  1 <<  9 ) ) )  |  ( nPoints <<  9 );
  }

  else if ( iDet == kSTaRTrack ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ (  1 <<  9 ) ) )  |  ( nPoints <<  9 );
  }

  else if ( iDet == kTOF ) {
    if      ( nPoints <  0 ) nPoints =  0;
    else if ( nPoints > 31 ) nPoints = 31;
    fNPoints = ( fNPoints & ( ~ ( 31 << 10 ) ) )  |  ( nPoints << 10 );
  }

  else if ( iDet == kMTOF ) {
    if      ( nPoints <  0 ) nPoints =  0;
    else if ( nPoints > 31 ) nPoints = 31;
    fNPoints = ( fNPoints & ( ~ ( 31 << 15 ) ) )  |  ( nPoints << 15 );
  }

  else if ( iDet == kGFI ) {
    if      ( nPoints <  0 ) nPoints =  0;
    else if ( nPoints > 15 ) nPoints = 15;
    fNPoints = ( fNPoints & ( ~ ( 15 << 20 ) ) )  |  ( nPoints << 20 );
  }

  else if ( iDet == kDCH ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ (  1 << 24 ) ) )  |  ( nPoints << 24 );
  }

  else if ( iDet == kLAND ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ (  1 << 25 ) ) )  |  ( nPoints << 25 );
  }

   else if ( iDet == kVETO ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ (  1 << 26 ) ) )  |  ( nPoints << 26 );
  }

   else if ( iDet == kMFI ) {
     if      ( nPoints < 0 ) nPoints = 0;
     else if ( nPoints > 1 ) nPoints = 1;
     fNPoints = ( fNPoints & ( ~ (  1 << 27 ) ) )  |  ( nPoints << 27 );
   }
  
   else if ( iDet == kPSP ) {
     if      ( nPoints < 0 ) nPoints = 0;
     else if ( nPoints > 1 ) nPoints = 1;
     fNPoints = ( fNPoints & ( ~ (  1 << 28 ) ) )  |  ( nPoints << 28 );
   }
  
   else if ( iDet == kLUMON ) {
     if      ( nPoints < 0 ) nPoints = 0;
     else if ( nPoints > 1 ) nPoints = 1;
     fNPoints = ( fNPoints & ( ~ (  1 << 29 ) ) )  |  ( nPoints << 29 );
   }
   else if ( iDet == kDTOF ){/*FIXME*/}
  else cout << "-E- R3BMCTrack::SetNPoints: Unknown detector ID "
	    << iDet << endl;

}
// -------------------------------------------------------------------------












ClassImp(R3BMCTrack)
