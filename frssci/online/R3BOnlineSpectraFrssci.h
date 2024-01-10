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

#ifndef R3BOnlineSpectraFrssci_H
#define R3BOnlineSpectraFrssci_H

#include "FairTask.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"

class TClonesArray;
class R3BEventHeader;

/**
 * This taks reads SCI data and plots online histograms
 */
class R3BOnlineSpectraFrssci : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineSpectraFrssci();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpectraFrssci(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpectraFrssci();

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
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

    /** Virtual method Reset **/
    virtual void Reset();

    void SetNbDets(Int_t ndets) { fNbDets = ndets; }
    void SetNbPmts(Int_t npmts) { fNbPmts = npmts; }

  private:
    TClonesArray* fMapped; /**< Array with R3BFrssciMappedData */

    Int_t fNbDets;
    Int_t fNbPmts;

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* fEventHeader; /**< Event header.      */
    Int_t fNEvents;               /**< Event counter.     */

    // Canvas at Mapped level
    TCanvas* cMapFT;
    TCanvas* cMapMult1D;
    TCanvas* cMapMult2D;

    // Histograms at Mapped level
    TH1I** fh1_finetime;     // [fNbDets * NbPmts];
    TH1I** fh1_multMap;      // [fNbDets * fNbPmts];
    TH2I** fh2_multMap;      // [fNbDets];
    TH2I** fh2_multMap_RvsL; // [fNbDets];

  public:
    ClassDef(R3BOnlineSpectraFrssci, 1)
};

#endif
