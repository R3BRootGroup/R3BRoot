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

#ifndef NEWTASK_H
#define NEWTASK_H

#include "FairTask.h"
#include <Rtypes.h>

class TClonesArray;

class NewTask : public FairTask
{
  public:
    // Default constructor
    NewTask();

    // Standard constructor
    NewTask(const TString& name, Int_t iVerbose = 1);

    // Destructor
    virtual ~NewTask();

    // Initiliazation of task at the beginning of a run
    virtual InitStatus Init() override;

    // ReInitiliazation of task when the runID changes
    virtual InitStatus ReInit() override;

    // Executed for each event
    virtual void Exec(Option_t* opt) override;

    // Load the parameter container from the runtime database
    virtual void SetParContainers() override;

    // Finish task called at the end of the run
    virtual void Finish() override;

    // Virtual method Reset
    virtual void Reset();

    // Method to setup online mode
    void SetOnline(Bool_t opt) { fOnline = opt; }

  private:
    // Store data for online
    Bool_t fOnline;

    // Input array from previous already existing data level
    TClonesArray* fDataInput;

    // Output array to  new data level
    TClonesArray* fDataOutput;

  public:
    // Class definition
    ClassDefOverride(NewTask, 1);
};

#endif /* NewTask_H */
