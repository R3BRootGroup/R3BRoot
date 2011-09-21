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
R3BCaloHit::R3BCaloHit() : FairMultiLinkedData() {
	fNbOfCrystalHits = 0;
	fEnergy = fTheta = fPhi = -1; 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloHit::R3BCaloHit(UInt_t Nb, Double_t ene,
					   Double_t theta, Double_t phi) 
: FairMultiLinkedData() {
	
	fNbOfCrystalHits = Nb;
	fEnergy = ene;
	fTheta = theta;	
	fPhi = phi;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCaloHit::~R3BCaloHit() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCaloHit::Print(const Option_t* opt) const {
	cout << "-I- R3BCaloHit: a CALIFA Hit made of " << fNbOfCrystalHits 
	<< " crystalHits. Energy = " << fEnergy << " GeV" << endl;
	cout << "    Theta(polar) " << fTheta << " deg  " << endl;
	cout << "    Phi(azimuthal) " << fPhi << " deg  " << endl;
}
// -------------------------------------------------------------------------
