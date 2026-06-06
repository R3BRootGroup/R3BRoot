/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------
// -----                 R3BMwpc0Mapped2Cal                   -----
// -----        Created 07/10/19 by J.L. Rodriguez-Sanchez    -----
// ----------------------------------------------------------------

#pragma once

#include "R3BMwpcCalData.h"
#include <FairTask.h>
#include <TArrayF.h>
#include <string>

class TClonesArray;
class R3BMwpc0CalPar;

class R3BMwpc0Mapped2Cal : public FairTask
{
  public:
    // Default constructor
    R3BMwpc0Mapped2Cal();

    // Standard constructor
    explicit R3BMwpc0Mapped2Cal(const std::string& name, int iVerbose = 1);

    // Destructor
    virtual ~R3BMwpc0Mapped2Cal();

    // Method Exec
    void Exec(Option_t* /*opt*/) override;

    // Virtual method Reset
    virtual void Reset();

    void SetParContainers() override;

    // Method Init
    InitStatus Init() override;

    // Method ReInit
    InitStatus ReInit() override;

    void SetOnline(bool option = true) { fOnline = option; }

  private:
    void SetParameter();

    Int_t NumPadX = 64;
    Int_t NumPadY = 64;
    Int_t NumParams = 2;
    TArrayF* CalParams;

    bool fOnline = false; // Don't store data for online

    R3BMwpc0CalPar* fCal_Par = nullptr;        // Parameter container
    TClonesArray* fMwpcMappedDataCA = nullptr; // Array with Mapped input data
    TClonesArray* fMwpcCalDataCA = nullptr;    // Array with Cal output data

    // Private method AddCalData
    R3BMwpcCalData* AddCalData(Int_t plane, Int_t pad, Float_t charge);

  public:
    // Class definition
    ClassDefOverride(R3BMwpc0Mapped2Cal, 1); // NOLINT
};
