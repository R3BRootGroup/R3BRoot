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

#pragma once

#ifndef R3BCalifaCrystalCal2TotCalPar_H
#define R3BCalifaCrystalCal2TotCalPar_H 1

#include <FairTask.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TH2D.h>
#include <utility> // for std::pair
#include <vector>

class TClonesArray;
class R3BCalifaMappingPar;
class R3BCalifaTotCalPar;
class R3BEventHeader;

class R3BCalifaCrystalCal2TotCalPar : public FairTask
{

  public:
    /** Default constructor **/
    R3BCalifaCrystalCal2TotCalPar();

    /** Standard constructor **/
    R3BCalifaCrystalCal2TotCalPar(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BCalifaCrystalCal2TotCalPar();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void FinishEvent();

    virtual void FinishTask();

    virtual void Reset();

    virtual InitStatus ReInit();

    virtual void SetParContainers();

    virtual void Search_TotParams();

    /** Accessor functions **/
    const Int_t GetNumCrystals() { return fNumCrystals; }
    const Double_t GetThreshold() { return fThreshold; }
    const Int_t GetNumParameterFit() { return fNumParam; }
    const Int_t GetMinStatistics() { return fMinStatistics; }

    void SetNumCrystals(Int_t numberCry) { fNumCrystals = numberCry; }
    void SetThreshold(Double_t threshold) { fThreshold = threshold; }
    void SetNumParameterFit(Int_t numberParFit) { fNumParam = numberParFit; }
    void SetMinStatistics(Int_t minstat) { fMinStatistics = minstat; }
    void SetDebugMode(Bool_t debug) { fDebugMode = debug; }
    void SetRange(Double_t H_left, Double_t H_right, Double_t H_left_proton, Double_t H_right_proton)
    {
        fLeft = H_left;
        fRight = H_right;
        fLeftProton = H_left_proton;
        fRightProton = H_right_proton;
    }
    void SetInitFitParams(Double_t a0, Double_t a1, Double_t a0p, Double_t a1p)
    {
        fa0 = a0;
        fa1 = a1;
        fa0proton = a0p;
        fa1proton = a1p;
    }
    std::pair<std::pair<Double_t, Double_t>, std::pair<Double_t, Double_t>> ProjectAndFit(TH2* h2,
                                                                                          Int_t binx_low,
                                                                                          Int_t binx_high,
                                                                                          Double_t& prev_band1,
                                                                                          Double_t& prev_band2);

  private:
    void SetParameter();
    Bool_t fDebugMode;
    Int_t fNumCrystals;
    Int_t fNumParam;
    Int_t fMinStatistics;

    Double_t fThreshold;
    Double_t fLeft;        // lower limit of calibrated energy [keV] to be used for ToT fits (gamma range)
    Double_t fRight;       // upper limit of calibrated energy [keV] to be used for ToT fits (gamma range)
    Double_t fLeftProton;  // lower limit of calibrated energy [keV] to be used for ToT fits (proton range)
    Double_t fRightProton; // upper limit of calibrated energy [keV] to be used for ToT fits (proton range)
    Double_t fa0, fa1, fa0proton, fa1proton; // Initial parameters for the ToT fit functions

    R3BCalifaMappingPar* fMap_Par;   // Parameter container with mapping
    R3BCalifaTotCalPar* fTotCal_Par; // Container for Tot-Cal parameters
    TClonesArray* fCrystalCalDataCA; // Array with calibrated energy
    TClonesArray* fMappedDataCA;     // Array with uncalibrated ToT

    std::vector<TH2D*> tot_vs_energy_hist;
    std::vector<TCanvas*> totCanvases;
    std::vector<TGraphErrors*> fGraphs1;
    std::vector<TGraphErrors*> fGraphs2;
    std::vector<TF1*> fFits1;
    std::vector<TF1*> fFits2;

  public:
    ClassDef(R3BCalifaCrystalCal2TotCalPar, 2);
};

#endif
