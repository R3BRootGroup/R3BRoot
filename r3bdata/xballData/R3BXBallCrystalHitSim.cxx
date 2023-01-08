/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------------
// -----              R3BXBallCrystalHitSim source file                  -----
// -------------------------------------------------------------------------

#include "R3BXBallCrystalHitSim.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BXBallCrystalHitSim::R3BXBallCrystalHitSim()
    : R3BXBallCrystalHit()
{
    fCrystalType = fNSteps = -1;
    fEinc = -1;
    fTrackID = fVolumeID = fParentTrackID = fTrackPID = fTrackUniqueID = -1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BXBallCrystalHitSim::R3BXBallCrystalHitSim(Int_t type,
                                             Int_t crysnb,
                                             Double_t energy,
                                             Double_t time,
                                             Int_t steps,
                                             Double_t einc,
                                             Int_t trackid,
                                             Int_t volid,
                                             Int_t partrackid,
                                             Int_t pdgid,
                                             Int_t uniqueid)
    : R3BXBallCrystalHit()
{

    fCrystalType = type;
    fCrystalNb = crysnb;
    fEnergy = energy;
    fTime = time;
    fNSteps = steps;
    fEinc = einc;
    fTrackID = trackid;
    fVolumeID = volid;
    fParentTrackID = partrackid;
    fTrackPID = pdgid;
    fTrackUniqueID = uniqueid;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BXBallCrystalHitSim::~R3BXBallCrystalHitSim() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BXBallCrystalHitSim::Print(const Option_t* opt) const
{
    cout << "-I- R3BXBallCrystalHitSim: a calo crystalHit in crystal type " << fCrystalType
         << " and unique crystal identifier number: " << fCrystalNb << endl;
    cout << "    Energy = " << fEnergy << " GeV" << endl;
    cout << "    Time " << fTime << " ns  " << endl;
    cout << "    Steps " << fNSteps << endl;
    cout << "    Einc = " << fEinc << " GeV" << endl;
    cout << " TrackID: " << fTrackID << " in Volume ID: " << fVolumeID << " and Parent TrackID: " << fParentTrackID
         << endl;
    cout << " PDG Id: " << fTrackPID << " Unique Id: " << fTrackUniqueID << endl;
}
// -------------------------------------------------------------------------
