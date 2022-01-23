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

// -----------------------------------------------------------
// -----          R3BRpcvsLosOnlineSpectra               -----
// -----   Created 23/01/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill RPC online histograms              -----
// -----------------------------------------------------------

#ifndef R3BRpcvsLosOnlineSpectra_H
#define R3BRpcvsLosOnlineSpectra_H

#include "FairTask.h"

#include "TCanvas.h"
#include "TMath.h"
#include <Rtypes.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class TClonesArray;
class R3BEventHeader;
class TH1F;
class TH2F;

/**
 * This taks reads RPC vs Los data and plots online histograms
 */
class R3BRpcvsLosOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BRpcvsLosOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BRpcvsLosOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BRpcvsLosOnlineSpectra();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option) override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent() override;

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask() override;

    /**
     * Method to reset histograms
     */
    void Reset_RpcvsLos_Histo();

    /**
     * Method to set the number of rpc strips
     */
    void SetNbStrips(Int_t n) { fNbStrips = n; }

    /**
     * Method to set the trigger
     */
    void SetTrigger(Int_t trigg) { fTrigger = trigg; }

  private:
    R3BEventHeader* fEventHeader;  // // Pointer to the R3BEventHeader structure
    TClonesArray* fRpcMappedItems; // Array with mapped items.
    TClonesArray* fRpcCalItems;    // Array with cal items.
    TClonesArray* fRpcHitItems;    // Array with hit items.
    TClonesArray* fLosHitItems;    // Array with hit items.

    R3BEventHeader* header; // Event header.
    Int_t fTrigger;         // Trigger value.
    Int_t fNEvents;         // Event counter.
    Int_t fNbStrips;        // Number of RPC strips.

    // Histograms for hit data
    std::vector<TH1F*> fh1_tof;

  public:
    ClassDefOverride(R3BRpcvsLosOnlineSpectra, 1)
};

#endif /* R3BRpcvsLosOnlineSpectra_H */
