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

#ifndef R3BFI23BCAL2HIT
#define R3BFI23BCAL2HIT

#include "R3BBunchedFiberCal2Hit_s494.h"

class R3BFi23bCal2Hit : public R3BBunchedFiberCal2Hit_s494
{
  public:
    R3BFi23bCal2Hit(enum R3BTCalEngine::CTDCVariant, Direction, Bool_t, Bool_t, Bool_t, Int_t = 1);
    virtual ~R3BFi23bCal2Hit();
    UInt_t FixMistake(UInt_t);

    ClassDef(R3BFi23bCal2Hit, 1)
};

#endif
