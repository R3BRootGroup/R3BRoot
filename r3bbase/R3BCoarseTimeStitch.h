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

#ifndef R3BCoarseTimeStitch_H
#define R3BCoarseTimeStitch_H 1

#include "TObject.h"
#include "TString.h"
#include <Rtypes.h>

class R3BCoarseTimeStitch : public TObject
{
  public:
    // Default Constructor for sync two tamex channels
    R3BCoarseTimeStitch();

    // Destructor
    virtual ~R3BCoarseTimeStitch() {}

    Float_t GetRange1() const { return fRange1; }
    Float_t GetRange2() const { return fRange2; }
    Double_t GetTime(Double_t, TString name1 = "tamex", TString name2 = "tamex");

    void SetRange1(Float_t range) { fRange1 = range; }
    void SetRange2(Float_t range) { fRange2 = range; }

    void SetClockTDC150() { fRangeClockTDC = fRangeClockTDC150; }

  private:
    Double_t fRange1;
    Double_t fRange2;
    Double_t fRangeTamex;
    Double_t fRangeVftx;
    Double_t fRangeTrb;
    Double_t fRangeClockTDC;
    Double_t fRangeClockTDC150;

  public:
    ClassDef(R3BCoarseTimeStitch, 1)
};

#endif
