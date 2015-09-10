// ----------------------------------------------------------------
// -----                     R3BLosTcalFill                   -----
// -----             Created 18-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#ifndef R3BLOSTCALFILL_H
#define R3BLOSTCALFILL_H

#include "FairTask.h"

class R3BTCalPar;
class TClonesArray;
class R3BTCalEngine;

/**
 * An analysis task for time calibration of LOS.
 * This class fills the TDC distribution per module,
 * and calculates the calibration parameters using
 * the R3BTCalEngine class.
 * @author D. Kresan
 * @since September 7, 2015
 */
class R3BLosTcalFill : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of this class using default parameters.
     */
    R3BLosTcalFill();

    /**
     * Standard constructor.
     * Creates an instance of this class.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosTcalFill(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Deletes an instance of this class.
     */
    virtual ~R3BLosTcalFill();

    /**
     * Method for task initialization.
     * It is called by the framework prior to the event loop.
     * Implements initialization of input data and parameter
     * containers.
     * @return Status of initialization: kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

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
     * Method for setting the update rate of calibration parameters.
     * @param rate an update rate value in events.
     */
    inline void SetUpdateRate(Int_t rate)
    {
        fUpdateRate = rate;
    }

    /**
     * Method for setting minimum required statistics per module.
     * Only detector modules with number of entries larger this value
     * will be calibrated.
     * @param minStats a value of minimum statistics.
     */
    inline void SetMinStats(Int_t minStats)
    {
        fMinStats = minStats;
    }

    /**
     * Method for setting of number of detector modules.
     * @param nCh a number of detector modules.
     */
    inline void SetNofModules(Int_t nCh)
    {
        fNofCh = nCh;
    }

  private:
    Int_t fUpdateRate; /**< Update rate of calibration parameters. **/
    Int_t fMinStats;   /**< Minimum required statistics per module. **/
    Int_t fNofCh;      /**< Number of detctor modules. **/

    Int_t fNEvents;       /**< Event counter **/
    R3BTCalPar* fCal_Par; /**< Parameter container **/
    TClonesArray* fHits;  /**< Array of raw items - input data. **/

    R3BTCalEngine* fEngine; /**< Instance of TCAL engine. **/

  public:
    ClassDef(R3BLosTcalFill, 1)
};

#endif
