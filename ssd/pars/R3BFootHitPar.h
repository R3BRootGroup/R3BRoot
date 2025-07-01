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

// ------------------------------------------------------------------
// -----                R3BFootHitPar source file               -----
// -----         Created 17/05/25 by Pablo González Rusell      -----
// ------------------------------------------------------------------

#pragma once

#include <FairParGenericSet.h>

#include <Rtypes.h>
#include <TArrayF.h>
#include <TObject.h>

class FairParamList;

class R3BFootHitPar : public FairParGenericSet
{
  public:
    // Standard constructor
    R3BFootHitPar(const char* name = "footHitPar",
                  const char* title = "FOOT Hit Parameters",
                  const char* context = "FootHitParContext");

    // Destructor
    virtual ~R3BFootHitPar();

    // Method to reset all parameters
    void clear() override;

    // Method to store all parameters using FairRuntimeDB
    void putParams(FairParamList* list) override;

    // Method to retrieve all parameters using FairRuntimeDB
    Bool_t getParams(FairParamList* list) override;

    // Method to print values of parameters to the standard output
    void print() override;

    // Getters
    [[nodiscard]] inline const int GetNumDets() const { return fNumDets; }
    [[nodiscard]] inline const int GetNumParsFit() const { return fNumParsCal; }
    [[nodiscard]] inline const int GetNumParsEtaCorr() const { return fNumParsEtaCorr; }

    [[nodiscard]] inline const TArrayF* GetMultCharParams() { return fMultCharPar; }
    [[nodiscard]] inline const TArrayF* GetCharCalParams() { return fCharCalPar; }
    [[nodiscard]] inline const TArrayF* GetCharCalParamsSM() { return fCharCalParSM; }
    [[nodiscard]] inline const TArrayF* GetEtaCorrParams() { return fEtaCorrPar; }

    // Setters
    inline void SetNumDets(int ndet) { fNumDets = ndet; }
    inline void SetNumParsCal(int npar) { fNumParsCal = npar; }
    inline void SetNumParsEtaCorr(int npar) { fNumParsEtaCorr = npar; }

    inline void SetCharCalPars(float value, int index) { fCharCalPar->AddAt(value, index); }
    inline void SetCharCalParsSM(float value, int index) { fCharCalParSM->AddAt(value, index); }
    inline void SetEtaCorrParams(float value, int index) { fEtaCorrPar->AddAt(value, index); }
    inline void SetMultCharParams(float value, int index) { fMultCharPar->AddAt(value, index); }

  private:
    int fNumDets = 8;        // Number of detectors
    int fNumParsCal = -1;    // Number of parameters of for the calibration
    int fNumParsEtaCorr = 5; // Number of parameters for the eta correction

    TArrayF* fMultCharPar;  // Number of charge states at every asic
    TArrayF* fCharCalPar;   // Parameters for the calibration charge vs energy
    TArrayF* fCharCalParSM; // Parameters for the calibration charge vs energy (single strip case)
    TArrayF* fEtaCorrPar;   // Parameters for the eta correction

    const R3BFootHitPar& operator=(const R3BFootHitPar&);
    R3BFootHitPar(const R3BFootHitPar&);

  public:
    ClassDefOverride(R3BFootHitPar, 1); // NOLINT
};
