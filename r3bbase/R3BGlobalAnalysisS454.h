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
// -----                  R3BGlobalAnalysisS454                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BGLOBALANALYSISS454
#define R3BGLOBALANALYSISS454
#define N_PLANE_MAX_TOFD 4
#define N_PADDLE_MAX_TOFD 50
#define N_PADDLE_MAX_PTOF 100
#define N_PSPX_S454 4
#define N_FIBER_PLOT 1050 // range to plot

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>

#include "TClonesArray.h"
#include "TMath.h"
#include <cstdlib>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;

/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
 */
class R3BGlobalAnalysisS454 : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BGlobalAnalysisS454();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BGlobalAnalysisS454(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BGlobalAnalysisS454();

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

    virtual void Output(Double_t tracker[6], Double_t chi2[2]);

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat) { fTpat = tpat; }

    /**
     * Methods for setting reset and readout times for Bmon
     */
    inline void SetBmon(Int_t sens_SEE, Int_t sens_IC)
    {
        fsens_SEE = sens_SEE;
        fsens_IC = sens_IC;
    }

  private:
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fHitItems;
    TClonesArray* fMCTrack;

    enum DetectorInstances
    {
        DET_CALIFA,
        DET_BMON,
        DET_ROLU,
        DET_FI_FIRST,
        DET_FI3A = DET_FI_FIRST,
        DET_FI3B,
        DET_FI10,
        DET_FI11,
        DET_FI12,
        DET_FI13,
        DET_FI_LAST = DET_FI13,
        DET_TOFD,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = { "Califa", "BeamMonitor", "Rolu", "Fi3a", "Fi3b", "Fi10",
                                                "Fi11",   "Fi12",        "Fi13", "Tofd", NULL };

    // If FiberI is present or not:
    Int_t ifibdet;
    // Number of fibers per detector
    Double_t n_fiber[NOF_FIB_DET] = { 512., 512., 2048., 2048., 2048., 2048. };

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat;

    unsigned long long time_start = 0, time = 0;
    unsigned long ic_start = 0, see_start = 0, tofdor_start = 0;
    unsigned long fNEvents = 0, fNEvents_start = 0; /**< Event counter. */

    Int_t maxevent;

    Int_t fsens_SEE, fsens_IC; // SEETRAM and IC sensitivity, between -4 and -10
    Double_t calib_SEE = 1.;   // SEETRAM calibration factor
    Double_t see_offset = 7.1; // SEETRAM offset in kHz
    Double_t counts_SEE = 0;
    Double_t counts_IC = 0;
    Double_t counts_TofD = 0;

    Double_t Pxf = 0.;
    Double_t Pyf = 0.;
    Double_t Pzf = 0.;
    Double_t Xf = 0.;
    Double_t Yf = 0.;
    Double_t Zf = 0.;
    Double_t Tf = 0.;
    Double_t Pf_tot = 0.;

    UInt_t num_spills = 0;

    TH1F* fh_Tpat;
    TH1F* fh_Trigger;
    TH1F* fh_IC;
    TH1F* fh_SEE;
    TH1F* fh_TOFDOR;

    TH2F* fh_xy_Fib[NOF_FIB_DET];
    TH1F* fh_fibers_Fib[NOF_FIB_DET];
    TH1F* fh_fiber_Fib[NOF_FIB_DET];
    TH1F* fh_mult_Fib[NOF_FIB_DET];
    TH2F* fh_Fib_ToF[NOF_FIB_DET];
    TH1F* fh_xpos_Fib[NOF_FIB_DET];
    TH1F* fh_ypos_Fib[NOF_FIB_DET];

    TH2F* fh_time_Fib[NOF_FIB_DET];
    TH2F* fh_multihit_m_Fib[NOF_FIB_DET];
    TH2F* fh_multihit_s_Fib[NOF_FIB_DET];
    TH2F* fh_ToT_m_Fib[NOF_FIB_DET];
    TH2F* fh_ToT_s_Fib[NOF_FIB_DET];
    TH2F* fh_Fib_vs_Events[NOF_FIB_DET];
    TH2F* fh_Fibs_vs_Events[NOF_FIB_DET];
    TH2F* fh_Fibs_vs_Tofd[NOF_FIB_DET];

    TH2F* fh_Fib_vs_Fib[NOF_FIB_DET][NOF_FIB_DET];
    TH2F* fh_Fib_dx[NOF_FIB_DET][NOF_FIB_DET];

    TH2F* fh_Cave_position;

    TH2F* fh_xy_tofd;
    TH1F* fh_tofd_charge;
    TH1F* fh_TimePreviousEvent;
    TH1F* fh_tofd_mult;

    TH2F* fh_ToF_vs_events[NOF_FIB_DET];

    TH2F* fh_target_xy;
    TH1F* fh_target_px;
    TH1F* fh_target_py;
    TH1F* fh_target_pz;

    TH1F* fh_dx;
    TH1F* fh_dy;
    TH1F* fh_dz;
    TH1F* fh_dpx;
    TH1F* fh_dpy;
    TH1F* fh_dpz;
    TH1F* fh_dp;
    TH2F* fh_thetax_dpx;
    TH2F* fh_thetay_dpy;
    TH2F* fh_x_dpx;
    TH2F* fh_y_dpy;
    TH2F* fh_thetax_dpy;
    TH2F* fh_thetay_dpx;

  public:
    ClassDef(R3BGlobalAnalysisS454, 1)
};

#endif
