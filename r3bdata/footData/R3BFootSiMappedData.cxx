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
// -----            R3BFootSiMappedData source file                  -----
// -----------------------------------------------------------------------

#include "R3BFootSiMappedData.h"

R3BFootSiMappedData::R3BFootSiMappedData()
    : fDetId(0)
    , fStripId(0)
    , fEnergy(0)
{
}

R3BFootSiMappedData::R3BFootSiMappedData(Int_t detId, Int_t stripId, Int_t energy)
    : fDetId(detId)
    , fStripId(stripId)
    , fEnergy(energy)
{
}

ClassImp(R3BFootSiMappedData);
