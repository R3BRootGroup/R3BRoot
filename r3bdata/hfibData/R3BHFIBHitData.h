/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
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

#include <Rtypes.h>
#include <RtypesCore.h>
#include <TObject.h>
#include <cmath>

class R3BHFIBHitData : public TObject
{

  public:
    /** Default constructor **/
    R3BHFIBHitData() = default;

    explicit R3BHFIBHitData(UInt_t detId, UInt_t fibId, double pos, double time, double charge); // NOLINT

    /** Accessors **/
    [[nodiscard]] UInt_t GetDetId() const { return fDetId; }
    [[nodiscard]] UInt_t GetFiberId() const { return fFibId; }
    [[nodiscard]] double GetPos() const { return fPos; }
    [[nodiscard]] double GetTime() const { return fTime; }
    [[nodiscard]] double GetCharge() const { return fCharge; }

    /** Modifiers **/
    void SetDetId(UInt_t detId) { fDetId = detId; }
    void SetFiberId(UInt_t fibId) { fFibId = fibId; }
    void SetPos(double pos) { fPos = pos; }
    void SetTime(double tim) { fTime = tim; }
    void SetCharge(double charge) { fCharge = charge; }

  private:
    // Basic Hit information
    UInt_t fDetId = 0;
    UInt_t fFibId = 0;
    double fPos = std::nan("");
    double fTime = std::nan("");
    double fCharge = std::nan("");

  public:
    ClassDefOverride(R3BHFIBHitData, 1)
};
