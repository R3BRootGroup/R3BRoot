/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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
#include <TObject.h>
#include <cmath>
#include <cstdint>

// for the data analysis of the Tofd detectors.
// Introduced by M.Heil, May 2016
// Modified by Pablo González Rusell, Oct 2025

class R3BTofdHitData : public TObject
{
  public:
    // Default Constructor
    R3BTofdHitData() = default;

    // Standard Constructor
    R3BTofdHitData(Double_t t,
                   Double_t x,
                   Double_t y,
                   Double_t Z,
                   Double_t tdiff,
                   Double_t ELoss = 0,
                   UInt_t ID = 0,
                   UInt_t iBar = 0,
                   Double_t traw = -1000,
                   Double_t tof = 0.);

    // Destructor
    virtual ~R3BTofdHitData() = default;

    [[nodiscard]] Double_t GetTime() const { return fTime; }
    [[nodiscard]] Double_t GetX() const { return fXPos; }
    [[nodiscard]] Double_t GetY() const { return fYPos; }
    [[nodiscard]] Double_t GetChargeZ() const { return fZCharge; }
    [[nodiscard]] Double_t GetTimeDiff() const { return fTimeDiff; }
    [[nodiscard]] Double_t GetEloss() const { return fELoss; }
    [[nodiscard]] UInt_t GetPlaneId() const { return fPlaneId; }
    [[nodiscard]] UInt_t GetBarId() const { return fBarId; }
    [[nodiscard]] Double_t GetTimeRaw() const { return fTimeRaw; }
    [[nodiscard]] Double_t GetTof() const { return fTof; }

    void SetEloss(const Double_t eloss) { fELoss = eloss; }
    void SetTime(const Double_t time) { fTime = time; }

    // Deprecated methods
    [[deprecated("Use GetPlaneId() instead.")]] UInt_t GetDetId() const { return fPlaneId; }
    [[deprecated]] UInt_t GetHitId() const { return -1; }

  private:
    Double_t fTime = std::nan("");
    Double_t fXPos = std::nan("");
    Double_t fYPos = std::nan("");
    Double_t fZCharge = std::nan("");
    Double_t fTimeDiff = std::nan("");
    Double_t fELoss = std::nan("");
    UInt_t fPlaneId = 0;
    UInt_t fBarId = 0;
    Double_t fTimeRaw = -1000;
    Double_t fTof = 0.;

  public:
    ClassDef(R3BTofdHitData, 4);
};
