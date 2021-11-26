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
// -----                  R3BTrackerTestS494                -----
// -----            Created 28-05-2020 by M.Heil          -----
// -----               Test Tracker with simulation             -----
// ------------------------------------------------------------

#ifndef R3BTRACKERTESTS494
#define R3BTRACKERTESTS494

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include "TCutG.h"

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
class R3BTrackerTestS494 : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTrackerTestS494();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTrackerTestS494(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTrackerTestS494();

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

    virtual void Output1(Double_t tracker[6], Double_t chi2[2]);
    virtual void Output2(Double_t tracker[6], Double_t chi2[2]);


    /**
     * Methods for setting reset and readout times for Bmon
     */

    /**
     * Methods for setting cuts
     */
    inline void SetCuts(Bool_t cuts)
    {
        fCuts = cuts;
    }

    inline void SetGraphicalCuts(Bool_t graphCuts)
    {
        fGraphCuts = graphCuts;
    }

    inline void SetGhost(Bool_t ghost)
    {
        fGhost = ghost;
    }

    inline void SetPairs(Bool_t p)
    {
        fPairs = p;
    }

    inline void SetBfield(Int_t B)
    {
        fB = B;
    }
    inline void SetSimu(Int_t simu)
    {
        fSimu = simu;
    }

  private:
    TClonesArray* fMCTrack;
    TClonesArray* fTofdPoints;
    TClonesArray* fFi23aPoints;
    TClonesArray* fFi23bPoints;
    TClonesArray* fFi30Points;
    TClonesArray* fFi31Points;
    TClonesArray* fFi32Points;
    TClonesArray* fFi33Points;
    TClonesArray* fTrackItems;
    Int_t fNofTrackItems;


    enum DetectorInstances
    {
        DET_CALIFA,
        DET_BMON,
        DET_ROLU,
        DET_FI_FIRST,
        DET_FI23A = DET_FI_FIRST,
        DET_FI23B,
        DET_FI30,
        DET_FI31,
        DET_FI32,
        DET_FI33,
        DET_FI_LAST = DET_FI33,
        DET_TOFD,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = { "Califa", "BeamMonitor", "Rolu", "Fi23a", "Fi23b", "Fi30",
                                                "Fi31",   "Fi32",        "Fi33", "Tofd", NULL };

    // If FiberI is present or not:
    Int_t ifibdet;
    // Number of fibers per detector
    Double_t n_fiber[NOF_FIB_DET] = { 512., 512., 2048., 2048., 2048., 2048. };

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat;
	Bool_t fCuts;
	Bool_t fGhost;
	Bool_t fPairs;
	Bool_t fGraphCuts;
	Bool_t fSimu;
	Int_t fB;
	Bool_t tracker = true;

	TCutG *cut_fi31_fi23a;
	TCutG *cut_fi32_fi23b;
	TCutG *cut_Fi33vsFi31;
	TCutG *cut_Fi30vsFi32;
	
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

	Double_t XHes, YHes, ZHes, XCs, YCs, ZCs, THes, TCs;
	Double_t pHexs, pHeys, pHezs, pCxs, pCys, pCzs, pHes, pCs;
	
	Double_t amu = 931.49410242;
	Double_t pHex, pHey, pHez, pCx, pCy, pCz;
	Double_t Pxf, Pyf, Pzf, Xf, Yf, Zf, Pf_tot;
//	Double_t mHe = 4.00260325413*amu;
//	Double_t mC = 12. * amu;
	Double_t mHe = 3727.409;
	Double_t mC = 11174.950;
	
	Int_t Q = 0;
	Double_t tPrev[10];
	Int_t detPrev[10];
	
	Int_t counter1 = 0;
	Int_t counter2 = 0;
	Int_t counter3 = 0;
	Int_t counter4 = 0;
	Int_t counterTofd = 0;
	Int_t counterTofdMulti = 0;
	Int_t counterCalifa = 0;
	Int_t counterWrongTpat = 0;
	Int_t counterWrongTrigger = 0;
	Int_t counterRolu = 0;
	Int_t counterTracker = 0;
	Int_t countdet;
	Int_t countdet_s;
	Int_t countdet_ss;
	Double_t delta;
	Int_t miss23a = 0;
	Int_t miss23b = 0;
	Int_t miss30 = 0;
	Int_t miss31 = 0;
	Int_t miss32 = 0;
	Int_t miss33 = 0;

    UInt_t num_spills = 0;

	Int_t ndet = 10;
	
    TH1F* fh_Tpat;
    TH1F* fh_Trigger;
    TH1F* fh_IC;
    TH1F* fh_SEE;
    TH1F* fh_TOFDOR;

    TH2F* fh_califa_energy;
	
    TH2F* fh_xy_Fib[NOF_FIB_DET];
    TH2F* fh_xy_Fib_ac[NOF_FIB_DET];
    TH1F* fh_mult_Fib[NOF_FIB_DET];
    TH1F* fh_mult_Fib_ac[NOF_FIB_DET];
    TH2F* fh_Fib_ToF[NOF_FIB_DET];
    TH2F* fh_Fib_ToF_ac[NOF_FIB_DET];
    TH2F* fh_Fib_Time[NOF_FIB_DET];
    TH2F* fh_Fib_Time_ac[NOF_FIB_DET];
    TH2F* fh_ToT_Fib[NOF_FIB_DET];
    TH2F* fh_ToT_Fib_ac[NOF_FIB_DET];
    TH2F* fh_Fib_vs_Events[NOF_FIB_DET];
    TH2F* fh_Fib_vs_Events_ac[NOF_FIB_DET];
    TH2F* fh_Fibs_vs_Tofd[NOF_FIB_DET];
    TH2F* fh_Fibs_vs_Tofd_ac[NOF_FIB_DET];
    TH2F* fh_ToF_vs_Events[NOF_FIB_DET];
    TH2F* fh_ToF_vs_Events_ac[NOF_FIB_DET];

	TH2F* fh_Fib33_vs_Fib31;
	TH2F* fh_Fib33_vs_Fib31_dx;
	TH2F* fh_Fib31_vs_Fib23a;
	TH2F* fh_Fib31_vs_Fib23a_dx;
	TH2F* fh_Fib30_vs_Fib32;
	TH2F* fh_Fib30_vs_Fib32_dx;
	TH2F* fh_Fib32_vs_Fib23b;
	TH2F* fh_Fib32_vs_Fib23b_dx;

	TH2F* fh_Fib33_vs_Fib31_back;
	TH2F* fh_Fib33_vs_Fib31_dx_back;
	TH2F* fh_Fib31_vs_Fib23a_back;
	TH2F* fh_Fib31_vs_Fib23a_dx_back;
	TH2F* fh_Fib30_vs_Fib32_back;
	TH2F* fh_Fib30_vs_Fib32_dx_back;
	TH2F* fh_Fib32_vs_Fib23b_back;
	TH2F* fh_Fib32_vs_Fib23b_dx_back;

    TH2F* fh_Cave_position;

    TH2F* fh_xy_tofd;
    TH2F* fh_xy_tofd_ac;
    TH1F* fh_tofd_charge;
    TH1F* fh_tofd_charge_ac;
    TH1F* fh_tofd_time;
    TH1F* fh_tofd_time_ac;
    TH1F* fh_TimePreviousEvent;
    TH1F* fh_tofd_mult;
    TH1F* fh_tofd_mult_ac;
	TH2F* fh_tofd_q2_vs_q1;
	TH2F* fh_tofd_q2_vs_q1_ac;

    TH2F* fh_target_xy;
    TH2F* fh_target_dxdy;
    TH1F* fh_target_px;
    TH1F* fh_target_py;
    TH1F* fh_target_pz;
    TH1F* fh_target_p;
    TH1F* fh_chi2;

    TH1F* fh_px_He;
    TH1F* fh_py_He;
    TH1F* fh_pz_He;
    TH1F* fh_p_He;
    TH1F* fh_dpx_He;
    TH1F* fh_dpy_He;
    TH1F* fh_dpz_He;
    TH1F* fh_dp_He;

    TH1F* fh_px_C;
    TH1F* fh_py_C;
    TH1F* fh_pz_C;
    TH1F* fh_p_C;
    TH1F* fh_dpx_C;
    TH1F* fh_dpy_C;
    TH1F* fh_dpz_C;
    TH1F* fh_dp_C;

    TH2F* fh_chiy_vs_chix;
    TH2F* fh_chi2_vs_Erel;
    
    TH1F* fh_dx;
    TH1F* fh_dy;
    TH1F* fh_dz;
    TH1F* fh_dpx;
    TH1F* fh_dpy;
    TH1F* fh_dpz;
    TH1F* fh_dp;
    TH2F* fh_thetax_dpx;
    TH2F* fh_thetax_dpx_abs;
    TH2F* fh_thetay_dpy;
    TH2F* fh_x_dpx;
    TH2F* fh_y_dpy;
    TH2F* fh_thetax_dpy;
    TH2F* fh_thetay_dpx;
    TH2F* fh_dpy_dpx;

	TH1F* fh_theta26_simu;
	TH1F* fh_Erel_simu;
	TH1F* fh_theta26;
	TH1F* fh_Erel;

	TH2F* fh_dErel_vs_x;
	TH2F* fh_dErel_vs_y;
	
	TH1F* fh_res_xA[10];
	TH1F* fh_res_yA[10];
	TH1F* fh_res_xC[10];
	TH1F* fh_res_yC[10];
	
	TH2F* fh_xy[10];
	TH2F* fh_p_vs_x[10];
	TH2F* fh_p_vs_x_test[10];
	TH2F* fh_Erel_vs_xfh_Erel_vs_x;

	TH1F* fh_totalPz;
	TH1F* fh_totalPz_reco;
	
	
  public:
    ClassDef(R3BTrackerTestS494, 1)
};

#endif
