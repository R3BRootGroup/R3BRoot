// ------------------------------------------------------------------
// -----                  R3BLosCal2Hit                         -----
// -----            Created March 10th 2016 by R.Plag           -----
// ----- Convert time calibrated data to hit level (single time) ----
// ------------------------------------------------------------------



#ifndef R3BLOSCAL2HIT
#define R3BLOSCAL2HIT

#include <map>

#include "FairTask.h"

class TClonesArray;

/**
 * TODO: This explanation is humbug.
 * An analysis task to apply TCAL calibration for NeuLAND.
 * This class reads NeuLAND mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BLosCal2HitFill task.
 */
class R3BLosCal2Hit : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BLosCal2Hit();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosCal2Hit(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BLosCal2Hit();

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

  private:
    TClonesArray* fCalItems; /**< Array with Cal items - input data. */
    TClonesArray* fHitItems; /**< Array with Hit items - output data. */
    UInt_t fNofHitItems;     /**< Number of hit items for cur event. */
  public:
    ClassDef(R3BLosCal2Hit, 1)
};

#endif

