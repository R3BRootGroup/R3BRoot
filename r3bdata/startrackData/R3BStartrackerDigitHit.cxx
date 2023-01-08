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
// -----                      R3BStartrackerDigitHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BStartrackerDigitHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BStartrackerDigitHit::R3BStartrackerDigitHit()
    : FairMultiLinkedData()
{
    // fNbOfTrackerHits = 0;
    fEnergy = -1.;
    fDetector = 0;
    fChip = -1;
    fSide = -1;
    fStrip = -1;
    /*
        fStripfrt = -1;
        fStripbck = -1;
    */
    fTime = -1.;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
// R3BStartrackerDigitHit::R3BStartrackerDigitHit(Double_t ene, Int_t det, Int_t Stripfrt, Int_t Stripbck, Double_t
// time)
R3BStartrackerDigitHit::R3BStartrackerDigitHit(Int_t det,
                                               Int_t chip,
                                               Int_t side,
                                               Int_t strip,
                                               Double_t energy,
                                               Double_t time)
    : FairMultiLinkedData()
{

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
R3BStartrackerDigitHit::~R3BStartrackerDigitHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
// void R3BStartrackerHit::Print(const Option_t* opt) const {
// cout << "-I- R3BStartrackerHit: a Si tracker Hit made of " << fNbOfTrackerHits
//<< " Energy = " << fEnergy << " GeV" << endl;
//}
// -------------------------------------------------------------------------
