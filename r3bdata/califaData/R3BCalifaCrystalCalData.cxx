// -------------------------------------------------------------------------
// -----            R3BCalifaCrystalCalData source file                -----
// -------------------------------------------------------------------------

#include "R3BCalifaCrystalCalData.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCalifaCrystalCalData::R3BCalifaCrystalCalData()
  : FairMultiLinkedData(),
    fEnergy(-1.),
    fNf(-1.),
    fNs(-1.),
    fToT_Energy(-1.),
    fTime(0),
    fCrystalId(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCalifaCrystalCalData::R3BCalifaCrystalCalData(Int_t ident, Double_t energy, Double_t Nf, Double_t Ns, ULong64_t time, Double_t tot_energy) 
  : FairMultiLinkedData(),
    fEnergy(energy),
    fNf(Nf),
    fNs(Ns),
    fToT_Energy(tot_energy),
    fTime(time),
    fCrystalId(ident)
{
}
// -------------------------------------------------------------------------



R3BCalifaCrystalCalData::R3BCalifaCrystalCalData(const R3BCalifaCrystalCalData& right)
  : FairMultiLinkedData(right),
    fEnergy(right.fEnergy),
    fNf(right.fNf),
    fNs(right.fNs),
    fToT_Energy(right.fToT_Energy),
    fTime(right.fTime),
    fCrystalId(right.fCrystalId)
{
}



// -----   Destructor   ----------------------------------------------------
R3BCalifaCrystalCalData::~R3BCalifaCrystalCalData()
{
}
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCalifaCrystalCalData::Print(const Option_t* opt) const {
  cout << "-I- R3BCalifaCrystalCalData: a crystalCalData level hit in crystal identifier " << fCrystalId << endl;
  cout << "    Energy = " << fEnergy << " (GeV in sim)" << endl;
  cout << "    ToT_Energy = " << fToT_Energy << " (GeV in sim)" << endl;
  cout << "    Nf = " << fNf << " (GeV in sim); fNs =" << fNs << " (GeV in sim)" << endl;
  cout << "    Time =" << fTime << " ns  " << endl;
}
// -------------------------------------------------------------------------
