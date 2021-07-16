/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------
// -----            R3BFootSiCalData source file                     -----
// -----------------------------------------------------------------------

#include "R3BFootSiCalData.h"

// -----   Default constructor   -------------------------------------------
R3BFootSiCalData::R3BFootSiCalData()
    : FairMultiLinkedData()
    , fDetId(0)
    , fStripId(0)
    , fEnergy(0.)
{
}

// -----   Standard constructor   ------------------------------------------
R3BFootSiCalData::R3BFootSiCalData(Int_t detid, Int_t stripid, Double_t energy)
    : FairMultiLinkedData()
    , fDetId(detid)
    , fStripId(stripid)
    , fEnergy(energy)
{
}
// -------------------------------------------------------------------------

R3BFootSiCalData::R3BFootSiCalData(const R3BFootSiCalData& right)
    : FairMultiLinkedData(right)
    , fDetId(right.fDetId)
    , fStripId(right.fStripId)
    , fEnergy(right.fEnergy)
{
}

ClassImp(R3BFootSiCalData);
