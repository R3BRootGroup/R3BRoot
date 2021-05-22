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

// ------------------------------------------------------------
// -----                R3BEventHeaderCal2Hit                -----
// -----            Created Jan 13th 2018 by M.Heil       -----
// ----- Convert mapped data to time calibrated data      -----
// ------------------------------------------------------------

#ifndef R3BEVENTHEADERMAPPED2CAL
#define R3BEVENTHEADERMAPPED2CAL

#include "FairTask.h"
#include "R3BEventHeader.h"

/**
 * An analysis task to apply TCAL calibration.
 * This class reads mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BEventHeaderCal2HitPar task.
 */
class R3BEventHeaderCal2Hit : public FairTask
{
  public:
    /**
     * Standard constructors.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     * @param a_variant CTDC firmware variant, see R3BTCalEngine.
     * @param a_skip_spmt Don't process SPMT side for pure MAPMT tests.
     */
    R3BEventHeaderCal2Hit();
    R3BEventHeaderCal2Hit(int);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BEventHeaderCal2Hit();

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
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

  private:
    TString fName;
    R3BEventHeader* fHeader;

  public:
    ClassDef(R3BEventHeaderCal2Hit, 0)
};

#endif
