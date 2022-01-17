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

#ifndef R3BRPCONLINESPECTRA
#define R3BRPCONLINESPECTRA

#include "FairTask.h"
#include "TCanvas.h"
#include "TMath.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#define Nb_Sides 2
#define Nb_Channels 44

class TClonesArray;
class TH1F;
class TH1I;
class TH2F;
class R3BEventHeader;

/**
 * This taks reads RPC data and plots online histograms
 */
class R3BRpcOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BRpcOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BRpcOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BRpcOnlineSpectra();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /** Virtual method ReInit **/
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

    /**
     * Method to select binning and max range
     */
    inline void SetRange_bins(Int_t Histos_bins) { fMapHistos_bins = Histos_bins; }
    inline void SetRange_max(Int_t Histos_max) { fMapHistos_max = Histos_max; }

    /**
     * Method for setting Tot histograms
     */
    inline void SetTotHist(Bool_t opt) { fTotHist = opt; }

    /**
     * Method to reset histograms
     */
    void Reset_RPC_Histo();

    /**
     * Method to change histogram scales
     */
    void Log_RPC_Histo();

  private:
    void SetParameters();

    Int_t fMapHistos_max;
    Int_t fMapHistos_bins;

    TClonesArray* fMappedItems; /**< Array with mapped items.    */
    TClonesArray* fCalItems;    /**< Array with cal items.       */
    TClonesArray* fHitItems;    /**< Array with hit items.       */

    // Check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header.  */
    Int_t fNEvents;         /**< Event counter. */

    Int_t fNumSides;          /**< Number of Sides, left and right.   */
    Int_t fNumChannels;       /**< Number of Channels.   */

    Bool_t fLogScale;       /**< Selecting scale. */
    Bool_t fTotHist;        /**< Tot histograms selector. */

    // Canvas
    TCanvas* cRPCMult;
    TCanvas* cMap[Nb_Sides][Nb_Channels];
    TCanvas* cRPCTime;

    // Histograms
    TH1F* fh1_RPC_Mult;
    TH2F* fh2_RPC_tof_time;

  public:
    ClassDef(R3BRpcOnlineSpectra, 1)
};

#endif
