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

// ------------------------------------------------------------
// -----              R3BFiberMAPMTMapped2Cal             -----
// -----         Created May 13th 2021 by V. Panin        -----
// -----     Convert mapped data to time calibrated data  -----
// ------------------------------------------------------------

#pragma once

#include "R3BTCalEngine.h"

#include <FairTask.h>
#include <TClonesArray.h>

class R3BTCalPar;

/**
 * An analysis task to apply TCAL calibration.
 * This class reads mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BFiberMAPMTMapped2CalPar task.
 */
class R3BFiberMAPMTMapped2Cal : public FairTask
{
  public:
    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     * @param a_variant CTDC firmware variant, see R3BTCalEngine.
     * @param a_skip_spmt Don't process SPMT side for pure MAPMT tests.
     */
    explicit R3BFiberMAPMTMapped2Cal(const std::string&, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BFiberMAPMTMapped2Cal();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    /**
     * Method for initialization of the parameter containers.
     * Called by the framework prior to Init() method.
     */
    void SetParContainers() override;

    /**
     * Method for re-initialization of parameter containers
     * in case the Run ID has changed.
     */
    InitStatus ReInit() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t* /*option*/) override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent() override;

    // Accessor to select online mode
    inline void SetOnline(bool option = true) { fOnline = option; }

  private:
    TString fName;
    R3BTCalPar* fMAPMTTCalPar = nullptr;
    R3BTCalPar* fMAPMTTrigTCalPar = nullptr;
    TClonesArray* fMappedItems = nullptr;
    TClonesArray* fCalItems = nullptr;
    TClonesArray* fCalTriggerItems = nullptr;
    double fClockFreq = 1000. / 150;
    // Don't store data for online
    bool fOnline = false;
    unsigned long long fnEvents = 0;

  public:
    // Class definition
    ClassDefOverride(R3BFiberMAPMTMapped2Cal, 1); // NOLINT
};
