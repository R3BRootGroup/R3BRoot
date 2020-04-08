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

#include <list>

class TH1F;
class TH2F;

class R3BBunchedFiberCalData;
class R3BBunchedFiberHitPar;
class R3BBunchedFiberHitModulePar;

#define BUNCHED_FIBER_TRIGGER_MAP_SET(array) \
  MAPMTTriggerMapSet(array, sizeof array)

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
        ToT(R3BBunchedFiberCalData const*, R3BBunchedFiberCalData const*, Double_t, Double_t, Double_t);
        R3BBunchedFiberCalData const* lead;
        R3BBunchedFiberCalData const* trail;
        Double_t lead_ns, tail_ns, tot_ns;
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

  private:
    TString fName;
    Int_t fnEvents;
    Int_t maxevent;
    Int_t fnEventsfill = 0;

	Int_t multi=0;
	Double_t energy[2048];
	Int_t counts[2048];
	Double_t tsync_temp[2048]={0};
	Double_t  gain_temp[2048]={10};
    Bool_t tofdin;

    double fClockFreq;
    Direction fDirection;
    UInt_t fSubNum;
    UInt_t fChPerSub[2];
    Bool_t fIsCalibrator;
    Bool_t fIsGain;
    Bool_t fIsTsync;
    TClonesArray* fCalItems;
    TClonesArray* fCalTriggerItems;
    TClonesArray* fHitItems;
    TClonesArray* fTofdHitItems;
    unsigned const *fMAPMTTriggerMap;
    R3BBunchedFiberHitPar* fCalPar; /**< Parameter container. */
    R3BBunchedFiberHitPar* fHitPar; /**< Hit parameter container. */
    Int_t fNofHitPars;              /**< Number of modules in parameter file. */
    Int_t fNofHitItems;
    // [0=MAPMT,1=SPMT][Channel].
    std::vector<Channel> fChannelArray[2];

    // histograms for gain matching
    TH2F* fh_ToT_MA_Fib;
    TH2F* fh_ToT_Single_Fib;
    TH2F* fh_ToT_s_Fib[4];
    TH2F* fh_ToT_ToT;
    TH2F* fh_dt_Fib;
    TH2F* fh_Fib_ToF;
    TH2F* fh_Test;
    TH1F* fh_multi;
    
    TH2F* fh_69_70;
    TH2F* fh_69_59;
    TH2F* fh_69_37;
    TH2F* fh_69_45;
    TH2F* fh_69_77;
    TH2F* fh_69_109;
    TH2F* fh_69_101;
    TH2F* fh_69_123;
    TH2F* fh_69_91;
    TH2F* fh_69_49;
    TH2F* fh_69_71;

    TH2F* fh_69_60;
    TH2F* fh_69_38;
    TH2F* fh_69_46;
    TH2F* fh_69_78;
    TH2F* fh_69_110;
    TH2F* fh_69_102;
    TH2F* fh_69_124;
    TH2F* fh_69_92;
    TH2F* fh_69_50;
    
    

  public:
    ClassDef(R3BBunchedFiberCal2Hit, 3)
};

#endif
