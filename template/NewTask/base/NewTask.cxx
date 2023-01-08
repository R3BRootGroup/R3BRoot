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

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// R3B headers
#include "NewTask.h"

// ---- Default constructor -------------------------------------------
NewTask::NewTask()
    : NewTask("NewTask", 1)
{
}

// ---- Standard Constructor ------------------------------------------
NewTask::NewTask(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fOnline(kFALSE)
{
}

// ---- Destructor ----------------------------------------------------
NewTask::~NewTask()
{
    LOG(debug) << "Destructor of NewTask";
    if (fDataInput)
        delete fDataInput;
}

// ----  Initialisation  ----------------------------------------------
void NewTask::SetParContainers()
{
    LOG(debug) << "SetParContainers of NewTask";
    // Load all necessary parameter containers from the runtime data base
}

// ---- Init ----------------------------------------------------------
InitStatus NewTask::Init()
{
    LOG(info) << "NewTask::Init()";

    // Get a handle from the IO manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        return kFATAL;
    }

    // Get a pointer to the previous already existing data level
    /*
    fDataInput = (TClonesArray*) ioman->GetObject("InputDataLevelName");
    if ( ! fDataInput ) {
    return kERROR;
    }
  */

    // Create the TClonesArray for the output data and register it
    /*
    fDataOutput = new TClonesArray("OutputDataLevelName", 10);
    ioman->Register("OutputDataLevelName","OutputDataLevelName",fDataOutput,fOnline);
  */

    // Do whatever else is needed at the initilization stage
    // Create histograms to be filled
    // initialize variables

    return kSUCCESS;
}

// ---- ReInit  -------------------------------------------------------
InitStatus NewTask::ReInit()
{
    LOG(debug) << "ReInit of NewTask";
    SetParContainers();
    return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void NewTask::Exec(Option_t* opt) { LOG(debug) << "Exec of NewTask"; }

// ---- Finish --------------------------------------------------------
void NewTask::Finish() { LOG(debug) << "Finish of NewTask"; }

// ---- Reset ---------------------------------------------------------
void NewTask::Reset()
{
    LOG(debug) << "Reset Data Structures";
    if (fDataOutput)
        fDataOutput->Clear();
}

ClassImp(NewTask);
