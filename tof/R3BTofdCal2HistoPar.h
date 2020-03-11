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
// ----------------------------------------------------
// -----      Create hit parameters for TOFD      -----
// -----        Created Jul 2019 by L.Bott        -----
// ----------------------------------------------------

#ifndef R3BTOFDCAL2HISTOPAR_H
#define R3BTOFDCAL2HISTOPAR_H

#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44

#include "FairTask.h"
#include "R3BTCalEngine.h"
#include "TObject.h"

class R3BTofdHitPar;
class TClonesArray;
class R3BEventHeader;
class TH1F;
class TH2F;

#ifdef __CINT__
#pragma link C++ class R3BTofdHitModulePar + ;
#endif
class R3BTofdCal2HistoPar : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofdCal2HistoPar();
    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofdCal2HistoPar(const char* name, Int_t iVerbose = 1);
    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofdCal2HistoPar();
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
     * Method for setting the update rate for control histograms
     * @param rate an update rate value (events).
     */
    inline void SetUpdateRate(Int_t rate) { fUpdateRate = rate; }

    /**
     * Method for setting minimum required statistics per module.
     * Only detector modules with number of entries in TDC
     * distribution greater than minimum statistics will be
     * calibrated.
     * @param minStats a value of minimum statistics required.
     */
    inline void SetMinStats(Int_t minStats) { fMinStats = minStats; }

    /**
     * Method for selecting events with certain trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }

    /**
     * Method for selecting a certain paramter calculation.
     */
    inline void SetParameter(Int_t parameter) { fParameter = parameter; }

    /**
     * Method for setting number of LOS detectors and channels.
     * @param nDets number of detectors.
     * @param nCh number of channels per detector (4+master trigger?)
     */
    inline void SetNofModules(Int_t nDets, Int_t nCh)
    {
        fNofPlanes = nDets;
        fPaddlesPerPlane = nCh;
        fNofModules = nDets * nCh * 1; // 1 edge per channel
    }
    /**
     * Method for setting the y-position of a horizonzal sweep run for calibration of effective velocity of light
     */
    inline void SetTofdPos(Double_t Y) { fTofdY = Y; }
    /**
     * Method for setting the nuclear charge of main beam
     */
    inline void SetTofdQ(Double_t Q) { fTofdQ = Q; }
    /**
     * Method for setting the lower range of ToT for offset calibration
     */
    inline void SetTofdTotLow(Double_t TotLow) { fTofdTotLow = TotLow; }
    /**
     * Method for setting the upper range of ToT for offset calibration
     */
    inline void SetTofdTotHigh(Double_t TotHigh) { fTofdTotHigh = TotHigh; }
    /**
     *
     */
    inline void ReadParaFile(TString file) { fParaFile = file; }
    /**
     *
     */
    inline void ReadHistoFile(TString file) { fHistoFile = file; }

    /**
     * Method for calculation of offset.
     */
    virtual void calcOffset();

    /**
     * Method for calculation of ToT offset.
     */
    virtual void calcToTOffset(Double_t TotLow, Double_t TotHigh);

    /**
     * Method for calculation of sync offset between paddles.
     */
    virtual void calcSync();

    /**
     * Method for calculation of effective speed of light.
     */
    virtual void calcVeff();

    /**
     * Method for calculation of light attenuation factor.
     */
    virtual void calcLambda(Double_t TotLow, Double_t TotHigh);

    /**
     * Method for calculation of position dependence of scintillator.
     */
    virtual void doubleExp(TH2F* histo, Double_t min, Double_t max, Double_t*);

    /**
     * Method for calculation of z correction.
     */
    virtual void zcorr(TH2F* histo, Int_t min, Int_t max, Double_t*);

  private:
    Int_t fUpdateRate; /**< An update rate. */
    Int_t fMinStats;   /**< Minimum statistics required per module. */
    Int_t fTrigger;    /**< Trigger value. */
    Int_t fParameter;  /**< Parameter #. */

    UInt_t fNofPlanes;       /**< Number of planes. */
    UInt_t fPaddlesPerPlane; /**< Number of bars per plane. */
    UInt_t fNofModules;      /**< Total number of modules (=edges) to calibrate */

    Int_t fNEvents;             /**< Event counter. */
    R3BTofdHitPar* fCal_Par;    /**< Parameter container. */
    TClonesArray* fCalItemsLos; /**< Array with cal items. */
    TClonesArray* fCalData;     /**< Array with mapped data - input data. */
    R3BEventHeader* header;     /**< Event header - input data. */
    Double_t fClockFreq;        /**< Clock cycle in [ns]. */
    Double_t fTofdY;
    Double_t fTofdQ;
    Double_t fTofdTotLow;
    Double_t fTofdTotHigh;
    TString fParaFile;
    TString fHistoFile;

  public:
    ClassDef(R3BTofdCal2HistoPar, 1)
};

#endif
