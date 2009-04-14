#include "R3BDetPoint.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
R3BDetPoint::R3BDetPoint() : FairMCPoint() { }
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BDetPoint::R3BDetPoint(Int_t trackID, Int_t detID, 
					 TVector3 pos, TVector3 mom, 
					 Double_t tof, Double_t length,
					 Double_t eLoss)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss) { }
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BDetPoint::~R3BDetPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BDetPoint::Print(const Option_t* opt) const {
  cout << "-I- R3BDetPoint: TutorialDet point for track " << fTrackID
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(R3BDetPoint)
  
