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

#include "R3BPspxCalData.h"

R3BPspxCalData::R3BPspxCalData()
    : fDetector(0)
    , fStrip(0)
    , fEnergy1(0)
    , fEnergy2(0)
{
}

R3BPspxCalData::R3BPspxCalData(UShort_t detector, UShort_t strip, Float_t energy1, Float_t energy2)
    : fDetector(detector)
    , fStrip(strip)
    , fEnergy1(energy1)
    , fEnergy2(energy2)
{
}

ClassImp(R3BPspxCalData)
