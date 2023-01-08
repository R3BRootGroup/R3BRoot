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

// ----------------------------------------------------------------
// ----- Create time calib parameters for TOFD via VFTX/Tamex -----
// -----      Adopted Apr 2016 from Neuland by R. Plag    -----
// ----------------------------------------------------------------

#ifndef R3BSTARTRACKMAPPED2TCALPAR_H
#define R3BSTARTRACKMAPPED2TCALPAR_H

#include "FairTask.h"

class TClonesArray;
class R3BTCalEngine;

/**
 * An analysis task for TCAL calibration of NeuLAND Tamex data.
 * This class fills TDC distribution for each Photomultiplier
 * of the NeuLAND detector and calculates the calibration
 * parameters using the R3BTCalEngine.
 * @author D. Kresan
 * @since September 7, 2015
 */
class R3BStartrackMapped2CalPar : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BStartrackMapped2CalPar();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BStartrackMapped2CalPar(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BStartrackMapped2CalPar();

    void SetOutputFile(const char* outFile);

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
     * Method for setting number of LOS detectors and channels.
     * @param nDets number of detectors.
     * @param nCh number of channels per detector (4+master trigger?)
     */
    inline void SetNofLadder(Int_t nDetsIn, Int_t nDetsOut)
    {
        fNofInLadder = nDetsIn;
        fNofOutLadder = nDetsOut;
        fNofStrips = (nDetsIn * 12 * 128 * 2) + (nDetsOut * 16 * 128 * 2);
    }

  private:
    Int_t fUpdateRate; /**< An update rate. */
    Int_t fMinStats;   /**< Minimum statistics required per module. */
    Int_t fTrigger;    /**< Trigger value. */

    UInt_t fNofInLadder;  /**< Total number of inner ladders to calibrate */
    UInt_t fNofOutLadder; /**< Total number of outer ladders to calibrate */
    UInt_t fNofStrips;    /**< Total number of Strips to calibrate */

    UInt_t* fNumEvents;
    UInt_t NEvents; /**< Event counter. */

    TClonesArray* fStartrackMappedDataCA; /**< Array with mapped data - input data. */

    R3BTCalEngine* fEngine;

    char* fOutputFile;

  public:
    ClassDef(R3BStartrackMapped2CalPar, 1)
};

#endif
