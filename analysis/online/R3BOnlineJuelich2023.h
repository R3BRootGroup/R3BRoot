// ------------------------------------------------------------
// -----              R3BOnlineJuelich2023             -----
// -----            Created July-2020 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BONLINEJUELICH2023
#define R3BONLINEJUELICH2023

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
class R3BOnlineJuelich2023 : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineJuelich2023();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineJuelich2023(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineJuelich2023();

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

    void Reset_ROLU_Histo();
    void Reset_Fi0_Histo();
    void Reset_Fi60_Histo();
    void Reset_Fi61_Histo();
    void Reset_Fi61vsFi60_Histo();

  private:
    TClonesArray* fMappedItems_rolu; /**< Array with Cal items - input data. */
    TClonesArray* fCalItems_rolu;    /**< Array with Cal items - input data. */
    TClonesArray* fHitItems_rolu;    /**< Array with Cal items - input data. */
    TClonesArray* fMappedItems_fi0;  /**< Array with Cal items - input data. */
    TClonesArray* fCalItems_fi0;     /**< Array with Cal items - input data. */
    TClonesArray* fHitItems_fi0;     /**< Array with Cal items - input data. */
    TClonesArray* fMappedItems_fi60; /**< Array with Cal items - input data. */
    TClonesArray* fCalItems_fi60;    /**< Array with Cal items - input data. */
    TClonesArray* fHitItems_fi60;    /**< Array with Cal items - input data. */
    TClonesArray* fMappedItems_fi61; /**< Array with Cal items - input data. */
    TClonesArray* fCalItems_fi61;    /**< Array with Cal items - input data. */
    TClonesArray* fHitItems_fi61;    /**< Array with Cal items - input data. */

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat;
    Int_t fSamp;
    Int_t fNEvents;
    Double_t ClockFreq = 5.;
    Int_t totalFi0 = 0;
    Int_t triggerCountsFi0 = 0;
    Int_t detectedFi0 = 0;
    Int_t totalFi60 = 0;
    Int_t triggerCountsFi60 = 0;
    Int_t detectedFi60 = 0;
    Int_t totalFi61 = 0;
    Int_t triggerCountsFi61 = 0;
    Int_t detectedFi61 = 0;

    TH1F* fhTpat;
    TH1F* fh_spill_length;
    TH1F* fhTrigger;

    TH2F* fh_rolu_tot;
    TH1F* fh_rolu_channels;

    TH1F* fh_fi0_fiber;
    TH1F* fh_fi0_mult;
    TH2F* fh_fi0_Tot;
    TH2F* fh_fi0_Time;
    TH1F* fh_fi0_fiber_mc;
    TH1F* fh_fi0_fiber_ave;
    TH1F* fh_fi0_mult_mc;
    TH2F* fh_fi0_Tot_mc;
    TH2F* fh_fi0_Tot_ave;
    TH2F* fh_fi0_Time_mc;
    TH2F* fh_fi0_pdc;
    TH2F* fh_fi0_pdc_time;
    TH2F* fh_fi0_pdc_eloss;
    TH2F* fh_fi0_tot2_tot1;
    TH2F* fh_fi0_cor;
    TH2F* fh_fi0_eff;
    TH2F* fh_fi0_eff_vs_time;

    TH1F* fh_fi60_fiber;
    TH1F* fh_fi60_mult;
    TH2F* fh_fi60_Tot;
    TH2F* fh_fi60_Time;
    TH1F* fh_fi60_fiber_mc;
    TH1F* fh_fi60_fiber_ave;
    TH1F* fh_fi60_mult_mc;
    TH2F* fh_fi60_Tot_mc;
    TH2F* fh_fi60_Tot_ave;
    TH2F* fh_fi60_Time_mc;
    TH2F* fh_fi60_pdc;
    TH2F* fh_fi60_pdc_time;
    TH2F* fh_fi60_pdc_eloss;
    TH2F* fh_fi60_tot2_tot1;
    TH2F* fh_fi60_cor;
    TH2F* fh_fi60_eff;
    TH2F* fh_fi60_eff_vs_time;

    TH1F* fh_fi61_fiber;
    TH1F* fh_fi61_mult;
    TH2F* fh_fi61_Tot;
    TH2F* fh_fi61_Time;
    TH1F* fh_fi61_fiber_mc;
    TH1F* fh_fi61_fiber_ave;
    TH1F* fh_fi61_mult_mc;
    TH2F* fh_fi61_Tot_mc;
    TH2F* fh_fi61_Tot_ave;
    TH2F* fh_fi61_Time_mc;

    TH2F* fh_fi61_pdc;
    TH2F* fh_fi61_pdc_time;
    TH2F* fh_fi61_pdc_eloss;
    TH2F* fh_fi61_tot2_tot1;
    TH2F* fh_fi61_cor;
    TH2F* fh_fi61_eff;
    TH2F* fh_fi61_eff_vs_time;

    TH2F* fh_fi61_vs_fi60_tot;
    TH2F* fh_fi61_vs_fi60_fibers;
    TH2F* fh_fi60_vs_fi0_fibers;
    TH2F* fh_fi61_vs_fi0_fibers;

  public:
    ClassDef(R3BOnlineJuelich2023, 2)
};

#endif
