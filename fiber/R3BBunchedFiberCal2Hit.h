#ifndef R3BBUNCHEDFIBERCAL2HIT
#define R3BBUNCHEDFIBERCAL2HIT

#include "FairTask.h"

class R3BBunchedFiberCalData;

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
 * This class will then figure out how map to the actual fibers (except for
 * non-ambiguous dorting errors).
 */
class R3BBunchedFiberCal2Hit: public FairTask
{
  public:
    enum Direction {
      HORIZONTAL,
      VERTICAL
    };
    struct Channel {
      R3BBunchedFiberCalData const *prev;
      double max_ToT;
      R3BBunchedFiberCalData const *max_leading;
      R3BBunchedFiberCalData const *max_trailing;
    };

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BBunchedFiberCal2Hit(const char *, Int_t, Direction, UInt_t, UInt_t, UInt_t);

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

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t *);

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
      * Getting all fibers correct is difficult, this function lets us fix that
      * per detector.
      */
    virtual UInt_t FixMistake(UInt_t) = 0;

  private:
    TString fName;
    Direction fDirection;
    UInt_t fSubNum;
    UInt_t fChPerSub[2];
    TClonesArray *fCalItems;
    TClonesArray *fHitItems;
    Int_t fNofHitItems;
    // [0=MAPMT,1=SPMT][Channel].
    std::vector<Channel> fChannelArray[2];

  public:
    ClassDef(R3BBunchedFiberCal2Hit, 2)
};

#endif
