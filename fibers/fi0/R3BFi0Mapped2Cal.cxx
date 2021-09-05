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

#include "R3BFi0Mapped2Cal.h"

R3BFi0Mapped2Cal::R3BFi0Mapped2Cal(Int_t a_verbose, enum R3BTCalEngine::CTDCVariant a_variant)
    : R3BBunchedFiberMapped2Cal("Fi0", a_verbose, R3BTCalEngine::CTDC_8_12_FWD_250)
{
}

R3BFi0Mapped2Cal::~R3BFi0Mapped2Cal() {}

ClassImp(R3BFi0Mapped2Cal)
