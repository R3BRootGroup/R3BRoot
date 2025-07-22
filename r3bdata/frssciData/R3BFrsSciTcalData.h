/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include <TObject.h>

class R3BFrsSciTcalData : public TObject
{
  public:
    // Default Constructor
    R3BFrsSciTcalData() = default;

    // Standard Constructor
    R3BFrsSciTcalData(UShort_t, UShort_t, Double_t, UInt_t);

    // Destructor
    virtual ~R3BFrsSciTcalData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetPmt() const { return fPmt; }
    inline const Double_t& GetRawTimeNs() const { return fRawTimeNs; }
    inline const UInt_t& GetTimeCoarse() const { return fTimeCoarse; }

  private:
    UShort_t fDetector = 0;   // 1..n with n number of sci from S2 to cave C
    UShort_t fPmt = 0;        // 1..3
    Double_t fRawTimeNs = 0.; // (5*coarse_time - fine_time)
    UInt_t fTimeCoarse = 0;

  public:
    ClassDef(R3BFrsSciTcalData, 3); // NOLINT
};
