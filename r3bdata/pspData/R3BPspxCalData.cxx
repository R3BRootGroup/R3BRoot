/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BPspxCalData.h"

R3BPspxCalData::R3BPspxCalData()
    : fStrip(-1)
    , fEnergy(-1)
    , fPos(-1)
{
}

R3BPspxCalData::R3BPspxCalData(Int_t strip, Float_t energy, Float_t pos)
    : fStrip(strip)
    , fEnergy(energy)
    , fPos(pos)
{
}

ClassImp(R3BPspxCalData)
