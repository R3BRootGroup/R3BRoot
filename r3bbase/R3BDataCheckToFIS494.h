// ------------------------------------------------------------
// -----                  R3BDataCheckToFIS494                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BDATACHECKTOFIS494
#define R3BDATACHECKTOFIS494



#define N_PLANE_MAX_TOFI 1
#define N_PADDLE_MAX_TOFI 24


#define N_FIBER_PLOT_S494 520 // range to plot
#define NbSections 1
#define NbAnodes 16

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "TClonesArray.h"
#include "TMath.h"
#include <cstdlib>
#include <map>

#include <TGraph.h>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;
class TTree;


/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
 */
class R3BDataCheckToFIS494 : public FairTask
{

  public:
      
    
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BDataCheckToFIS494();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BDataCheckToFIS494(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BDataCheckToFIS494();

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
     * Methods for setting position offset and effective velocity of light
     */
 
    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat1, Int_t tpat2) { fTpat1 = tpat1;  fTpat2 = tpat2;}

    /**
     * Methods for setting number of planes and paddles
     */
    inline void SetNofModules(Int_t planes, Int_t ppp)
    {
        fNofPlanes = planes;
        fPaddlesPerPlane = ppp;
    }

    void Reset_TOFI_Histo();

  private:
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fHitItems;
    
    TClonesArray* fCalTriggerItems; /**< Array with trigger Cal items - input data. */
   
    enum DetectorInstances
    {
        
        DET_TOFI,
        DET_MAX
    };

    const char* fDetectorNames[DET_MAX + 1] = { "Tofi",  NULL };


    
    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat1, fTpat2;
    Double_t fClockFreq; /**< Clock cycle in [ns]. */
    UInt_t fNofPlanes;
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */


    unsigned long long time_V_mem = 0, time_start = 0, time = 0, time_mem = 0;
    unsigned long long time_prev_read = 0, time_to_read = 0;
    unsigned long ic_mem = 0, see_mem = 0, tofdor_mem = 0;
    unsigned long ic_start = 0, see_start = 0, tofdor_start = 0;
    unsigned long long time_spill_start = 0, time_spill_end = 0;
    unsigned long long time_previous_event = 0;
    Double_t time_clear = -1.;
    Double_t tdiff = 0.;
    Double_t fNorm = 1.;
    Int_t iclear_count = 1;
    UInt_t reset_time;         // time after which bmon spectra are reseted
    Double_t read_time;        // step in which scalers are read, in sec
    Int_t fsens_SEE, fsens_IC; // SEETRAM and IC sensitivity, between -4 and -10
    Double_t calib_SEE = 1.;   // SEETRAM calibration factor
    Double_t see_offset = 7.1; // SEETRAM offset in kHz
    int spill_number = 0;

	Double_t spill_time = 0.50;
	UInt_t spill_eventno = 0;

    unsigned long fNEvents = 0, fNEvents_start = 0; /**< Event counter. */


	TGraph* spillstarts;
	TGraph* spillstops;
	
	TGraph* spillstarts_time;
	TGraph* spillstops_time;
	
	TGraph* spillstarts_eventno;
	TGraph* spillstops_eventno;
	
	

	TList * TL_ToT_channels[N_PLANE_MAX_TOFI];

       
    // ToFI Histograms
    TH1F* fh_tofi_check_channels[N_PLANE_MAX_TOFI];
    TH1F* fh_tofi_check_channels_sum[N_PLANE_MAX_TOFI];
    
    TH2F* fh_tofi_check_ToT_channels[N_PLANE_MAX_TOFI];
    TH2F* fh_tofi_check_ToT_channels_sum[N_PLANE_MAX_TOFI];
      
    
    
  public:
    ClassDef(R3BDataCheckToFIS494, 2)
};

#endif
