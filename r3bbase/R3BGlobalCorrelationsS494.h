/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum f�r Schwerionenforschung GmbH    *
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
// -----                  R3BGlobalCorrelations                -----
// -----            Created July 2021 A. Kelic-Heil       -----
// -----                                                  -----
// ------------------------------------------------------------

#ifndef R3BGLOBALCORRELETAIONSS494
#define R3BGLOBALCORRELETAIONSS494
#define N_PLANE_MAX_TOFD_S494 4
#define N_PADDLE_MAX_TOFD_S494 44
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
class R3BGlobalCorrelationsS494 : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BGlobalCorrelationsS494();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BGlobalCorrelationsS494(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BGlobalCorrelationsS494();

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
    inline void SetTpat(Int_t tpat1, Int_t tpat2) { fTpat1 = tpat1;  fTpat2 = tpat2;}
    inline void SetVeto(Bool_t veto) { fVeto = veto; }
    
	inline void SetTofLimitsFib2x(Double_t tofmin, Double_t tofmax){
		ftofminFib2x=tofmin;
		ftofmaxFib2x=tofmax;
	}
	inline void SetTofLimitsFib3x(Double_t tofmin, Double_t tofmax){
		ftofminFib3x=tofmin;
		ftofmaxFib3x=tofmax;
	}	
	inline void SetTofLimitsTofi(Double_t tofmin, Double_t tofmax){
		ftofminTofi=tofmin;
		ftofmaxTofi=tofmax;
	}	
	inline void SetQLimits(Double_t qmin, Double_t qmax){
		fqtofdmin=qmin;
		fqtofdmax=qmax;
	}
	inline void SetBmon(Int_t time_range, Double_t time_step, Int_t sens_SEE, Int_t sens_IC)
    {
        reset_time = time_range;
        read_time = time_step;
        fsens_SEE = sens_SEE;
        fsens_IC = sens_IC;
    }
    /**
     * Methods for setting cuts
     */
    inline void SetCuts(Bool_t cuts)
    {
        fCuts = cuts;
    }
    inline void SetCountOutput(std::string const& counter_file)
    {
        fcounter_file = counter_file;
    }
  //  void Reset_All();
  //  void Reset_BMON_Histo();
    
  private:
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fHitItems;
    TClonesArray* fMappedItemsCalifa;

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
        DET_ROLU,
        DET_BMON,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = { "Fi23a", "Fi23b", "Fi30",  "Fi31",  
                                                "Fi32", "Fi33",  "Tofi", "Tofd", "Rolu",
                                                "BeamMonitor",  NULL };

    // If FiberI is present or not:
    Int_t ifibdet;
    // Number of fibers per detector
    Double_t n_fiber[NOF_FIB_DET] = { 384., 384., 512., 512., 512., 512. };

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat1, fTpat2;
	Double_t delta;
	Bool_t fCuts;
	Bool_t fVeto;
	std::string fcounter_file;
	long long  time_start = -1, time_begin = -1, time = 0, time_mem = 0, time_end = -1;
    long long time_prev_read = 0, time_to_read = 0;
    long long nBeamParticle=0, nOffSpillParticle=0;
    long long ic_mem = 0, see_mem = 0, see_first = 0;
    long long ic_start = 0, see_start = 0;
    long long time_spill_start = -10, time_spill_end = -10;
    Double_t see_spill = 0, ic_spill = 0;
    long long time_previous_event = 0;
    int yIC_mem_mem=0, ySEE_mem_mem=0;
    Double_t fxmin,fxmax,fwindow_mv;
    
    long long tofdor_start = 0;
    long long fNEvents = 0, fNEvents_start = 0, fNEvents_local = 0, fNEvents_veto = 0, fNEvents_califa = 0, fNEvents_total=0,
    fNEvents_tofi = 0, fNEvents_fibers = 0, fNEvents_pair = 0,fNEvents_zeroToFD = 0, fNSpills = 0, nAccepted=0; /**< Event counter. */
    unsigned long seeLastSpill = 0, seeCount=0;
    Double_t ftofminFib2x, ftofmaxFib2x, ftofminFib3x, ftofmaxFib3x, ftofminTofi, ftofmaxTofi, fqtofdmin, fqtofdmax;
    Int_t maxevent;
    Bool_t spill_on = false;
    Double_t time_clear = -1.;
    Double_t tdiff = 0.;
    Double_t fNorm = 1.;
    Int_t iclear_count = 1;
    UInt_t reset_time;         // time after which bmon spectra are reseted
    Double_t read_time;        // step in which scalers are read, in sec
    Int_t fsens_SEE, fsens_IC; // SEETRAM and IC sensitivity, between -4 and -10
    Double_t calib_SEE = 1.;   // SEETRAM calibration factor
    Double_t see_offset = 0.2; // SEETRAM offset in kHz
	Double_t calib_IC = 1.;   // SEETRAM calibration factor
    Double_t ic_offset = 0; // SEETRAM offset in kHz
    Int_t in_spill_off = 0;
    	
	Int_t Q = 0;
	
	Int_t counter1 = 0;
	Int_t counter2 = 0;
	Int_t counter3 = 0;
	Int_t counter4 = 0;
	Int_t counterTofd = 0;
	Int_t counterTofdMulti = 0;
	Int_t counterTofdMultir = 0;
	Int_t counterTofdMultil = 0;
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

			Int_t n_det = 14;
            TH2F* fh_ztofd;
            TH2F* fh_ztofd_ac;
            TH2F* fh_xy_tofd;
            TH1F* fh_time_tofd[2];
            TH2F* fh_Z_vs_x;
            TH2F* fh_Z_vs_time;
            TH2F* fh_Z_vs_dthit;
            TH2F* fh_dqvsdt_Tofd;
            TH2F* fh_ql_vs_qr_Tofd;
            TH2F* fh_xl_vs_xr_Tofd;
            TH2F* fh_xy_target;
            TH2F* fh_ql_vs_qr_Tofd_Zsum8;
            TH2F* fh_dt_Tofd_Zsum8;
            TH2F* fh_xl_vs_xr_Tofd_Zsum8;
            TH2F* fh_xy_target_Zsum8;
            TH2F* fh_qr_Tofd;
            TH2F* fh_xr_Tofd;
            TH2F* fh_ql_Tofd;
            TH2F* fh_xl_Tofd;
            TH2F* fh_q_vs_tofFib23aTofd;
            TH2F* fh_qplane1_vs_qplane2_l;
            TH2F* fh_qplane1_vs_qplane2_r;
            
            
            TH1F* fh_Fib_ToF[NOF_FIB_DET];
            TH2F* fh_xFib_vs_xTofd[NOF_FIB_DET];
            TH2F* fh_yFib_vs_yTofd[NOF_FIB_DET];
            TH2F* fh_qFib_vs_qTofd[NOF_FIB_DET];
            TH2F* fh_ToF_vs_Events[NOF_FIB_DET];
            TH2F* fh_x_vs_Events[NOF_FIB_DET];
            TH1F* fh_Fib_ToF_ac[NOF_FIB_DET];
            TH2F* fh_xFib_vs_xTofd_ac[NOF_FIB_DET];
            TH2F* fh_yFib_vs_yTofd_ac[NOF_FIB_DET];
            TH2F* fh_qFib_vs_qTofd_ac[NOF_FIB_DET];
                
            TH2F* fh_xFib31_vs_xFib33;
			TH2F* fh_yFib31_vs_yFib33;
			TH2F* fh_qFib31_vs_qFib33;
			TH2F* fh_tFib31_diff_tFib33;
			
			TH2F* fh_xFib31_vs_xFib33_ac;
			TH2F* fh_yFib31_vs_yFib33_ac;
			TH2F* fh_qFib31_vs_qFib33_ac;
			TH2F* fh_tFib31_diff_tFib33_ac;
					
            TH2F* fh_xFib23a_vs_xFib31;
			TH2F* fh_yFib23a_vs_yFib31;
			TH2F* fh_qFib23a_vs_qFib31;
			TH2F* fh_tFib23a_diff_tFib31; 
			
			TH2F* fh_xFib23a_vs_xFib31_ac;
			TH2F* fh_yFib23a_vs_yFib31_ac;
			TH2F* fh_qFib23a_vs_qFib31_ac;
			TH2F* fh_tFib23a_diff_tFib31_ac;
			
			TH2F* fh_xFib23b_vs_xFib31;
			TH2F* fh_yFib23b_vs_yFib31;
			TH2F* fh_qFib23b_vs_qFib31;
			TH2F* fh_tFib23b_diff_tFib31;
							
			TH2F* fh_xFib23b_vs_xFib31_ac;
			TH2F* fh_yFib23b_vs_yFib31_ac;
			TH2F* fh_qFib23b_vs_qFib31_ac;
			TH2F* fh_tFib23b_diff_tFib31_ac;	
			
			TH2F* fh_xFib30_vs_xFib32;
			TH2F* fh_yFib30_vs_yFib32;
			TH2F* fh_qFib30_vs_qFib32;
			TH2F* fh_tFib30_diff_tFib32;
			
			TH2F* fh_xFib30_vs_xFib32_ac;
			TH2F* fh_yFib30_vs_yFib32_ac;
			TH2F* fh_qFib30_vs_qFib32_ac;
			TH2F* fh_tFib30_diff_tFib32_ac;
			
			TH2F* fh_xFib23a_vs_xFib30;
			TH2F* fh_yFib23a_vs_yFib30;
			TH2F* fh_qFib23a_vs_qFib30;
			TH2F* fh_tFib23a_diff_tFib30;
			
			TH2F* fh_xFib23a_vs_xFib30_ac;
			TH2F* fh_yFib23a_vs_yFib30_ac;
			TH2F* fh_qFib23a_vs_qFib30_ac;
			TH2F* fh_tFib23a_diff_tFib30_ac;
						
			TH2F* fh_xFib23b_vs_xFib30;
			TH2F* fh_yFib23b_vs_yFib30;
			TH2F* fh_qFib23b_vs_qFib30;
			TH2F* fh_tFib23b_diff_tFib30;	
			
			TH2F* fh_xFib23b_vs_xFib30_ac;
			TH2F* fh_yFib23b_vs_yFib30_ac;
			TH2F* fh_qFib23b_vs_qFib30_ac;
			TH2F* fh_tFib23b_diff_tFib30_ac;				
			

			TH1F* fh_Tpat;
			TH1F* fh_Trigger;
			TH1F* fh_IC_spill;
			TH1F* fh_SEE_spill;
			TH1F* fh_IC_SEE;
  public:
    ClassDef(R3BGlobalCorrelationsS494, 2)
};

#endif
