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

#ifndef R3BFI5CAL2HIT
#define R3BFI5CAL2HIT

#include "R3BBunchedFiberCal2Hit.h"

class R3BFi5Cal2Hit : public R3BBunchedFiberCal2Hit
{
  public:
    R3BFi5Cal2Hit(Bool_t, Bool_t, Bool_t, Int_t = 1);
    virtual ~R3BFi5Cal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi5Cal2Hit, 1)
};

#endif
