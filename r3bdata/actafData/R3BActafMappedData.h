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
// -----            R3BActafMappedData source file             -----
// -----      Created 14/02/25 by J.L. Rodriguez-Sanchez       -----
// -----      Modified by Pablo González Rusell (24/09/2025)   -----
// -----------------------------------------------------------------

#pragma once

#include <TObject.h>
#include <array>
#include <cstdint>
#include <iostream>
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
                                double baselineFilt = 0);

    /** Constructor for AMBER data
     *@param pad           Pad number
     *@param det_mask      Detector mask
     *@param timetag       AMBER timestamp
     *@param spill_nb      Spill on/off
     **/
    explicit R3BActafMappedData(UInt_t pad, int det_mask, int timetag, int spill_nb);

    virtual ~R3BActafMappedData() = default;

    [[nodiscard]] inline UInt_t GetPad() const { return fPad; }
    [[nodiscard]] inline const std::array<double, ACTAF_BINS>& GetTrace() const { return fTrace; }
    [[nodiscard]] inline double GetE() const { return fE; }
    [[nodiscard]] inline double GetBaseline() const { return fBaseline; }
    [[nodiscard]] inline double GetRisetime() const { return fRisetime; }
    [[nodiscard]] inline int GetMaxpos() const { return fMaxpos; }
    [[nodiscard]] inline double GetMaxampl() const { return fMaxamplitude; }
    [[nodiscard]] inline double GetLeadingEdgeTime() const { return fLeadingEdge10; }
    [[nodiscard]] inline double GetRms() const { return fRms; }
    [[nodiscard]] inline double GetRmsFilt() const { return fRmsFilt; }
    [[nodiscard]] inline double GetBaselineFilt() const { return fBaselineFilt; }
    [[nodiscard]] inline int GetTimeTag() const { return fTimeTag; }
    [[nodiscard]] inline int GetDetMask() const { return fDetMask; }

    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    UInt_t fPad = 0;
    double fE = 0., fBaseline = 0., fRisetime = 0.; // Energy, baseline, risetime
    int fMaxpos = 0;
    double fMaxamplitude = 0.;
    std::array<double, ACTAF_BINS> fTrace{};
    double fLeadingEdge10 = 0;
    double fRms = 0, fRmsFilt = 0, fBaselineFilt = 0;
    int fDetMask = 0, fTimeTag = 0, fSpillNb = 0;

  public:
    ClassDefOverride(R3BActafMappedData, 6);
};

std::ostream& operator<<(std::ostream& os, const R3BActafMappedData& data);
