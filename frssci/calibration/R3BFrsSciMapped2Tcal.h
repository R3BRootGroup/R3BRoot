/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// *** *************************************************************** *** //
// ***                  R3BFrsSciMapped2Tcal                           *** //
// ***    convert Mapped data to tcal data                             *** //
// ***    from the fine and coarse times, calculate a raw time in ns   *** //
// *** *************************************************************** *** //

#pragma once

#include <FairTask.h>
#include <TClonesArray.h>
#include <TRandom3.h>

#include "R3BFrsSciTcalData.h"
#include "R3BFrsSciTcalPar.h"

class TRandom3;
class R3BFrsSciTcalData;

class R3BFrsSciMapped2Tcal : public FairTask
{
  public:
    // --- Default constructor --- //
    R3BFrsSciMapped2Tcal();

    // --- Standard constructor --- //
    R3BFrsSciMapped2Tcal(const char* name, Int_t iVerbose = 1);

    // --- Destructor --- //
    virtual ~R3BFrsSciMapped2Tcal();

    virtual void Exec(Option_t* option);

    virtual void SetParContainers();

    virtual InitStatus Init();

    virtual InitStatus ReInit();

    virtual void Reset();

    void SetOnline(bool option = true) { fOnline = option; }

  private:
    UInt_t fNevent = 0;
    TClonesArray* fMapped = nullptr;      // input data - FrsSci
    R3BFrsSciTcalPar* fTcalPar = nullptr; // tcal parameters container - FrsSci
    TClonesArray* fTcal = nullptr;        // output data
    bool fOnline = false;                 // Don't store data for online

    TRandom3 rand;

    Double_t CalculateTimeNs(UShort_t det, UShort_t pmt, UInt_t tf, UInt_t tc);
    R3BFrsSciTcalData* AddTcalData(UShort_t det, UShort_t pmt, Double_t tns, UInt_t tc);

  public:
    ClassDef(R3BFrsSciMapped2Tcal, 1); // NOLINT
};
