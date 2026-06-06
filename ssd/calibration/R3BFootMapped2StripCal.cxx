/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------------
// -----                  R3BAmsMapped2StripCal source file            -----
// -----             Created 29/05/18  by J.L. Rodriguez-Sanchez       -----
// -------------------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TDirectory.h>
#include <TF1.h>
#include <TMath.h>
#include <TSystem.h>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// FOOT headers
#include "R3BFootCalData.h"
#include "R3BFootCalPar.h"
#include "R3BFootMapped2StripCal.h"
#include "R3BFootMappedData.h"
#include "R3BLogger.h"
#include "R3BShared.h"

// R3BFootMapped2StripCal: Default Constructor --------------------------
R3BFootMapped2StripCal::R3BFootMapped2StripCal()
    : R3BFootMapped2StripCal("FootMapped2StripCal", 1)
{
}

// R3BFootMapped2StripCalPar: Standard Constructor --------------------------
R3BFootMapped2StripCal::R3BFootMapped2StripCal(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fFootMappedData("FootMappedData")
{
    // Initialize histograms for each detector
    h2_raw.resize(fNDets);
    h2_fine.resize(fNDets);
    fine_sigmas.resize(fNDets, std::vector<Double_t>(fNStrip, 0.0f));
    for (Int_t i = 0; i < fNDets; i++)
    {
        TString hname = TString::Format("h2_raw_%d", i);
        h2_raw[i] = R3B::root_owned<TH2F>(hname.Data(), hname.Data(), 640, 1, 641, 700, 0, 700);
        h2_raw[i]->SetDirectory(0);

        hname = TString::Format("h2_fine_%d", i);
        h2_fine[i] = R3B::root_owned<TH2F>(hname.Data(), hname.Data(), 640, 1, 641, 400, -200, 200);
        h2_fine[i]->SetDirectory(0);
    }
}

// Virtual R3BFootMapped2StripCal: Destructor
R3BFootMapped2StripCal::~R3BFootMapped2StripCal()
{

    R3BLOG(debug1, "");
    delete fFootCalData;
}

void R3BFootMapped2StripCal::SetParContainers()
{
    // Parameter Container
    // Reading footCalPar from FairRuntimeDb

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(error, !rtdb, "FairRuntimeDb not found");

    fCal_Par = dynamic_cast<R3BFootCalPar*>(rtdb->getContainer("footCalPar"));
    if (!fCal_Par)
    {
        R3BLOG(error, "footCalPar container not found");
    }
    else
    {
        R3BLOG(info, "footCalPar found");
    }
}

void R3BFootMapped2StripCal::SetParameter()
{

    if (fCal_Par == nullptr)
    {
        R3BLOG(error, "footCalPar not found");
        return;
    }

    fNDets = fCal_Par->GetNumDets();
    fNStrip = fCal_Par->GetNumStrips();
    fNumParams = fCal_Par->GetNumParsFit();

    R3BLOG(info, "Nb detectors: " << fNDets);
    R3BLOG(info, "Nb strips: " << fNStrip);
    R3BLOG(info, "Nb parameters from pedestal fit: " << fNumParams);

    // Initialize CalParams
    CalParams = fCal_Par->GetStripCalParams();
    if (!CalParams)
    {
        R3BLOG(error, "Error loading strip calibration parameters!");
        return;
    }
    Int_t array_size = fNDets * fNStrip * fNumParams;
    CalParams->Set(array_size);

    // Initialize fine sigmas
    TArrayF* fineSigmasArray = fCal_Par->GetFineSigmas();
    if (fineSigmasArray)
    {
        for (Int_t d = 0; d < fNDets; d++)
        {
            for (Int_t i = 0; i < fNStrip; i++)
            {
                fine_sigmas[d][i] = fineSigmasArray->GetAt(d * fNStrip + i);
            }
        }
    }
    else
    {
        R3BLOG(warn,
               "FineSigmas array not found, initializing with default value 20. Please, disregard the 10.000 entries!");
        for (Int_t d = 0; d < fNDets; d++)
        {
            for (Int_t i = 0; i < fNStrip; i++)
            {
                fine_sigmas[d][i] = 20.;
                fCal_Par->SetFineSigma(0., d * fNStrip + i);
            }
        }
        fCal_Par->setChanged(kTRUE);
    }

    // Count the number of dead strips per FOOT detector
    for (Int_t d = 0; d < fNDets; d++)
    {
        Int_t numdeadstrips = 0;
        for (Int_t i = 0; i < fNStrip; i++)
        {
            if (CalParams->GetAt(fNumParams * i + 1 + fNStrip * d * fNumParams) == -1)
                numdeadstrips++;
        }
        R3BLOG(info, "Nb of dead strips in FOOT detector " << d + 1 << ": " << numdeadstrips);
    }
}

void R3BFootMapped2StripCal::ReadPedestalsAndSigmas()
{
    pedestals.resize(fNDets, std::vector<double>(fNStrip, 0.0));
    sigmas.resize(fNDets, std::vector<double>(fNStrip, 0.0));

    for (Int_t d = 0; d < fNDets; d++)
    {
        for (Int_t i = 0; i < fNStrip; i++)
        {
            pedestals[d][i] = CalParams->GetAt(fNumParams * i + d * fNumParams * fNStrip);
            sigmas[d][i] = CalParams->GetAt(fNumParams * i + 1 + d * fNumParams * fNStrip);
        }
    }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BFootMapped2StripCal::Init()
{
    R3BLOG(info, "");

    FairRootManager* rmg = FairRootManager::Instance();
    R3BLOG_IF(fatal, rmg == nullptr, "FairRootManager not found");

    // INPUT DATA
    fFootMappedData.Init();

    // OUTPUT DATA
    fFootCalData = new TClonesArray("R3BFootCalData");
    rmg->Register("FootCalData", "FOOT strip Cal", fFootCalData, !fOnline);
    fFootCalData->Clear();

    SetParameter();
    ReadPedestalsAndSigmas();

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BFootMapped2StripCal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method FinishEvent   ------------------------------------------
void R3BFootMapped2StripCal::FinishEvent() { eventNumber++; }

// -----   Public method SetPedestals   -----------------------------------------
void R3BFootMapped2StripCal::SetPedestals(Int_t thr, TString conf)
{

    // Histograms to contain the pedestals and sigma values
    TH1D* h1_peds = nullptr;
    TH1D* h1_sigma = nullptr;
    TH1D* h_ignore0 = nullptr;
    TH1D* h_ignore_chi2 = nullptr;

    // Choose between setting the pedestals or fine sigmas
    std::vector<TH2F*>* h2 = nullptr;

    if (conf == "pedestals")
    {
        h2 = &h2_raw;
    }
    if (conf == "fine_sigmas")
    {
        h2 = &h2_fine;
    }

    if (!h2)
    {
        R3BLOG(error, "Invalid configuration: " << conf);
        return;
    }

    for (Int_t iDet = 0; iDet < fNDets; iDet++)
    {

        if ((*h2)[iDet]->Integral() < 1)
        {
            continue;
        }

        // Clean the raw histogram for bins below threshold
        TString hname = TString((*h2)[iDet]->GetTitle()) + "_clean";

        auto* h2_clean = R3B::root_owned<TH2F>(hname.Data(),
                                               hname.Data(),
                                               (*h2)[iDet]->GetNbinsX(),
                                               (*h2)[iDet]->GetXaxis()->GetXmin(),
                                               (*h2)[iDet]->GetXaxis()->GetXmax() + 1,
                                               (*h2)[iDet]->GetNbinsY(),
                                               (*h2)[iDet]->GetYaxis()->GetXmin(),
                                               (*h2)[iDet]->GetYaxis()->GetXmax() + 1);

        h2_clean->SetDirectory(0);
        for (int xbin = 1; xbin <= (*h2)[iDet]->GetNbinsX(); ++xbin)
        {
            for (int ybin = 1; ybin <= (*h2)[iDet]->GetNbinsY(); ++ybin)
            {
                double binContent = (*h2)[iDet]->GetBinContent(xbin, ybin);
                h2_clean->SetBinContent(xbin, ybin, (binContent < thr) ? 0.0 : binContent);
            }
        }

        // Perform profiling of the cleaned data along x bins

        TF1 foo("foo", "gaus", -700, 700);

        if (conf == "fine_sigmas")
        {
            foo.SetRange(-200, 200);
        }

        h2_clean->FitSlicesY(&foo, 1, h2_clean->GetNbinsX(), 0, "QNR", 0);

        h_ignore0 = dynamic_cast<TH1D*>(gDirectory->Get(TString(h2_clean->GetTitle()) + "_0"));
        if (h_ignore0)
            h_ignore0->SetDirectory(0);
        h_ignore_chi2 = dynamic_cast<TH1D*>(gDirectory->Get(TString(h2_clean->GetTitle()) + "_chi2"));
        if (h_ignore_chi2)
            h_ignore_chi2->SetDirectory(0);
        h1_peds = dynamic_cast<TH1D*>(gDirectory->Get(TString(h2_clean->GetTitle()) + "_1"));
        if (h1_peds)
            h1_peds->SetDirectory(0);
        h1_sigma = dynamic_cast<TH1D*>(gDirectory->Get(TString(h2_clean->GetTitle()) + "_2"));
        if (h1_sigma)
            h1_sigma->SetDirectory(0);

        if (conf == "pedestals")
        { // Update the pedestal parameters in the container
            for (Int_t istrip = 0; istrip < fNStrip; istrip++)
            {
                Float_t ped = h1_peds->GetBinContent(istrip + 1);
                Float_t sig = h1_sigma->GetBinContent(istrip + 1);

                // Do not update dead strip
                if (sigmas[iDet][istrip] == -1)
                {
                    sig = -1.;
                    ped = -1.;
                }

                sigmas[iDet][istrip] = sig;
                pedestals[iDet][istrip] = ped;

                Int_t index_ped = fNumParams * istrip + iDet * fNumParams * fNStrip;
                Int_t index_sig = fNumParams * istrip + 1 + iDet * fNumParams * fNStrip;
                fCal_Par->SetStripCalParams(ped, index_ped);
                fCal_Par->SetStripCalParams(sig, index_sig);
                fCal_Par->setChanged(kTRUE);
            }
        }

        if (conf == "fine_sigmas")
        {
            for (Int_t istrip = 0; istrip < fNStrip; istrip++)
            {
                fine_sigmas[iDet][istrip] = h1_sigma->GetBinContent(istrip + 1);
                fCal_Par->SetFineSigma(fine_sigmas[iDet][istrip], iDet * fNStrip + istrip);
                fCal_Par->setChanged(kTRUE);
                Float_t sig;
                if (sigmas[iDet][istrip] == -1)
                {
                    sig = -1.;
                }
                else
                {
                    sig = fine_sigmas[iDet][istrip];
                }
                sigmas[iDet][istrip] = sig;
            }
        }

        if (h_ignore0)
        {
            delete h_ignore0;
            h_ignore0 = nullptr;
        }
        if (h_ignore_chi2)
        {
            delete h_ignore_chi2;
            h_ignore_chi2 = nullptr;
        }

        (*h2)[iDet]->Reset();
    }

    // Clean up dynamically created histograms
    if (h1_peds)
    {
        delete h1_peds;
        h1_peds = nullptr;
    }
    if (h1_sigma)
    {
        delete h1_sigma;
        h1_sigma = nullptr;
    }
}

// -----   Public method Execution   -------------------------------------------
void R3BFootMapped2StripCal::Exec(Option_t* /*option*/)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Mapped Data --
    const auto mappedDataNl = fFootMappedData.Retrieve();

    int8_t detId;
    int16_t stripId;

    const int detindex = fCal_Par->GetNumDets();
    const int asics_index = 10;

    std::vector<int> StripNAve(detindex, 0);
    std::vector<std::vector<double>> AveASIC(detindex, std::vector<double>(asics_index, 0.0));
    std::vector<std::vector<int>> StripNAveASIC(detindex, std::vector<int>(asics_index, 0));
    std::vector<int> StripCounter(detindex, 0);

    std::vector<std::vector<double>> energies(fNDets, std::vector<double>(fNStrip, 0.0));

    for (const auto& data : mappedDataNl)
    {
        detId = data->GetDetId() - 1;
        stripId = data->GetStripId() - 1;
        energies[detId][stripId] = data->GetEnergy();
    }

    if (((eventNumber % fPedestalRefRate) == 0) && (eventNumber != 0))
    {
        SetPedestals(fThresholdPed, "pedestals");
        fCal_Par->setChanged(kTRUE);
    }

    if (((eventNumber % fSigmaRefRate) == 0) && (eventNumber != 0))
    {
        SetPedestals(fThresholdSig, "fine_sigmas");
        fCal_Par->setChanged(kTRUE);
    }

    // Pedestal subtraction
    for (const auto& data : mappedDataNl)
    {
        detId = data->GetDetId() - 1;
        stripId = data->GetStripId() - 1;

        h2_raw[detId]->Fill(stripId + 1, energies[detId][stripId]);

        energies[detId][stripId] -= pedestals[detId][stripId];
        if (energies[detId][stripId] < fTimesSigma * sigmas[detId][stripId])
        {
            Int_t ASIC = static_cast<int>(stripId / 64);
            AveASIC[detId][ASIC] += energies[detId][stripId];
            StripNAveASIC[detId][ASIC]++;
        }
    }
    for (Int_t i = 0; i < detindex; i++)
    {
        for (Int_t j = 0; j < asics_index; j++)
        {
            if (StripNAveASIC[i][j] != 0)
                AveASIC[i][j] /= StripNAveASIC[i][j];
        }
    }

    // ASIC Average correction
    for (const auto& data : mappedDataNl)
    {
        detId = data->GetDetId() - 1;
        stripId = data->GetStripId() - 1;

        Int_t ASIC = static_cast<int>(stripId / 64);
        energies[detId][stripId] -= AveASIC[detId][ASIC];

        if (energies[detId][stripId] < fTimesSigma * sigmas[detId][stripId])
        {
            h2_fine[detId]->Fill(stripId + 1, energies[detId][stripId]);
        }

        if (energies[detId][stripId] > 0. && sigmas[detId][stripId] != -1)
            StripCounter[detId]++;
    }

    for (const auto& data : mappedDataNl)
    {
        detId = data->GetDetId() - 1;
        stripId = data->GetStripId() - 1;

        if (sigmas[detId][stripId] != -1 &&
            StripCounter[detId] < fNStrip) // allow also negative values for the energies
        {
            AddCalData(detId + 1, stripId + 1, energies[detId][stripId], fine_sigmas[detId][stripId]);
        }
    }
}

// -----   Public method Reset   ------------------------------------------------
void R3BFootMapped2StripCal::Reset()
{

    R3BLOG(debug1, "Clearing StripCalData Structure");
    if (fFootCalData)
        fFootCalData->Clear();
}

// -----   Private method AddCalData  --------------------------------------------
R3BFootCalData* R3BFootMapped2StripCal::AddCalData(int8_t detid, int16_t stripid, double energy, Double_t sigma)
{
    TClonesArray& clref = *fFootCalData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFootCalData(detid, stripid, energy, sigma);
}

ClassImp(R3BFootMapped2StripCal)