/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------------
// -----                  R3BLosTCal2Hit                        -----
// -----       Updated January 16th 2023 by N.Mozumdar          -----
// ----- Convert time calibrated data to hit level (single time) ----
// ------------------------------------------------------------------

#pragma once

#include <map>

#include <FairTask.h>

#include <TArrayF.h>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;
class R3BLosHitPar;
class R3BCoarseTimeStitch;

class R3BLosTCal2Hit : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BLosTCal2Hit();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosTCal2Hit(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BLosTCal2Hit();

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

    inline void SetLosParamMCFD(Double_t offsetX, Double_t offsetY, Double_t veffX, Double_t veffY)
    {
        flosOffsetX = offsetX;
        flosOffsetY = offsetY;
        flosVeffX = veffX;
        flosVeffY = veffY;
    }

    inline void SetLosParamToT(Double_t offsetXQ, Double_t offsetYQ, Double_t veffXQ, Double_t veffYQ)
    {
        flosOffsetXQ = offsetXQ;
        flosOffsetYQ = offsetYQ;
        flosVeffXQ = veffXQ;
        flosVeffYQ = veffYQ;
    }

    inline void SetLosParamTAMEX(Double_t offsetXT, Double_t offsetYT, Double_t veffXT, Double_t veffYT)
    {
        flosOffsetXT = offsetXT;
        flosOffsetYT = offsetYT;
        flosVeffXT = veffXT;
        flosVeffYT = veffYT;
    }

    /**
     * Methods for setting input files
     */
    inline void SetLosInput(std::string const& walk_param_file, std::string const& tot_param_file)
    {
        fwalk_param_file = walk_param_file;
        ftot_param_file = tot_param_file;
    }

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat) { fTpat = tpat; }
    inline void SetNofDet(UInt_t nDets) { fNofDetectors = nDets; }

    virtual void SetParContainers();

    /** Method to select online mode **/
    inline void SetOnline(bool option = true) { fOnline = option; }

    inline void SetVFTXWindow(Double_t window) { fWindowV = window; }

  private:
    void SetParameter();
    R3BLosHitPar* fLosHit_Par = nullptr;        // Parameter container
    TClonesArray* fTCalItems = nullptr;         /**< Array with Cal items - input data. */
    TClonesArray* fTCalTriggerItems = nullptr;  /**< Array with Trigger Cal items - input data. */
    TClonesArray* fHitItems = nullptr;          /**< Array with Hit items - output data. */
    R3BCoarseTimeStitch* fTimeStitch = nullptr; /**< Array with Hit items - output data. */
    Float_t fp0 = 0., fp1 = 1., fp2 = 0.;
    TArrayF* fLEMatchParams;
    TArrayF* fTEMatchParams;

    bool fOnline = false; // Don't store data for online

    R3BEventHeader* header = nullptr; /**< Event header. */

    // check for trigger
    Int_t fTrigger; /**< Trigger value. */
    Int_t fTpat;

    Int_t fNumParamsTamexLE;
    Int_t fNumParamsTamexTE;
    UInt_t fNofDetectors; /**< Number of detectors. */

    Double_t fClockFreq; /**< Clock cycle in [ns]. */
    Double_t fWindowV;   // VFTX coincidence window in ns
    Double_t flosVeffX;
    Double_t flosVeffY;
    Double_t flosOffsetX;
    Double_t flosOffsetY;
    Double_t flosVeffXQ;
    Double_t flosVeffYQ;
    Double_t flosOffsetXQ;
    Double_t flosOffsetYQ;
    Double_t flosVeffXT;
    Double_t flosVeffYT;
    Double_t flosOffsetXT;
    Double_t flosOffsetYT;
    Double_t walk_par[16][11]{}; // Array containing walk parameters: x=PM, y=min,max,p0...p9; MCFD and TAMEX considered
    Double_t tot_par[8][4]{};    // Array containing walk parameters: x=PM, y=p0...p3;
    std::string fwalk_param_file;
    std::string ftot_param_file;

    Int_t Icount = 0;

  public:
    ClassDef(R3BLosTCal2Hit, 1)
};
