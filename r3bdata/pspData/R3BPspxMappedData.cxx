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

#include "R3BPspxMappedData.h"

R3BPspxMappedData::R3BPspxMappedData()
    : fDetector(0)
    , fChannel(0)
    , fEnergy(0)
{
}

R3BPspxMappedData::R3BPspxMappedData(UShort_t detector, UShort_t channel, Int_t energy)
    : fDetector(detector)
    , fChannel(channel)
    , fEnergy(energy)
{
}

ClassImp(R3BPspxMappedData)
