/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------
// -----                   R3BAlpideHitData                    -----
// -----          Created 28/01/2022 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#pragma once

#include <TObject.h>
#include <TVector3.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

class R3BAlpideHitData : public TObject
{
  public:
    // Default Constructor
    R3BAlpideHitData() = default;

    /** Standard Constructor
     *@param track    Reconstructed trajectory
     **/
    explicit R3BAlpideHitData(uint16_t sensorId, uint16_t clustersize, double posx, double posy, double posz = 0.);

    // Destructor
    virtual ~R3BAlpideHitData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const uint16_t GetSensorId() const { return fSensorId; }
    [[nodiscard]] inline const uint16_t GetClusterSize() const { return fClustersize; }
    [[nodiscard]] inline const double GetX() const { return fX; }
    [[nodiscard]] inline const double GetY() const { return fY; }
    [[nodiscard]] inline const double GetZ() const { return fZ; }
    [[nodiscard]] inline const double GetPosl() const { return fX; }
    [[nodiscard]] inline const double GetPost() const { return fY; }
    [[nodiscard]] inline const double GetTheta() const { return fTheta; }
    [[nodiscard]] inline const double GetPhi() const { return fPhi; }
    [[nodiscard]] inline const TVector3 GetTrack() const { return fTrack; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    uint16_t fSensorId = 0;
    uint16_t fClustersize = 0;
    double fX = std::nan("");
    double fY = std::nan("");
    double fZ = std::nan("");
    TVector3 fTrack;
    double fTheta = std::nan(""); // Reconstructed Theta
    double fPhi = std::nan("");   // Reconstructed Phi

  public:
    ClassDefOverride(R3BAlpideHitData, 1)
};

// Operator overloading for printing R3BAlpideHitData
std::ostream& operator<<(std::ostream& os, const R3BAlpideHitData& data);
