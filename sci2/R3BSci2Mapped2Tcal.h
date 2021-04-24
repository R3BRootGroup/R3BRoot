#ifndef R3BSCI2MAPPED2TCAL
#define R3BSCI2MAPPED2TCAL

#include <map>

#include "FairTask.h"

#include "R3BSci2TcalData.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BTCalModulePar;
class R3BTCalPar;
class R3BEventHeader;

class R3BSci2Mapped2Tcal : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BSci2Mapped2Tcal();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BSci2Mapped2Tcal(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BSci2Mapped2Tcal();

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
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }

    /**
     */
    inline void SetNofModules(Int_t nDets, Int_t nChs)
    {
        fNofDetectors = nDets;
        fNofChannels = nChs; //=3
        fNofModules = nChs * nDets * 3;
    }

  private:
    // std::map<Int_t, R3BTCalModulePar*> fMapPar; /**< Map for matching mdoule ID with parameter container. */
    TClonesArray* fMapped; /**< Array with mapped items - input data. */
    TClonesArray* fTcal;   /**< Array with cal items - output data. */
    Int_t fNofTcalItems;   /**< Number of produced time items per event. */
    Int_t Icounts_good = 0;
    Int_t Icounts_tot = 0;

    R3BTCalPar* fTcalPar; /**< TCAL parameter container. */
    UInt_t fNofTcalPars;  /**< Number of modules in parameter file. */

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */

    UInt_t fNofDetectors; /**< Number of detectors. */
    UInt_t fNofChannels;  /**< Number of channels per detector. */
    UInt_t fNofModules;   /**< Total number of channels. */
    Double_t fClockFreq;  /**< Clock cycle in [ns]. */
    UInt_t fNEvent;

    // Private method to add TcalData
    R3BSci2TcalData* AddTcalData(Int_t det, Int_t ch, Double_t tns);

  public:
    ClassDef(R3BSci2Mapped2Tcal, 1)
};

#endif
