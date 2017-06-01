// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------



#ifndef R3BONLINESPECTRA
#define R3BONLINESPECTRA
#define N_PLANE_MAX 100
#define N_PADDLE_MAX 100
#define N_PADDLE_MAX_PTOF 100
#define N_PSPX 4

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;


/**
 * This taks reads all detector data items and plots histograms 
 * for online checks. 
 */
class R3BOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpectra(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpectra();

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
    
    /**
     * Methods for setting position offset and effective velocity of light
     */
    inline void SetLosParamMCFD(Double_t offsetX1, Double_t offsetY1, Double_t veffX1, Double_t veffY1,
                                 Double_t offsetX2, Double_t offsetY2, Double_t veffX2, Double_t veffY2)
    {
        flosOffsetX1 = offsetX1;
        flosOffsetY1 = offsetY1;
        flosVeffX1 = veffX1;
        flosVeffY1 = veffY1;     
        flosOffsetX2 = offsetX2;
        flosOffsetY2 = offsetY2;
        flosVeffX2 = veffX2;
        flosVeffY2 = veffY2;
  
    }    
    
    inline void SetLosParamMCFDwc(Double_t offsetX1wc, Double_t offsetY1wc, Double_t veffX1wc, Double_t veffY1wc,
                                 Double_t offsetX2wc, Double_t offsetY2wc, Double_t veffX2wc, Double_t veffY2wc)
    {
        flosOffsetX1wc = offsetX1wc;
        flosOffsetY1wc = offsetY1wc;
        flosVeffX1wc = veffX1wc;
        flosVeffY1wc = veffY1wc;     
        flosOffsetX2wc = offsetX2wc;
        flosOffsetY2wc = offsetY2wc;
        flosVeffX2wc = veffX2wc;
        flosVeffY2wc = veffY2wc;
    }    
    
    inline void SetLosParamToT(Double_t offsetX1Q, Double_t offsetY1Q, Double_t veffX1Q, Double_t veffY1Q,
                                 Double_t offsetX2Q, Double_t offsetY2Q, Double_t veffX2Q, Double_t veffY2Q)
    {
        flosOffsetX1Q = offsetX1Q;
        flosOffsetY1Q = offsetY1Q;
        flosVeffX1Q = veffX1Q;
        flosVeffY1Q = veffY1Q;     
        flosOffsetX2Q = offsetX2Q;
        flosOffsetY2Q = offsetY2Q;
        flosVeffX2Q = veffX2Q;
        flosVeffY2Q = veffY2Q;
  
    }    
    
    inline void SetLosParamTAMEX(Double_t offsetX1T, Double_t offsetY1T, Double_t veffX1T, Double_t veffY1T,
                                 Double_t offsetX2T, Double_t offsetY2T, Double_t veffX2T, Double_t veffY2T)
    {
        flosOffsetX1T = offsetX1T;
        flosOffsetY1T = offsetY1T;
        flosVeffX1T = veffX1T;
        flosVeffY1T = veffY1T;     
        flosOffsetX2T = offsetX2T;
        flosOffsetY2T = offsetY2T;
        flosVeffX2T = veffX2T;
        flosVeffY2T = veffY2T;  
    }     
    
    /**
     * Methods for setting number of planes and paddles
     */
    inline void SetNofModules(Int_t planes, Int_t ppp)
    {
        fNofPlanes   = planes;
        fPaddlesPerPlane  = ppp;
    }


  private:
    TClonesArray* fMappedItemsLos;                 /**< Array with mapped items. */
    TClonesArray* fCalItemsLos;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsTofd;                /**< Array with mapped items. */
    TClonesArray* fCalItemsTofd;                   /**< Array with cal items. */
    TClonesArray* fMappedItemsPspx;                    /**< Array with mapped items. */
    TClonesArray* fCalItemsPspx;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi1;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi1;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi5;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi5;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi6;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi6;                    /**< Array with cal items. */
    TClonesArray* fCalItemsPtof;                   /**< Array with cal items. */
    
	// check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger;                             /**< Trigger value. */
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */
    UInt_t fNofPlanes;  
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */    

    Int_t fNEvents;         /**< Event counter. */

    UInt_t fNofDetectors;  /**< Number of detectors. */
    UInt_t fNofChannels;   /**< Number of channels per detector. */    
    UInt_t fNofModules;    /**< Total number of channels. */
    Double_t flosVeffX1;   
    Double_t flosVeffY1;
    Double_t flosOffsetX1;
    Double_t flosOffsetY1;
    Double_t flosVeffX2;   
    Double_t flosVeffY2;
    Double_t flosOffsetX2;
    Double_t flosOffsetY2;
    Double_t flosVeffX1wc;   
    Double_t flosVeffY1wc;
    Double_t flosOffsetX1wc;
    Double_t flosOffsetY1wc;
    Double_t flosVeffX2wc;   
    Double_t flosVeffY2wc;
    Double_t flosOffsetX2wc;
    Double_t flosOffsetY2wc;
    Double_t flosVeffX1Q;   
    Double_t flosVeffY1Q;
    Double_t flosOffsetX1Q;
    Double_t flosOffsetY1Q;
    Double_t flosVeffX2Q;   
    Double_t flosVeffY2Q;
    Double_t flosOffsetX2Q;
    Double_t flosOffsetY2Q;
    Double_t flosVeffX1T;   
    Double_t flosVeffY1T;
    Double_t flosOffsetX1T;
    Double_t flosOffsetY1T;
    Double_t flosVeffX2T;   
    Double_t flosVeffY2T;
    Double_t flosOffsetX2T;
    Double_t flosOffsetY2T; 

    TH1F *fh_los_channels;    
    TH1F *fh_los_tres_MCFD;
    TH1F *fh_los_tres_TAMEX;
    TH1F *fh_los_tot;
    TH2F *fh_los_pos;

    TH1F *fh_tofd_channels[N_PLANE_MAX];    

    TH1F* fh_tofd_TotPm1[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fh_tofd_TotPm2[N_PLANE_MAX][N_PADDLE_MAX]; 
 
 
    TH1F *fh_ptof_channels; 
    TH1F *fh_ptof_channels_cut; 
    TH1F *fh_ptof_test1;   
    TH1F *fh_ptof_test2;   
    TH1F* fh_ptof_TotPm1[N_PADDLE_MAX_PTOF]; 
    TH1F* fh_ptof_TotPm2[N_PADDLE_MAX_PTOF]; 

    
    TH1F *fh_pspx_strips_psp[N_PSPX];
    TH1F *fh_pspx_energy_psp[N_PSPX];
    TH1F *fh_pspx_multiplicity_psp[N_PSPX];
    
    TH2F *fh_pspx_pos1_strips;
    TH2F *fh_pspx_pos2_strips;
    TH2F *fh_pspx_pos1_energy;
    TH2F *fh_pspx_pos2_energy;
    
    TH2F *fh_pspx_cor_x_strips;
    TH2F *fh_pspx_cor_y_strips;
    TH2F *fh_pspx_cor_x_energy;
    TH2F *fh_pspx_cor_y_energy;

    TH1F *fh_Fi1_channels;
    TH1F *fh_Fi1_fibers;
    TH2F *fh_Fi1_multihit;
    TH2F *fh_Fi1_ToT;
    TH2F *fh_Fi1_ToTvsTime;
    TH2F *fh_Fi1_multihit_s;
    TH2F *fh_Fi1_ToT_s;
    TH2F *fh_Fi1_ToTvsTime_s;
   
    TH1F *fh_Fi5_channels;
    TH1F *fh_Fi5_fibers;
    TH2F *fh_Fi5_multihit;
    TH2F *fh_Fi5_ToT;
    TH2F *fh_Fi5_ToTvsTime;
    TH2F *fh_Fi5_multihit_s;
    TH2F *fh_Fi5_ToT_s;
    TH2F *fh_Fi5_ToTvsTime_s;
   
    TH1F *fh_Fi6_channels;
    TH1F *fh_Fi6_fibers;
    TH2F *fh_Fi6_multihit;
    TH2F *fh_Fi6_ToT;
    TH2F *fh_Fi6_ToTvsTime;
    TH2F *fh_Fi6_multihit_s;
    TH2F *fh_Fi6_ToT_s;
    TH2F *fh_Fi6_ToTvsTime_s;
    
    TH1F* fh_los_t7;
    TH1F* fh_los_t5;
    TH1F* fh_los_t3;
    TH1F* fh_los_t1;
    TH1F* fh_los_t2;
    TH1F* fh_los_t4;
    TH1F* fh_los_t6;
    TH1F* fh_los_t8;
  
  public:
    ClassDef(R3BOnlineSpectra, 1)
};

#endif
