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

// ------------------------------------------------------------
// -----            R3BOnlineSpectraFiber_s522            -----
// -----    Created 13/03/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill tracking online histograms          -----
// ------------------------------------------------------------

#ifndef R3BOnlineSpectraFiber_s522_H
#define R3BOnlineSpectraFiber_s522_H

#define N_FIBER_PLOT_S522 520 // range to plot

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "TMath.h"
#include <cstdlib>
#include <list>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;
class R3BFiberMAPMTCalData;

class R3BOnlineSpectraFiber_s522 : public FairTask
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
    R3BOnlineSpectraFiber_s522();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpectraFiber_s522(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpectraFiber_s522();

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
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat1, Int_t tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }
    void Reset_Fiber_Histo();

  private:
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fHitItems;
    std::vector<TClonesArray*> fCalTriggerItems;

    enum DetectorInstances
    {
        DET_FI_FIRST,
        DET_FI30 = DET_FI_FIRST,
        DET_FI31,
        DET_FI32,
        DET_FI33,
        DET_FI_LAST = DET_FI23B,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = { "Fi30", "Fi31", "Fi32", "Fi33", NULL };

    // If FiberI is present or not:
    Int_t ifibdet;
    // Number of fibers per detector
    Double_t n_fiber[NOF_FIB_DET] = { 512., 512., 512., 512. };
    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat1, fTpat2;
    Int_t fSamp;
    Double_t fClockFreq; /**< Clock cycle in [ns]. */
    std::vector<Channel> fChannelArray[2];
    unsigned const* fTriggerMap[2];
    unsigned long fNEvents = 0, fNEvents_start = 0; /**< Event counter. */

    TH1F* fh_channels_Fib[NOF_FIB_DET];
    TH1F* fh_channels_single_Fib[NOF_FIB_DET];
    TH1F* fh_fibers_Fib[NOF_FIB_DET];
    TH1F* fh_mult_Fib[NOF_FIB_DET];
    TH2F* fh_Fib_pos[NOF_FIB_DET];
    TH2F* fh_time_Fib[NOF_FIB_DET];
    TH2F* fh_multihit_m_Fib[NOF_FIB_DET];
    TH2F* fh_multihit_s_Fib[NOF_FIB_DET];
    TH2F* fh_ToT_Fib[NOF_FIB_DET];
    TH2F* fh_Fib_vs_Events[NOF_FIB_DET];
    TH2F* fh_ToTup_vs_ToTdown[NOF_FIB_DET];
    TH2F* fh_chan_corell[NOF_FIB_DET];
    TH2F* fh_raw_tot_up[NOF_FIB_DET];
    TH2F* fh_raw_tot_down[NOF_FIB_DET];
    TH2F* fh_chan_dt_cal[NOF_FIB_DET];
    TH2F* fh_xy_global;
    TH2F* fh_dtime_Fib23;
    TH2F* fh_test2;

  public:
    ClassDef(R3BOnlineSpectraFiber_s522, 1)
};

#endif
