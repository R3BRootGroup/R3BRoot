// ------------------------------------------------------------------
// -----                  R3BTofdCal2Hit                         -----
// -----            Created May 30th 2016 by M.Heil           -----
// ------------------------------------------------------------------



#ifndef R3BTOFDCAL2HIT
#define R3BTOFDCAL2HIT

#define N_PLANE_MAX 100
#define N_PADDLE_MAX 100

#include <map>

#include "FairTask.h"

class TClonesArray;
class R3BTofdHitModulePar;
class R3BTofdHitPar;
class TH1F;
class TH2F;
 
/**
 * An analysis task to apply HIT calibration for ToFd.
 * it also applies walk corrections and Z-calibration
*/
class R3BTofdCal2Hit : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofdCal2Hit();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofdCal2Hit(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofdCal2Hit();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for re-initialization of parameter containers
     * in case the Run ID has changed.
     */
    virtual InitStatus ReInit();

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

    virtual void SetParContainers();

    /**
     * Method for setting the nuclear charge of main beam
     */
    inline void SetTofdQ(Double_t Q)
    {
        fTofdQ = Q;
    }

    /**
     * Method for walk calculation.
     */
    virtual Double_t walk(Double_t Q);


    /**
     * Method for calculation of saturation.
     */
    virtual Double_t saturation(Double_t x);
    /**
     * Methods for setting number of planes and paddles
     */
    inline void SetNofModules(Int_t planes, Int_t ppp)
    {
        fNofPlanes   = planes;
        fPaddlesPerPlane  = ppp;
    }

  private:
    TClonesArray* fCalItems; /**< Array with Cal items - input data. */
    TClonesArray* fHitItems; /**< Array with Hit items - output data. */
    TClonesArray* fCalItemsLos;                    /**< Array with cal items. */
    UInt_t fNofHitItems;     /**< Number of hit items for cur event. */
    R3BTofdHitPar* fHitPar;                       /**< Hit parameter container. */
    UInt_t fNofHitPars;                        /**< Number of modules in parameter file. */
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */
    Int_t fTrigger;                             /**< Trigger value. */
    Double_t fTofdQ;
    UInt_t fNofPlanes;  
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */    

// arrays of control histograms
    TH1F* fhPos[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fhTotPm1[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fhTotPm2[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fhTotPm1Sat[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fhTotPm2Sat[N_PLANE_MAX][N_PADDLE_MAX];     
    TH1F* fhQPm1[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fhQPm2[N_PLANE_MAX][N_PADDLE_MAX]; 
    TH1F* fhQ[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhQ1vsPos[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhQ2vsPos[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhQvsPos[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhTotPm1vsTotPm2[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhTotPm1satvsTotPm2sat[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhTdiffvsQ[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhTot1vsPos[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhTot2vsPos[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhQvsQ[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhQvsTof[N_PLANE_MAX][N_PADDLE_MAX];   
    TH2F* fhSqrtQvsPos[N_PLANE_MAX][N_PADDLE_MAX];   
    
    TH2F* fhxy;   
    TH1F* fhCharge3mm; 
    TH1F* fhCharge5mm; 
    TH1F* fhCharge; 
    TH2F* fhSaturation1;     
    TH2F* fhSaturation2;
    TH1F* fhLosTimeM;     
    TH1F* fhLosTimeP;
    TH1F* fhLosTimeP_corr;
    TH2F* fhLosTimeP_vs_TimeM;
    TH2F* fhLosTimeP_vs_TimeM_corr;
    TH1F* fhLosQ1;
    TH1F* fhLosQ2;
    TH2F* fhLosQ;
    TH2F* fhLosQ_corr;
    TH2F* fhLosQvsTP;
    TH2F* fhLosQvsTP_corr;
    TH2F* fhLosQvsTM;
    TH2F* fhLosQvsX;
    TH2F* fhLosQvsY;
    TH2F* fhLosXYP;
    TH2F* fh_los_pos;
    
       
  public:
    ClassDef(R3BTofdCal2Hit, 1)
};

#endif

