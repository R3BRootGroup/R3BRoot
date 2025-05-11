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

// -------------------------------------------------------------
// -----      R3BFiberMAPMTOnlineSpectra source file       -----
// -----    Created 25/04/22 by J.L. Rodriguez-Sanchez     -----
// -------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include <TMath.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;
class R3BFiberMappingPar;
class R3BFiberMAPMTCalData;

class R3BFiberMAPMTOnlineSpectra : public FairTask
{
  public:
    struct Channel
    {
        std::list<R3BFiberMAPMTCalData const*> lead_list;
    };
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BFiberMAPMTOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    explicit R3BFiberMAPMTOnlineSpectra(const TString, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BFiberMAPMTOnlineSpectra() = default;

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    InitStatus ReInit() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t*) override;

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

    void SetParContainers() override;

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    void SetTrigger(int trigger) { fTrigger = trigger; }
    void SetTpat(int tpat1, int tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }
    void SetClock(double CF, double CP)
    {
        fClockFreq = CF;
        fClockPeriods = CP;
    }

    void Reset_Histo();

  private:
    TString fName;
    UInt_t fNbfibersplot = 520;
    UInt_t fNbfibers = 512;
    R3BFiberMappingPar* fMapPar = nullptr;
    TClonesArray* fMappedItems = nullptr;
    TClonesArray* fCalItems = nullptr;
    TClonesArray* fHitItems = nullptr;
    TClonesArray* fCalTriggerItems = nullptr;

    R3BEventHeader* header = nullptr;
    int fTrigger = -1;
    int fTpat1 = -1, fTpat2 = -1;
    double fClockFreq = 150.;
    double fClockPeriods = 4096.;
    std::vector<Channel> fChannelArray[2];
    unsigned const* fTriggerMap[2];
    unsigned long long fNEvents = 0;

    TH1F* fh_channels_Fib;
    TH1F* fh_channels_single_Fib;
    TH1F* fh_fibers_Fib;
    TH1F* fh_mult_Fib;
    TH2F* fh_Fib_pos;
    TH2F* fh_time_Fib;
    TH2F* fh_multihit_m_Fib;
    TH2F* fh_multihit_s_Fib;
    TH2F* fh_ToT_Fib;
    TH2F* fh_Fib_vs_Events;
    TH2F* fh_ToTup_vs_ToTdown;
    TH2F* fh_chan_corell;
    TH2F* fh_raw_tot_up;
    TH2F* fh_raw_tot_down;
    TH2F* fh_chan_dt_cal;

  public:
    // Class definition
    ClassDefOverride(R3BFiberMAPMTOnlineSpectra, 1); // NOLINT
};
