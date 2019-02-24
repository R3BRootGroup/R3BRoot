// ------------------------------------------------------------
// -----                  R3BPspxOnlineSpectra            -----
// -----          Created Feb 23th 2019 by J.L. Rodriguez -----
// ------------------------------------------------------------

#ifndef R3BPspxOnlineSpectra_H
#define R3BPspxOnlineSpectra_H

#define N_PSPX 6 // max value 6 at the moment
#define N_STRIPS_PSPX 32

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>

#include "TClonesArray.h"
#include "TMath.h"
#include <cstdlib>
class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;

/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
 */
class R3BPspxOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BPspxOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BPspxOnlineSpectra(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BPspxOnlineSpectra();

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
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }

    void Reset_PSPX_Histo();

  private:

    TClonesArray* fMappedItemsPspx; /**< Array with Mapped items for Pspx. */
    TClonesArray* fPrecalItemsPspx; /**< Array with Precal items for Pspx. */
    TClonesArray* fCalItemsPspx;    /**< Array with Cal items for Pspx. */
    TClonesArray* fHitItemsPspx;    /**< Array with Hit items for Pspx. */

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;            /**< Event header. */
    Int_t fTrigger;                    /**< Trigger value. */
    Int_t fNEvents;        	     /**< Event counter. */


    TH1F* fh_pspx_channel_x[N_PSPX];      /**< PSPX x channel profile on mapped level */
    TH1F* fh_pspx_channel_y[N_PSPX];      /**< PSPX y channel profile on mapped level */
    TH1F* fh_pspx_multiplicity_x[N_PSPX]; /**< PSPX x multiplicity on mapped level */
    TH1F* fh_pspx_multiplicity_y[N_PSPX]; /**< PSPX y multiplicity on mapped level */

    TH2F* fh_pspx_strips_position[N_PSPX];        /**< PSPX 2D position on mapped level */
    TH2F* fh_pspx_hit_position[(N_PSPX + 1) / 2]; /**< PSPX 2D position on hit level */

    TH1F* fh_pspx_hit_energy[(N_PSPX + 1) / 2];           /**< PSPX energy on hit level */
    TH2F* fh_pspx_cal_energy_frontback[(N_PSPX + 1) / 2]; /**< PSPX energy front vs back on cal level */

  public:
    ClassDef(R3BPspxOnlineSpectra, 1)
};

#endif
