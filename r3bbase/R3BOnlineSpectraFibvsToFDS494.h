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
// -----                  R3BOnlineSpectra                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BONLINESPECTRAFIBVSTOFDS494
#define R3BONLINESPECTRAFIBVSTOFDS494
#define N_PLANE_MAX_TOFD 4
#define N_PADDLE_MAX_TOFD 44
#define T_TOF_MIN -10000
#define T_TOF_MAX  10000

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
class R3BBunchedFiberCalData;
/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
 */
class R3BOnlineSpectraFibvsToFDS494 : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineSpectraFibvsToFDS494();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpectraFibvsToFDS494(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpectraFibvsToFDS494();

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
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat) { fTpat = tpat; }
    
    /**
     * Method for setting the window width for calculating fibers
     * efficiency
     */
    inline void SetMovingWindow(size_t window_mv){ fwindow_mv = window_mv;}
    inline void SetXwindow(Double_t xmin, Double_t xmax){
		fxmin=xmin;
		fxmax=xmax;
	}
	inline void SetTofLimits(Double_t tofmin, Double_t tofmax){
		ftofmin=tofmin;
		ftofmax=tofmax;
	}
    /**
     * Methods for setting cuts
     */
    inline void SetCuts(Bool_t cuts)
    {
        fCuts = cuts;
    }

    void Reset_All();
    
  private:
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fHitItems;


    enum DetectorInstances
    {
        DET_FI_FIRST,
        DET_FI23A = DET_FI_FIRST,
        DET_FI23B,
        DET_FI30,
        DET_FI31,
        DET_FI32,
        DET_FI33,
        DET_FI_LAST = DET_FI33,
        DET_TOFI,
        DET_TOFD,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = { "Fi23a", "Fi23b", "Fi30",  "Fi31",  
                                                 "Fi32", "Fi33",  "Tofi", "Tofd",   NULL };

    // If FiberI is present or not:
    Int_t ifibdet;
    // Number of fibers per detector
    Double_t n_fiber[NOF_FIB_DET] = { 384., 384., 512., 512., 512., 512. };

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat;
	Double_t delta;
	Bool_t fCuts;
    unsigned long long time_start = 0, time = 0;
    unsigned long long tofdor_start = 0;
    unsigned long fNEvents = 0, fNEvents_start = 0; /**< Event counter. */
	Double_t ftofmin, ftofmax;
    Int_t maxevent;
	
	Int_t Q = 0;
	
	Int_t counter1 = 0;
	Int_t counter2 = 0;
	Int_t counter3 = 0;
	Int_t counter4 = 0;
	Int_t counterTofd = 0;
	Int_t counterTofdMulti = 0;
	Int_t counterWrongTpat = 0;
	Int_t counterWrongTrigger = 0;
	Int_t countdet;
	Double_t hits1 = 0;
	Double_t hits30 = 0;
	Double_t hits30bc = 0;
	Double_t hits31 = 0;
	Double_t hits31bc = 0;
	Double_t hits32 = 0;
	Double_t hits32bc = 0;
	Double_t hits33 = 0;
	Double_t hits33bc = 0;

    Double_t nHitstemp;
    long long totalToFD = 0;
    long long totalToFDl = 0;
    long long totalToFDr = 0;
    long long totalFi30 = 0;
    long long totalFi31 = 0;
    long long totalFi32 = 0;
    long long totalFi33 = 0;
    long long totalfibFi30 = 0;
    long long totalfibFi31 = 0;
    long long totalfibFi32 = 0;
    long long totalfibFi33 = 0;
    long long NfibFi30 = 0;
    long long NfibFi31 = 0;
    long long NfibFi32 = 0;
    long long NfibFi33 = 0;
   
    std::size_t fwindow_mv ;
    std::deque<int> windowToFD;
    std::deque<int> windowToFDl;
    std::deque<int> windowToFDr;
    std::deque<int> windowFi30;
    std::deque<int> windowFi31;
    std::deque<int> windowFi32;
    std::deque<int> windowFi33;
    
    std::deque<int> fiberFi30;
    std::deque<int> fiberFi31;
    std::deque<int> fiberFi32;
    std::deque<int> fiberFi33;

    Double_t fxmin, fxmax;
    Double_t Nsumm_tofd = 0;
    Double_t Nsumm_tofdr = 0;
    Double_t Nsumm_tofdl = 0;
    Double_t Nevents30 = 0;
    Double_t Nevents31 = 0;
    Double_t Nevents32 = 0;
    Double_t Nevents33 = 0;        
	Int_t n_det = 13;
	
	Double_t ncounts_tofd[70]={0};
	Double_t ncounts_fi30[70]={0};
	Double_t ncounts_fi31[70]={0};
	Double_t ncounts_fi32[70]={0};
	Double_t ncounts_fi33[70]={0};
    
    TH1F* fh_Tofi_ToF;
    TH1F* fh_Tofi_ToF_ac;
    	
    TH2F* fh_xy_Fib[NOF_FIB_DET];
    TH2F* fh_xy_Fib_ac[NOF_FIB_DET];
    TH1F* fh_Fib_ToF[NOF_FIB_DET];
    TH1F* fh_Fib_ToF_ac[NOF_FIB_DET];
    TH2F* fh_ToT_Fib[NOF_FIB_DET];
    TH2F* fh_ToT_Fib_ac[NOF_FIB_DET];
    TH2F* fh_Fibs_vs_Tofd[NOF_FIB_DET];
    TH2F* fh_Fibs_vs_Tofd_ac[NOF_FIB_DET];
    TH2F* fh_ToF_vs_Events[NOF_FIB_DET];
    
    TH2F* fh_counter_fi30;
    TH2F* fh_counter_fi31;
    TH2F* fh_counter_fi32;
    TH2F* fh_counter_fi33;
    
    TH2F* fh_test;
	TH2F* fh_test1;
	TH2F* fh_test2;
  public:
    ClassDef(R3BOnlineSpectraFibvsToFDS494, 2)
};

#endif
