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

// -----------------------------------------------------------
// -----             R3BRpcOnlineSpectra                 -----
// -----   Created 23/01/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill RPC online histograms              -----
// -----------------------------------------------------------

#ifndef R3BRpcOnlineSpectra_H
#define R3BRpcOnlineSpectra_H

#include "FairTask.h"

#include "TCanvas.h"
#include "TMath.h"
#include <Rtypes.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class TClonesArray;
class R3BEventHeader;
class TH1F;
class TH2F;

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

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option) override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent() override;

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask() override;

    /**
     * Method to reset histograms
     */
    void Reset_RPC_Histo();

    /**
     * Method to set the trigger
     */
    void SetTrigger(Int_t trigg) { fTrigger = trigg; }

  private:
    R3BEventHeader* fEventHeader; // // Pointer to the R3BEventHeader structure
    TClonesArray* fRefMappedItems;   // Array with mapped items.
    TClonesArray* fPmtMappedItems;   // Array with mapped items.
    TClonesArray* fStripMappedItems;   // Array with mapped items.
    TClonesArray* fStripPreCalDataItems;
    TClonesArray* fStripCalDataItems;
    TClonesArray* fPmtPreCalItems;
    TClonesArray* fStripHitDataItems;

    Float_t meanCharges[41][5]={{0.0}};
    Int_t counts=0;



    R3BEventHeader* header; // Event header.
    Int_t fTrigger;         // Trigger value.
    Int_t fNEvents;         // Event counter.

    /* ----- Map Histograms -----*/
    TH1F **stripCoarseRightHisto;
    TH1F **stripFineRightHisto;

    TH1F **stripCoarseLeftHisto;
    TH1F **stripFineLeftHisto;

    TH1F **pmtCoarseHisto;
    TH1F **pmtFineHisto;

    TH1F **refCoarseHisto;
    TH1F **refFineHisto;

    TH2F *stripCoarseLeftCorr;
    TH2F *stripCoarseRightCorr;

    TH2F *stripFineLeftCorr;
    TH2F *stripFineRightCorr;

    /* ----- Map Canvases*/
    TCanvas *leftStripCanvasCoarse;
    TCanvas *rightStripCanvasCoarse;

    TCanvas *leftStripCanvasFine;
    TCanvas *rightStripCanvasFine;

    TCanvas *stripCoarseLeftCorrCanvas;
    TCanvas *stripCoarseRightCorrCanvas;

    TCanvas *stripFineLeftCorrCanvas;
    TCanvas *stripFineRightCorrCanvas;

    TCanvas *pmtCoarseCanvas;
    TCanvas *pmtFineCanvas;
    TCanvas *refFineCanvas;
    TCanvas *refCoarseCanvas;

    /* ----- Pre Cal Histograms ----- */
    TH2F *stripLeftTotCorr;
    TH2F *stripRightTotCorr;
    TH2F *stripLeftTimeCorr;
    TH2F *stripRightTimeCorr;
    TH1F **pmtPreCalTimeHisto;
    TH1F **pmtPreCalTotHisto;

    /* ----- Pre Cal Canvases ----- */
    TCanvas *stripLeftTotCorrCanvas;
    TCanvas *stripRightTotCorrCanvas;
    TCanvas *stripLeftTimeCorrCanvas;
    TCanvas *stripRightTimeCorrCanvas;
    TCanvas *pmtPreCalTimeCanvas;
    TCanvas *pmtPreCalTotCanvas;


    /* ----- Cal Histograms ----- */
    TH2F *stripCalTimeCorr;
    TH2F *stripCalToTCorr;

    /* ----- Cal Canvases ----- */
    TCanvas *stripCalTimeCorrCanvas;
    TCanvas *stripCalTotCorrCanvas;

    /* ----- Hit Histograms ----- */
    TH2F *stripPosHitCorr;
    TH2F *meanChargeCorr;
    TH1F *totalChargeHist;

    /* ----- Hit Canvases ----- */
    TCanvas *hitMapCanvas;


  public:
    ClassDefOverride(R3BRpcOnlineSpectra, 1)
};

#endif /* R3BRpcOnlineSpectra_H */
