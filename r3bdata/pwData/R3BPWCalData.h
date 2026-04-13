/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
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

#include "Rtypes.h"
#include "TObject.h"

class R3BPWCalData : public TObject
{
  public:
    /** Default constructor **/
    R3BPWCalData();

    explicit R3BPWCalData(UShort_t DetId, UShort_t Bar, double Time0, double Time1, double Tot0, double Tot1);

    /** Destructor **/
    ~R3BPWCalData() override = default;

    /** Accessors **/
    [[nodiscard]] UShort_t GetDetId() const { return fDetId; }
    [[nodiscard]] UShort_t GetChannelId() const { return fBar; }
    [[nodiscard]] double GetTime0() const { return fTime0; }
    [[nodiscard]] double GetTime1() const { return fTime1; }
    [[nodiscard]] double GetTot0() const { return fTot0; }
    [[nodiscard]] double GetTot1() const { return fTot1; }

  private:
    UShort_t fDetId; // Detector identifier
    UShort_t fBar;   // Bar unique identifier
    double fTime0;   // Time from 1st SiPM
    double fTime1;   // Time from 2nd SiPM
    double fTot0;    // Tot from 1st SiPM
    double fTot1;    // Tot from 2nd SiPM

  public:
    ClassDefOverride(R3BPWCalData, 1)
};
