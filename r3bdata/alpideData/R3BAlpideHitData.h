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
    explicit R3BAlpideHitData(uint16_t sensorId,
                              uint16_t clustersize,
                              uint16_t genus,
                              double elong,
                              TVector3 majorUnit,
                              double holePerimeter,
                              double totalPerimeter,
                              std::vector<double> hu,
                              double posx,
                              double posy,
                              double posz = std::nan(""),
                              double locposx = std::nan(""),
                              double locposy = std::nan(""));

    // Destructor
    virtual ~R3BAlpideHitData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const uint16_t GetSensorId() const { return fSensorId; }
    [[nodiscard]] inline const uint16_t GetClusterSize() const { return fClustersize; }
    [[nodiscard]] inline const uint16_t GetGenus() const { return fGenus; }
    [[nodiscard]] inline const uint16_t GetElong() const { return fElong; }
    [[nodiscard]] inline const TVector3 GetMajorUnit() const { return fMajorUnit; }
    [[nodiscard]] inline const double GetHolePerimeter() const { return fHolePerimeter; }
    [[nodiscard]] inline const double GetTotalPerimeter() const { return fTotalPerimeter; }
    [[nodiscard]] inline const std::vector<double> GetHu() const { return fHu; }
    [[nodiscard]] inline const double GetX() const { return fX; }
    [[nodiscard]] inline const double GetY() const { return fY; }
    [[nodiscard]] inline const double GetZ() const { return fZ; }
    [[nodiscard]] inline const double GetPosl() const { return fXloc; }
    [[nodiscard]] inline const double GetPost() const { return fYloc; }
    [[nodiscard]] inline const double GetTheta() const { return fTheta; }
    [[nodiscard]] inline const double GetPhi() const { return fPhi; }
    [[nodiscard]] inline const TVector3 GetTrack() const { return fTrack; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    uint16_t fSensorId = 0;
    uint16_t fClustersize = 0;
    uint16_t fGenus = 0;
    double fElong = 0;
    TVector3 fMajorUnit;
    double fHolePerimeter;
    double fTotalPerimeter;
    std::vector<double> fHu;
    double fX = std::nan("");
    double fY = std::nan("");
    double fZ = std::nan("");
    double fXloc = std::nan("");
    double fYloc = std::nan("");
    TVector3 fTrack;
    double fTheta = std::nan(""); // Reconstructed Theta
    double fPhi = std::nan("");   // Reconstructed Phi

  public:
    // Class definition
    ClassDefOverride(R3BAlpideHitData, 1); // NOLINT
};

// Operator overloading for printing R3BAlpideHitData
std::ostream& operator<<(std::ostream& os, const R3BAlpideHitData& data);
