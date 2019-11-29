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

#include "R3BTofdHitData.h"

// for the data analysis of the Tofd detector.
// Introduced by M.Heil, May 2016

R3BTofdHitData::R3BTofdHitData() {}

R3BTofdHitData::R3BTofdHitData(Double_t t,
                               Double_t x,
                               Double_t y,
                               Double_t Z,
                               Double_t tdiff,
                               Double_t ELoss,
                               Double_t ID)
    : R3BHit(ID, x, y, ELoss, tdiff)
{
}

ClassImp(R3BTofdHitData)
