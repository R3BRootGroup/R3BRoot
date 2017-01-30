// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------



#ifndef R3BONLINESPECTRA
#define R3BONLINESPECTRA
#define N_PLANE_MAX 100
#define N_PADDLE_MAX 100
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
    inline void SetLosParameters(Double_t offsetX, Double_t offsetY, Double_t veffX, Double_t veffY)
    {
        flosOffsetX = offsetX;
        flosOffsetY = offsetY;
        flosVeffX = veffX;
        flosVeffY = veffY;       
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
    
	// check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger;                             /**< Trigger value. */
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */
    UInt_t fNofPlanes;  
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */    


    UInt_t fNofDetectors;  /**< Number of detectors. */
    UInt_t fNofChannels;   /**< Number of channels per detector. */    
    UInt_t fNofModules;    /**< Total number of channels. */
    Double_t flosVeffX;   
    Double_t flosVeffY;
    Double_t flosOffsetX;
    Double_t flosOffsetY;

    TH1F *fh_los_channels;    
    TH1F *fh_los_tres;
    TH2F *fh_los_pos;

    TH1F *fh_tofd_channels[N_PLANE_MAX];    

    TH1F* fhTotPm1[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fhTotPm2[N_PLANE_MAX][N_PADDLE_MAX]; 

    TH1F *fh_cherenkovLos1;    
    TH1F *fh_cherenkovLos2;    
    TH1F *fh_cherenkovLos3;    

    
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
    
  public:
    ClassDef(R3BOnlineSpectra, 1)
};

#endif
