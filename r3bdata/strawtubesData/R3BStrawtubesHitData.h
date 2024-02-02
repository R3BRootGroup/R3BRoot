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

#ifndef R3BSTRAWTUBESHITDATA_H
#define R3BSTRAWTUBESHITDATA_H

#include "TObject.h"

class R3BStrawtubesCalData;

class R3BStrawtubesHitData : public TObject
{
  public:
    R3BStrawtubesHitData(R3BStrawtubesCalData const&);
    virtual ~R3BStrawtubesHitData();

  private:
    UChar_t fPlane;
    UChar_t fStraw;
    Double_t fAverage_ns;
    Double_t fDiff_ns;

  public:
    ClassDef(R3BStrawtubesHitData, 0)
};

#endif
