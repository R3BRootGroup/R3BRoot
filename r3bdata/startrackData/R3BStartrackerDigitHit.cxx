// -------------------------------------------------------------------------
// -----                      R3BStartrackerDigitHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BStartrackerDigitHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BStartrackerDigitHit::R3BStartrackerDigitHit() : FairMultiLinkedData() {
	//fNbOfTrackerHits = 0;
	fEnergy = -1.; 
	fDetector = 0;
	fChip = -1;
	fSide = -1;
	fStrip = -1;
/*
	fStripfrt = -1;
	fStripbck = -1;
*/
	fTime= -1.;
 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
//R3BStartrackerDigitHit::R3BStartrackerDigitHit(Double_t ene, Int_t det, Int_t Stripfrt, Int_t Stripbck, Double_t time)  
R3BStartrackerDigitHit::R3BStartrackerDigitHit(Int_t det, Int_t chip, Int_t side, Int_t strip, Double_t energy, Double_t time)  
: FairMultiLinkedData() {
	
  /*	fEnergy = ene;
	fDetector = det;
	fStripfrt = Stripfrt;
	fStripbck = Stripbck;
  */
	fDetector = det;
	fChip = chip;
	fSide = side;
	fStrip = strip;
   	fEnergy = energy;
        fTime = time;
	
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BStartrackerDigitHit::~R3BStartrackerDigitHit() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
//void R3BStartrackerHit::Print(const Option_t* opt) const {
	//cout << "-I- R3BStartrackerHit: a Si tracker Hit made of " << fNbOfTrackerHits 
	//<< " Energy = " << fEnergy << " GeV" << endl;
//}
// -------------------------------------------------------------------------
