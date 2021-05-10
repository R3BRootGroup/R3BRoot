#ifndef R3BONLINESPECTRAPID
#define R3BONLINESPECTRAPID

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "TCanvas.h"
#include "TClonesArray.h"
#include "TH1.h"
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
class R3BOnlineSpectraPID : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineSpectraPID();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpectraPID(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpectraPID();

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
    inline void SetLosXYMCFD(Double_t offsetX1,
                             Double_t offsetY1,
                             Double_t veffX1,
                             Double_t veffY1,
                             Double_t offsetX2,
                             Double_t offsetY2,
                             Double_t veffX2,
                             Double_t veffY2)
    {
        flosOffsetXV[0] = offsetX1;
        flosOffsetYV[0] = offsetY1;
        flosVeffXV[0] = veffX1;
        flosVeffYV[0] = veffY1;
        flosOffsetXV[1] = offsetX2;
        flosOffsetYV[1] = offsetY2;
        flosVeffXV[1] = veffX2;
        flosVeffYV[1] = veffY2;
    }


    inline void SetLosXYToT(Double_t offsetXQ1,
                            Double_t offsetYQ1,
                            Double_t veffXQ1,
                            Double_t veffYQ1,
                            Double_t offsetXQ2,
                            Double_t offsetYQ2,
                            Double_t veffXQ2,
                            Double_t veffYQ2)
    {
        flosOffsetXQ[0] = offsetXQ1;
        flosOffsetYQ[0] = offsetYQ1;
        flosVeffXQ[0] = veffXQ1;
        flosVeffYQ[0] = veffYQ1;
        flosOffsetXQ[1] = offsetXQ2;
        flosOffsetYQ[1] = offsetYQ2;
        flosVeffXQ[1] = veffXQ2;
        flosVeffYQ[1] = veffYQ2;
    }
    /* Method for pile-up */
    inline void SetEpileup(Double_t Epileup) { fEpileup = Epileup; }

    /* Method for setting number of LOS detectors */
    inline void SetNofLosModules(Int_t nDets)
    {
        fNofLosDetectors = nDets;
    }

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat) { fTpat = tpat; }
    inline void SetLosEcor(Double_t par0_ZLos_cor, Double_t par1_ZLos_cor){
		fpar0_ZLos_cor = par0_ZLos_cor;
		fpar1_ZLos_cor = par1_ZLos_cor;
	}
    inline void SetLosZpar(Double_t par0_ZLos, Double_t par1_ZLos){
		fpar0_ZLos = par0_ZLos;
		fpar1_ZLos = par1_ZLos;
	}

    void Reset_PID();

  private:
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fTcalItems;
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fHitItems;
    TClonesArray* fHitItemsMus; /**< Array with hit items. */    
    TClonesArray* fWRLos;
    TClonesArray* fWRS2;

    enum DetectorInstances
    {
		DET_SCI2,
        DET_LOS,
        DET_MAX
    };

    const char* fDetectorNames[DET_MAX + 1] = { "Sci2", "Los", NULL };

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat;
    Int_t fSamp;
    Double_t fClockFreq; /**< Clock cycle in [ns]. */
    Int_t nLosEvents = 0;
    Int_t fncalib = 10000;
    //   TClonesArray *fbmonMappedItems;
    Int_t fNofLosDetectors; /**< Number of LOS detectors. */

    Double_t flosVeffXV[2];
    Double_t flosVeffYV[2];
    Double_t flosOffsetXV[2];
    Double_t flosOffsetYV[2];
    Double_t flosVeffXQ[2];
    Double_t flosVeffYQ[2];
    Double_t flosOffsetXQ[2];
    Double_t flosOffsetYQ[2];
    Double_t fpar0_ZLos, fpar1_ZLos;
    Double_t fpar0_ZLos_cor, fpar1_ZLos_cor;
    Double_t fdaq_offset;

    unsigned long long time_V_mem = 0, time_start = 0, time = 0, time_mem = 0;
    unsigned long long ts_los = 0,time_start_wrlos=0, ts_s2 = 0,time_start_s2=0;
    std::vector<unsigned long long> time_prev;
    unsigned long long time_prev_read = 0, time_to_read = 0;
    unsigned long ic_mem = 0, see_mem = 0, tofdor_mem = 0;
    unsigned long ic_start = 0, see_start = 0, tofdor_start = 0;
    unsigned long long time_spill_start = 0, time_spill_end = 0;
    long samplerPrev = 0, samplerCurr = 0, samplerSpill = 0;
    unsigned long long time_previous_event = 0;

    Double_t time_clear = -1.;
    Double_t tdiff = 0.;
    Double_t fNorm = 1.;
    Double_t fEpileup;
    Double_t dtrig;
    Double_t tref_sci2;

    unsigned long fNEvents = 0, fNEvents_start = 0; /**< Event counter. */

    TH1F* fh1_Mushit_z;
    TH1F* fh1_Mushit_z_cond;
    TH1F *fh1_dttrigger;
    TH1F *fh1_getoffset;   

	public:

	// --- hard coded calibration 
	void SetToFoffset(Double_t offset) {fToFoffset=offset;} 
	void SetToFmin(Double_t t) {fToFmin=t;}
	void SetToFmax(Double_t t) {fToFmax=t;}
	void SetTof2InvV_p0(Double_t p) {fTof2InvV_p0=p;}
	void SetTof2InvV_p1(Double_t p) {fTof2InvV_p1=p;}
	void SetFlightLength(Double_t L) {fL2 = L;}
	void SetPos_p0(Double_t p) {fPos_p0 = p;}	
	void SetPos_p1(Double_t p) {fPos_p1 = p;}	
	void SetDispersionS2(Double_t DS2)    {fDispersionS2 = DS2;}		
	void SetBrho0_S2toCC(Double_t Brho0)	{fBrho0_S2toCC = Brho0;}

  private:

	// --- hard coded calibration 
	// --- offset required when DAQ (BUS or S2) is rebooted
	Double_t fToFoffset;	
	// --- 1/ select the good hit from the ToF from S2 to Cave C:
	Double_t fToFmin;
	Double_t fToFmax;
	
	// --- 2/ calibration in beta and ToF
	Double_t fTof2InvV_p0;
	Double_t fTof2InvV_p1;
	Double_t fL2;

	// --- 3/ calibration in position
	Double_t fPos_p0;
	Double_t fPos_p1;

	// --- 4/ AoQ calculation
	Double_t fDispersionS2;
	Double_t fBrho0_S2toCC;

	// --- end of declaration of calibration parameters for AoQ

	// --- TCanvas
  TCanvas* cTofFromS2;
  TCanvas* cBeta;
  TCanvas* cZvsAoQ;
  TCanvas* cTofFromS2vsZ;
  TCanvas* cPos;

	// --- HISTOGRAMS for mult==1 
	TH1D* fh1_RawTofFromS2_TcalMult1;
  TH2F* fh1_RawTofFromS2_TcalMult1vsZ;
  TH1F* fh1_Beta_m1;
  TH1F* fh1_Betacorr_m1;
  TH2F* fh2_ZvsAoQ_m1;
  TH2F* fh2_ZvsAoQ_test;
  TH2F* fh2_ZvsBeta_m1;
  TH2F* fh2_ZvsBeta1_m1;
  TH2F* fh2_ZvsX_m1;
  TH2F* fh2_ZvsY_m1;
  TH2F* fh2_AoQ_vs_xLOS_corr;
  TH2F* fh2_Brho_vs_xLOS;
  
  TH1F* fh1_RawPos_m1;
  TH1F* fh1_CalPos_m1;
  TH2F* fh2_AoQ_vs_xLOS;

	// --- HSITOGRAMS with the selection of the good hit
  TH1D* fh1_RawTofFromS2;
  TH1F* fh1_Beta;
  TH2F* fh2_ZvsAoQ;
  TH2F* fh2_Zcorel;
  TH1F* fh1_RawPos;
  TH1F* fh1_CalPos;
  TH2F* fh2_ZmusicvsAoQ_m1;


  public:
    ClassDef(R3BOnlineSpectraPID, 2)
};

#endif
