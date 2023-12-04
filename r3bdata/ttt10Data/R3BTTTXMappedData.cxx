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
// -----                      R3BTTTXMappedData source file            -----
// -------------------------------------------------------------------------

#include "R3BTTTXMappedData.h"

// -----   Default constructor   -------------------------------------------
R3BTTTXMappedData::R3BTTTXMappedData()
    : fDetID(0)
    , fStripID(0)
    , fTime(0)
    , fEnergy(0)
    , fPileup(kFALSE)
    , fOverflow(kFALSE)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BTTTXMappedData::R3BTTTXMappedData(UInt_t DetID, UInt_t StripID, Int_t time, Int_t energy, Bool_t pu, Bool_t ov)
    : fDetID(DetID)
    , fStripID(StripID)
    , fTime(time)
    , fEnergy(energy)
    , fPileup(pu)
    , fOverflow(ov)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BTTTXMappedData);
