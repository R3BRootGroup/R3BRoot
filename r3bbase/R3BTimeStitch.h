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

#ifndef R3BTimeStitch_H
#define R3BTimeStitch_H 1

#include "TObject.h"
#include "TString.h"
#include <Rtypes.h>

class R3BTimeStitch : public TObject
{
  public:
    // Default Constructor for sync two tamex channels
    R3BTimeStitch();

    // Destructor
    virtual ~R3BTimeStitch() {}

    Float_t GetRange1() const { return fRange1; }
    Float_t GetRange2() const { return fRange2; }
    Double_t GetTime(Double_t) const;
    Double_t GetTime(Double_t, TString, TString);

    void SetRange1(Float_t range) { fRange1 = range; }
    void SetRange2(Float_t range) { fRange2 = range; }

  private:
    Double_t fRange1;
    Double_t fRange2;
    Double_t fRangeTamex;
    Double_t fRangeVftx;
    Double_t fRangeTrb;
    Double_t fRangeClockTDC;

  public:
    ClassDef(R3BTimeStitch, 1)
};

#endif
