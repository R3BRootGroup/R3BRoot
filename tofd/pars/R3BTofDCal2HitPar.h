/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
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
// -----               R3BTofDCal2HitPar                  -----
// -----    Created 20/04/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#ifndef R3BTofDCal2HitPar_H
#define R3BTofDCal2HitPar_H 1

#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44

#include <map>

#include "FairTask.h"
#include "TObject.h"
#include <string.h>

class R3BTofDHitPar;
class TClonesArray;
class R3BEventHeader;
class R3BTofDMappingPar;
class R3BCoarseTimeStitch;
class TH1F;
class TH2F;

class R3BTofDCal2HitPar : public FairTask
{
  public:
    /**
     * Default constructor.
     */
    R3BTofDCal2HitPar();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofDCal2HitPar(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofDCal2HitPar();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    virtual void SetParContainers();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

    void SetParameterOption(Int_t opt) { fParameter = opt; }

    /**
     * Method for setting minimum required statistics per module.
     * Only detector modules with number of entries in TDC
     * distribution greater than minimum statistics will be
     * calibrated.
     * @param minStats a value of minimum statistics required.
     */
    void SetMinStats(Int_t minStats) { fMinStats = minStats; }

    /**
     * Method for selecting events with certain trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    void SetTpat(Int_t tpat) { fTpat = tpat; }

    /**
     * Method for setting number of LOS detectors and channels.
     * @param nDets number of detectors.
     * @param nCh number of channels per detector (4+master trigger?)
     */
    void SetNofModules(Int_t nDets, Int_t nCh)
    {
        fNofPlanes = nDets;
        fPaddlesPerPlane = nCh;
        fNofModules = nDets * nCh * 1; // 1 edge per channel
    }
    /**
     * Method for setting the y-position of a horizonzal sweep run for calibration of effective velocity of light
     */
    void SetTofdPos(Double_t Y) { fTofdY = Y; }

    /**
     * Method for setting the nuclear charge of main beam
     */
    void SetTofdQ(Double_t Q) { fTofdQ = Q; }

    /**
     * Method for setting the max. charge for histograms
     */
    void SetMaxQ(Double_t Q) { fMaxQ = Q; }

    /**
     * Method for setting the nb of Z peaks to be calibrated
     */
    void SetNbZPeaks(Int_t nb) { fNbZPeaks = nb; }

    /**
     * Method for setting charge correction
     */
    void SetTofdZ(Bool_t Z) { fTofdZ = Z; }

    /**
     * Method for setting the mean ToF
     */
    void SetMeanTof(Double_t tof) { fMeanTof = tof; }

    /**
     * Method for setting the fit for the charge
     * pol1
     + pol2
     */
    void SetChargeFit(TString type) { fZfitType = type; }

    /**
     * Method for using smiley or double exponential charge correction
     */
    void SetTofdSmiley(Bool_t Smiley) { fTofdSmiley = Smiley; }

    /**
     * Method for setting the lower range of ToT for offset calibration
     */
    void SetTofdTotLow(Double_t TotLow) { fTofdTotLow = TotLow; }

    /**
     * Method for setting the upper range of ToT for offset calibration
     */
    void SetTofdTotHigh(Double_t TotHigh) { fTofdTotHigh = TotHigh; }
    
    void SetTotRange(Double_t totmin, Double_t totmax) {fToTMin = totmin; fToTMax = totmax;}

    inline void ReadHistoFile(TString file) { fHistoFile = file; }

  private:
    R3BCoarseTimeStitch* fTimeStitch;
    /**
     * Method for creating histograms.
     */
    void CreateHistograms(Int_t, Int_t);
    void SetHistogramsNULL(Int_t, Int_t);
void calcZoffset();
    void calcOffset();
    void calcSync();
    void calcVeff();
    void calcLambda(Double_t, Double_t);

    void smiley(TH2F* histo, Double_t min, Double_t max, Double_t* para);

    void doubleExp(TH2F* histo, Double_t min, Double_t max, Double_t* para);

    /**
     * Method for calculation of z correction.
     */
    int zcorr(TH2F* histo, Double_t min, Double_t max, Double_t*, Int_t, Int_t, Int_t index);

    /**
     * Method for calculation of ToT offset.
     */
    void calcToTOffset(Double_t, Double_t);

    /**
     * new Method for walk calculation.
     */
    Double_t walk(Double_t Q, Double_t par1, Double_t par2, Double_t par3, Double_t par4, Double_t par5);


    /**
     * Method for calculation of saturation.
     */
    Double_t saturation(Double_t x);

    Int_t fParameter;
    Int_t fUpdateRate; /* An update rate. */
    Int_t fMinStats;   /* Minimum statistics required per module. */
    Int_t fTrigger;    /* Trigger value. */
    Int_t fTpat;
    TString fZfitType;
    UInt_t fNofPlanes;       /* Number of planes. */
    UInt_t fPaddlesPerPlane; /* Number of bars per plane. */
    UInt_t fNofModules;      /* Total number of modules (=edges) to calibrate */

    UInt_t fNEvents; /* Event counter. */
    R3BTofDMappingPar* fMapPar;
    R3BTofDHitPar* fHitPar;         /* Parameter container. */
    TClonesArray* fCalData;         /* Array with mapped data - input data. */
    TClonesArray* fCalTriggerItems; /* Array with trigger Cal items - input data. */
    R3BEventHeader* fHeader;        /* Event header  */
    Double_t fTofdY;
    Double_t fTofdQ;
    Double_t fMinQ;
    Double_t fMaxQ;
    Double_t fToTMin;
    Double_t fToTMax;
    Int_t fNbZPeaks;
    Bool_t fTofdSmiley;
    Bool_t fTofdZ;
    UInt_t maxevent;
    Double_t fTofdTotLow;
    Double_t fTofdTotHigh;
    Double_t fMeanTof;
    TString fHistoFile;

    // Arrays of control histograms
    TH2F* fh_tofd_TotPm[N_TOFD_HIT_PLANE_MAX][3];
    TH2F* fh_tofd_TotPm1[N_TOFD_HIT_PADDLE_MAX];
    TH2F* fh_tofd_TotPm2[N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhTdiff[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhTsync[N_TOFD_HIT_PLANE_MAX];
    TH1F* fh1_tofsync[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhLogTot1vsLogTot2[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhSqrtQvsPosToT[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhQvsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhQXvsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
     TH2F* fhTot1vsTot2[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhTot1vsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhTot2vsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhTofsync[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhlos[N_TOFD_HIT_PLANE_MAX];
    TH2F* fhlost[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fhtott[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fh1_walk_t[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
    TH2F* fh1_walk_b[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
   TH2F* fZoffset[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX];
  public:
    ClassDef(R3BTofDCal2HitPar, 1)
};

#endif /* R3BTofDCal2HitPar_H */
