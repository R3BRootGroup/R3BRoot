/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "FairTask.h"
#include "TCanvas.h"
#include "TH2.h"

class TClonesArray;
class R3BEventHeader;
class R3BIncomingIDPar;

/**
 * This taks reads SCI data and plots online histograms
 */
class R3BOnlineSpectraFrsSciVsLos : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineSpectraFrsSciVsLos();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpectraFrsSciVsLos(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpectraFrsSciVsLos();

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
    virtual void Reset() {}

    virtual void SetParContainers();

    void SetFrsSciNbDets(UShort_t ndets) { fFrsSciNbDets = ndets; }
    void SetFrsSciNbPmts(UShort_t npmts) { fFrsSciNbPmts = npmts; }

  private:
    R3BEventHeader* fHeader;    /**< Event header  */
    TClonesArray* fFrsSci_Tcal; /**< Array with R3BFrsSciTcalData */

    void SetParameters();
    R3BIncomingIDPar* fIncomingID_Par; // Parameter container
    TArrayF* fTof2InvV_p0;
    TArrayF* fTof2InvV_p1;
    TArrayF* fBrho0_S2toCC;
    TArrayF* fDispersionS2;

    Int_t fNEvents; /**< Event counter.     */
    UShort_t fFrsSciNbDets;
    UShort_t fFrsSciNbPmts;

    // Canvas Tof
    TCanvas* cTcal_TofRaw;
    TCanvas* cAoQ;

    // Histograms TofRaw
    TH1D** fh1_Tcal1Hit_TofRaw; // [fNumFrsSci] START=FrsSci, STOP=LOS
    TH1D** fh1_Tcal1Hit_AoQraw; // [fNumFrsSci] with Brho from FrsSci to Los

  public:
    ClassDef(R3BOnlineSpectraFrsSciVsLos, 1)
};
