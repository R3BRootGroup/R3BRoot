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

// -----------------------------------------------------------------
// -----         R3BFootMapped2StripCal Header File            -----
// -----        Created 08/09/21 by J.L. Rodriguez-Sanchez     -----
// -----------------------------------------------------------------

#pragma once

#include "R3BFootCalData.h"
#include "R3BFootMappedData.h"

#include "TCAConnector.h"

#include <FairTask.h>

#include <Rtypes.h>
#include <TArrayF.h>

#include <TH1D.h>
#include <TH1F.h>
#include <TH2F.h>
#include <vector>

class TClonesArray;
class R3BFootCalPar;

class R3BFootMapped2StripCal : public FairTask
{
  public:
    /* Default constructor */
    R3BFootMapped2StripCal();

    /* Standard constructor */
    R3BFootMapped2StripCal(const TString& name, Int_t iVerbose = 1);

    /* Destructor */
    virtual ~R3BFootMapped2StripCal();

    /* Virtual method Exec */
    void Exec(Option_t* /*option*/) override;

    /* Virtual method Reset */
    virtual void Reset();

    void SetParContainers() override;

    /* Virtual method Init */
    InitStatus Init() override;

    /* Virtual method ReInit */
    InitStatus ReInit() override;

    /* Method to set online mode */
    void SetOnline(bool option) { fOnline = option; }

    /*
     * Method for setting the thresholds: Signal > sigma_strip * fTimesSigma
     */
    void SetThresholdSigma(Double_t th) { fTimesSigma = th; }

    /*
     * Method for setting the number of strip hits per FOOT detector
     */
    void SetNStrip(Double_t nstrip) { fNStrip = nstrip; }

    /*
     * Method to update the event number after each event
     */
    void FinishEvent() override;

    /*
     * Method for calculating the pedestals
     */
    void SetPedestals(Int_t thr, TString conf);

    /*
     * Method for reading the pedestals and sigmas
     */
    void ReadPedestalsAndSigmas();

    /*
     * Method for setting the thresholds
     */
    void SetThresholds(Int_t pedestal, Int_t sigma)
    {
        fThresholdPed = pedestal;
        fThresholdSig = sigma;
    }

    /*
     * Method for setting the pedestal refreshing rate
     */
    void SetRefRate(Int_t pedestal, Int_t sigma)
    {
        fPedestalRefRate = pedestal;
        fSigmaRefRate = sigma;
    }

  private:
    void SetParameter();

    int fNDets = 16;
    int fNStrip = 640;
    int MaxSigma = 6;
    int fNumParams = 2;
    int eventNumber = 0;
    int fThresholdPed = 10; // threshold for the pedestal substraction
    int fThresholdSig = 10; // threshold for fine sigma tunning
    int fPedestalRefRate = 30000;
    int fSigmaRefRate = 30000;
    double fTimesSigma = 5.;
    TArrayF* CalParams = nullptr;

    // Array of histograms for raw signals per detector
    std::vector<TH2F*> h2_raw;
    std::vector<TH2F*> h2_fine;
    std::vector<std::vector<Double_t>> fine_sigmas;
    std::vector<std::vector<double>> pedestals;
    std::vector<std::vector<double>> sigmas;

    bool fOnline = false; // Do not store data online

    R3BFootCalPar* fCal_Par = nullptr; // Parameter container
    TCAInputConnector<R3BFootMappedData> fFootMappedData;
    TClonesArray* fFootCalData = nullptr;

    // Private method to add calibrated data
    R3BFootCalData* AddCalData(int8_t detid, int16_t stripid, double energy, Double_t sigma);

  public:
    // ROOT class definition
    ClassDefOverride(R3BFootMapped2StripCal, 1)
};