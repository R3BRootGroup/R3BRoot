// ------------------------------------------------------------
// -----                     R3BLosTcal                   -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#ifndef R3BLOSTCAL
#define R3BLOSTCAL

#include <map>

#include "FairTask.h"

class TClonesArray;
class R3BTCalPar;
class R3BTCalModulePar;

/**
 * An analysis task for apllying the time calibration for LOS.
 * This class makes a loop over LOS raw items with TDC values,
 * and calculates LOS time items with time value in [ns].
 * @author D. Kresan
 * @since September 7, 2015
 */
class R3BLosTcal : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of this class using default parameters.
     */
    R3BLosTcal();

    /**
     * Standard constructor.
     * Creates an instance of this class.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosTcal(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Deletes an instance of this class.
     */
    virtual ~R3BLosTcal();

    /**
     * Method for task initialization.
     * It is called by the framework prior to the event loop.
     * Implements initialization of input data and parameter
     * containers.
     * @return Status of initialization: kSUCCESS, kERROR or kFATAL.
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
     * Fills the TDC distribution for each module.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * Method called after processing each event.
     */
    virtual void FinishEvent();

    /**
     * Method called after task has finished.
     */
    virtual void FinishTask();

    /**
     * Method for setting of number of detector modules.
     * @param nModules a number of detector modules.
     */
    inline void SetNofModules(Int_t nModules)
    {
        fNofModules = nModules;
    }

  private:
    TClonesArray* fRawHit;                      /**< Array of raw items - input data. */
    TClonesArray* fHit;                         /**< Array of time items - output data. */
    Int_t fNHit;                                /**< Number of produced time items per event. */
    std::map<Int_t, R3BTCalModulePar*> fMapPar; /**< Map for matching of detector module with parameter container. */
    R3BTCalPar* fTcalPar;                       /**< Parameter container for the whole detector. */
    Int_t fNofModules;                          /**< Number of modules. */
    Double_t fClockFreq;                        /**< Clock cycle in [ns]. */

    /**
     * Method for retrieving parameter container for specific module ID.
     * @param channel a detector module ID.
     * @param par an output parameter container for this module.
     * @return kTRUE if parameter container for this module exists, else kFALSE.
     */
    Bool_t FindChannel(Int_t channel, R3BTCalModulePar** par);

  public:
    ClassDef(R3BLosTcal, 1)
};

#endif
