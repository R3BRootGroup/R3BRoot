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

// ------------------------------------------------------------
// -----               R3BTofDMapped2CalPar               -----
// -----    Created 31/03/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#pragma once

#include <FairTask.h>

class R3BTCalPar;
class TClonesArray;
class R3BEventHeader;
class R3BTCalEngine;

class R3BTofDMapped2CalPar : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofDMapped2CalPar();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    explicit R3BTofDMapped2CalPar(const char*, int = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofDMapped2CalPar();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t*) override;

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    void FinishTask() override;

    /**
     * Method for setting the update rate for control histograms
     * @param rate an update rate value (events).
     */
    inline void SetUpdateRate(UInt_t);

    /**
     * Method for setting minimum required statistics per module.
     * Only detector modules with number of entries in TDC
     * distribution greater than minimum statistics will be
     * calibrated.
     * @param minStats a value of minimum statistics required.
     */
    inline void SetMinStats(UInt_t);

    /**
     * Method for setting number of TofD detectors and paddles.
     */
    inline void SetNofModules(Int_t, Int_t);

  private:
    UInt_t fUpdateRate = 1000000; // An update rate
    UInt_t fMinStats = 100;       // Minimum statistics required per module
    UInt_t Icount[5][48][4];      // NOLINT

    UInt_t fNofPlanes = 5;
    UInt_t fPaddlesPerPlane = 48;
    UInt_t fNofModules = fNofPlanes * fPaddlesPerPlane * 4;

    R3BTCalPar* fCalPar = nullptr;
    TClonesArray* fMapped = nullptr;
    TClonesArray* fMappedTrigger = nullptr;

    R3BTCalEngine* fEngine = nullptr;

  public:
    ClassDefOverride(R3BTofDMapped2CalPar, 1); // NOLINT
};
