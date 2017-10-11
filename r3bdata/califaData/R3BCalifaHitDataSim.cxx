// -------------------------------------------------------------------------
// -----             R3BCalifaHitDataSim source file                   -----
// -------------------------------------------------------------------------

#include "R3BCalifaHitDataSim.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCalifaHitDataSim::R3BCalifaHitDataSim()
  : R3BCalifaHitData(),
    fEinc(0.)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCalifaHitDataSim::R3BCalifaHitDataSim(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns,
			     Double_t theta, Double_t phi, Double_t einc) 
  : R3BCalifaHitData(Nb, ene, nf, ns, theta, phi, 0),
    fEinc(einc)
{
}
// -------------------------------------------------------------------------



R3BCalifaHitDataSim::R3BCalifaHitDataSim(const R3BCalifaHitDataSim& right)
  : R3BCalifaHitData(right),
    fEinc(right.fEinc)
{
}



// -----   Destructor   ----------------------------------------------------
R3BCalifaHitDataSim::~R3BCalifaHitDataSim() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCalifaHitDataSim::Print(const Option_t* opt) const {
	// --- cout << "-I- R3BCalifaHitData: a CALIFA Hit made of " << fNbOfCrystalHits 
	cout << " Total incident Energy  " << fEinc << endl;
}
// -------------------------------------------------------------------------
