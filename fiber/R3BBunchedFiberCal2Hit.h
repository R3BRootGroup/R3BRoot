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

#ifndef R3BBUNCHEDFIBERCAL2HIT
#define R3BBUNCHEDFIBERCAL2HIT

#include "FairTask.h"

#include <R3BTCalEngine.h>
#include "TClonesArray.h"

#include <list>

class TH1F;
class TH2F;

class R3BBunchedFiberCalData;
class R3BBunchedFiberHitPar;
class R3BBunchedFiberHitModulePar;
class R3BEventHeader;

#define BUNCHED_FIBER_TRIGGER_MAP_SET(mapmt_arr, spmt_arr) \
  MAPMTTriggerMapSet(mapmt_arr, sizeof mapmt_arr);\
  SPMTTriggerMapSet(spmt_arr, sizeof spmt_arr)
/**
 * Transforms bunched fiber Cal level data to Hit level.
 *
 * A note about the UCESB unpacker:
 * All channels in a full plane of fibers should be mapped in sequence. For
 * example, Fib4 is built out of two "sub-detectors" (left and right side),
 * where each subdet has 256 CTDC/MAPMT 2 TAMEX2/SPMT channels, which should be
 * mapped as:
 *  FIB4_MAPMT1 .. FIB4_MAPMT512
 *  FIB4_SPMT1 .. FIB4_SPMT4
 * This class will then figure out how to map the actual fibers (except for
 * non-ambiguous sorting errors).
 */
class R3BBunchedFiberCal2Hit : public FairTask
{
  public:
    enum Direction
    {
        HORIZONTAL,
        VERTICAL
    };
    
                                 
    struct ToT
    {
        ToT(R3BBunchedFiberCalData const*, R3BBunchedFiberCalData const*, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t);
        R3BBunchedFiberCalData const* lead;
        R3BBunchedFiberCalData const* trail;
        Double_t lead_raw, trail_raw, lead_ns, trail_ns, tot_ns, lead_trig_ns, trail_trig_ns;
    };
    struct Channel
    {
        std::list<R3BBunchedFiberCalData const*> lead_list;
        std::list<ToT> tot_list;
    };

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BBunchedFiberCal2Hit(const char*,
                           Int_t,
                           enum R3BTCalEngine::CTDCVariant,
                           Direction,
                           UInt_t,
                           UInt_t,
                           UInt_t,
                           Bool_t,   // is calib
                           Bool_t,   // is gain
                           Bool_t);  // is tsync

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BBunchedFiberCal2Hit();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for re-initialization of parameter containers
     * in case the Run ID has changed.
     */
    virtual InitStatus ReInit();


    virtual void SetParContainers();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t*);

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
  //  inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
  //  inline void SetTpat(Int_t tpat) { fTpat = tpat; }
    
    R3BBunchedFiberHitModulePar* GetModuleParAt(Int_t fiber);

    /**
     * Getting all fibers correct is difficult, this function lets us fix that
     * per detector.
     */
    virtual UInt_t FixMistake(UInt_t) = 0;

    /**
     * Can provide an array that lists for every channel (i.e. fiber bunch, not
     * fiber!) the entry in the <name>TriggerCal TClonesArray for trigger
     * times.
     */
    void MAPMTTriggerMapSet(unsigned const *, size_t);
    void SPMTTriggerMapSet(unsigned const *, size_t);

  private:
    TString fName;
    unsigned long fnEvents = 0, fnEvents_start = 0; /**< Event counter. */
    Int_t maxevent;
    Int_t fnEventsfill = 0;

	Int_t multi=0;
	Int_t summmpt_ac = 0;
    Int_t summsm1_ac = 0;
    Int_t summsm2_ac = 0;
    Int_t summsm3_ac = 0;
    Int_t summsm4_ac = 0;
    Int_t summmpt = 0;
    Int_t summsm1 = 0;
    Int_t summsm2 = 0;
    Int_t summsm3 = 0;
    Int_t summsm4 = 0;
    Bool_t cond = true;
    Bool_t cut = false;
    
	Double_t energy[2048];
	Int_t counts[2048];
	Double_t tsync_temp[2048];
	Double_t  gainM_temp[2048];
	Double_t  gainS_temp[2048];
	Double_t  offset1_temp[2048];
	Double_t  offset2_temp[2048];
	double s1 = -99.;
	double s2 = -99.;
    double s3 = -99.;
    double s4 = -99.;
    double s1_ac = -99.;
    double s2_ac = -99.;
    double s3_ac = -99.;
    double s4_ac = -99.;
	Bool_t tofdin;
    double tot_mapmt_max = -1 ;
    double tot_spmt_max = -1;
    int tot_mapmt_max_fiber_id = 0;
    int tot_spmt_max_fiber_id = 0;
    double ttemp[100][1024],etemp[100][1024];
    int    mtemp[1024];
    int ichan = 254;
    Bool_t ctest;
    size_t cal_num;
    
    std::vector<Double_t> mpmt_ch;
    std::vector<Double_t> spmt_ch; 
    
    unsigned long long time_start = 0, time = 0, time_spill_start = 0;
    Bool_t spill_on = false;

    double fClockFreq;
    double fClockPeriod;
    Direction fDirection;
    UInt_t fSubNum;
    UInt_t fChPerSub[2];
    Bool_t fIsCalibrator;
    Bool_t fIsGain;
    Bool_t fIsTsync;
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger = 0;                             /**< Trigger value. */
    Int_t fTpat = 1;
    TClonesArray* fCalItems;
    TClonesArray* fMAPMTCalTriggerItems;
    TClonesArray* fSPMTCalTriggerItems;
    TClonesArray* fHitItems;
    TClonesArray* fTofdHitItems;
    unsigned const *fMAPMTTriggerMap;
    unsigned const *fSPMTTriggerMap;
    R3BBunchedFiberHitPar* fCalPar; /**< Parameter container. */
    R3BBunchedFiberHitPar* fHitPar; /**< Hit parameter container. */
    Int_t fNofHitPars;              /**< Number of modules in parameter file. */
    Int_t fNofHitItems;
    // [0=MAPMT,1=SPMT][Channel].
    std::vector<Channel> fChannelArray[2];

    // histograms for gain matching
    TH2F* fh_ToT_MA_Fib;
    TH2F* fh_ToT_MA_Fib_ac;   
    TH2F* fh_ToT_MA_Fib_max;
    TH2F* fh_ToT_Single_Fib;
    TH2F* fh_ToT_Single_Fib_ac;
    TH2F* fh_dt_Fib;
    TH2F* fh_Fib_ToF;
    TH2F* fh_dt_Fib_ac;
    TH2F* fh_Fib_ToF_ac;
    TH2F* fh_ecorell;
    TH2F* fh_tcorell;
    TH2F* fh_tmapmt;
    TH2F* fh_tsapmt;
    TH2F* fh_tmapmt_ac;
    TH2F* fh_tsapmt_ac;
    TH2F* fh_dttrig_all;
    TH2F* fh_ToT_MA_Fib_raw;
    TH2F* fh_ToT_SA_Fib_raw;
    TH2F* fh_x_vs_y;
    TH2F* fh_ch_corr;
    TH2F* fh_ToT_ifib;
    TH2F* fh_tot_SA_ch;
    TH2F* fh_iFib_nHit;
    TH2F* fh_iFib_nHit_ac;
    TH2F* fh_ToT_s_Fib[4];
    TH2F* fh_ToT1_ToT2;
    TH2F* fh_ToT1_ToT3;
    TH2F* fh_ToT1_ToT4;
    TH2F* fh_ToT1_ToT2_ac;
    TH2F* fh_ToT1_ToT3_ac;
    TH2F* fh_ToT1_ToT4_ac;      
    TH1F* fh_multi;
    TH2F* fh_lowMtot;
    TH2F* fh_Mtot_vs_NEvents;
    TH2F* fh_fibId_vs_NEvents;
    TH2F* fh_dt_dtot;
    TH2F* fh_t_ihit;
    TH2F* fh_tot_ihit;

  public:
    ClassDef(R3BBunchedFiberCal2Hit, 3)
};

#endif
