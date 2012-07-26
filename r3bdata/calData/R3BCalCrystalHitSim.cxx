// -------------------------------------------------------------------------
// -----              R3BCalCrystalHitSim source file                  -----
// -------------------------------------------------------------------------

#include "R3BCalCrystalHitSim.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCalCrystalHitSim::R3BCalCrystalHitSim() : R3BCalCrystalHit() {
  fCrystalType = fNSteps = -1;
  fEinc = -1; 
  fTrackID = fVolumeID = fParentTrackID = fTrackPID = fTrackUniqueID = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCalCrystalHitSim::R3BCalCrystalHitSim(Int_t type, Int_t crysnb,
					Double_t energy, Double_t time,
					Int_t steps, Double_t einc,
					Int_t trackid, Int_t volid, Int_t partrackid,
					Int_t pdgid, Int_t uniqueid) 
  : R3BCalCrystalHit() {
  
  fCrystalType   = type;
  fCrystalNb     = crysnb;
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
R3BCalCrystalHitSim::~R3BCalCrystalHitSim() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCalCrystalHitSim::Print(const Option_t* opt) const {
  cout << "-I- R3BCalCrystalHitSim: a calo crystalHit in crystal type " << fCrystalType 
       << " and unique crystal identifier number: " << fCrystalNb << endl;
  cout << "    Energy = " << fEnergy << " GeV" << endl;
  cout << "    Time " << fTime << " ns  " << endl;
  cout << "    Steps " << fNSteps << endl;
  cout << "    Einc = " << fEinc << " GeV" << endl;
  cout << " TrackID: " << fTrackID << " in Volume ID: " << fVolumeID << " and Parent TrackID: " << fParentTrackID << endl;
  cout << " PDG Id: " << fTrackPID << " Unique Id: " << fTrackUniqueID << endl;
}
// -------------------------------------------------------------------------
