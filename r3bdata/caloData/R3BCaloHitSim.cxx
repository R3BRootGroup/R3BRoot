// -------------------------------------------------------------------------
// -----                   R3BCaloHitSim source file                   -----
// -------------------------------------------------------------------------

#include "R3BCaloHitSim.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCaloHitSim::R3BCaloHitSim()
  : R3BCaloHit(),
    fEinc(0.)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloHitSim::R3BCaloHitSim(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns,
			     Double_t theta, Double_t phi, Double_t einc) 
  : R3BCaloHit(Nb, ene, nf, ns, theta, phi),
    fEinc(einc)
{
}
// -------------------------------------------------------------------------



R3BCaloHitSim::R3BCaloHitSim(const R3BCaloHitSim& right)
  : R3BCaloHit(right),
    fEinc(right.fEinc)
{
}



// -----   Destructor   ----------------------------------------------------
R3BCaloHitSim::~R3BCaloHitSim() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCaloHitSim::Print(const Option_t* opt) const {
	// --- cout << "-I- R3BCaloHit: a CALIFA Hit made of " << fNbOfCrystalHits 
	cout << " Total incident Energy  " << fEinc << endl;
}
// -------------------------------------------------------------------------
