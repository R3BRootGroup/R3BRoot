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

class R3BPWOnlineSpectra : public FairTask
{

  public:
    /* --------- Default Constructor ---------- */
    R3BPWOnlineSpectra();

    /* --------- Standard Constructor ---------- */
    R3BPWOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /* ------------ Standard  Destructor ----------- */
    virtual ~R3BPWOnlineSpectra() = default;

    /* ------------ Init Method ----------- */
    InitStatus Init() override;

    /* ------------ Exec Method ----------- */
    void Exec(Option_t* option) override;

    /* ------------ Finish Event ----------- */
    void FinishEvent() override;

    /* ------- Reset Methods : Clear histograms and time counters -------*/
    void Reset_PW_Histo();

    void SetPWTimeAxisRange(Int_t bins, Float_t leftLimit, Float_t rightLimit)
    {

        fLeftPWTimeLim = leftLimit;
        fRightPWTimeLim = rightLimit;
        fPWTimeBins = bins;
    };

    void SetPWToTAxisRange(Int_t bins, Float_t leftLimit, Float_t rightLimit)
    {

        fLeftPWToTLim = leftLimit;
        fRightPWToTLim = rightLimit;
        fPWToTBins = bins;
    };

  private:
    TClonesArray* fMappedDataItems = nullptr;
    TClonesArray* fPreCalDataItems = nullptr;
    TClonesArray* fCalDataItems = nullptr;
    TClonesArray* fHitDataItems = nullptr;

    Float_t fRightPWTimeLim = 6000;
    Float_t fLeftPWTimeLim = -6000;
    Int_t fPWTimeBins = 6000;

    Float_t fRightPWToTLim = 100;
    Float_t fLeftPWToTLim = 0;
    Int_t fPWToTBins = 400;

    // ----- Map Histograms -----
    TH1F* SiPMChannelsHisto;
    TH1F* refChannelsHisto;

    // ----- Map Canvases
    TCanvas* PWChannelsCanvas;
    TCanvas* RefChannelsCanvas;

    // ----- Pre Cal Histograms -----
    TH2F* SiPMTotCorr;
    TH2F* SiPMTimeCorr;

    // ----- Pre Cal Canvases -----
    TCanvas* PWTotCorrCanvas;
    TCanvas* PWTimeCorrCanvas;

    // ----- Cal Histograms -----
    TH2F* SiPMCalTimeCorr[16];
    TH2F* SiPMCalTotCorr[16];

    // ----- Cal Canvases -----
    TCanvas* PWCalTimeCorrCanvas1;
    TCanvas* PWCalTotCorrCanvas1;
    TCanvas* PWCalTimeCorrCanvas2;
    TCanvas* PWCalTotCorrCanvas2;

    // ----- Hit Histograms -----
    TH2F* ToTBarHitCorr;
    TH2F* ToTPosHitCorr;
    TH2F* TimeBarHitCorr;
    TH2F* TimePosHitCorr;

    TH1F* BarHitHisto;
    TH1F* NhitsHisto;
    TH1F* NbarsHisto;
    TH2F* BarVsHits;

    // ----- Hit Canvases -----
    TCanvas* hitCorrCanvas;
    TCanvas* hitMultCanvas;

  public:
    ClassDefOverride(R3BPWOnlineSpectra, 1)
};
