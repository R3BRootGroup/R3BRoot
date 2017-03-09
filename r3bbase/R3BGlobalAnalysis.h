// ------------------------------------------------------------
// -----                  R3BGlobalAnalysis                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------



#ifndef R3BGLOBALANALYSIS
#define R3BGLOBALANALYSIS

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;


/**
 * This taks reads all detector data items and plots histograms 
 * for online checks. 
 */
class R3BGlobalAnalysis : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BGlobalAnalysis();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BGlobalAnalysis(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BGlobalAnalysis();

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
    inline void SetTrigger(Int_t trigger)
    {
        fTrigger = trigger;
    }
    

  private:
    TClonesArray* fMappedItemsLos;                 /**< Array with mapped items. */
    TClonesArray* fCalItemsLos;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsPtof;                /**< Array with mapped items. */
    TClonesArray* fCalItemsPtof;                   /**< Array with cal items. */
    
	// check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger;                             /**< Trigger value. */
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */

    TH1F *fh_los_channels;    
    TH1F *fh_los_tres;

    TH1F *fh_ptof_channels;    
    TH1F *fh_ptof_tot;    
    TH1F *fh_ptof_tof;    
    TH2F *fh_ptof_tot_vs_tof;    

    
  public:
    ClassDef(R3BGlobalAnalysis, 1)
};

#endif
