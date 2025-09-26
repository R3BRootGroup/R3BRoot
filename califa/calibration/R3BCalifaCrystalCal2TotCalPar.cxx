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

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

#include <TClonesArray.h>
#include <TFile.h>
#include <TLegend.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TSpectrum.h>
#include <TVector3.h>
#include <iostream>
#include <stdlib.h>

#include "R3BCalifaCrystalCal2TotCalPar.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappedData.h"
#include "R3BCalifaMappingPar.h"
#include "R3BCalifaTotCalPar.h"

R3BCalifaCrystalCal2TotCalPar::R3BCalifaCrystalCal2TotCalPar()
    : R3BCalifaCrystalCal2TotCalPar("R3BCalifaCrystalCal2TotCalPar", 1)
{
}

R3BCalifaCrystalCal2TotCalPar::R3BCalifaCrystalCal2TotCalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fDebugMode(true)
    , fNumCrystals(5088)
    , fNumParam(2)
    , fMinStatistics(20)
    , fThreshold(100)
    , fLeft(10000.)
    , fRight(25000.)
    , fLeftProton(130000.)
    , fRightProton(250000.)
    , fa0(6000.)
    , fa1(1000.)
    , fa0proton(60000.)
    , fa1proton(1000.)
    , fMap_Par(nullptr)
    , fTotCal_Par(nullptr)
    , fCrystalCalDataCA(nullptr)
    , fMappedDataCA(nullptr)
    , tot_vs_energy_hist()
    , totCanvases()
    , fGraphs1()
    , fGraphs2()
    , fFits1()
    , fFits2()
{
}

R3BCalifaCrystalCal2TotCalPar::~R3BCalifaCrystalCal2TotCalPar()
{
    // Clean up TGraphErrors and TF1 objects first
    for (Int_t i = 0; i < fGraphs1.size(); i++)
    {
        if (fGraphs1[i])
        {
            delete fGraphs1[i];
            fGraphs1[i] = nullptr;
        }
        if (fGraphs2[i])
        {
            delete fGraphs2[i];
            fGraphs2[i] = nullptr;
        }

        delete fFits1[i];
        delete fFits2[i];
        fFits1[i] = nullptr;
        fFits2[i] = nullptr;
    }

    // Clean up ToT_vs_energy hists
    for (Int_t j = 0; j < tot_vs_energy_hist.size(); j++)
    {
        if (tot_vs_energy_hist[j])
        {
            delete tot_vs_energy_hist[j];
            tot_vs_energy_hist[j] = nullptr;
        }
    }

    for (Int_t k = 0; k < totCanvases.size(); k++)
    {
        if (totCanvases[k])
        {
            delete totCanvases[k];
            totCanvases[k] = nullptr;
        }
    }

    fCrystalCalDataCA = nullptr;
    fMappedDataCA = nullptr;
}

void R3BCalifaCrystalCal2TotCalPar::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "FairRuntimeDb not opened!";
    }
    fMap_Par = dynamic_cast<R3BCalifaMappingPar*>(rtdb->getContainer("califaMappingPar"));
    if (!fMap_Par)
    {
        LOG(error) << "R3BCalifaCrystalCal2TotCalPar::Init() Couldn't get handle on califamappingpar container";
    }
    else
    {
        LOG(info) << "R3BCalifaCrystalCal2TotCalPar:: califamappingpar container open";
    }
}

void R3BCalifaCrystalCal2TotCalPar::SetParameter()
{
    if (!fMap_Par)
    {
        LOG(error) << "R3BCalifaMapped2CrystalCalPar::Container califaMappingPar not found.";
        return;
    }
    //--- Parameter Container ---
    fNumCrystals = fMap_Par->GetNumCrystals(); // Number of crystals x 2
    LOG(info) << "R3BCalifaMapped2CrystalCalPar::NumCry " << fNumCrystals;
}

InitStatus R3BCalifaCrystalCal2TotCalPar::Init()
{
    LOG(info) << "R3BCalifaCrystalCal2TotCalPar::Init() called";
    gROOT->SetBatch(kTRUE);

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(error) << "R3BCalifaCrystalCal2TotCalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fCrystalCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("CalifaCrystalCalData"));
    if (!fCrystalCalDataCA)
    {
        LOG(error) << "R3BCalifaCrystalCal2TotCalPar::Init() CalifaCrystalCalData not found";
        return kFATAL;
    }

    fMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("CalifaMappedData"));
    if (!fMappedDataCA)
    {
        LOG(error) << "R3BCalifaCrystalCal2TotCalPar::Init() CalifaMappedData not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BCalifaCrystalCal2TotCalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTotCal_Par = dynamic_cast<R3BCalifaTotCalPar*>(rtdb->getContainer("CalifaTotCalPar"));
    if (!fTotCal_Par)
    {
        LOG(error) << "R3BCalifaCrystalCal2TotCalPar::Init() Couldn't get handle on CalifaTotCalPar container";
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    // Initialize vectors with proper size
    fGraphs1.clear();
    fGraphs2.clear();
    fFits1.clear();
    fFits2.clear();
    tot_vs_energy_hist.clear();
    totCanvases.clear();

    fGraphs1.resize(fNumCrystals, nullptr);
    fGraphs2.resize(fNumCrystals, nullptr);
    fFits1.resize(fNumCrystals, nullptr);
    fFits2.resize(fNumCrystals, nullptr);
    tot_vs_energy_hist.resize(fNumCrystals, nullptr);
    totCanvases.resize(fNumCrystals, nullptr);

    for (Int_t i = 0; i < fNumCrystals; i++)
    {
        if (fMap_Par->GetInUse(i + 1) == 1)
        {
            // Create histograms
            if (i < fNumCrystals / 2) // Gamma range
            {
                TString hname = Form("rawtot_vs_calenergy_gamma_crystal_%d", i + 1);
                TString htitle = Form("Raw_Tot vs Cal_Energy_crystal %d;Cal_Energy [keV];Raw_ToT", i + 1);
                tot_vs_energy_hist[i] = new TH2D(hname, htitle, 600, 0, 3e4, 800, 0, 4000);
            }
            else // Proton range
            {
                TString hname = Form("rawtot_vs_calenergy_proton_crystal_%d", i + 1);
                TString htitle = Form("Raw_Tot vs Cal_Energy_crystal %d;Cal_Energy [keV];Raw_ToT", i + 1);
                tot_vs_energy_hist[i] = new TH2D(hname, htitle, 800, 0, 4e5, 800, 0, 4000);
            }

            if (tot_vs_energy_hist[i])
            {
                tot_vs_energy_hist[i]->SetDirectory(nullptr);
            }
        }
    }

    LOG(info) << "R3BCalifaCrystalCal2TotCalPar::Init() completed successfully";
    return kSUCCESS;
}

InitStatus R3BCalifaCrystalCal2TotCalPar::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BCalifaCrystalCal2TotCalPar::Reset() {}

void R3BCalifaCrystalCal2TotCalPar::FinishEvent() {}

void R3BCalifaCrystalCal2TotCalPar::FinishTask()
{
    LOG(info) << "R3BCalifaCrystalCal2TotCalPar::FinishTask() called";

    // Perform the calibration parameter search
    Search_TotParams();

    // Write debug output if requested
    if (fDebugMode)
    {
        FairRootManager* ioman = FairRootManager::Instance();
        if (!ioman)
        {
            LOG(error) << "No FairRootManager instance available";
            return;
        }

        TFile* outFile = ioman->GetOutFile();
        if (!outFile)
        {
            LOG(error) << "No output file available";
            return;
        }

        outFile->cd(); // Ensure we're in the output file directory

        for (Int_t i = 0; i < fNumCrystals; i++)
        {
            // Write histograms
            if (tot_vs_energy_hist[i] && tot_vs_energy_hist[i]->GetEntries() > 0)
            {
                tot_vs_energy_hist[i]->SetDirectory(outFile);
                tot_vs_energy_hist[i]->Write();
                tot_vs_energy_hist[i]->SetDirectory(nullptr); // Reset to avoid double deletion
            }

            // Write canvases with content
            if (totCanvases[i])
            {
                totCanvases[i]->cd();
                totCanvases[i]->Modified();
                totCanvases[i]->Update();
                totCanvases[i]->Write();
                LOG(info) << "Written canvas for crystal " << i + 1;
            }
        }

        outFile->Flush(); // Ensure everything is written to disk
    }

    LOG(info) << "R3BCalifaCrystalCal2TotCalPar::FinishTask() completed";
}

void R3BCalifaCrystalCal2TotCalPar::Exec(Option_t* opt)
{
    // Reset entries in output arrays, local arrays
    // Reset();
    auto nHits = fCrystalCalDataCA->GetEntriesFast();

    if (nHits == 0)
        return;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto* CalHit = dynamic_cast<R3BCalifaCrystalCalData*>(fCrystalCalDataCA->At(i));
        auto* MapHit = dynamic_cast<R3BCalifaMappedData*>(fMappedDataCA->At(i));

        if (!CalHit || !MapHit)
            continue;

        Int_t crystalId = CalHit->GetCrystalId();

        // Check bounds and validity
        if (crystalId < 1 || crystalId > fNumCrystals)
            continue;
        if (!tot_vs_energy_hist[crystalId - 1])
            continue;

        // Fill histograms
        if (CalHit->GetCalEnergy() > fThreshold)
        {
            tot_vs_energy_hist[crystalId - 1]->Fill(CalHit->GetCalEnergy(), MapHit->GetTot());
        }
    }
}

void R3BCalifaCrystalCal2TotCalPar::Search_TotParams()
{
    // ID of last barrel gamma range crystal
    const Int_t barrelGammaEnd = 1952;

    LOG(info) << "R3BCalifaCrystalCal2TotCalPar::Search_TotParams() called";

    if (!fTotCal_Par)
    {
        LOG(error) << "fTotCal_Par is null!";
        return;
    }

    fTotCal_Par->SetNumCrystals(fNumCrystals);
    fTotCal_Par->SetNumParametersFit(fNumParam);
    fTotCal_Par->GetCryCalParams()->Set(fNumParam * fNumCrystals);

    for (Int_t i = 0; i < fNumCrystals; i++)
    {
        if (fMap_Par->GetInUse(i + 1) != 1)
            continue;

        if (!tot_vs_energy_hist[i] || tot_vs_energy_hist[i]->GetEntries() < fMinStatistics)
            continue;

        if (i < fNumCrystals / 2) // Gamma range
        {
            if (i < barrelGammaEnd) // Barrel gamma crystals
            {
                TString funcName = Form("f_crystal_%d", i + 1);
                TF1* fitFunc = new TF1(funcName, "[1]*TMath::Log(x/[0])", fLeft, fRight);
                fitFunc->SetParameters(6060, 1028);

                Int_t fitResult = tot_vs_energy_hist[i]->Fit(fitFunc, "RQ"); // Store fit for plotting

                if (fitResult == 0) // Successful fit
                {
                    for (Int_t h = 0; h < fNumParam; h++)
                    {
                        fTotCal_Par->SetCryCalParams(fitFunc->GetParameter(h), fNumParam * i + h + 1);
                        LOG(info) << "Crystal ID " << i + 1 << " has ToT param: a" << h << "="
                                  << fitFunc->GetParameter(h);
                    }
                }

                delete fitFunc;
            }
            else // Forward gamma crystals (IpHos & CEPA)
            {
                LOG(info) << "Processing forward gamma crystal " << (i + 1) << " (array index " << i << ")";

                // Collect slice-wise peak positions
                std::vector<Double_t> eCenters, band1, band2, band1Err, band2Err;

                const Int_t minbin = tot_vs_energy_hist[i]->GetXaxis()->FindBin(fLeft);
                const Int_t maxbin = tot_vs_energy_hist[i]->GetXaxis()->FindBin(fRight);
                const Int_t step = 10; // Increased step size to get more statistics per slice

                Double_t prev_band1 = std::numeric_limits<Double_t>::quiet_NaN();
                Double_t prev_band2 = std::numeric_limits<Double_t>::quiet_NaN();

                for (Int_t bx = minbin; bx <= maxbin; bx += step)
                {
                    Int_t bx2 = std::min(bx + step - 1, maxbin);

                    auto [shadowPeak, mainPeak] = ProjectAndFit(tot_vs_energy_hist[i], bx, bx2, prev_band1, prev_band2);

                    if (std::isnan(mainPeak.first) || std::isnan(shadowPeak.first))
                    {
                        LOG(debug) << "Crystal " << (i + 1) << ": Failed to find peaks in energy slice "
                                   << tot_vs_energy_hist[i]->GetXaxis()->GetBinCenter(bx) << " - "
                                   << tot_vs_energy_hist[i]->GetXaxis()->GetBinCenter(bx2);
                        continue;
                    }

                    Double_t eCenter = tot_vs_energy_hist[i]->GetXaxis()->GetBinCenter((bx + bx2) / 2);
                    eCenters.push_back(eCenter);
                    band1.push_back(mainPeak.first);
                    band2.push_back(shadowPeak.first);
                    band1Err.push_back(mainPeak.second);
                    band2Err.push_back(shadowPeak.second);

                    LOG(debug) << "Crystal " << (i + 1) << ": Found peaks at E=" << eCenter
                               << ", Main=" << mainPeak.first << ", Shadow=" << shadowPeak.first;
                }

                if (eCenters.size() < 3)
                {
                    LOG(warn) << "Not enough points for crystal " << (i + 1) << " (only " << eCenters.size()
                              << " points), skipping canvas creation";
                    continue;
                }

                LOG(info) << "Found " << eCenters.size() << " data points for crystal " << (i + 1);

                // Create TGraphs - these will be managed by our class
                TGraphErrors* g1 =
                    new TGraphErrors(eCenters.size(), eCenters.data(), band1.data(), nullptr, band1Err.data());
                TGraphErrors* g2 =
                    new TGraphErrors(eCenters.size(), eCenters.data(), band2.data(), nullptr, band2Err.data());

                // Create fit functions
                TF1* fLog1 = new TF1(Form("fLog1_crystal%d", i + 1), "[1]*TMath::Log(x/[0])", fLeft, fRight);
                fLog1->SetParameters(7700, 1000);
                g1->Fit(fLog1, "RQ"); // Store fit in graph for plotting
                fLog1->SetLineColor(kRed);

                TF1* fLog2 = new TF1(Form("fLog2_crystal%d", i + 1), "[1]*TMath::Log(x/[0])", fLeft, fRight);
                fLog2->SetParameters(7700, 1000);
                g2->Fit(fLog2, "RQ"); // Store fit in graph for plotting
                fLog2->SetLineColor(kBlack);

                TF1* f2_clone = g2->GetFunction(Form("fLog2_crystal%d", i + 1));
                if (f2_clone)
                {
                    f2_clone->SetLineColor(kBlack);
                }

                fGraphs1[i] = g1;
                fGraphs2[i] = g2;
                fFits1[i] = fLog1;
                fFits2[i] = fLog2;

                // Create and draw on canvas if in debug mode
                if (fDebugMode)
                {

                    // Create canvas
                    totCanvases[i] =
                        new TCanvas(Form("c_crystal%d", i + 1), Form("Crystal %d ToT Calibration", i + 1), 800, 600);
                    totCanvases[i]->SetBatch(kTRUE);
                    totCanvases[i]->cd();

                    // Draw histogram first
                    if (tot_vs_energy_hist[i] && tot_vs_energy_hist[i]->GetEntries() > 0)
                    {
                        tot_vs_energy_hist[i]->Draw("colz");
                    }

                    // Draw graphs with fits
                    if (g1 && g1->GetN() > 0)
                    {
                        g1->SetMarkerColor(kRed);
                        g1->SetMarkerStyle(20);
                        g1->SetMarkerSize(1.5);
                        g1->Draw("P SAME");
                    }

                    if (g2 && g2->GetN() > 0)
                    {
                        g2->SetMarkerColor(kBlack);
                        g2->SetMarkerStyle(23);
                        g2->SetMarkerSize(1.5);
                        g2->Draw("P SAME");
                    }

                    // Add a legend
                    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
                    legend->AddEntry(g1, "Main Band", "p");
                    legend->AddEntry(g2, "Shadow Band", "p");
                    legend->Draw();

                    totCanvases[i]->Update();
                    totCanvases[i]->Modified();
                }

                // Extract parameters from the main fit
                for (Int_t h = 0; h < fNumParam; h++)
                {
                    fTotCal_Par->SetCryCalParams(fLog1->GetParameter(h), fNumParam * i + h + 1);
                    LOG(info) << "Crystal ID " << i + 1 << " has ToT param: a" << h << "=" << fLog1->GetParameter(h);
                }
            }
        }
        else // Proton range
        {
            gROOT->cd(); // cd to top level directory instead of being inside the canvases (prevent any previous from
                         // being overwritten)
            TString funcName = Form("f3_crystal_%d", i + 1);
            TF1* fitFunc = new TF1(funcName, "[1]*TMath::Log(x/[0])", 130000, 250000);
            fitFunc->SetParameters(6060, 1028);

            Int_t fitResult = tot_vs_energy_hist[i]->Fit(fitFunc, "RQ"); // Store fit for plotting

            if (fitResult == 0) // Successful fit
            {
                for (Int_t h = 0; h < fNumParam; h++)
                {
                    fTotCal_Par->SetCryCalParams(fitFunc->GetParameter(h), fNumParam * i + h + 1);
                    LOG(info) << "Crystal ID " << i + 1 << " has ToT param: a" << h << "=" << fitFunc->GetParameter(h);
                }
            }

            delete fitFunc;
        }
    }

    fTotCal_Par->setChanged();
    LOG(info) << "Search_TotParams completed";
}

// Project slice and fit 2 peaks
std::pair<std::pair<Double_t, Double_t>, std::pair<Double_t, Double_t>> R3BCalifaCrystalCal2TotCalPar::ProjectAndFit(
    TH2* h2,
    Int_t binx_low,
    Int_t binx_high,
    Double_t& prev_band1,
    Double_t& prev_band2)
{
    // Project slice in Y (ToT)
    TH1D* proj = h2->ProjectionY("_py_temp", binx_low, binx_high);
    if (!proj || proj->GetEntries() < fMinStatistics)
    {
        if (proj)
            delete proj;
        return { { std::numeric_limits<Double_t>::quiet_NaN(), std::numeric_limits<Double_t>::quiet_NaN() },
                 { std::numeric_limits<Double_t>::quiet_NaN(), std::numeric_limits<Double_t>::quiet_NaN() } };
    }

    // Try manual peak finding first for more control
    Int_t maxBin = proj->GetMaximumBin();
    Double_t maxVal = proj->GetBinContent(maxBin);
    Double_t mainPeakPos = proj->GetBinCenter(maxBin);

    // Look for second peak
    Double_t secondPeakPos = std::numeric_limits<Double_t>::quiet_NaN();
    Double_t secondPeakVal = 0;

    // Minimum separation between peaks (in raw ToT units)
    Double_t minSeparation = 50;

    for (Int_t bin = 1; bin <= proj->GetNbinsX(); bin++)
    {
        Double_t binPos = proj->GetBinCenter(bin);
        Double_t binVal = proj->GetBinContent(bin);

        // Check if this could be a second peak
        if (std::abs(binPos - mainPeakPos) > minSeparation &&
            binVal > maxVal * 0.05 && // At least 5% of main peak height
            binVal > secondPeakVal)
        {
            secondPeakPos = binPos;
            secondPeakVal = binVal;
        }
    }

    // If manual method found 2 peaks, use them
    if (!std::isnan(secondPeakPos))
    {
        Double_t peak1, peak2, err1, err2;

        // Assign based on continuity or position
        if (std::isnan(prev_band1))
        {
            if (maxVal < secondPeakVal)
            {
                peak1 = mainPeakPos;
                err1 = std::sqrt(maxVal);
                peak2 = secondPeakPos;
                err2 = std::sqrt(secondPeakVal);
            }
            else
            {
                peak1 = secondPeakPos;
                err1 = std::sqrt(secondPeakVal);
                peak2 = mainPeakPos;
                err2 = std::sqrt(maxVal);
            }
        }
        else
        {
            // Use continuity
            if (std::abs(prev_band1 - mainPeakPos) < std::abs(prev_band1 - secondPeakPos))
            {
                peak1 = mainPeakPos;
                err1 = std::sqrt(maxVal);
                peak2 = secondPeakPos;
                err2 = std::sqrt(secondPeakVal);
            }
            else
            {
                peak1 = secondPeakPos;
                err1 = std::sqrt(secondPeakVal);
                peak2 = mainPeakPos;
                err2 = std::sqrt(maxVal);
            }
        }

        prev_band1 = peak1;
        prev_band2 = peak2;

        delete proj;
        return { { peak1, err1 }, { peak2, err2 } };
    }

    TSpectrum* activeSpec = nullptr;
    Int_t nFound = 0;

    // Try progressively more sensitive searches
    TSpectrum* spec1 = new TSpectrum(5);
    nFound = spec1->Search(proj, 1.0, "", 0.02); // sigma=1.0, threshold=2%
    activeSpec = spec1;

    if (nFound < 2)
    {
        // Try more sensitive search
        TSpectrum* spec2 = new TSpectrum(5);
        nFound = spec2->Search(proj, 0.5, "", 0.01); // sigma=0.5, threshold=1%
        activeSpec = spec2;
        delete spec1;
    }

    if (nFound < 2)
    {
        delete activeSpec;
        delete proj;
        return { { std::numeric_limits<Double_t>::quiet_NaN(), std::numeric_limits<Double_t>::quiet_NaN() },
                 { std::numeric_limits<Double_t>::quiet_NaN(), std::numeric_limits<Double_t>::quiet_NaN() } };
    }

    Double_t* xPeaks = activeSpec->GetPositionX(); // x positions of peaks

    // Estimate errors by fitting small Gaussians around each peak
    Double_t peakErr[2] = { 5.0, 5.0 }; // Default error values

    for (int i = 0; i < 2 && i < nFound; i++)
    {
        Double_t peakPos = xPeaks[i];
        TF1* fpeak = new TF1("fpeak", "gaus", peakPos - 10, peakPos + 10);
        proj->Fit(fpeak, "RQ0");
        xPeaks[i] = fpeak->GetParameter(1);
        peakErr[i] = fpeak->GetParameter(2);
    }

    // Assign main vs shadow using continuity
    Double_t peak1, peak2, err1, err2;

    if (std::isnan(prev_band1))
    {
        // First iteration - assign based on position
        if (xPeaks[0] < xPeaks[1])
        {
            peak1 = xPeaks[0];
            err1 = peakErr[0];
            peak2 = xPeaks[1];
            err2 = peakErr[1];
        }
        else
        {
            peak1 = xPeaks[1];
            err1 = peakErr[1];
            peak2 = xPeaks[0];
            err2 = peakErr[0];
        }
    }
    else
    {
        // Use continuity to assign peaks
        if (std::abs(prev_band1 - xPeaks[0]) < std::abs(prev_band1 - xPeaks[1]))
        {
            peak1 = xPeaks[0];
            err1 = peakErr[0];
            peak2 = xPeaks[1];
            err2 = peakErr[1];
        }
        else
        {
            peak1 = xPeaks[1];
            err1 = peakErr[1];
            peak2 = xPeaks[0];
            err2 = peakErr[0];
        }
    }

    // Update previous values
    prev_band1 = peak1;
    prev_band2 = peak2;

    delete activeSpec;
    delete proj;
    return { { peak1, err1 }, { peak2, err2 } };
}

ClassImp(R3BCalifaCrystalCal2TotCalPar)
