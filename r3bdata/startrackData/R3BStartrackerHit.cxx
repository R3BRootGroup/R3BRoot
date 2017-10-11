// -------------------------------------------------------------------------
// -----                      R3BStartrackerHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BStartrackerHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BStartrackerHit::R3BStartrackerHit() : FairMultiLinkedData() {
	//fNbOfTrackerHits = 0;
	fEnergy = -1.; 
	fDetector = 0;
	fXHit = 0.;
	fYHit = 0.;
	fZHit = 0.;
    fPx = 0.;
    fPy = 0.;
    fPz = 0.;
    fThetaZero = 0.;
    fPhiZero = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BStartrackerHit::R3BStartrackerHit(Double_t ene, Int_t det, Double_t x, Double_t y, Double_t z, Double_t px, Double_t py, Double_t pz, Double_t th, Double_t phi) 
//R3BStartrackerHit::R3BStartrackerHit(Double_t ene, Int_t det) 
: FairMultiLinkedData() {
	
	//fNbOfTrackerHits = Nb;
	fEnergy = ene;
	fDetector = det;
	fXHit = x;
	fYHit = y;
	fZHit = z;
    fPx = px;
    fPy = py;
    fPz = pz;
    fThetaZero = th;
    fPhiZero = phi;
	
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BStartrackerHit::~R3BStartrackerHit() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
//void R3BStartrackerHit::Print(const Option_t* opt) const {
	//cout << "-I- R3BStartrackerHit: a Si tracker Hit made of " << fNbOfTrackerHits 
	//<< " Energy = " << fEnergy << " GeV" << endl;
//}
// -------------------------------------------------------------------------
