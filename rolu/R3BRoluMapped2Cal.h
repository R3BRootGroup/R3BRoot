/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ---------------------------------------------------------------
// -----                   R3BRoluMapped2Cal                 -----
// -----          Created July 2019 by A. Kelic-Heil         -----
// -----      Convert mapped data to time calibrated data    -----
// -----              Following R3BLosMapped2Cal             -----
// ---------------------------------------------------------------

#ifndef R3BROLUMAPPED2CAL_H
#define R3BROLUMAPPED2CAL_H

#include <map>

#include "FairTask.h"
#include "R3BRoluCalData.h"

class TClonesArray;
class R3BTCalModulePar;
class R3BTCalPar;
class R3BEventHeader;

/**
 * An analysis task to apply TCAL calibration for NeuLAND.
 * This class reads NeuLAND mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BRoluMapped2CalFill task.
 */
class R3BRoluMapped2Cal : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BRoluMapped2Cal();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BRoluMapped2Cal(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BRoluMapped2Cal();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for initialization of the parameter containers.
     * Called by the framework prior to Init() method.
     */
    virtual void SetParContainers();

    /**
     * Method for re-initialization of parameter containers
     * in case the Run ID has changed.
     */
    virtual InitStatus ReInit();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent();

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }

    /**
     * Method for setting the number of modules.
     * @param nPMTs a number of photomultipliers.
     */
    inline void SetNofModules(Int_t nDets, Int_t nChs)
    {
        fNofDetectors = nDets;
        fNofChannels = nChs; //=4
        fNofTypes = 2;       // leading, trailing
        fNofModules = nChs * nDets * 2;
    }

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    TClonesArray* fMappedItems;        /**< Array with mapped items - input data. */
    TClonesArray* fMappedTriggerItems; /**< Array with mapped items - input data. */
    TClonesArray* fCalItems;           /**< Array with cal items - output data. */
    TClonesArray* fCalTriggerItems;    /**< Array with cal items - output data. */
    Int_t fNofCalItems;                /**< Number of produced time items per event. */

    R3BTCalPar* fTcalPar; /**< TCAL parameter container. */
    UInt_t fNofTcalPars;  /**< Number of modules in parameter file. */

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */

    UInt_t fNofDetectors; /**< Number of detectors. */
    UInt_t fNofChannels;  /**< Number of channels per detector. */
    UInt_t fNofTypes = 2;
    UInt_t fNofModules;  /**< Total number of channels. */
    Double_t fClockFreq; /**< Clock cycle in [ns]. */
    UInt_t fNEvent;
    // Don't store data for online
    Bool_t fOnline;

  public:
    ClassDef(R3BRoluMapped2Cal, 2)
};

#endif // R3BROLUMAPPED2CAL_H
