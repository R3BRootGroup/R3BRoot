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

// ----------------------------------------------------------------
// -----     Create time calib parameters for LOS via VFTX    -----
// -----      Adopted Feb 4th 2016 from Neuland by R. Plag    -----
// ----------------------------------------------------------------

#pragma once

#include "FairTask.h"

class R3BTCalPar;
class TClonesArray;
class R3BEventHeader;
class R3BTCalEngine;

/**
 * An analysis task for TCAL calibration of Tamex and VFTX modules
 */

class R3BLosMapped2CalPar : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BLosMapped2CalPar();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    explicit R3BLosMapped2CalPar(const char* name, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BLosMapped2CalPar();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

    /**
     * Method for setting the update rate for control histograms
     * @param rate an update rate value (events).
     */
    inline void SetUpdateRate(Int_t rate) { fUpdateRate = rate; }

    /**
     * Method for setting minimum required statistics per module.
     * Only detector modules with number of entries in TDC
     * distribution greater than minimum statistics will be
     * calibrated.
     * @param minStats a value of minimum statistics required.
     */
    inline void SetMinStats(Int_t minStats) { fMinStats = minStats; }

    /**
     * Method for selecting events with certain trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }

    /**
     * Method for setting number of LOS detectors and channels
     * @param nDets number of detectors
     * @param nCh number of channels per detector
     */
    inline void SetNofModules(Int_t nDets, Int_t nCh)
    {
        fNofDetectors = nDets;
        fNofChannels = nCh;
        fNofTypes = 3;
        fNofModules = nDets * nCh * fNofTypes;
    }

  private:
    Int_t fUpdateRate = 1000000;
    Int_t fMinStats = 1000;
    Int_t fTrigger = -1;
    Int_t Icount[2][16][3];
    Int_t Icounttrig[16][3];

    UInt_t fNofDetectors = 1; // Number of LOS detectors
    UInt_t fNofChannels = 8;  // Number of channels per detector
    UInt_t fNofTypes = 3;     // Number of time-types per channel (VFTX, TAMEX leading/trailing)
    UInt_t fNofModules = fNofDetectors * fNofChannels * fNofTypes; // Total number of modules (=edges) to calibrate

    unsigned int fNEvents = 0;
    R3BTCalPar* fCal_Par = nullptr;
    TClonesArray* fMapped = nullptr;
    TClonesArray* fMappedTriggerItems = nullptr;
    R3BEventHeader* header = nullptr;

    R3BTCalEngine* fEngine = nullptr;

  public:
    ClassDef(R3BLosMapped2CalPar, 1); // NOLINT
};
