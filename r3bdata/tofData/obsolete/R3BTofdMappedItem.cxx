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

#include "R3BTofdMappedItem.h"

// for the data analysis of the Tofd detector.
// Introduced by Ralf, March 2016

R3BTofdMappedItem::R3BTofdMappedItem()
    : fPaddle(0)
    , fPm(0)
    , fTimeCoarse(0)
    , fTimeFine(0)
{
}

R3BTofdMappedItem::R3BTofdMappedItem(UChar_t paddle, UChar_t pm, UInt_t timeCoarse, UInt_t timeFine)
    : fPaddle(paddle)
    , fPm(pm)
    , fTimeCoarse(timeCoarse)
    , fTimeFine(timeFine)
{
}

ClassImp(R3BTofdMappedItem)
