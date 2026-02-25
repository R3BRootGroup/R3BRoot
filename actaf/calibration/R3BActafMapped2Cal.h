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

// -------------------------------------------------------------
// -----               R3BActafMapped2Cal                 -----
// -----     Created 14/09/25 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include "R3BActafCalData.h"
#include "R3BActafMappedData.h"

#include <Rtypes.h>
#include <vector>

class TClonesArray;
class R3BActafCalPar;

class R3BActafMapped2Cal : public FairTask
{
  public:
    /** Default constructor **/
    R3BActafMapped2Cal();

    /** Standard constructor **/
    R3BActafMapped2Cal(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BActafMapped2Cal() override;

    /** Method Exec **/
    void Exec(Option_t*) override;

    /** Virtual method Reset **/
    virtual void Reset();

    /** Method Init **/
    InitStatus Init() override;

    /** Method ReInit **/
    InitStatus ReInit() override;

    void SetParContainers() override;

    // Method to setup online mode
    inline void SetOnline(bool option = true) { fOnline = option; }

    // Method to time converter
    inline void SetTimeConversion(double opt) { fConversionCh2ns = opt; }

    // Method to define the velocity
    inline void SetVelocity(double opt) { fVelocity = opt; }

    // Method to apply SG filter
    void SetSGFilter(bool use = true) { fApplySGFilter = use; }

    // Method to set pulser channel/pad
    void SetPulserChannel(int chn) { fPulserCh = chn; }

    // Method to set max integral
    void SetMaxEIntegral(int max) { fMaxE = max; }

  private:
    void SetParameter();

    static constexpr int fPad = 128;
    bool fOnline = false; // Don't store data for online
    bool fApplySGFilter = false;
    int fPulserCh = 62;
    int fMaxE = 300000;
    double fConversionCh2ns = 51.44; // in ns/bin
    double fVelocity = 0.02888;      // in cm/ns

    TClonesArray* fActafMappedData = nullptr; // Array with Actaf Mapped input data
    TClonesArray* fActafCalData = nullptr;    // Array with Actaf Cal output data

    R3BActafCalPar* fCal_Par = nullptr; // Parameter container
    std::vector<double> fEGain;
    std::vector<double> fEThr;
    std::vector<double> fSgCoeffs;
    double fNbSgCoeffs = 0;
    bool fDisplayTrace = true;

    // Private method AddCalData
    R3BActafCalData* AddCalData(UInt_t padId,
                                double energy,
                                double maxampl,
                                double drift,
                                double zpos,
                                double syntime,
                                const std::array<double, ACTAF_BINS>& trace,
                                double rmsRaw,
                                double rms,
                                double meanRaw,
                                double mean,
                                double maw);

  public:
    // Class definition
    ClassDefOverride(R3BActafMapped2Cal, 2);
};
