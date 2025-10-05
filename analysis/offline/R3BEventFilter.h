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

// ----------------------------------------------------------------
// -----                R3BEventFilter                        -----
// -----     Created 05/10/25 by J.L. Rodriguez-Sanchez       -----
// ----------------------------------------------------------------

/*
 * This class selects which events should be stored in the ROOT file.
 * It acts as a filter, deciding whether an event is kept or discarded.
 */

#pragma once

// ROOT headers
#include <TCutG.h>
#include <TMath.h>
#include <TString.h>

// FAIR headers
#include <FairTask.h>

// R3B headers
#include "R3BEventHeader.h"
#include "R3BFrsData.h"

class TClonesArray;

class R3BEventFilter : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BEventFilter();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    explicit R3BEventFilter(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    ~R3BEventFilter() = default;

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    InitStatus ReInit() override;
    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t* option) override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent() override;

    void SetParContainers() override;

    inline void SetTCutFrsId(TCutG* tcut) { fCutFrsId = tcut; };
    inline void SetChargeLimits(const std::vector<std::vector<double>>& vec) { fChargeLimits = vec; }
    inline void SetTofd() { fUseTofd = true; }
    inline void UseTwoPlanes() { fUseAllPlanes = false; }

  private:
    void SetParameter();
    void StoreEvent(bool valid = true);

    R3BEventHeader* fHeader = nullptr;
    TClonesArray* fTofdHit = nullptr;
    TClonesArray* fFrsData = nullptr;

    TCutG* fCutFrsId = nullptr;
    std::vector<std::vector<double>> fChargeLimits;

    bool fUseTofd = false;
    bool fUseAllPlanes = true;

  public:
    ClassDefOverride(R3BEventFilter, 1); // NOLINT
};
