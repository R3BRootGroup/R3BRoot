/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum f�r Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/
// ------------------------------------------------------------
// -----                 R3BTofdTofiCal2HitS494                   -----
// -----         Created May 30th 2016 by M.Heil          -----
// -----           Modified Mar 2020 by L.Bott            -----
// ------------------------------------------------------------

#ifndef R3BTOFDTOFICAL2HITS494
#define R3BTOFDTOFICAL2HITS494

#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44
#define N_TOFI_HIT_PLANE_MAX 1
#define N_TOFI_HIT_PADDLE_MAX 24
#include <map>

#include "FairTask.h"
#include "THnSparse.h"

class TClonesArray;
class R3BTofdHitModulePar;
class R3BTofdHitPar;
class R3BTofiHitModulePar;
class R3BTofiHitPar;
class R3BEventHeader;
class TH1F;
class TH2F;

/**
 * An analysis task to apply HIT calibration for ToFd.
 * it also applies walk corrections and Z-calibration
 */
class R3BTofdTofiCal2HitS494 : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofdTofiCal2HitS494();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofdTofiCal2HitS494(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofdTofiCal2HitS494();

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

    virtual void CreateHistograms(Int_t iPlane, Int_t iBar);

    /**
     * Method for setting the nuclear charge of main beam
     */
    inline void SetTofdQ(Double_t Q) { fTofdQ = Q; }
	inline void SetTofiQ(Double_t Q) { fTofiQ = Q; }
    /**
     * Method for setting histograms
     */
    inline void SetTofdHisto(Bool_t Histo) { fTofdHisto = Histo; }
    /**
     * Method for setting y calculation via ToT instead of tdiff
     */
    inline void SetTofdTotPos(Bool_t ToTy) { fTofdTotPos = ToTy; }

    inline void SetSimu(Bool_t Simu) { fSimu = Simu; }
    
    inline void SetTofiTotPos(Bool_t ToTy) { fTofiTotPos = ToTy; }
    /**
     * Method for setting gap between left and right half
     */
    inline void SetTofiGap(Float_t Gap) { fTofiGap = Gap; }

    inline void SetShowProgress(Float_t progress) { fShowProgress = progress; }
    
    inline void SetCalOpt(Bool_t cal) {cal_hand = cal;}

    /**
     * Old Method for walk calculation.
     */
    /// virtual Double_t walk(Double_t Q);
    /**
     * new Method for walk calculation.
     */
    virtual Double_t walk(Double_t Q, Double_t par1, Double_t par2, Double_t par3, Double_t par4, Double_t par5);
    /**
     * Method for beta correction.
     */
    virtual Double_t betaCorr(Double_t delta);
    /**
     * Method for calculation of saturation.
     */
    virtual Double_t saturation(Double_t x);

    /**
     * Method for selecting events with certain trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat1, Int_t tpat2) { fTpat1 = tpat1;  fTpat2 = tpat2;}
    inline void SetYOffset(Bool_t yoffset) { fYOffset = yoffset;}
    /**
     * Methods for setting number of planes and paddles
     */
    inline void SetNofModulesTofd(Int_t planes, Int_t ppp)
    {
        fNofPlanes = planes;
        fPaddlesPerPlane = ppp;
    }
	inline void SetNofModulesTofi(Int_t planes, Int_t ppp)
    {
        fNofPlanesTofi = planes;
        fPaddlesPerPlaneTofi = ppp;
    }
  private:
    std::vector<TClonesArray*> fCalItems;
    
    TClonesArray* fHitItems;        /**< Array with Hit items - output data. */
    UInt_t fNofHitItems;            /**< Number of hit items for cur event. */
    
    R3BTofdHitPar* fHitPar;         /**< Hit parameter container. */
    UInt_t fNofHitPars;             /**< Number of modules in parameter file. */
    R3BTofiHitPar* fHitParTofi;         /**< Hit parameter container. */
    UInt_t fNofHitParsTofi;             /**< Number of modules in parameter file. */
    
    R3BEventHeader* header;         /**< Event header - input data. */

    enum DetectorInstances
    {
        DET_TOFD,
        DET_TOFD_TRIGG,
        DET_TOFI,
        DET_TOFI_TRIGG,
        DET_MAX
    };

    const char* fDetectorNames[DET_MAX + 1] = { "Tofd", "TofdTrigger", "Tofi", "TofiTrigger", NULL };

    Double_t fClockFreq;            /**< Clock cycle in [ns]. */
    Int_t fTrigger;                 /**< Trigger value. */
    Int_t fTpat1;
    Int_t fTpat2;
    Double_t fTofdQ;
    Double_t fTofiQ;
    Double_t fTofiGap;
    Bool_t cal_hand;
    Bool_t fShowProgress;
    Bool_t fTofdHisto;
    Bool_t fTofdTotPos, fTofiTotPos;
    Bool_t fYOffset;
    Bool_t fSimu;
    UInt_t fnEvents=0;
    UInt_t lasttpatevent=0;
    UInt_t fNofPlanes;
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */
    UInt_t fNofPlanesTofi;
    UInt_t fPaddlesPerPlaneTofi; /**< Number of paddles per plane. */
    UInt_t maxevent=0;
    UInt_t wrongtrigger=0;
    UInt_t wrongtpat=0;
    UInt_t notpat=0;
    UInt_t headertpat=0;
    UInt_t tofd_events_in_cal_level=0;
    UInt_t tofi_events_in_cal_level=0;
    UInt_t tofd_inbarcoincidence=0;
    UInt_t tofi_inbarcoincidence=0;
    UInt_t tofd_eventstore=0;
    UInt_t tofi_eventstore=0;
    UInt_t tofd_singlehit=0;
    UInt_t tofi_singlehit=0;
    UInt_t tofd_multihit=0;
    UInt_t tofi_multihit=0;
    UInt_t tofd_bars_with_multihit=0;
    UInt_t tofi_bars_with_multihit=0;
    UInt_t events_wo_tofd_hits=0;
    UInt_t goodpair=0;
    UInt_t goodpair4=0;
    UInt_t goodpair3=0;
    UInt_t goodpair1=0;
    UInt_t goodpair2=0;
    UInt_t goodpair5=0;
    UInt_t goodpair6=0;
    UInt_t goodpair7=0;
    UInt_t events_wo_tofi_hits=0;

    Double_t ytofd_offsetZ2[2][44]={0},ytofd_offsetZ6[2][44]={0},ytofd_offsetZ8[2][44]={0};
	Double_t ytofd_offsetZ3[2][44]={0},ytofd_offsetZ4[2][44]={0},ytofd_offsetZ5[2][44]={0},ytofd_offsetZ7[2][44]={0};

    // arrays of control histograms
    TH1F* fhTpat;
    TH1F* fhNoTpat;
    TH2F* fhQvsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhQ[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhxy[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhQvsEvent[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhTdiff[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhTsync[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhQ0Qt[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhTvsQ[N_TOFD_HIT_PLANE_MAX];

  public:
    ClassDef(R3BTofdTofiCal2HitS494, 1)
};

#endif
