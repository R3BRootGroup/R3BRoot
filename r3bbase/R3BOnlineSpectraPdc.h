// ------------------------------------------------------------
// -----              R3BOnlineSpectraPdc             -----
// -----            Created July-2020 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BONLINESPECTRAPDC
#define R3BONLINESPECTRAPDC

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "TClonesArray.h"
#include "TMath.h"
#include <cstdlib>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;

#define N_PLANE_MAX_PDC 4 
/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
 */
class R3BOnlineSpectraPdc : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineSpectraPdc();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpectraPdc(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpectraPdc();

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

    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat) { fTpat = tpat; }

    void Reset_PDC_Histo();
    void Reset_Fi0_Histo();
    void Reset_Fi1_Histo();

  private:
    TClonesArray* fMappedItems; /**< Array with Cal items - input data. */
    TClonesArray* fCalItems;    /**< Array with Cal items - input data. */
    TClonesArray* fHitItems;    /**< Array with Cal items - input data. */

    TClonesArray* fMappedItems_fi0; /**< Array with Cal items - input data. */
    TClonesArray* fCalItems_fi0;    /**< Array with Cal items - input data. */
    TClonesArray* fHitItems_fi0;    /**< Array with Cal items - input data. */

    TClonesArray* fMappedItems_fi1a; /**< Array with Cal items - input data. */
    TClonesArray* fCalItems_fi1a;    /**< Array with Cal items - input data. */
    TClonesArray* fHitItems_fi1a;    /**< Array with Cal items - input data. */

    TClonesArray* fMappedItems_fi1b; /**< Array with Cal items - input data. */
    TClonesArray* fCalItems_fi1b;    /**< Array with Cal items - input data. */
    TClonesArray* fHitItems_fi1b;    /**< Array with Cal items - input data. */

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat;
    Int_t fSamp;
    Int_t fNEvents;

    TH1F* fhTpat;
    TH1F* fh_spill_length;
    TH1F* fhTrigger;

    TH1F* fh_Pdc_Wire[N_PLANE_MAX_PDC];
    TH2F* fh_Pdc_Tot[N_PLANE_MAX_PDC];
    TH2F* fh_Pdc_mult[N_PLANE_MAX_PDC];
    TH2F* fh_Pdc_ToF[N_PLANE_MAX_PDC];
    TH1F* fh_Pdc_x[N_PLANE_MAX_PDC];
    TH1F* fh_Pdc_y[N_PLANE_MAX_PDC];
    TH2F* fh_Pdc_xy[N_PLANE_MAX_PDC];
    TH2F* fh_Pdc_Time[N_PLANE_MAX_PDC];
    TH2F* fh_Pdc_Wire_vs_Events[N_PLANE_MAX_PDC];
    TH2F* fh_Pdc_ToT_vs_Time[144];
    TH2F* fh_Pdc_ToT_vs_Hit[144];

    TH1F* fh_fi0_0_fiber;
    TH1F* fh_fi0_1_fiber;
    TH1F* fh_fi0_0_mult;
    TH1F* fh_fi0_1_mult;
    TH2F* fh_fi0_0_Tot;
    TH2F* fh_fi0_1_Tot;
    TH2F* fh_fi0_0_Time;
    TH2F* fh_fi0_1_Time;
    TH1F* fh_fi0_0_fiber_mc;
    TH1F* fh_fi0_1_fiber_mc;
    TH1F* fh_fi0_0_mult_mc;
    TH1F* fh_fi0_1_mult_mc;
    TH2F* fh_fi0_0_Tot_mc;
    TH2F* fh_fi0_1_Tot_mc;
    TH2F* fh_fi0_0_Time_mc;
    TH2F* fh_fi0_1_Time_mc;
    
    TH2F* fh_fi0_0_pdc;
    TH2F* fh_fi0_1_pdc;
    
    TH1F* fh_fi1a_fiber;
    TH1F* fh_fi1b_fiber;
    TH1F* fh_fi1a_mult;
    TH1F* fh_fi1b_mult;
    TH2F* fh_fi1a_Tot;
    TH2F* fh_fi1b_Tot;
    TH2F* fh_fi1a_Time;
    TH2F* fh_fi1b_Time;
    TH1F* fh_fi1a_fiber_mc;
    TH1F* fh_fi1b_fiber_mc;
    TH1F* fh_fi1a_mult_mc;
    TH1F* fh_fi1b_mult_mc;
    TH2F* fh_fi1a_Tot_mc;
    TH2F* fh_fi1b_Tot_mc;
    TH2F* fh_fi1a_Time_mc;
    TH2F* fh_fi1b_Time_mc;
    
    TH2F* fh_fi1a_pdc;
    TH2F* fh_fi1b_pdc;

  public:
    ClassDef(R3BOnlineSpectraPdc, 2)
};

#endif
