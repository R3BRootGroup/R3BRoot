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
#include "TLatex.h"


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
    void Reset_Efficiencies();


    void SetTofAxisRange(Int_t bins, Float_t leftLimit,Float_t rightLimit){

        fLeftTofLim  = leftLimit;
        fRightTofLim = rightLimit;
        fTofBins = bins;
    };


    void SetRpcTimeAxisRange(Int_t bins, Float_t leftLimit,Float_t rightLimit){

        fLeftRpcTimeLim  = leftLimit;
        fRightRpcTimeLim = rightLimit;
        fRpcTimeBins = bins;
    };


    void SetRpcToTAxisRange(Int_t bins, Float_t leftLimit,Float_t rightLimit){

        fLeftRpcToTLim  = leftLimit;
        fRightRpcToTLim = rightLimit;
        fRpcToTBins = bins;
    };



    /**
     * Method to set the trigger
     */
    void SetTrigger(Int_t trigg) { fTrigger = trigg; }

    double fmod_calc(double a, double b) {
         Int_t c = 2048*5;
	 return fmod(a - b +c +c/2.,c) -c/2.;
    }

  private:
    R3BEventHeader* fEventHeader; // // Pointer to the R3BEventHeader structure
    int fcounter;
    TClonesArray* fMappedDataItems;   // Array with mapped items.
    TClonesArray* fPreCalDataItems;
    TClonesArray* fCalDataItems;
    TClonesArray* fHitDataItems;
    TClonesArray* fPmtHitDataItems;

    Float_t bar_1_hits[90]={0.0};
    Float_t bar_2_hits[90]={0.0};
    Float_t bar_3_hits[90]={0.0};


    Float_t rpcHitMatrix[41][50] = {{0.0}};
    Float_t meanCharges[41][50]  = {{0.0}};


    Int_t counts=0;
    Int_t tpatbin,fTPat;
    int64_t fTimeStart,fTimeEnd;


    Float_t fLeftTofLim;
    Float_t fRightTofLim;

    Float_t fLeftRpcTimeLim;
    Float_t fRightRpcTimeLim;

    Float_t fLeftRpcToTLim;
    Float_t fRightRpcToTLim;

    Int_t fTofBins;
    Int_t fRpcTimeBins;
    Int_t fRpcToTBins;
    Float_t fBarHits;
    Float_t fStrip21Hits;



    R3BEventHeader* header; // Event header.
    Int_t fTrigger;         // Trigger value.
    Int_t fNEvents;         // Event counter.

    /* ----- Map Histograms -----*/
    TH1F **stripCoarseRightHisto;
    TH1F **stripFineRightHisto;

    TH1F **stripCoarseLeftHisto;
    TH1F **stripFineLeftHisto;

    TH1F **pmtCoarseHistoTop;
    TH1F **pmtFineHistoTop;
    TH1F **pmtCoarseHistoBottom;
    TH1F **pmtFineHistoBottom;


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
    TH2F **stripLeftBanana;
    TH2F **stripRightBanana;
    TH1F **pmtPreCalTimeHistoTop;
    TH1F **pmtPreCalTotHistoTop;
    TH1F **pmtPreCalTimeHistoBottom;
    TH1F **pmtPreCalTotHistoBottom;


    /* ----- Pre Cal Canvases ----- */
    TCanvas *stripLeftTotCorrCanvas;
    TCanvas *stripRightTotCorrCanvas;
    TCanvas *stripLeftTimeCorrCanvas;
    TCanvas *stripRightTimeCorrCanvas;
    TCanvas *stripLeftBananaCanvas;
    TCanvas *stripRightBananaCanvas;
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

    /* ------ ToF Correlations Histograms ------*/
    TH2F *tofCorr;
    TH1F **stripTofHisto;
    TH2F *tofVsPosCorr;

    /* ------ ToF Correlations Canvases ------*/
    TCanvas *tofCorrCanvas;
    TCanvas *stripTofCanvas;
    TCanvas *tofPosCanvas;

    /* ------ Strip - Bar Correlation -----*/
    TCanvas *timeDiffStripPmtCanvas;
    TCanvas *timeDiffStripPmtCorrCanvas;
    TCanvas *posRpcCorrCanvas;
    TCanvas *timeDiffLosPmtCanvas;
    TCanvas *stripIdVsHitsCanvas;

    /* ------ Strip - Bar Correlation Histograms -----*/
    TH1F **timeDiffStripPmtHisto;
    TH2F *timeDiffStripPmtCorr;

    TH2F *deltaTVsPosCorr;
    TH2F *posRpcVsPosBar;

    TH2F *stripIdVsNHitsCorr;

    /*------------ Efficiency Canvases --------*/
    TCanvas *stripMultCanvas;
    TCanvas *efficiencyCanvas;

    /*--------- Efficiency Histograms ----------*/
    TH1F *stripMultHisto;
    TH2F *bar_1_efficiency_pos_corr;
    TH2F *bar_2_efficiency_pos_corr;
    TH2F *bar_3_efficiency_pos_corr;

    TLatex *tex1,*tex2;

  public:
    ClassDefOverride(R3BRpcOnlineSpectra, 1)
};

#endif /* R3BRpcOnlineSpectra_H */
