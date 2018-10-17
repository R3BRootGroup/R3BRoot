// ------------------------------------------------------------
// -----                R3BTofdMapped2Cal                -----
// -----            Created Apr 2016 by R.Plag            -----
// ----- Convert mapped data to time calibrated data      -----
// ------------------------------------------------------------

#ifndef R3BTOFDMAPPED2TCAL
#define R3BTOFDMAPPED2TCAL

#include <vector>

#include "FairTask.h"

class TClonesArray;
class R3BTCalPar;
class R3BTofdMappedData;
class R3BTofdCalData;
class R3BEventHeader;

/**
 * An analysis task to apply TCAL calibration for NeuLAND.
 * This class reads NeuLAND mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BTofdMapped2CalFill task.
 */
class R3BTofdMapped2Cal : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofdMapped2Cal();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofdMapped2Cal(const char *, Int_t = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofdMapped2Cal();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init();

    /**
     * Method for initialization of the parameter containers.
     * Called by the framework prior to Init() method.
     */
    void SetParContainers();

    /**
     * Method for re-initialization of parameter containers
     * in case the Run ID has changed.
     */
    InitStatus ReInit();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t *);

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent();

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    void FinishTask();

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger)
    {
        fTrigger = trigger;
    }

    /**
     * Method for setting the number of NeuLAND modules.
     * @param nPMTs a number of photomultipliers.
     * @param n17 a number of channels with stop signal (17-th channel).
     */
    void SetNofModules(Int_t, Int_t);

  private:
    size_t GetCalLookupIndex(R3BTofdMappedData const &) const;

    TClonesArray *fMappedItems;                 /**< Array with mapped items - input data. */
    TClonesArray *fCalItems;                    /**< Array with cal items - output data. */
    Int_t fNofCalItems;                         /**< Number of produced time items per event. */

    R3BTCalPar *fTcalPar;                       /**< TCAL parameter container. */
    UInt_t fNofTcalPars;                        /**< Number of modules in parameter file. */

    UInt_t fNofPlanes;  
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */    
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */

	// check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger;                             /**< Trigger value. */

    // Fast lookup for matching mapped data.
    std::vector<std::vector<R3BTofdCalData *>> fCalLookup;

  public:
    ClassDef(R3BTofdMapped2Cal, 1)
};

#endif
