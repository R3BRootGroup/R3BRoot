//
// Creates time calib parameters for bunched fiber detector.
//

#ifndef R3BBUNCHEDFIBERMAPPED2CALPAR_H
#define R3BBUNCHEDFIBERMAPPED2CALPAR_H

#include "FairTask.h"

class TClonesArray;
class R3BTCalPar;
class R3BEventHeader;
class R3BTCalEngine;

class R3BBunchedFiberMapped2CalPar: public FairTask
{
  public:
    enum Electronics {
      CTDC,
      TAMEX
    };

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param a_name a name of the task.
     * @param a_verbose a FairTask verbosity level.
     * @param a_update_rate a update rate for online histograms.
     * @param a_min_stats a minimum statistics for calibration.
     */
    R3BBunchedFiberMapped2CalPar(const char *, Int_t, enum Electronics = TAMEX, Int_t = 1e6, Int_t = 1e5);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BBunchedFiberMapped2CalPar();

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
     * Method for setting the update rate for control histograms
     * @param rate an update rate value (events).
     */
    virtual void SetUpdateRate(Int_t);

    /**
     * Method for setting minimum required statistics per module.
     * Only detector modules with number of entries in TDC
     * distribution greater than minimum statistics will be
     * calibrated.
     * @param minStats a value of minimum statistics required.
     */
    virtual void SetMinStats(Int_t);

  private:
    TString fName;
    enum Electronics fSPMTElectronics;
    TClonesArray *fMapped;
    R3BTCalPar    *fMAPMTTCalPar;
    R3BTCalEngine *fMAPMTEngine;
    R3BTCalPar    *fSPMTTCalPar;
    R3BTCalEngine *fSPMTEngine;
    Int_t fUpdateRate;
    Int_t fMinStats;

  public:
    ClassDef(R3BBunchedFiberMapped2CalPar, 1)
};

#endif
