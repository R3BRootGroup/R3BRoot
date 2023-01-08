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

#include "R3BTimeStitch.h"
#include "R3BLogger.h"

#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)

// Standard constructur
R3BTimeStitch::R3BTimeStitch()
    : fRange1(2048 * 5.)        // ns
    , fRange2(2048 * 5.)        // ns
    , fRangeTamex(2048 * 5.)    // ns
    , fRangeTrb(2048 * 5.)      // ns
    , fRangeVftx(8192 * 5.)     // ns
    , fRangeClockTDC(4096 * 5.) // ns
{
}

Double_t R3BTimeStitch::GetTime(Double_t time) const
{
    // R3BLOG_IF(error, IS_NAN(time), "Providing NaN time!");
    R3BLOG(debug, "Time: " << time << " , range1: " << fRange1 << " , range2: " << fRange2);
    return fmod(time + fRange2 + fRange1 / 2., fRange1) - fRange1 / 2.;
}

Double_t R3BTimeStitch::GetTime(Double_t time, TString name1, TString name2)
{
    if (name1 == "tamex")
    {
        fRange1 = fRangeTamex;
    }
    else if (name1 == "trb")
    {
        fRange1 = fRangeTrb;
    }
    else if (name1 == "vftx")
    {
        fRange1 = fRangeVftx;
    }
    else if (name1 == "clocktdc")
    {
        fRange1 = fRangeClockTDC;
    }

    if (name2 == "tamex")
    {
        fRange2 = fRangeTamex;
    }
    else if (name2 == "trb")
    {
        fRange2 = fRangeTrb;
    }
    else if (name2 == "vftx")
    {
        fRange2 = fRangeVftx;
    }
    else if (name2 == "clocktdc")
    {
        fRange2 = fRangeClockTDC;
    }

    Double_t c1 = TMath::Min(fRange1, fRange2);
    Double_t c2 = TMath::Max(fRange1, fRange2);

    R3BLOG(debug,
           "Time: " << time << " , range1(" << name1 << "): " << fRange1 << " , range2(" << name1 << "): " << fRange2);
    return fmod(time + c2 + c1 / 2., c1) - c1 / 2.;
}

ClassImp(R3BTimeStitch);
