// -------------------------------------------------------------------------
// -----                R3BCalifaHitData source file                   -----
// -----              Modified 23/12/16 by P.Cabanelas                 ----- 
// -------------------------------------------------------------------------

#include "R3BCalifaHitData.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCalifaHitData::R3BCalifaHitData()
  : FairMultiLinkedData(),
    fNbOfCrystalHits(0), fEnergy(-1.), fTheta(-1.), fPhi(-1.), fTime(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCalifaHitData::R3BCalifaHitData(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns,
		       Double_t theta, Double_t phi, ULong64_t time) 
  : FairMultiLinkedData(),
    fNbOfCrystalHits(Nb), fEnergy(ene), fNf(nf), fNs(ns),
    fTheta(theta), fPhi(phi), fTime(time)
{
}
// -------------------------------------------------------------------------



R3BCalifaHitData::R3BCalifaHitData(const R3BCalifaHitData& right)
  : FairMultiLinkedData(right),
    fNbOfCrystalHits(right.fNbOfCrystalHits),
    fEnergy(right.fEnergy), fNf(right.fNf), fNs(right.fNs),
    fTheta(right.fTheta), fPhi(right.fPhi), fTime(right.fTime)
{
}



// -----   Destructor   ----------------------------------------------------
R3BCalifaHitData::~R3BCalifaHitData()
{
}
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCalifaHitData::Print(const Option_t* opt) const {
	cout << "-I- R3BCalifaHitData: a CALIFA Hit made of " << fNbOfCrystalHits 
	<< " crystalHits. Energy = " << fEnergy << " GeV" << endl;
	cout << "    Theta(polar) " << fTheta << " deg  " << endl;
	cout << "    Phi(azimuthal) " << fPhi << " deg  " << endl;
}
// -------------------------------------------------------------------------
