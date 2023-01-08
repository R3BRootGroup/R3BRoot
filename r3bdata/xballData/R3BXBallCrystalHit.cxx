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
// -----                 R3BXBallCrystalHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BXBallCrystalHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BXBallCrystalHit::R3BXBallCrystalHit()
    : FairMultiLinkedData()
{
    fCrystalNb = -1;
    fEnergy = -1;
    fTime = -1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BXBallCrystalHit::R3BXBallCrystalHit(Int_t crysnb, Double_t energy, Double_t time)
    : FairMultiLinkedData()
{

    fCrystalNb = crysnb;
    fEnergy = energy;
    fTime = time;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BXBallCrystalHit::~R3BXBallCrystalHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BXBallCrystalHit::Print(const Option_t* opt) const
{
    cout << "-I- R3BXBallCrystalHit: a calo crystalHit in crystal number " << fCrystalNb << endl;
    cout << "    Energy = " << fEnergy << " GeV" << endl;
    cout << "    Time " << fTime << " ns  " << endl;
}
// -------------------------------------------------------------------------
