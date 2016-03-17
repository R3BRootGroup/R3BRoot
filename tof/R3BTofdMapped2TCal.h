// ------------------------------------------------------------
// -----                R3BTofdMapped2TCal                -----
// -----            Created Apr 2016 by R.Plag            -----
// ----- Convert mapped data to time calibrated data      -----
// ------------------------------------------------------------



#ifndef R3BLOSMAPPED2TCAL
#define R3BLOSMAPPED2TCAL

#include <map>

#include "FairTask.h"
#include "R3BPaddleCalData.h"

class TClonesArray;
class R3BTCalModulePar;
class R3BTCalPar;
class R3BEventHeader;

/**
 * An analysis task to apply TCAL calibration for NeuLAND.
 * This class reads NeuLAND mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BTofdMapped2TCalFill task.
 */
class R3BTofdMapped2TCal : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofdMapped2TCal();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofdMapped2TCal(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofdMapped2TCal();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for initialization of the parameter containers.
     * Called by the framework prior to Init() method.
     */
    virtual void SetParContainers();

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
    inline void SetNofModules(Int_t planes, Int_t ppp)
    {
        fNofPlanes   = planes;
        fPaddlesPerPlane  = ppp;
        fNofEdges  = ppp * planes * 4; // two PMs per paddle, two edges per PM
        
        if (fCalItemMap) delete [] fCalItemMap;
        fCalItemMap = new R3BPaddleCalData*[ fNofEdges/4 ];
    }

  private:
    TClonesArray* fMappedItems;                 /**< Array with mapped items - input data. */
    TClonesArray* fCalItems;                    /**< Array with cal items - output data. */
    R3BPaddleCalData** fCalItemMap;              /**< used internally to reconstruct data. */
    Int_t fNofCalItems;                         /**< Number of produced time items per event. */

    R3BTCalPar* fTcalPar;                       /**< TCAL parameter container. */
    UInt_t fNofTcalPars;                        /**< Number of modules in parameter file. */

	// check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger;                             /**< Trigger value. */

    UInt_t fNofPlanes;  
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */    
	UInt_t fNofEdges;      /**< Total number of edges. */
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */


  public:
    ClassDef(R3BTofdMapped2TCal, 1)
};

#endif
