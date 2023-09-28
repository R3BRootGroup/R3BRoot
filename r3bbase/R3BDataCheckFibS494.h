// ------------------------------------------------------------
// -----                  R3BDataCheckFibS494                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BDATACHECKFIBS494
#define R3BDATACHECKFIBS494


#define N_FIBER_PLOT_S494 520 // range to plot
#define NbSections 1
#define NbAnodes 16

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "TClonesArray.h"
#include "TMath.h"
#include <cstdlib>
#include <map>
#include <list>

#include <TGraph.h>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;
class TTree;
class R3BFiberMAPMTCalData;

/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
 */
class R3BDataCheckFibS494 : public FairTask
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
    R3BDataCheckFibS494();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BDataCheckFibS494(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BDataCheckFibS494();

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
     * Methods for setting position offset and effective velocity of light
     */

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat1, Int_t tpat2) { fTpat1 = tpat1;  fTpat2 = tpat2;}
    void Reset_Fiber_Histo();
  private:
    std::vector<TClonesArray *> fMappedItems;
    std::vector<TClonesArray *> fCalItems;
    std::vector<TClonesArray *> fHitItems;
    std::vector<TClonesArray *> fCalTriggerItems;


   enum DetectorInstances
    {
        DET_FI_FIRST,
        DET_FI30 = DET_FI_FIRST,
        DET_FI31,
        DET_FI32,
        DET_FI33,
        DET_FI23A,
        DET_FI23B,
        DET_FI_LAST = DET_FI23B,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = {  "Fi30",  "Fi31", "Fi32", "Fi33",
                                                 "Fi23a", "Fi23b", NULL };

    // If FiberI is present or not:
    Int_t  ifibdet;
    // Number of fibers per detector
    Double_t n_fiber[NOF_FIB_DET] = {  512., 512., 512., 512., 384., 384.};
	// check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger;                             /**< Trigger value. */
    Int_t fTpat1, fTpat2;
    Int_t fSamp;
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */
    std::vector<Channel> fChannelArray[2];
    unsigned const *fTriggerMap[2];
    unsigned long fNEvents = 0, fNEvents_start = 0;         /**< Event counter. */

    unsigned long long time_V_mem = 0, time_start = 0, time = 0, time_mem = 0;
    unsigned long long time_prev_read = 0, time_to_read = 0;
    unsigned long ic_mem = 0, see_mem = 0, tofdor_mem = 0;
    unsigned long ic_start = 0, see_start = 0, tofdor_start = 0;
    unsigned long long time_spill_start = 0, time_spill_end = 0;
    unsigned long long time_previous_event = 0;
    Double_t time_clear = -1.;
    Double_t tdiff = 0.;
    Double_t fNorm = 1.;
    Int_t iclear_count = 1;
    UInt_t reset_time;         // time after which bmon spectra are reseted
    Double_t read_time;        // step in which scalers are read, in sec
    Int_t fsens_SEE, fsens_IC; // SEETRAM and IC sensitivity, between -4 and -10
    Double_t calib_SEE = 1.;   // SEETRAM calibration factor
    Double_t see_offset = 7.1; // SEETRAM offset in kHz
    int spill_number = 0;

	Double_t spill_time = 0.50;
	UInt_t spill_eventno = 0;

	TGraph* spillstarts;
	TGraph* spillstops;

	TGraph* spillstarts_time;
	TGraph* spillstops_time;

	TGraph* spillstarts_eventno;
	TGraph* spillstops_eventno;


	TList * TL_ToT_channels_up[NOF_FIB_DET];
	TList * TL_ToT_channels_down[NOF_FIB_DET];

    // Fiber Histograms
    TH1F *fh_check_channels_Fib[NOF_FIB_DET];
    TH1F *fh_check_channels_single_Fib[NOF_FIB_DET];
    TH2F *fh_check_ToT_Fib[NOF_FIB_DET];

    TH1F *fh_check_channels_Fib_sum[NOF_FIB_DET];
    TH1F *fh_check_channels_single_Fib_sum[NOF_FIB_DET];
    TH2F *fh_check_ToT_Fib_sum[NOF_FIB_DET];

	TH2F *fh_check_raw_tot_up[NOF_FIB_DET];
    TH2F *fh_check_raw_tot_down[NOF_FIB_DET];

    TH2F *fh_check_raw_tot_up_sum[NOF_FIB_DET];
    TH2F *fh_check_raw_tot_down_sum[NOF_FIB_DET];

    TH2F *fh_multihit_m_Fib[NOF_FIB_DET];
    TH2F *fh_multihit_s_Fib[NOF_FIB_DET];





/*
    TH1F *fh_channels_Fib[NOF_FIB_DET];
    TH1F *fh_channels_single_Fib[NOF_FIB_DET];
    TH1F *fh_fibers_Fib[NOF_FIB_DET];
    TH1F *fh_mult_Fib[NOF_FIB_DET];
    TH2F *fh_Fib_pos[NOF_FIB_DET];
    TH2F *fh_time_Fib[NOF_FIB_DET];

    TH2F *fh_ToT_Fib[NOF_FIB_DET];
    TH2F *fh_Fib_vs_Events[NOF_FIB_DET];
    TH2F *fh_ToTup_vs_ToTdown[NOF_FIB_DET];
    TH2F *fh_chan_corell[NOF_FIB_DET];
    TH2F *fh_raw_tot_up[NOF_FIB_DET];
    TH2F *fh_raw_tot_down[NOF_FIB_DET];
    TH2F *fh_chan_dt_cal[NOF_FIB_DET];
    TH2F *fh_xy_global;
    TH2F *fh_dtime_Fib23;
    TH2F* fh_test2;*/

  public:
    ClassDef(R3BDataCheckFibS494, 2)
};

#endif
