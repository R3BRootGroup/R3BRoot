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

#ifndef R3BFiberMAPMTCAL2HIT
#define R3BFiberMAPMTCAL2HIT

#include "FairTask.h"

#include <R3BTCalEngine.h>

#include <list>

class TH1F;
class TH2F;

class R3BFiberMAPMTCalData;
class R3BFiberMAPMTHitPar;
class R3BFiberMAPMTHitModulePar;

#define FIBER_MAPMT_TRIGGER_MAP_SET(mapmt_arr) \
  MAPMTTriggerMapSet(mapmt_arr, sizeof mapmt_arr);\

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
class R3BFiberMAPMTCal2Hit : public FairTask
{
  public:
    enum Direction
    {
        HORIZONTAL,
        VERTICAL
    };
    struct ToT
    {
        ToT(R3BFiberMAPMTCalData const*, R3BFiberMAPMTCalData const*, Double_t, Double_t, Double_t);
        R3BFiberMAPMTCalData const* lead;
        R3BFiberMAPMTCalData const* trail;
        Double_t lead_ns, tail_ns, tot_ns;
    };
    struct Channel
    {
        std::list<R3BFiberMAPMTCalData const*> lead_list;
        std::list<ToT> tot_list;
    };

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BFiberMAPMTCal2Hit(const char*,
                           Int_t,
                           Direction,
                           UInt_t,
                           UInt_t,
                           UInt_t
                           );  // is tsync

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BFiberMAPMTCal2Hit();

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

    R3BFiberMAPMTHitModulePar* GetModuleParAt(Int_t fiber);

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
    Int_t fFiberNum;
    Int_t maxevent;
    Int_t fnEventsfill = 0;

	Int_t multi=0;
	Double_t energy[2048];
	Int_t counts[2048];

    double fClockFreq;
    Direction fDirection;
    TClonesArray* fCalItems;
    TClonesArray* fMAPMTCalTriggerItems;
    TClonesArray* fHitItems;
    unsigned const *fMAPMTTriggerMap;
    R3BFiberMAPMTHitPar* fCalPar; /**< Parameter container. */
    Int_t fNofHitItems;
    // [0=bottom,1=top][Channel].
    std::vector<Channel> fChannelArray[2];

  public:
    ClassDef(R3BFiberMAPMTCal2Hit, 1)
};

#endif
