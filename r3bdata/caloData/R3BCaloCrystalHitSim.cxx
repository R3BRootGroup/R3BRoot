// -------------------------------------------------------------------------
// -----             R3BCaloCrystalHitSim source file                  -----
// -------------------------------------------------------------------------

#include "R3BCaloCrystalHitSim.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCaloCrystalHitSim::R3BCaloCrystalHitSim() : R3BCaloCrystalHit() {
  fCrystalType = fCrystalCopy = fNSteps = -1;
  fEinc = -1; 
  fTrackID = fVolumeID = fParentTrackID = fTrackPID = fTrackUniqueID = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloCrystalHitSim::R3BCaloCrystalHitSim(Int_t type, Int_t copy, Int_t ident,
					Double_t energy, Double_t time,
					Int_t steps, Double_t einc,
					Int_t trackid, Int_t volid, Int_t partrackid,
					Int_t pdgid, Int_t uniqueid) 
  : R3BCaloCrystalHit() {
  
  fCrystalId     = ident;
  fCrystalType   = type;
  fCrystalCopy   = copy;
  fEnergy        = energy;
  fTime          = time;
  fNSteps        = steps;
  fEinc          = einc;
  fTrackID       = trackid;
  fVolumeID      = volid;
  fParentTrackID = partrackid;
  fTrackPID      = pdgid;
  fTrackUniqueID      = uniqueid;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCaloCrystalHitSim::~R3BCaloCrystalHitSim() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCaloCrystalHitSim::Print(const Option_t* opt) const {
  cout << "-I- R3BCaloCrystalHitSim: a sim calo crystalHit in crystal type " << fCrystalType 
       << " and copy  " << fCrystalCopy << ". Uniqued crystal identifier: " << fCrystalId << endl;
  cout << "    Energy = " << fEnergy << " GeV" << endl;
  cout << "    Time " << fTime << " ns  " << endl;
  cout << "    Steps " << fNSteps << endl;
  cout << "    Einc = " << fEinc << " GeV" << endl;
  cout << " TrackID: " << fTrackID << " in Volume ID: " << fVolumeID << " and Parent TrackID: " << fParentTrackID << endl;
  cout << " PDG Id: " << fTrackPID << " Unique Id: " << fTrackUniqueID << endl;
}
// -------------------------------------------------------------------------
