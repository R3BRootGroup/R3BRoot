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

// --------------------------------------------------------------
// -----             R3BWhiterabbitPropagator               -----
// -----   Created May 28th 2021 by J.L. Rodriguez-Sanchez  -----
// --------------------------------------------------------------

#ifndef R3BWhiterabbitPropagator_H
#define R3BWhiterabbitPropagator_H

#include "FairTask.h"

class TClonesArray;

class R3BWhiterabbitPropagator : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BWhiterabbitPropagator();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     * @param namewr a name of the whiterabbit.
     */
    R3BWhiterabbitPropagator(const TString& name, Int_t iVerbose = 1, const TString& namewr = "WRMaster");

    /**
     * Destructor.
     */
    virtual ~R3BWhiterabbitPropagator();

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
    TString fNameWR;
    TClonesArray* fInputItem;  // Array with input items
    TClonesArray* fOutputItem; // Array with output items

  public:
    ClassDef(R3BWhiterabbitPropagator, 0)
};

#endif // R3BWhiterabbitPropagator_H
