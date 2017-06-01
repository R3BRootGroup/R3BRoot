// ------------------------------------------------------------------
// -----                  R3BLosCal2Hit                         -----
// -----            Created March 10th 2016 by R.Plag           -----
// ----- Convert time calibrated data to hit level (single time) ----
// ------------------------------------------------------------------



#ifndef R3BLOSCAL2HIT
#define R3BLOSCAL2HIT

#include <map>

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
/**
 * An analysis task to apply TCAL calibration for NeuLAND.
 * This class reads NeuLAND mapped items with TDC values and
 * produces time items with time in [ns]. It requires TCAL
 * calibration parameters, which are produced in a separate
 * analysis run containing R3BLosCal2HitFill task.
 */
class R3BLosCal2Hit : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BLosCal2Hit();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosCal2Hit(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BLosCal2Hit();

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

    inline void SetLosParamToTc(Double_t offsetX1Qc, Double_t offsetY1Qc, Double_t veffX1Qc, Double_t veffY1Qc,
                                 Double_t offsetX2Qc, Double_t offsetY2Qc, Double_t veffX2Qc, Double_t veffY2Qc)
    {
        flosOffsetX1Qc = offsetX1Qc;
        flosOffsetY1Qc = offsetY1Qc;
        flosVeffX1Qc = veffX1Qc;
        flosVeffY1Qc = veffY1Qc;     
        flosOffsetX2Qc = offsetX2Qc;
        flosOffsetY2Qc = offsetY2Qc;
        flosVeffX2Qc = veffX2Qc;
        flosVeffY2Qc = veffY2Qc;
  
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
    }     /**
     * Methods for setting Sci type and walk-param_file
     */    
     inline void SetLosInput(Int_t SciType, std::string const &walk_param_file, std::string const &tot_param_file )
     {
	   	iSciType = SciType;
	   	fwalk_param_file = walk_param_file; 
	   	ftot_param_file = tot_param_file;
     }		 
     
    /**
     * Method for walk calculation.
     */
    virtual Double_t walk(Int_t inum, Double_t tot);
    
    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

  private:
    TClonesArray* fCalItems; /**< Array with Cal items - input data. */
    TClonesArray* fHitItems; /**< Array with Hit items - output data. */
    UInt_t fNofHitItems;     /**< Number of hit items for cur event. */
    Double_t fClockFreq;     /**< Clock cycle in [ns]. */
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
    Double_t flosVeffX1Qc;   
    Double_t flosVeffY1Qc;
    Double_t flosOffsetX1Qc;
    Double_t flosOffsetY1Qc;
    Double_t flosVeffX2Qc;   
    Double_t flosVeffY2Qc;
    Double_t flosOffsetX2Qc;
    Double_t flosOffsetY2Qc;
    Double_t flosVeffX1T;   
    Double_t flosVeffY1T;
    Double_t flosOffsetX1T;
    Double_t flosOffsetY1T;
    Double_t flosVeffX2T;   
    Double_t flosVeffY2T;
    Double_t flosOffsetX2T;
    Double_t flosOffsetY2T; 
    Double_t walk_par[16][11]{}; // Array containing walk parameters: x=PM, y=min,max,p0...p9; MCFD and TAMEX considered
    Double_t tot_par[8][8]{}; // Array containing walk parameters: x=PM, y=min,max,p0...p6;
    Int_t    iSciType;
    std::string fwalk_param_file;
    std::string ftot_param_file;
    
    TClonesArray* fMapped;  /**< Array with mapped data - input data. */
 
    TH1F* fhTres_M; 
    TH1F* fhTres_T;
    TH1F* fhTres_M_corr;    
    TH1F* fhTres_T_corr;  
    TH1F* fhTres_MT_corr;  
    TH1F* fhTres_M_evCh; 
    TH1F* fhTres_M_oddCh;
    TH1F* fhTres_M_evCh_corr;    
    TH1F* fhTres_M_oddCh_corr;
    TH1F* fhTres_T_evCh; 
    TH1F* fhTres_T_oddCh;
    TH1F* fhTres_T_evCh_corr;    
    TH1F* fhTres_T_oddCh_corr;   
    TH1F* fhRW;
    TH1F* fhQ_evCh;
    TH1F* fhQ_oddCh;
    TH1F* fhQ_evCh_corr;
    TH1F* fhQ_oddCh_corr;   
    TH1F* fhQ;
    TH1F* fhQtest;	
    TH1F* fht1t5M ;
	TH1F* fht2t6M ;	
	TH1F* fht3t7M ;	
	TH1F* fht4t8M ;	
	TH1F* fht1t5M_corr ;
	TH1F* fht2t6M_corr ;	
	TH1F* fht3t7M_corr ;	
	TH1F* fht4t8M_corr;	
	TH1F* fht1t5T ;
	TH1F* fht2t6T ;	
	TH1F* fht3t7T ;	
	TH1F* fht4t8T ;	
	TH1F* fht1t5T_corr ;
	TH1F* fht2t6T_corr ;	
	TH1F* fht3t7T_corr ;	
	TH1F* fht4t8T_corr ;
	TH1F* fhQpix;	
    
    
    TH2F* fhQ_L;
    TH2F* fhQ_T;
    TH2F* fhQ_R;
    TH2F* fhQ_B;    
    TH2F* fhQ_L_corr;
    TH2F* fhQ_T_corr;
    TH2F* fhQ_R_corr;
    TH2F* fhQ_B_corr;
    TH2F* fhQ_LB;
    TH2F* fhQ_LT;
    TH2F* fhQ_RT;
    TH2F* fhQ_RB;     
    TH2F* fhQ_LB_corr;
    TH2F* fhQ_LT_corr;
    TH2F* fhQ_RT_corr;
    TH2F* fhQ_RB_corr;    
    TH2F* fhQ_vs_X;
    TH2F* fhQ_vs_Y;    
    TH2F* fhQ_vs_X_corr;
    TH2F* fhQ_vs_Y_corr;
    TH2F* fhTM_vs_Q;
    TH2F* fhTT_vs_Q;
    TH2F* fhTM_vs_Q_corr;
    TH2F* fhTT_vs_Q_corr;    
    TH2F* fhXY;    
    TH2F* fhXYT;  
    TH2F* fhXYmean;   
    TH2F* fhXY_corr; 
    TH2F* fhXY1_corr;
    TH2F* fhXY2_corr;
    TH2F* fhXY_ToT;   
    TH2F* fhXY_ToT_corr;
    TH2F* fhXY1_ToT_corr;
    TH2F* fhXY2_ToT_corr;
    TH2F* fhXY1;
    TH2F* fhXY2;
    TH2F* fhXYT1;
    TH2F* fhXYT2;    
    TH2F* fhXYQ1;
    TH2F* fhXYQ2;    
    TH2F* fhXYproj;
    TH2F* fhQ1_vs_Q5;    
    TH2F* fhQ1_vs_Q5_corr;
    TH2F* fhQ2_vs_Q6;  
    TH2F* fhQ2_vs_Q6_corr;  
    TH2F* fhQ3_vs_Q7;      
    TH2F* fhQ3_vs_Q7_corr;  
    TH2F* fhQ4_vs_Q8;
    TH2F* fhQ4_vs_Q8_corr;;
    TH2F* fhTresX_M;
    TH2F* fhTresY_M;    
    TH2F* fhTresX_T;
    TH2F* fhTresY_T;
    TH2F* fhTresX_M_corr;
    TH2F* fhTresY_M_corr;    
    TH2F* fhTresX_T_corr;
    TH2F* fhTresY_T_corr;        
    TH2F* fht1Q1M;
    TH2F* fht3Q3M;
    TH2F* fht5Q5M;
    TH2F* fht7Q7M;
    TH2F* fht2Q2M;
    TH2F* fht4Q4M;
    TH2F* fht6Q6M;
    TH2F* fht8Q8M;    
    TH2F* fht1Q1M_corr;
    TH2F* fht3Q3M_corr;
    TH2F* fht5Q5M_corr;
    TH2F* fht7Q7M_corr;
    TH2F* fht2Q2M_corr;
    TH2F* fht4Q4M_corr;
    TH2F* fht6Q6M_corr;
    TH2F* fht8Q8M_corr;        
    TH2F* fht1Q1T;
    TH2F* fht3Q3T;
    TH2F* fht5Q5T;
    TH2F* fht7Q7T;
    TH2F* fht2Q2T;
    TH2F* fht4Q4T;
    TH2F* fht6Q6T;
    TH2F* fht8Q8T;    
    TH2F* fht1Q1T_corr;
    TH2F* fht3Q3T_corr;
    TH2F* fht5Q5T_corr;
    TH2F* fht7Q7T_corr;
    TH2F* fht2Q2T_corr;
    TH2F* fht4Q4T_corr;
    TH2F* fht6Q6T_corr;
    TH2F* fht8Q8T_corr;
    TH2F* fhQ1vsIcount;
    TH2F* fhQ3vsIcount;
    TH2F* fhQ5vsIcount;
    TH2F* fhQ7vsIcount;    
    TH2F* fhQ2vsIcount;
    TH2F* fhQ4vsIcount;
    TH2F* fhQ6vsIcount;
    TH2F* fhQ8vsIcount;
    TH2F* fhTresMvsIcount;    
    TH2F* fhTreswcMvsIcount;    
    TH2F* fhTreswcTvsIcount;
    TH2F* fhTresTvsIcount;


    Int_t Icount=0;
    
  public:
    ClassDef(R3BLosCal2Hit, 1)
};

#endif

