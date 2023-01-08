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

// -----------------------------------------------------------------------
// -----            R3BFootCalData source file                       -----
// -----------------------------------------------------------------------

#include "R3BFootCalData.h"

// -----   Default constructor   -------------------------------------------
R3BFootCalData::R3BFootCalData()
    : fDetId(0)
    , fStripId(0)
    , fEnergy(0.)
{
}

// -----   Standard constructor   ------------------------------------------
R3BFootCalData::R3BFootCalData(Int_t detid, Int_t stripid, Double_t energy)
    : fDetId(detid)
    , fStripId(stripid)
    , fEnergy(energy)
{
}

ClassImp(R3BFootCalData);
