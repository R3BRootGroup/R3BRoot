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

#pragma once

#include <TObject.h>
#include <cstdint>
#include <iostream>
#include <string>

class R3BActafMappedData : public TObject
{
  public:
    // Default Constructor
    R3BActafMappedData() = default;

    /** Standard Constructor
     *@param pad       Pad number
     *@param trace     Trace signal for each pad
     *@param energy    Energy for each pad
     *@param baseline  Baseline for each pad
     *@param riseTime  Rise time of the signal after baseline subtraction
     *@param maxpos        Position of the maximum amplitud for the signal
     *@param maxamplitude  Max. amplitude after baseline subtraction
     **/
    explicit R3BActafMappedData(UInt_t pad,
                                std::vector<UInt_t> trace,
                                double energy = 0.,
                                double baseline = 0.,
                                int risetime = 0,
                                int maxpos = 0,
                                double maxamplitude = 0.);

    // Destructor
    virtual ~R3BActafMappedData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const UInt_t& GetPad() const { return fPad; }
    [[nodiscard]] inline const std::vector<UInt_t>& GetTrace() const { return fTrace; }
    [[nodiscard]] inline const double& GetE() const { return fE; }
    [[nodiscard]] inline const double& GetBaseline() const { return fBaseline; }
    [[nodiscard]] inline const int& GetRisetime() const { return fRisetime; }
    [[nodiscard]] inline const int& GetMaxpos() const { return fMaxpos; }
    [[nodiscard]] inline const double& GetMaxampl() const { return fMaxamplitude; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    UInt_t fPad = 0;                // Pad number
    double fE = 0., fBaseline = 0.; // Energy and baseline
    int fRisetime = 0, fMaxpos = 0;
    double fMaxamplitude = 0.;
    std::vector<UInt_t> fTrace; // Trace signal for each channel

  public:
    ClassDefOverride(R3BActafMappedData, 2)
};

// Operator overloading for printing R3BActafMappedData
std::ostream& operator<<(std::ostream& os, const R3BActafMappedData& data);
