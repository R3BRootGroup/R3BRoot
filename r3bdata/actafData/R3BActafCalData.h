/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----             R3BActafCalData source file               -----
// -----      Created 14/09/25 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

#pragma once

#include <R3BActafMappedData.h>
#include <TObject.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

class R3BActafCalData : public TObject
{
  public:
    // Default Constructor
    R3BActafCalData() = default;

    /** Standard Constructor
     *@param pad          Pad number
     *@param energy       Deposited energy for each pad
     *@param maxampl      Maximum energy amplitude for each pad
     *@param leadingedge  Leading time for each pad
     *@param zpos         Z-position for each pad
     *@param syntime      Synchronization time
     *@param trace        Trace after the application of the SG filter
     **/
    explicit R3BActafCalData(UInt_t pad,
                             double energy = 0.,
                             double maxampl = 0.,
                             double leadingedge = 0.,
                             double zpos = 0.,
                             double syntime = 0.,
                             const std::array<double, ACTAF_BINS>& trace = {});

    // Destructor
    virtual ~R3BActafCalData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const std::array<double, ACTAF_BINS>& GetTrace() const { return fTrace; }
    [[nodiscard]] inline const UInt_t& GetPad() const { return fPad; }
    [[nodiscard]] inline const double& GetEnergy() const { return fEnergy; }
    [[nodiscard]] inline const double& GetEMaxAmpl() const { return fEMaxAmpl; }
    [[nodiscard]] inline const double& GetLeadingEdgeTime() const { return fLeadingEdge; }
    [[nodiscard]] inline const double& GetZpos() const { return fZPos; }
    [[nodiscard]] inline const double& GetSynTime() const { return fSynTime; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    UInt_t fPad = 0;                 // Pad number
    double fEnergy = std::nan("");   // Energy
    double fEMaxAmpl = std::nan(""); // Energy for the max. of the signal
    double fLeadingEdge = 0.;        // in ns
    double fZPos = 0.;               // in cm
    double fSynTime = std::nan("");  // in ns
    std::array<double, ACTAF_BINS> fTrace{};

  public:
    ClassDefOverride(R3BActafCalData, 1);
};

// Operator overloading for printing R3BActafCalData
std::ostream& operator<<(std::ostream& os, const R3BActafCalData& data);
