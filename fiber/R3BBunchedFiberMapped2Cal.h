// ------------------------------------------------------------
// -----                R3BBunchedFiberMapped2Cal                -----
// -----            Created Jan 13th 2018 by M.Heil       -----
// ----- Convert mapped data to time calibrated data      -----
// ------------------------------------------------------------

#ifndef R3BBUNCHEDFIBERMAPPED2CAL
#define R3BBUNCHEDFIBERMAPPED2CAL

#include "FairTask.h"

class R3BTCalPar;

/**
 * An analysis task to apply TCAL calibration.
 * This class reads mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BBunchedFiberMapped2CalPar task.
 */
class R3BBunchedFiberMapped2Cal: public FairTask
{
  public:
    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BBunchedFiberMapped2Cal(const char *, Int_t, Bool_t = false);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BBunchedFiberMapped2Cal();

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

  private:
    TString fName;
    UInt_t fSideNum[2];
    Bool_t fSkipSPMT;
    R3BTCalPar *fMAPMTTCalPar;
    R3BTCalPar *fSPMTTCalPar;
    TClonesArray *fMappedItems;
    TClonesArray *fCalItems;
    Int_t fNofCalItems;
    Double_t fClockFreq;
    Double_t fTamexFreq;

  public:
    ClassDef(R3BBunchedFiberMapped2Cal, 1)
};

#endif
