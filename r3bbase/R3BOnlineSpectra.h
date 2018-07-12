// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------



#ifndef R3BONLINESPECTRA
#define R3BONLINESPECTRA
#define N_PLANE_MAX_TOFD 4
#define N_PADDLE_MAX_TOFD 50
#define N_PADDLE_MAX_PTOF 100
#define N_PSPX 4

#include "FairTask.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <array>

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
 
    TClonesArray* fMappedItemsFi0;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi0;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi1a;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi1a;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi1b;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi1b;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi2a;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi2a;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi2b;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi2b;
    TClonesArray* fMappedItemsFi3a;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi3a;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi3b;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi3b;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi4;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi4;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi5;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi5;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi6;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi6;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi7;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi7;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi8;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi8;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi9;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi9;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi10;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi10;                    /**< Array with cal items. */
    TClonesArray* fMappedItemsFi11;                 /**< Array with mapped items. */
    TClonesArray* fHitItemsFi11;                    /**< Array with cal items. */  
    TClonesArray* fCalItemsPtof;                   /**< Array with cal items. */


    std::vector<TClonesArray*> aMapped = {fMappedItemsFi1a,fMappedItemsFi1b,fMappedItemsFi2a,fMappedItemsFi2b,
		                                  fMappedItemsFi3a,fMappedItemsFi3b,fMappedItemsFi4,fMappedItemsFi5,
		                                  fMappedItemsFi6,fMappedItemsFi7,fMappedItemsFi8,fMappedItemsFi9,
		                                  fMappedItemsFi10,fMappedItemsFi11};
                                      
    std::vector<TClonesArray*> aHit = {fHitItemsFi1a,fHitItemsFi1b,fHitItemsFi2a,fHitItemsFi2b,
		                               fHitItemsFi3a,fHitItemsFi3b,fHitItemsFi4,fHitItemsFi5,fHitItemsFi6,fHitItemsFi7,
		                               fHitItemsFi8,fHitItemsFi9,fHitItemsFi10,fHitItemsFi11};                                      
 // If FiberI is present or not:
    bool FibPresent[14]={false};            
    Int_t  ifibdet;           
    std::string Mapped[14]={"Fi1aMapped","Fi1bMapped","Fi2aMapped","Fi2bMapped","Fi3aMapped","Fi3bMapped",
	                        "Fi4Mapped","Fi5Mapped","Fi6Mapped","Fi7Mapped","Fi8Mapped","Fi9Mapped",
	                        "Fi10Mapped","Fi11Mapped"};
    std::string Hit[14]={"Fi1aHit","Fi1bHit","Fi2aHit","Fi2bHit","Fi3aHit","Fi3bHit",
	                     "Fi4Hit","Fi5Hit","Fi6Hit","Fi7Hit","Fi8Hit","Fi9Hit",
	                     "Fi10Hit","Fi11Hit"};
    const char* cMapped[14];
    const char* cHit[14]; 
// Number of fibers per detector
   Double_t n_fiber[14]={256.,256.,256.,256,512.,512.,2048.,2048.,1024.,512.,512.,512.,1024.,1024.};    
    
     
	// check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header;                     /**< Event header. */
    Int_t fTrigger;                             /**< Trigger value. */
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */
    UInt_t fNofPlanes;  
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */    

    Int_t fNEvents = 0;         /**< Event counter. */

    Int_t fNEvents1;
    
    UInt_t fNofDetectors;  /**< Number of detectors. */
    UInt_t fNofChannels;   /**< Number of channels per detector. */    
    UInt_t fNofModules;    /**< Total number of channels. */
    
    
       
    TH1F *fh_los_channels;    
    TH1F *fh_los_tres_MCFD;
    TH1F *fh_los_tres_TAMEX;
    TH2F *fh_los_tot;
    TH1F *fh_los_tot_mean;
    
    TH1F *fh_channels_Fib[14];
    TH1F *fh_fibers_Fib[14];
    TH1F *fh_mult_Fib[14];
    TH2F *fh_Fib_ToF[14];
    TH1F *fh_Fib_pos[14];
    TH2F *fh_time_Fib[14];
    TH2F *fh_multihit_m_Fib[14];   
    TH2F *fh_multihit_s_Fib[14];
    TH2F *fh_ToT_m_Fib[14];
    TH2F *fh_ToT_s_Fib[14];
    TH2F *fh_FibervsTime_Fib[14];
    
    
    TH1F *fh_tofd_channels[N_PLANE_MAX_TOFD];    
    TH2F *fh_tofd_ToF[N_PLANE_MAX_TOFD];

    TH2F* fh_tofd_TotPm1[N_PLANE_MAX_TOFD]; 
    TH2F* fh_tofd_TotPm2[N_PLANE_MAX_TOFD]; 
 
 
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


    
 
  
  public:
    ClassDef(R3BOnlineSpectra, 1)
};

#endif
