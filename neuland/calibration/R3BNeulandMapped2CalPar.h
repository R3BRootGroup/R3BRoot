/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BNEULANDMAPPED2CALPAR_H
#define R3BNEULANDMAPPED2CALPAR_H

#include "FairTask.h"

class R3BTCalPar;
class R3BEventHeader;
class R3BTCalEngine;

/**
 * An analysis task for TCAL calibration of NeuLAND data.
 * This class fills TDC distribution for each Photomultiplier
 * of the NeuLAND detector and calculates the calibration
 * parameters using the R3BTCalEngine.
 * @author D. Kresan
 * @since September 7, 2015
 */
class R3BNeulandMapped2CalPar : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BNeulandMapped2CalPar();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BNeulandMapped2CalPar(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BNeulandMapped2CalPar();

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
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent();

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

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
     * Method for setting number of modules in NeuLAND setup.
     * @param nPMTs a number of photomultipliers.
     */
    inline void SetNofModules(Int_t nPlanes, Int_t nBars)
    {
        fNofPlanes = nPlanes;
        fNofBarsPerPlane = nBars;
        fNofPMTs = nPlanes * nBars * 4;
    }

  private:
    Int_t fMinStats; /**< Minimum statistics required per module. */
    Int_t fTrigger;  /**< Trigger value. */

    Int_t fNofPlanes;       /**< Number of planes. */
    Int_t fNofBarsPerPlane; /**< Number of photomultipliers. */
    Int_t fNofPMTs;         /**< Number of NeuLAND modules. */

    Int_t checkcounts;
    Int_t counts[60][50][4];

    Int_t fNEvents;             /**< Event counter. */
    R3BTCalPar* fCal_Par;       /**< Parameter container. */
    TClonesArray* fHits;        /**< Array with NeuLAND hits - input data. */
    TClonesArray* fHitsTrigger; /**< Array with NeuLAND hits - triggers. */
    R3BEventHeader* header;     /**< Event header - input data. */

    R3BTCalEngine* fEngine; /**< Instance of the TCAL engine. */

  public:
    ClassDef(R3BNeulandMapped2CalPar, 1)
};

#endif
