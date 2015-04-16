// -------------------------------------------------------------------------
// -----                      R3BCaloHit source file                   -----
// -----                  Modified 15/09/11 by Enrico Fiori            ----- 
// -------------------------------------------------------------------------

#include "R3BCaloHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCaloHit::R3BCaloHit()
  : FairMultiLinkedData(),
    fNbOfCrystalHits(0), fEnergy(-1.), fTheta(-1.), fPhi(-1.), fTime(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloHit::R3BCaloHit(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns,
		       Double_t theta, Double_t phi, ULong64_t time) 
  : FairMultiLinkedData(),
    fNbOfCrystalHits(Nb), fEnergy(ene), fNf(nf), fNs(ns),
    fTheta(theta), fPhi(phi), fTime(time)
{
}
// -------------------------------------------------------------------------



R3BCaloHit::R3BCaloHit(const R3BCaloHit& right)
  : FairMultiLinkedData(right),
    fNbOfCrystalHits(right.fNbOfCrystalHits),
    fEnergy(right.fEnergy), fNf(right.fNf), fNs(right.fNs),
    fTheta(right.fTheta), fPhi(right.fPhi), fTime(right.fTime)
{
}



// -----   Destructor   ----------------------------------------------------
R3BCaloHit::~R3BCaloHit()
{
}
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCaloHit::Print(const Option_t* opt) const {
	cout << "-I- R3BCaloHit: a CALIFA Hit made of " << fNbOfCrystalHits 
	<< " crystalHits. Energy = " << fEnergy << " GeV" << endl;
	cout << "    Theta(polar) " << fTheta << " deg  " << endl;
	cout << "    Phi(azimuthal) " << fPhi << " deg  " << endl;
}
// -------------------------------------------------------------------------
