/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------------
// -----             R3BTttxCal2Hit source file                     -----
// -----               Created 18/01/24  by XXX                        -----
// -------------------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include <Rtypes.h>
#include <TArrayF.h>
#include <TRandom.h>

#include "R3BTttxHitData.h"

class TClonesArray;
class R3BTttxHitPar;

class R3BTttxCal2Hit : public FairTask
{
  public:
    // Default constructor
    R3BTttxCal2Hit();

    // Standard constructor
    R3BTttxCal2Hit(const TString& name, int iVerbose = 1);

    // Destructor
    ~R3BTttxCal2Hit();

    // Method Exec
    void Exec(Option_t* /*option*/) override;

    // Method SetParContainers
    void SetParContainers() override;

    // Method Init
    InitStatus Init() override;

    // Method ReInit
    InitStatus ReInit() override;

    inline void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void Reset();
    void SetParameter();

    int NumDets = 0;

    bool fOnline = false; // Don't store data for online

    R3BTttxHitPar* fHit_Par = nullptr;
    TClonesArray* fTttxCalData = nullptr;
    TClonesArray* fTttxHitData = nullptr;

    // Private method AddHitData
    R3BTttxHitData* AddHitData(double xpos, double energy, double ang, double charge, double time);

  public:
    ClassDefOverride(R3BTttxCal2Hit, 1)
};
