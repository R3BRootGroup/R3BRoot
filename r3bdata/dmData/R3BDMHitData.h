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

class R3BDMHitData : public TObject
{

  public:
    /** Default constructor **/
    R3BDMHitData() = default;

    explicit R3BDMHitData(UInt_t detId, UInt_t pad, double pos_x, double pos_y, double time, double charge); // NOLINT

    /** Accessors **/
    [[nodiscard]] UInt_t GetDetId() const { return fDetId; }
    [[nodiscard]] UInt_t GetPad() const { return fPad; }
    [[nodiscard]] double GetPosX() const { return fPosX; }
    [[nodiscard]] double GetPosY() const { return fPosY; }
    [[nodiscard]] double GetTime() const { return fTime; }
    [[nodiscard]] double GetCharge() const { return fCharge; }

    /** Modifiers **/
    void SetDetId(UInt_t detId) { fDetId = detId; }
    void SetPad(UInt_t pad) { fPad = pad; }
    void SetPosX(double posX) { fPosX = posX; }
    void SetPosY(double posY) { fPosY = posY; }
    void SetTime(double time) { fTime = time; }
    void SetCharge(double charge) { fCharge = charge; }

  private:
    // Basic Hit information
    UInt_t fDetId = 0;
    UInt_t fPad = 0;
    double fPosX = std::nan("");
    double fPosY = std::nan("");
    double fTime = std::nan("");
    double fCharge = std::nan("");

  public:
    ClassDefOverride(R3BDMHitData, 1)
};
