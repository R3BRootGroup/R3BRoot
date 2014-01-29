// -------------------------------------------------------------------------
// -----             R3BCaloCrystalHitSim source file                  -----
// -------------------------------------------------------------------------

#include "R3BCaloCrystalHitSim.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCaloCrystalHitSim::R3BCaloCrystalHitSim()
  : R3BCaloCrystalHit(),
    fCrystalType(-1), fCrystalCopy(-1), fNSteps(-1), fEinc(-1.), fTrackID(-1),
    fVolumeID(-1), fParentTrackID(-1), fTrackPID(-1), fTrackUniqueID(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloCrystalHitSim::R3BCaloCrystalHitSim(Int_t geover, Int_t type, Int_t copy, Int_t ident,
					   Double_t energy, Double_t Nf, Double_t Ns, Double_t time,
					   Int_t steps, Double_t einc,
					   Int_t trackid, Int_t volid, Int_t partrackid,
					   Int_t pdgid, Int_t uniqueid) 
  : R3BCaloCrystalHit(geover, ident, energy, Nf, Ns, time),
    fCrystalType(type), fCrystalCopy(copy), fNSteps(steps), fEinc(einc),
    fTrackID(trackid), fVolumeID(volid), fParentTrackID(partrackid),
    fTrackPID(pdgid), fTrackUniqueID(uniqueid)
{
}
// -------------------------------------------------------------------------



R3BCaloCrystalHitSim::R3BCaloCrystalHitSim(const R3BCaloCrystalHitSim& right)
  : R3BCaloCrystalHit(right),    
    fCrystalType(right.fCrystalType), fCrystalCopy(right.fCrystalCopy),
    fNSteps(right.fNSteps), fEinc(right.fEinc), fTrackID(right.fTrackID),
    fVolumeID(right.fVolumeID), fParentTrackID(right.fParentTrackID),
    fTrackPID(right.fTrackPID), fTrackUniqueID(right.fTrackUniqueID)
{
}



// -----   Destructor   ----------------------------------------------------
R3BCaloCrystalHitSim::~R3BCaloCrystalHitSim()
{
}
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCaloCrystalHitSim::Print(const Option_t* opt) const {
  cout << "-I- R3BCaloCrystalHitSim: a sim calo crystalHit in crystal type " << fCrystalType 
       << " and copy  " << fCrystalCopy << ". Uniqued crystal identifier: " << fCrystalId << endl;
  cout << "    GeoVersion = " << fGeoVersion << endl;
  cout << "    Energy = " << fEnergy << " GeV" << endl;
  cout << "    Time " << fTime << " ns  " << endl;
  cout << "    Steps " << fNSteps << endl;
  cout << "    Einc = " << fEinc << " GeV" << endl;
  cout << " TrackID: " << fTrackID << " in Volume ID: " << fVolumeID << " and Parent TrackID: " << fParentTrackID << endl;
  cout << " PDG Id: " << fTrackPID << " Unique Id: " << fTrackUniqueID << endl;
}
// -------------------------------------------------------------------------
