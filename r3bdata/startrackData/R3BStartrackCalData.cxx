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
// -----            R3BStartrackCalData source file                -----
// -------------------------------------------------------------------------

#include "R3BStartrackCalData.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BStartrackCalData::R3BStartrackCalData()
    : FairMultiLinkedData()
    , fTS(0)
    , // time of the interaction
    fTSext(0)
    , // time of the external signal
    fLadderId(-1)
    , // ladder unique identifier
    fSide(-1)
    , // ladder side unique identifier
    fAsicId(-1)
    , // side asic unique identifier
    fStripId(-1)
    , // asic strip unique identifier
    fEnergy(-1.)
    ,           // energy in the strip
    fNHits(-1.) // energy in the strip
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BStartrackCalData::R3BStartrackCalData(ULong_t ts,
                                         ULong_t tsext,
                                         Int_t ladderId,
                                         Int_t side,
                                         Int_t asicId,
                                         Int_t stripId,
                                         UInt_t energy,
                                         UInt_t nHits)
    : FairMultiLinkedData()
    , fTS(ts)
    , // time of the interaction
    fTSext(tsext)
    , // time of the external signal
    fLadderId(ladderId)
    , // ladder unique identifier
    fSide(side)
    , // ladder side unique identifier
    fAsicId(asicId)
    , // side asic unique identifier
    fStripId(stripId)
    , // asic strip unique identifier
    fEnergy(energy)
    ,             // energy in the strip
    fNHits(nHits) // hit identifier
{
}
// -------------------------------------------------------------------------

R3BStartrackCalData::R3BStartrackCalData(const R3BStartrackCalData& right)
    : FairMultiLinkedData(right)
    , fTS(right.fTS)
    , fTSext(right.fTSext)
    , fLadderId(right.fLadderId)
    , fSide(right.fSide)
    , fAsicId(right.fAsicId)
    , fStripId(right.fStripId)
    , fEnergy(right.fEnergy)
    , fNHits(right.fNHits)
{
}

// -----   Destructor   ----------------------------------------------------
R3BStartrackCalData::~R3BStartrackCalData() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BStartrackCalData::Print(const Option_t* opt) const
{
    cout << "-I- R3BStartrackCalData: a StartrackCalData level hit in Ladder identifier " << fLadderId << endl;
    cout << "    Side = " << fSide << endl;
    cout << "    Asic = " << fAsicId << endl;
    cout << "    Strip = " << fStripId << endl;
    cout << "    Energy = " << fEnergy << " (GeV in sim)" << endl;
    cout << "    Time =" << fTS << " ns  " << endl;
    cout << "    Time Ext=" << fTSext << " ns  " << endl;
    cout << "    N hit=" << fNHits << endl;
}
// -------------------------------------------------------------------------
