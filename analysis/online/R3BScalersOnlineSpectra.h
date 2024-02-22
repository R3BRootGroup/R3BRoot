/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024 Members of R3B Collaboration                          *
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
// -----            R3BScalersOnlineSpectra               -----
// -----    Created 21/02/24 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#pragma once

#include "TLatex.h"
#include <FairTask.h>
#include <TCanvas.h>
#include <memory>
#include <vector>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

class TH1F;
class TH2F;
class TClonesArray;
class R3BEventHeader;

constexpr const int8_t fNbHistScalers = 16;

class R3BScalersOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BScalersOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    explicit R3BScalersOnlineSpectra(const TString& name, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BScalersOnlineSpectra() = default;

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

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

    void SetParContainers() override{};

    /**
     * Method for setting the fTpat
     */
    inline void SetTpat(int tpat) { fTpat = tpat; }

    /**
     * Method to reset histograms
     */
    void Reset_Histo();

  private:
    TClonesArray* fScalerItems = nullptr;
    R3BEventHeader* fHeader = nullptr;

    int fNEvents = 0;
    int fTpat = 0;
    bool is_starting = false;
    int prevscaler[4][16];
    double nb_proj[4][16], ds[16], dt[16];

    TLatex* latex;

    const int triggerSpillOn = 12;
    const int triggerSpillOff = 13;
    std::vector<double> tpatextra;

    std::vector<TH1F*> fh1_rate;
    std::vector<TH1F*> fh1_dt;
    std::vector<TH1F*> fh1_dt2;
    std::vector<TH2F*> fh2_dt_time;

    std::unique_ptr<TCanvas> cRate;
    std::unique_ptr<TCanvas> cDts;
    std::unique_ptr<TCanvas> cDts2;
    std::unique_ptr<TCanvas> cDts_time;

  public:
    ClassDefOverride(R3BScalersOnlineSpectra, 1)
};
