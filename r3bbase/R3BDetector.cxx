#include "R3BDetector.h"

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
R3BDetector::R3BDetector() : FairDetector() {
 // Create Matrix Unity
    fGlobalRot = new TGeoRotation();

 // Create a null translation
    fGlobalTrans = new TGeoTranslation();
    fGlobalTrans->SetTranslation(0.0,0.0,0.0);
}

// -----   Standard constructor   ------------------------------------------
R3BDetector::R3BDetector( const Char_t* Name, Bool_t Active, Int_t detid)
    : FairDetector(Name,Active,detid) {

 // Create Matrix Unity
    fGlobalRot = new TGeoRotation();

 // Create a null translation
    fGlobalTrans = new TGeoTranslation();
    fGlobalTrans->SetTranslation(0.0,0.0,0.0);

}

// -----   Destructor   ----------------------------------------------------
R3BDetector::~R3BDetector() {

}

void R3BDetector::SetRotAngles(Double_t phi, Double_t theta, Double_t psi){
 // Euler Angles definition
 // This represent the composition of : first a rotation about Z axis with
 // angle phi, then a rotation with theta about the rotated X axis, and
 // finally a rotation with psi about the new Z axis.

  fGlobalRot->SetAngles(phi, theta, psi); // all angles in degrees

}

void R3BDetector::SetTranslation(Double_t tx, Double_t ty, Double_t tz){

  fGlobalTrans->SetTranslation(tx,ty,tz);

}


ClassImp(R3BDetector)
  
