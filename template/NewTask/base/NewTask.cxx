/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// NOTE: comments below are only meant for eduational purpose. DO NOT include them in your code!

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>
#include <TRandom.h>

// Fair headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// R3B headers
#include "NewTask.h"
#include <R3BLogger.h>
#include <fmt/core.h>

namespace R3B
{
    // ---- Standard Constructor ------------------------------------------
    NewTask::NewTask(const std::string& name, int iVerbose)
        : FairTask(name.c_str(), iVerbose)
    {
    }

    // ---- Default constructor -------------------------------------------
    // use constructor delegation
    NewTask::NewTask()
        : NewTask("NewTask", 1)
    {
    }

    // ----  Initialisation  ----------------------------------------------
    void NewTask::SetParContainers()
    {
        R3BLOG(debug, "SetParContainers of NewTask");
        // Load all necessary parameter containers from the runtime data base
    }

    // ---- Init ----------------------------------------------------------
    InitStatus NewTask::Init()
    {
        R3BLOG(debug, "NewTask::Init()");

        // Get a handle from the IO manager
        if (auto* ioman = FairRootManager::Instance(); ioman == nullptr)
        {
            return kFATAL;
        }

        input_data_.init();
        output_data_.init();

        // Do whatever else is needed at the initilization stage
        // Create histograms to be filled
        // initialize variables

        return kSUCCESS;
    }

    // ---- Exec ----------------------------------------------------------
    void NewTask::Exec(Option_t* /*opt*/)
    {
        R3BLOG(debug, "Exec of NewTask");
        output_data_.clear();

        // Read input data
        for (const auto& calData : input_data_)
        {
            // do something with calData
            fmt::print("qdc value: {}", calData.GetQdc());

            // to write data:
            output_data_.get().emplace_back();
        }
    }

    // ---- Finish --------------------------------------------------------
    void NewTask::Finish() { R3BLOG(debug, "Finish of NewTask"); }
} // namespace R3B

ClassImp(R3B::NewTask);
