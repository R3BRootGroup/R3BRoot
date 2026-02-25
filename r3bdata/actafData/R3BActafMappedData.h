/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025-2026 Members of R3B Collaboration                     *
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
// -----            R3BActafMappedData source file             -----
// -----      Created 14/02/25 by J.L. Rodriguez-Sanchez       -----
// -----      Modified by Pablo González Rusell (24/09/2025)   -----
// -----------------------------------------------------------------

#pragma once

#include <Rtypes.h>
#include <RtypesCore.h>
#include <TObject.h>

#include <array>
#include <cmath>
#include <cstdint>
#include <iosfwd>
#include <string>

constexpr int ACTAF_ECHN = 16;
constexpr int ACTAF_BINS = 2500;

class R3BActafMappedData : public TObject
{
  public:
    R3BActafMappedData() = default;

    /** Standard Constructor
     *@param pad           Pad number
     *@param trace         Trace signal for each pad
     *@param energy        Energy for each pad
     *@param baseline      Baseline for each pad
     *@param riseTime      Rise time of the signal after baseline subtraction
     *@param maxpos        Position of the maximum amplitude for the signal
     *@param maxamplitude  Max. amplitude after baseline subtraction
     *@param leadingedge10 Time to the beginning of the pulse
     *@param rms           RMS of the baseline
     *@param rmsFilt       RMS of the baseline after filtering
     *@param baselineFilt  Baseline for each pad after filtering
     *@param maw           Maw trigger parameter
     **/

    explicit R3BActafMappedData(UInt_t pad,
                                const std::array<double, ACTAF_BINS>& trace,
                                double energy = 0.,
                                double baseline = 0.,
                                double risetime = 0.,
                                int maxpos = 0,
                                double maxamplitude = 0.,
                                double leadingedge10 = 0.,
                                double rms = 0.,
                                double rmsFilt = 0.,
                                double baselineFilt = 0,
                                double maw = 0.);

    /** Constructor for AMBER data
     *@param pad           Pad number
     *@param det_mask      Detector mask
     *@param timetag       AMBER timestamp
     *@param spill_nb      Spill on/off
     **/
    explicit R3BActafMappedData(UInt_t pad,
                                int det_mask,
                                uint64_t timetag,
                                int spill_nb,
                                uint64_t rawtimetag); // NOLINT

    [[nodiscard]] UInt_t GetPad() const { return fPad; }
    [[nodiscard]] const std::array<double, ACTAF_BINS>& GetTrace() const { return fTrace; }
    [[nodiscard]] double GetE() const { return fE; }
    [[nodiscard]] double GetBaseline() const { return fBaseline; }
    [[nodiscard]] double GetRisetime() const { return fRisetime; }
    [[nodiscard]] int GetMaxpos() const { return fMaxpos; }
    [[nodiscard]] double GetMaxampl() const { return fMaxamplitude; }
    [[nodiscard]] double GetLeadingEdgeTime() const { return fLeadingEdge10; }
    [[nodiscard]] double GetRms() const { return fRms; }
    [[nodiscard]] uint64_t GetTimeTag() const { return fTimeTag; }
    [[nodiscard]] uint64_t GetRawTimeTag() const { return fRawTimeTag; }
    [[nodiscard]] int GetDetMask() const { return fDetMask; }
    [[nodiscard]] int GetSpillNb() const { return fSpillNb; }
    [[nodiscard]] double GetMaw() const { return fMaw; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t* /*option*/) const override;

  private:
    UInt_t fPad = 0;
    double fE = 0., fBaseline = 0., fRisetime = 0.; // Energy, baseline, risetime
    int fMaxpos = 0;
    double fMaxamplitude = 0.;
    std::array<double, ACTAF_BINS> fTrace{};
    double fLeadingEdge10 = 0;
    double fRms = 0, fRmsFilt = 0, fBaselineFilt = 0;
    int fDetMask = 0, fSpillNb = 0;
    uint64_t fTimeTag = 0;
    uint64_t fRawTimeTag = 0;
    double fMaw = 0.;

  public:
    ClassDefOverride(R3BActafMappedData, 7);
};

// Operator overloading for printing R3BActafMappedData
std::ostream& operator<<(std::ostream& output, const R3BActafMappedData& data);
