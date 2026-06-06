/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
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

// -----   Standard constructor   ------------------------------------------
R3BFootCalData::R3BFootCalData(uint8_t detid, uint16_t stripid, double energy, double sigma)
    : fDetId(detid)
    , fStripId(stripid)
    , fEnergy(energy)
    , fSigma(sigma)
{
}

ClassImp(R3BFootCalData)
