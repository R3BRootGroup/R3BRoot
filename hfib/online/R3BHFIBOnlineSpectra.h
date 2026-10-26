/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
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

class R3BHFIBOnlineSpectra : public FairTask
{

  public:
    /* --------- Default Constructor ---------- */
    R3BHFIBOnlineSpectra();

    /* --------- Standard Constructor ---------- */
    R3BHFIBOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /* ------------ Standard  Destructor ----------- */
    virtual ~R3BHFIBOnlineSpectra() = default;

    /* ------------ Init Method ----------- */
    InitStatus Init() override;

    /* ------------ Exec Method ----------- */
    void Exec(Option_t* option) override;

    /* ------------ Finish Event ----------- */
    void FinishEvent() override;

    /* ------- Reset Methods : Clear histograms and time counters -------*/
    void Reset_HFIB_Histo();

    void SetHFIBTimeAxisRange(Int_t bins, Float_t leftLimit, Float_t rightLimit)
    {
        fLeftHFIBTimeLim = leftLimit;
        fRightHFIBTimeLim = rightLimit;
        fHFIBTimeBins = bins;
    };

    void SetHFIBToTAxisRange(Int_t bins, Float_t leftLimit, Float_t rightLimit)
    {
        fLeftHFIBToTLim = leftLimit;
        fRightHFIBToTLim = rightLimit;
        fHFIBToTBins = bins;
    };

  private:
    TClonesArray* fMappedDataItems = nullptr;
    TClonesArray* fCalDataItems = nullptr;
    TClonesArray* fHitDataItems = nullptr;

    Float_t fRightHFIBTimeLim = 6000;
    Float_t fLeftHFIBTimeLim = -6000;
    Int_t fHFIBTimeBins = 6000;

    Float_t fRightHFIBToTLim = 100;
    Float_t fLeftHFIBToTLim = 0;
    Int_t fHFIBToTBins = 400;

    // ----- Map Histograms -----
    TH1F* Fib1_FaRICHChannelsHisto[8];
    TH1F* refChannelsHisto[6];

    // ----- Map Canvases
    TCanvas* Fib1_FaRICHChannelsCanvas;
    TCanvas* refChannelsCanvas;

    // ----- Cal Histograms -----
    TH2F* Fib1_ChTotCorrHisto[8];
    TH2F* Fib1_ChTimeCorrHisto[8];

    // ----- Cal Canvases -----
    TCanvas* Fib1_ChTotCorrCanvas;
    TCanvas* Fib1_ChTimeCorrCanvas;

    // ----- Hit Histograms -----
    TH2F* ToTFibHitCorr[6];
    TH2F* TimeFibHitCorr[6];

    TH1F* Fib1_FibHitHisto;
    TH1F* Fib1_NhitsHisto;
    TH1F* Fib1_NfibHisto;
    TH2F* Fib1_FibVsHits;

    // ----- Hit Canvases -----
    TCanvas* hitTimeCanvas;
    TCanvas* hitToTCanvas;
    TCanvas* Fib1_hitMultCanvas;

  public:
    ClassDefOverride(R3BHFIBOnlineSpectra, 1)
};
