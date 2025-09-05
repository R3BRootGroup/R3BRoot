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

// ------------------------------------------------------------
// -----                R3BMwpcOnlineSpectra              -----
// -----    Created 29/09/19 by J.L. Rodriguez-Sanchez    -----
// -----           Fill MWPC online histograms            -----
// ------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2F.h>
#include <TMath.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

class TClonesArray;
class R3BEventHeader;

/**
 * This tasks reads MWPC data and plots online histograms
 */
class R3BMwpcOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BMwpcOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     * @param namedet a name of the detector.
     */
    R3BMwpcOnlineSpectra(const TString& name, Int_t iVerbose = 1, const TString& namedet = "Mwpc0");

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BMwpcOnlineSpectra() = default;

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t* /*option*/) override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent() override;

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    void FinishTask() override;

    /**
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics
     */
    inline void SetTrigger(int trigger) { fTrigger = trigger; }
    inline void SetTpat(int tpat1, int tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }

  private:
    TClonesArray* fMapItemsMwpc = nullptr;
    TClonesArray* fCalItemsMwpc = nullptr;
    TClonesArray* fHitItemsMwpc = nullptr;

    R3BEventHeader* header = nullptr;
    unsigned long long fNEvents = 0;
    TString fNameDet;
    int fTrigger = -1;
    int fTpat1 = -1, fTpat2 = -1;

    // Canvas
    TCanvas* cMwpc_nPads;
    TCanvas* cMwpcMap;
    TCanvas *cMWPCCal, *cMWPCCal2D;
    TCanvas* chitx;
    TCanvas* chity;
    TCanvas* chitxy;

    // Histograms for Mapped data
    TH1I* fh1_mwpc_map_nPads[3];
    TH2I* fh2_mwpc_map_q[3];

    // Histograms for Cal data
    TH2F* fh2_mwpc_cal;
    TH1F* fh1_mwpc_cal[2];
    TH2F* fh2_mwpc_xq;
    TH2F* fh2_mwpc_yq;

    // Histograms for Hit data
    TH1F* fh1_Xpos;
    TH1F* fh1_Ypos;
    TH2F* fh2_XYpos;

  public:
    ClassDefOverride(R3BMwpcOnlineSpectra, 1); // NOLINT
};
