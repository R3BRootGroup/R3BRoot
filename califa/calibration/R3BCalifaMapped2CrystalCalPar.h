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

#include <FairTask.h>
#include <TArrayF.h>
#include <TH1F.h>

class TClonesArray;
class R3BCalifaMappingPar;
class R3BCalifaCrystalCalPar;
class R3BEventHeader;

class R3BCalifaMapped2CrystalCalPar : public FairTask
{
  public:
    // Default constructor
    R3BCalifaMapped2CrystalCalPar();

    // Standard constructor
    explicit R3BCalifaMapped2CrystalCalPar(const std::string& name, int iVerbose = 1);

    // Destructor
    virtual ~R3BCalifaMapped2CrystalCalPar();

    // Method Init
    InitStatus Init() override;

    // Method Exec
    void Exec(Option_t* /*opt*/) override;

    // Method FinishTask
    void FinishTask() override;

    // Method ReInit
    InitStatus ReInit() override;

    // Method Search peaks and calibrate
    virtual void SearchPeaks();

    // Method SetParContainers
    void SetParContainers() override;

    // Accessor functions
    const Int_t GetNumCrystals() { return fNumCrystals; }
    const Int_t GetCalRange_left() { return fMapHistos_left; }
    const Int_t GetCalRange_right() { return fMapHistos_right; }
    const Int_t GetCalRange_bins() { return fMapHistos_bins; }
    const Int_t GetNumPeaks() { return fNumPeaks; }
    const Double_t GetSigma() { return fSigma; }
    const Double_t GetThreshold() { return fThreshold; }
    const Int_t GetNumParameterFit() { return fNumParam; }
    const Int_t GetMinStadistics() { return fMinStadistics; }

    TArrayF* GetEnergyPeaks() { return fEnergyPeaks; }

    void SetNumCrystals(Int_t numberCry) { fNumCrystals = numberCry; }
    void SetCalRange_left(Int_t Histos_left) { fMapHistos_left = Histos_left; }
    void SetCalRange_right(Int_t Histos_right) { fMapHistos_right = Histos_right; }
    void SetCalRange_bins(Int_t Histos_bins) { fMapHistos_bins = Histos_bins; }
    void SetCalRangeP_left(Int_t Histos_left) { fMapHistos_leftp = Histos_left; }
    void SetCalRangeP_right(Int_t Histos_right) { fMapHistos_rightp = Histos_right; }
    void SetCalRangeP_bins(Int_t Histos_bins) { fMapHistos_binsp = Histos_bins; }
    void SetNumPeaks(Int_t numberpeaks) { fNumPeaks = numberpeaks; }
    void SetSigma(Double_t sigma) { fSigma = sigma; }
    void SetThreshold(Double_t threshold) { fThreshold = threshold; }
    void SetNumParameterFit(Int_t numberParFit) { fNumParam = numberParFit; }
    void SetMinStadistics(Int_t minstad) { fMinStadistics = minstad; }

    void SetDebugMode(Bool_t debug) { fDebugMode = debug; }

    void SetEnergyPeaks(TArrayF* thePeaks)
    {
        fEnergyPeaks = thePeaks;
        fNumPeaks = thePeaks->GetSize();
    }

  private:
    void SetParameter();
    bool fDebugMode = false;
    Int_t fNumCrystals = 1;
    Int_t fMapHistos_left = 0; // gamma range
    Int_t fMapHistos_right = 0;
    Int_t fMapHistos_bins = 0;
    Int_t fMapHistos_leftp = 0; // particle range
    Int_t fMapHistos_rightp = 0;
    Int_t fMapHistos_binsp = 0;

    Int_t fNumParam = 0;
    Int_t fMinStadistics = 100;

    Int_t fNumPeaks = 2;
    Double_t fSigma = 2.5;
    Double_t fThreshold = 0.002;

    TArrayF* fEnergyPeaks = nullptr;
    Double_t* fChannelPeaks;

    R3BCalifaMappingPar* fMap_Par = nullptr;     // Parameter container with mapping
    R3BCalifaCrystalCalPar* fCal_Par = nullptr;  // Container for Cal parameters
    TClonesArray* fCalifaMappedDataCA = nullptr; // Array with CALIFA Mapped-input data

    TH1F** fh1_Map_energy_crystal;

  public:
    ClassDefOverride(R3BCalifaMapped2CrystalCalPar, 2); // NOLINT
};
