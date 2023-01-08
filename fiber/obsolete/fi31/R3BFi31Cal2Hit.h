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

#ifndef R3BFI31CAL2HIT
#define R3BFI31CAL2HIT

#include "R3BBunchedFiberCal2Hit_s494.h"

class R3BFi31Cal2Hit : public R3BBunchedFiberCal2Hit_s494
{
  public:
    R3BFi31Cal2Hit(enum R3BTCalEngine::CTDCVariant, Bool_t, Bool_t, Bool_t, Int_t = 1);
    virtual ~R3BFi31Cal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi31Cal2Hit, 1)
};

#endif
