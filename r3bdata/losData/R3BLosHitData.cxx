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

#include "R3BLosHitData.h"

// for the data analysis of the Los detector.

// Introduced by M.Heil, May 2016

R3BLosHitData::R3BLosHitData() {}

R3BLosHitData::R3BLosHitData(Int_t detId, Double_t x, Double_t y, Double_t Eloss, Double_t t, Int_t hitId)
    : R3BHit(detId, x, y, Eloss, t)
{
}
ClassImp(R3BLosHitData)
    /*
    // Introduced by Ralf, Feb 2016

    R3BLosHitData::R3BLosHitData()
        : fDetector(0)
        , fTime_ns(0)
        , fX_cm(0)
        , fY_cm(0)
        , fZ(0)
    {
    }

    R3BLosHitData::R3BLosHitData(UChar_t detector, Double_t t, Double_t x, Double_t y, Double_t Z)

        : fDetector(detector)
        , fTime_ns(t)
        , fX_cm(x)
        , fY_cm(y)
        , fZ(Z)
    {
    }

    ClassImp(R3BLosHitData)
    */
