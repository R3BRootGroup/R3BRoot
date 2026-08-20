/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "FairTask.h"

#include "TCanvas.h"
#include "TLatex.h"
#include "TMath.h"
#include <Rtypes.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class TClonesArray;
class TH1F;
class TH2F;

class R3BDMOnlineSpectra : public FairTask
{

  public:
    /* --------- Default Constructor ---------- */
    R3BDMOnlineSpectra();

    /* --------- Standard Constructor ---------- */
    R3BDMOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /* ------------ Standard  Destructor ----------- */
    virtual ~R3BDMOnlineSpectra() = default;

    /* ------------ Init Method ----------- */
    InitStatus Init() override;

    /* ------------ Exec Method ----------- */
    void Exec(Option_t* option) override;

    /* ------------ Finish Event ----------- */
    void FinishEvent() override;

    /* ------- Reset Methods : Clear histograms and time counters -------*/
    void Reset_DM_Histo();

    void SetDMTimeAxisRange(Int_t bins, Float_t leftLimit, Float_t rightLimit)
    {
        fLeftDMTimeLim = leftLimit;
        fRightDMTimeLim = rightLimit;
        fDMTimeBins = bins;
    };

    void SetDMToTAxisRange(Int_t bins, Float_t leftLimit, Float_t rightLimit)
    {
        fLeftDMToTLim = leftLimit;
        fRightDMToTLim = rightLimit;
        fDMToTBins = bins;
    };

  private:
    TClonesArray* fMappedDataItems = nullptr;
    TClonesArray* fCalDataItems = nullptr;
    TClonesArray* fHitDataItems = nullptr;

    Float_t fRightDMTimeLim = 6000;
    Float_t fLeftDMTimeLim = -6000;
    Int_t fDMTimeBins = 6000;

    Float_t fRightDMToTLim = 100;
    Float_t fLeftDMToTLim = 0;
    Int_t fDMToTBins = 400;

    // ----- Map Histograms -----
    TH1F* TDCChannelsHisto;
    TH1F* refChannelsHisto;

    // ----- Map Canvases
    TCanvas* DMChannelsCanvas;
    TCanvas* RefChannelsCanvas;

    // ----- Cal Histograms -----
    TH2F* ChCalTotCorr[4];
    TH2F* ChCalTimeCorr[4];

    // ----- Cal Canvases -----
    TCanvas* DM_ChTotCorrCanvas;
    TCanvas* DM_ChTimeCorrCanvas;

    // ----- Hit Histograms -----
    TH2F* ToTPadHitCorr1;
    TH2F* TimePadHitCorr1;
    TH2F* ToTPadHitCorr2;
    TH2F* TimePadHitCorr2;

    TH1F* PadHitHisto1;
    TH2F* PadHitsXY1;
    TH1F* NpadsHisto1;
    TH2F* PadVsHits1;

    TH1F* PadHitHisto2;
    TH2F* PadHitsXY2;
    TH1F* NpadsHisto2;
    TH2F* PadVsHits2;

    // ----- Hit Canvases -----
    TCanvas* hitCorrCanvas;
    TCanvas* hitPadCanvas1;
    TCanvas* hitPadCanvas2;

  public:
    ClassDefOverride(R3BDMOnlineSpectra, 1)
};
