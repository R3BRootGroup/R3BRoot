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
// -----         Created 17/05/25 by J.L. Rodriguez-Sanchez     -----
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

    // Accessor functions
    [[nodiscard]] inline const int GetNumDets() const { return fNumDets; }
    [[nodiscard]] inline const int GetNumParsFit() const { return fNumParsFit; }
    inline void SetNumDets(int ndet) { fNumDets = ndet; }
    TArrayF* GetCharCalParams() { return fCharCalPar; }

    // Method for setting the calibration parameters
    inline void SetCharCalPars(float value, int index) { fCharCalPar->AddAt(value, index); }
    inline void SetNumParsFit(int npar) { fNumParsFit = npar; }

  private:
    int fNumDets = 8;     // Number of detectors
    int fNumParsFit = 2;  // Number of parameters of for the calibration
    TArrayF* fCharCalPar; // Parameters for the calibration charge vs energy

    const R3BFootHitPar& operator=(const R3BFootHitPar&);
    R3BFootHitPar(const R3BFootHitPar&);

  public:
    ClassDefOverride(R3BFootHitPar, 1); // NOLINT
};
