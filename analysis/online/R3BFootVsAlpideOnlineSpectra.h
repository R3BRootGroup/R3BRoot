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

// --------------------------------------------------------------
// -----       R3BFootVsAlpideOnlineSpectra             --
// -----    Created 17/02/19  by J.L. Rodriguez-Sanchez        --
// ----- Fill foot and alpide correlations in online histograms --
// --------------------------------------------------------------

#pragma once

#include "FairTask.h"
#include "R3BEventHeader.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TMath.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

class TClonesArray;
class R3BEventHeader;
class R3BFootMappingPar;

/**
 *  This tasks reads hit data from foot and alpide detectors and plots
 *  online histograms
 */
class R3BFootVsAlpideOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BFootVsAlpideOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BFootVsAlpideOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BFootVsAlpideOnlineSpectra();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init() override;

    /** Virtual method SetParContainers **/
    void SetParContainers() override;

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
     * Method for setting the number of FOOT detectors
     */
    void SetNumDet(Int_t NbDet) { fNbDet = NbDet; }

    void SetTPat(Int_t tpat1, Int_t tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }
    void SetTrigger(Int_t trig) { fTrigger = trig; }

    void Reset_FOOT_ALPIDE_Histo();

  private:
    void SetParameter();

    TClonesArray* fHitItemsFoot;   /**< Array with FOOT hit items. */
    TClonesArray* fHitItemsAlpide; /**< Array with ALPIDE hit items. */

    R3BFootMappingPar* fMap_Par = nullptr; // Parameter container with mapping

    Double_t fClockFreq; /**< Clock cycle in [ns]. */

    // check for trigger should be done globally (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger = -1;    /**< Trigger value. */
    Int_t fNEvents;         /**< Event counter. */
    Int_t fTpat1 = -1;
    Int_t fTpat2 = -1;

    TCanvas *cPosCorr, *cCharCorr;

    std::vector<int> fXDet = { 0, 2, 5, 7 };
    std::vector<int> fYDet = { 1, 3, 4, 6 };

    std::vector<TH2F*> fh2_foot_alpide_pos_corr;
    std::vector<TH2F*> fh2_foot_alpide_char_corr;

    Int_t fNbDet = 8; /**< Number of FOOT detectors. */

  public:
    ClassDef(R3BFootVsAlpideOnlineSpectra, 1)
};
